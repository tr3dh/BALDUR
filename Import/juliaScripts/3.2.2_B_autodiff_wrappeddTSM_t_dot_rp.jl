# Julia Skript
#
# unique external nodes :
# | arg 'd', order [0], dimensions {}
# | arg 'dd1_dXi1', order [2], dimensions {3, 3}
# | arg 'Xi', order [2], dimensions {3, 3}
# | arg 'dd2_dXi2', order [4], dimensions {3, 3, 3, 3}
# | arg 'dd3_dXi3', order [6], dimensions {3, 3, 3, 3, 3, 3}

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


function autodiff_dTSM_t_dot_rp(d, dd1_dXi1, Xi, dd2_dXi2, dd3_dXi3)

	@assert ndims(d) == 0
	@assert size(dd1_dXi1) == (3, 3)
	@assert size(Xi) == (3, 3)
	@assert size(dd2_dXi2) == (3, 3, 3, 3)
	@assert size(dd3_dXi3) == (3, 3, 3, 3, 3, 3)

	println("[Ausdruck mit 2 temporären Dependencies substituiert]")

	println("[Evaluating 'tmpRes_0', Komplexität 12(4, 6)]")
	@tullio tmpRes_0 := ((d + (dd1_dXi1[idx52, idx53] * Xi[idx52, idx53])) + (((0.5 * dd2_dXi2[idx60, idx61, idx62, idx63]) * Xi[idx62, idx63]) * Xi[idx60, idx61]))

	println("[Evaluating 'tmpRes_1', Komplexität 8(6, 0)]")
	@tullio tmpRes_1 := ((((0.166667 * dd3_dXi3[idx78, idx79, idx80, idx81, idx82, idx83]) * Xi[idx82, idx83]) * Xi[idx80, idx81]) * Xi[idx78, idx79])

	println("[Evaluating final Result, Komplexität 2(0, 0)]")
	res = (tmpRes_0 + tmpRes_1)

	return res

end

start_time = time()
res = autodiff_dTSM_t_dot_rp(rand(), rand(3, 3), rand(3, 3), rand(3, 3, 3, 3), rand(3, 3, 3, 3, 3, 3))
elapsed = time() - start_time
println("Laufzeit: ", elapsed, " s")
println("Ergebnis: ", res)