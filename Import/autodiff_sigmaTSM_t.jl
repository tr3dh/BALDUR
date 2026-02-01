# Julia Skript
#
# unique external nodes :
# | arg 'E0', order [2], dimensions {3, 3}
# | arg 'D', order [2], dimensions {3, 3}
# | arg 'epsilon', order [1], dimensions {3}
# | arg 'epsilonv', order [1], dimensions {3}
# | arg 'depsilonv1_dD1', order [3], dimensions {3, 3, 3}
# | arg 'Identity_ord4_dm3333', order [4], dimensions {3, 3, 3, 3}

using LinearAlgebra

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


function autodiff_sigmaTSM_t(E0, D, epsilon, epsilonv, depsilonv1_dD1)

	@assert size(E0) == (3, 3)
	@assert size(D) == (3, 3)
	@assert length(epsilon) == 3
	@assert length(epsilonv) == 3
	@assert size(depsilonv1_dD1) == (3, 3, 3)
	Identity_ord4_dm3333 = create_Identity(3, 3, 3, 3)

	res = Base.zeros(3)

	res[1] = (sum(idx5 -> (E0[1, idx5] + D[1, idx5]) * (epsilon[idx5] - (epsilonv[idx5] + sum(idx18 -> sum(idx19 -> depsilonv1_dD1[idx5, idx18, idx19] * D[idx18, idx19], 1:3), 1:3))), 1:3) + sum(idx43 -> sum(idx44 -> (sum(idx45 -> Identity_ord4_dm3333[1, idx43, idx44, idx45] * (epsilon[idx45] - (epsilonv[idx45] + sum(idx36 -> sum(idx37 -> depsilonv1_dD1[idx45, idx36, idx37] * D[idx36, idx37], 1:3), 1:3))), 1:3) + sum(idx51 -> (E0[1, idx51] + D[1, idx51]) * -1 * (depsilonv1_dD1[idx51, idx43, idx44] + sum(idx67 -> sum(idx68 -> depsilonv1_dD1[idx51, idx67, idx68] * Identity_ord4_dm3333[idx67, idx68, idx43, idx44], 1:3), 1:3)), 1:3)) * D[idx43, idx44], 1:3), 1:3))
	res[2] = (sum(idx5 -> (E0[2, idx5] + D[2, idx5]) * (epsilon[idx5] - (epsilonv[idx5] + sum(idx18 -> sum(idx19 -> depsilonv1_dD1[idx5, idx18, idx19] * D[idx18, idx19], 1:3), 1:3))), 1:3) + sum(idx43 -> sum(idx44 -> (sum(idx45 -> Identity_ord4_dm3333[2, idx43, idx44, idx45] * (epsilon[idx45] - (epsilonv[idx45] + sum(idx36 -> sum(idx37 -> depsilonv1_dD1[idx45, idx36, idx37] * D[idx36, idx37], 1:3), 1:3))), 1:3) + sum(idx51 -> (E0[2, idx51] + D[2, idx51]) * -1 * (depsilonv1_dD1[idx51, idx43, idx44] + sum(idx67 -> sum(idx68 -> depsilonv1_dD1[idx51, idx67, idx68] * Identity_ord4_dm3333[idx67, idx68, idx43, idx44], 1:3), 1:3)), 1:3)) * D[idx43, idx44], 1:3), 1:3))
	res[3] = (sum(idx5 -> (E0[3, idx5] + D[3, idx5]) * (epsilon[idx5] - (epsilonv[idx5] + sum(idx18 -> sum(idx19 -> depsilonv1_dD1[idx5, idx18, idx19] * D[idx18, idx19], 1:3), 1:3))), 1:3) + sum(idx43 -> sum(idx44 -> (sum(idx45 -> Identity_ord4_dm3333[3, idx43, idx44, idx45] * (epsilon[idx45] - (epsilonv[idx45] + sum(idx36 -> sum(idx37 -> depsilonv1_dD1[idx45, idx36, idx37] * D[idx36, idx37], 1:3), 1:3))), 1:3) + sum(idx51 -> (E0[3, idx51] + D[3, idx51]) * -1 * (depsilonv1_dD1[idx51, idx43, idx44] + sum(idx67 -> sum(idx68 -> depsilonv1_dD1[idx51, idx67, idx68] * Identity_ord4_dm3333[idx67, idx68, idx43, idx44], 1:3), 1:3)), 1:3)) * D[idx43, idx44], 1:3), 1:3))

	return res
end

print(autodiff_sigmaTSM_t(rand(3, 3), rand(3, 3), rand(3), rand(3), rand(3, 3, 3)))