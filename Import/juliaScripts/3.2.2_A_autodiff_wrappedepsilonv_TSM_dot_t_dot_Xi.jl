# Julia Skript
#
# unique external nodes :
# | arg 'eta', order [0], dimensions {}
# | arg 'S', order [2], dimensions {3, 3}
# | arg 'dE01_dXi1', order [4], dimensions {3, 3, 3, 3}
# | arg 'Identity_ord4_dm3333', order [4], dimensions {3, 3, 3, 3}
# | arg 'epsilon', order [1], dimensions {3}
# | arg 'epsilonv', order [1], dimensions {3}
# | arg 'E0', order [2], dimensions {3, 3}
# | arg 'depsilonv1_dXi1', order [3], dimensions {3, 3, 3}

using LinearAlgebra
using Tullio
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


function autodiff_epsilonv_TSM_dot_t_dot_Xi(eta, S, dE01_dXi1, epsilon, epsilonv, E0, depsilonv1_dXi1)

	@assert ndims(eta) == 0
	@assert size(S) == (3, 3)
	@assert size(dE01_dXi1) == (3, 3, 3, 3)
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

	println("[Evaluating 'tmpRes_4', Komplexität 10(6, 2)]")
	@tullio tmpRes_4[idx58, idx69, idx70] := (((tmpRes_1 * S[idx58, idx59]) * (dE01_dXi1[idx59, idx69, idx70, idx71] + Identity_ord4_dm3333[idx59, idx69, idx70, idx71])) * (epsilon[idx71] - epsilonv[idx71]))

	println("[Evaluating 'tmpRes_5', Komplexität 12(4, 6)]")
	@tullio tmpRes_5[idx58, idx69, idx70] := (((tmpRes_3 * S[idx58, idx87]) * E0[idx87, idx91]) * (-1 * (depsilonv1_dXi1[idx91, idx69, idx70] + (depsilonv1_dXi1[idx91, idx103, idx104] * Identity_ord4_dm3333[idx103, idx104, idx69, idx70]))))

	println("[Evaluating final Result, Komplexität 2(0, 0)]")
	@tullio res[idx58, idx69, idx70] := (tmpRes_4[idx58, idx69, idx70] + tmpRes_5[idx58, idx69, idx70])

	return res

end

start_time = time()
res = autodiff_epsilonv_TSM_dot_t_dot_Xi(rand(), rand(3, 3), rand(3, 3, 3, 3), rand(3), rand(3), rand(3, 3), rand(3, 3, 3))
elapsed = time() - start_time
println("Laufzeit: ", elapsed, " s")
println("Ergebnis: ", res)