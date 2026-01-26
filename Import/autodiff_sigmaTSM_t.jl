# Julia Skript
#
# unique external nodes :
# | arg 'E0', order [2], dimensions {6, 6}
# | arg 'D', order [2], dimensions {6, 6}
# | arg 'epsilon', order [1], dimensions {6}
# | arg 'epsilonv', order [1], dimensions {6}

using LinearAlgebra
using Tullio

function levi_civita(indices...)
    
    n = length(indices)
    if length(unique(indices)) != n
        return 0
    end
    perm = collect(indices)
    sign = 1
    for i in 1:n-1
        for j in i+1:n
            if perm[i] > perm[j]
                sign *= -1
            end
        end
    end
    return sign
end

function identity_tensor(indices...)
    
    return all(i -> i == indices[1], indices) ? 1 : 0
end

zeros(indices::Integer...) = 0
ones(indices::Integer...) = 1
eps(indices::Integer...) = levi_civita(indices...)
Identity(indices::Integer...) = identity_tensor(indices...)

function autodiff_sigmaTSM_t(E0, D, epsilon, epsilonv)

	@assert size(E0) == (6, 6)
	@assert size(D) == (6, 6)
	@assert length(epsilon) == 6
	@assert length(epsilonv) == 6

	res = Base.zeros(6)

	@tullio res[idx4] = ((E0[idx4, idx5] + D[idx4, idx5]) * (epsilon[idx5] - epsilonv[idx5]))

	return res
end