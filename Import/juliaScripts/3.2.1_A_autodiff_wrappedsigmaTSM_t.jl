# Julia Skript
#
# unique external nodes :
# | arg 'E0', order [2], dimensions {3, 3}
# | arg 'D', order [2], dimensions {3, 3}
# | arg 'epsilon', order [1], dimensions {3}
# | arg 'epsilonv', order [1], dimensions {3}
# | arg 'Identity_ord4_dm3333', order [4], dimensions {3, 3, 3, 3}
# | arg 'depsilonv1_dD1', order [3], dimensions {3, 3, 3}
# | arg 'depsilonv2_dD2', order [5], dimensions {3, 3, 3, 3, 3}

using LinearAlgebra
using TensorOperations
using Dates

function create_zeros(dims::Integer...)
    return zeros(Float64, dims...)
end

function create_ones(dims::Integer...)
    return ones(Float64, dims...)
end

function create_Identity(dims::Integer...)
    n = dims[1]
    @assert all(d -> d == n, dims) "All dimensions must be equal for Identity"
    tensor = zeros(Float64, dims...)
    for i in 1:n
        indices = ntuple(x -> i, length(dims))
        tensor[indices...] = 1.0
    end
    return tensor
end

function create_eps(dims::Integer...)
    # Alle Dimensionen müssen gleich sein
    n = dims[1]
    @assert all(d -> d == n, dims) "All dimensions must be equal for Levi-Civita"
    @assert n == 3 "Levi-Civita only implemented for dimension 3"
    @assert length(dims) == 3 "Levi-Civita must be 3D tensor"
    
    eps_tensor = Base.zeros(Float64, dims...)
    
    for i in 1:n
        for j in 1:n
            for k in 1:n
                indices = [i, j, k]
                if length(unique(indices)) != 3
                    continue
                end
                sign = 1
                for x in 1:2
                    for y in x+1:3
                        if indices[x] > indices[y]
                            sign *= -1
                        end
                    end
                end
                eps_tensor[i, j, k] = sign
            end
        end
    end
    
    return eps_tensor
end


function autodiff_sigmaTSM_t(E0, D, epsilon, epsilonv, depsilonv1_dD1, depsilonv2_dD2)

	@assert size(E0) == (3, 3)
	@assert size(D) == (3, 3)
	@assert length(epsilon) == 3
	@assert length(epsilonv) == 3
	Identity_ord4_dm3333 = create_Identity(3, 3, 3, 3)
	@assert size(depsilonv1_dD1) == (3, 3, 3)
	@assert size(depsilonv2_dD2) == (3, 3, 3, 3, 3)

	println("[Ausdruck mit 3 temporären Dependencies substituiert]")

	println("[Evaluating 'tmpRes_0', Komplexität 6(2, 2)]")
	@tensor tmpRes_0[idx106] := ((E0[idx106, idx107] + D[idx106, idx107]) * (epsilon[idx107] - epsilonv[idx107]))

	println("[Evaluating 'tmpRes_1', Komplexität 14(12, 0)]")
	@tensor tmpRes_1[idx106] := (((Identity_ord4_dm3333[idx106, idx125, idx126, idx127] * (epsilon[idx127] - epsilonv[idx127])) + ((E0[idx106, idx133] + D[idx106, idx133]) * (-1 * depsilonv1_dD1[idx133, idx125, idx126]))) * D[idx125, idx126])

	println("[Evaluating 'tmpRes_2', Komplexität 18(10, 6)]")
	@tensor tmpRes_2[idx106, idx159, idx160, idx156, idx157] := (((Identity_ord4_dm3333[idx106, idx159, idx160, idx161] * (-1 * depsilonv1_dD1[idx161, idx156, idx157])) + (Identity_ord4_dm3333[idx106, idx159, idx160, idx175] * (-1 * depsilonv1_dD1[idx175, idx156, idx157]))) + ((E0[idx106, idx181] + D[idx106, idx181]) * (-1 * depsilonv2_dD2[idx181, idx159, idx160, idx156, idx157])))

	println("[Evaluating final Result, Komplexität 10(2, 6)]")
	@tensor res[idx106] := ((tmpRes_0[idx106] + tmpRes_1[idx106]) + (((0.5 * tmpRes_2[idx106, idx159, idx160, idx156, idx157]) * D[idx156, idx157]) * D[idx159, idx160]))

	return res

end

start_time = time()
res = autodiff_sigmaTSM_t(rand(3, 3), rand(3, 3), rand(3), rand(3), rand(3, 3, 3), rand(3, 3, 3, 3, 3))
elapsed = time() - start_time
println("Laufzeit: ", elapsed, " s")
println("Ergebnis: ", res)