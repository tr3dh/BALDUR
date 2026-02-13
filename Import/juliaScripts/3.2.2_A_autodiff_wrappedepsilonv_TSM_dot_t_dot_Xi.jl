# Julia Skript
#
# unique external nodes :
# | arg 'eta', order [0], dimensions {}
# | arg 'S', order [2], dimensions {3, 3}
# | arg 'Identity_ord4_dm3333', order [4], dimensions {3, 3, 3, 3}
# | arg 'epsilon', order [1], dimensions {3}
# | arg 'epsilonv', order [1], dimensions {3}
# | arg 'E0', order [2], dimensions {3, 3}
# | arg 'depsilonv1_dXi1', order [3], dimensions {3, 3, 3}

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


function autodiff_epsilonv_TSM_dot_t_dot_Xi(eta, S, epsilon, epsilonv, E0, depsilonv1_dXi1)

	@assert ndims(eta) == 0
	@assert size(S) == (3, 3)
	Identity_ord4_dm3333 = create_Identity(3, 3, 3, 3)
	@assert length(epsilon) == 3
	@assert length(epsilonv) == 3
	@assert size(E0) == (3, 3)
	@assert size(depsilonv1_dXi1) == (3, 3, 3)

	println("[Ausdruck mit 6 temporären Dependencies substituiert]")

	println("[Evaluating 'tmpRes_0', Komplexität 0()]")
	tmpRes_0 = eta

	println("[Evaluating 'tmpRes_1', Komplexität 1(0)]")
	tmpRes_1 = (1/tmpRes_0)

	println("[Evaluating 'tmpRes_2', Komplexität 0()]")
	tmpRes_2 = eta

	println("[Evaluating 'tmpRes_3', Komplexität 1(0)]")
	tmpRes_3 = (1/tmpRes_2)

	println("[Evaluating 'tmpRes_4', Komplexität 8(4, 2)]")
	@tensor tmpRes_4[idx46, idx53, idx54] := (((tmpRes_1 * S[idx46, idx47]) * Identity_ord4_dm3333[idx47, idx53, idx54, idx55]) * (epsilon[idx55] - epsilonv[idx55]))

	println("[Evaluating 'tmpRes_5', Komplexität 12(4, 6)]")
	@tensor tmpRes_5[idx46, idx53, idx54] := (((tmpRes_3 * S[idx46, idx63]) * E0[idx63, idx67]) * (-1 * (depsilonv1_dXi1[idx67, idx53, idx54] + (depsilonv1_dXi1[idx67, idx79, idx80] * Identity_ord4_dm3333[idx79, idx80, idx53, idx54]))))

	println("[Evaluating final Result, Komplexität 2(0, 0)]")
	@tensor res[idx46, idx53, idx54] := (tmpRes_4[idx46, idx53, idx54] + tmpRes_5[idx46, idx53, idx54])

	return res

end

start_time = time()
res = autodiff_epsilonv_TSM_dot_t_dot_Xi(rand(), rand(3, 3), rand(3), rand(3), rand(3, 3), rand(3, 3, 3))
elapsed = time() - start_time
println("Laufzeit: ", elapsed, " s")
println("Ergebnis: ", res)