# Julia Skript
#
# unique external nodes :
# | arg '__INVALID__', order [0], dimensions {}
# | arg 'E0', order [2], dimensions {3, 3}
# | arg 'D', order [2], dimensions {3, 3}
# | arg 'epsilon', order [1], dimensions {3}
# | arg 'epsilonv', order [1], dimensions {3}
# | arg 'depsilonv1_dD1', order [3], dimensions {3, 3, 3}
# | arg 'depsilonv2_dD2', order [5], dimensions {3, 3, 3, 3, 3}
# | arg 'depsilonv3_dD3', order [7], dimensions {3, 3, 3, 3, 3, 3, 3}
# | arg 'zeros_ord4_dm3333', order [4], dimensions {3, 3, 3, 3}
# | arg 'Identity_ord4_dm3333', order [4], dimensions {3, 3, 3, 3}
# | arg 'zeros_ord3_dm333', order [3], dimensions {3, 3, 3}
# | arg 'depsilonv4_dD4', order [9], dimensions {3, 3, 3, 3, 3, 3, 3, 3, 3}
# | arg 'zeros_ord6_dm333333', order [6], dimensions {3, 3, 3, 3, 3, 3}
# | arg 'zeros_ord5_dm33333', order [5], dimensions {3, 3, 3, 3, 3}
# | arg 'depsilonv5_dD5', order [11], dimensions {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3}
# | arg 'zeros_ord8_dm33333333', order [8], dimensions {3, 3, 3, 3, 3, 3, 3, 3}
# | arg 'zeros_ord7_dm3333333', order [7], dimensions {3, 3, 3, 3, 3, 3, 3}
# | arg 'depsilonv6_dD6', order [13], dimensions {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3}

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


function autodiff_sigmaTSM_t(__INVALID__, E0, D, epsilon, epsilonv, depsilonv1_dD1, depsilonv2_dD2, depsilonv3_dD3, depsilonv4_dD4, depsilonv5_dD5, depsilonv6_dD6)

	@assert ndims(__INVALID__) == 0
	@assert size(E0) == (3, 3)
	@assert size(D) == (3, 3)
	@assert length(epsilon) == 3
	@assert length(epsilonv) == 3
	@assert size(depsilonv1_dD1) == (3, 3, 3)
	@assert size(depsilonv2_dD2) == (3, 3, 3, 3, 3)
	@assert size(depsilonv3_dD3) == (3, 3, 3, 3, 3, 3, 3)
	zeros_ord4_dm3333 = create_zeros(3, 3, 3, 3)
	Identity_ord4_dm3333 = create_Identity(3, 3, 3, 3)
	zeros_ord3_dm333 = create_zeros(3, 3, 3)
	@assert size(depsilonv4_dD4) == (3, 3, 3, 3, 3, 3, 3, 3, 3)
	zeros_ord6_dm333333 = create_zeros(3, 3, 3, 3, 3, 3)
	zeros_ord5_dm33333 = create_zeros(3, 3, 3, 3, 3)
	@assert size(depsilonv5_dD5) == (3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3)
	zeros_ord8_dm33333333 = create_zeros(3, 3, 3, 3, 3, 3, 3, 3)
	zeros_ord7_dm3333333 = create_zeros(3, 3, 3, 3, 3, 3, 3)
	@assert size(depsilonv6_dD6) == (3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3)



end

start_time = time()
res = autodiff_sigmaTSM_t(rand(), rand(3, 3), rand(3, 3), rand(3), rand(3), rand(3, 3, 3), rand(3, 3, 3, 3, 3), rand(3, 3, 3, 3, 3, 3, 3), rand(3, 3, 3, 3, 3, 3, 3, 3, 3), rand(3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3), rand(3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3))
elapsed = time() - start_time
println("Laufzeit: ", elapsed, " s")
println("Ergebnis: ", res)