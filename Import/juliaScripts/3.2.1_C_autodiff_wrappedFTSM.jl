# Julia Skript
#
# unique external nodes :
# | arg 'deltaEpsilon', order [1], dimensions {3}
# | arg 'E0', order [2], dimensions {3, 3}
# | arg 'epsilon', order [1], dimensions {3}
# | arg 'deltaU', order [1], dimensions {3}
# | arg 'b', order [1], dimensions {3}
# | arg 't', order [1], dimensions {3}

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


function autodiff_FTSM(deltaEpsilon, E0, epsilon, deltaU, b, t)

	@assert length(deltaEpsilon) == 3
	@assert size(E0) == (3, 3)
	@assert length(epsilon) == 3
	@assert length(deltaU) == 3
	@assert length(b) == 3
	@assert length(t) == 3

	println("[Ausdruck mit 0 temporären Dependencies substituiert]")

	println("[Evaluating final Result, Komplexität 12(8, 2)]")
	@tullio res[] := ((((deltaEpsilon[idx19] * E0[idx19, idx21]) * epsilon[idx21]) - (deltaU[idx26] * b[idx26])) - (deltaU[idx30] * t[idx30]))

	return res

end

start_time = time()
res = autodiff_FTSM(rand(3), rand(3, 3), rand(3), rand(3), rand(3), rand(3))
elapsed = time() - start_time
println("Laufzeit: ", elapsed, " s")
println("Ergebnis: ", res)