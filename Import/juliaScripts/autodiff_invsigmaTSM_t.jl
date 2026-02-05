# Julia Skript
#
# unique external nodes :
# | arg 'E0', order [2], dimensions {3, 3}
# | arg 'D', order [2], dimensions {3, 3}

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


function autodiff_sigmaTSM_t(E0, D)

	@assert size(E0) == (3, 3)
	@assert size(D) == (3, 3)

	println("[Ausdruck mit 4 temporären Dependencies substituiert]")

	println("[Evaluating 'tmpRes_0', Komplexität 2(0, 0)]")
	@tullio tmpRes_0[idx1214, idx1217] := (E0[idx1214, idx1215] * D[idx1215, idx1217])

	println("[Evaluating 'tmpRes_1', Komplexität 1(0)]")
	tmpRes_1 = (inv(tmpRes_0))

	println("[Evaluating 'tmpRes_2', Komplexität 0()]")
	@tullio tmpRes_2[idx1222, idx1223] := E0[idx1222, idx1223]

	println("[Evaluating 'tmpRes_3', Komplexität 1(0)]")
	tmpRes_3 = (det(tmpRes_2))

	println("[Evaluating final Result, Komplexität 4(2, 0)]")
	@tullio res[] := (tmpRes_1[idx1219, idx1219] + tmpRes_3[])

	return res

end

start_time = time()
res = autodiff_sigmaTSM_t(rand(3, 3), rand(3, 3))
elapsed = time() - start_time
println("Laufzeit: ", elapsed, " s")
println("Ergebnis: ", res)