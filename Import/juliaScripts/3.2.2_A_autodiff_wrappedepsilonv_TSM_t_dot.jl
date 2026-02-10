# Julia Skript
#
# unique external nodes :
# | arg 'eta', order [0], dimensions {}
# | arg 'S', order [2], dimensions {3, 3}
# | arg 'E', order [2], dimensions {3, 3}
# | arg 'depsilon1_dt1', order [1], dimensions {3}

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


function autodiff_epsilonv_TSM_t_dot(eta, S, E, depsilon1_dt1)

	@assert ndims(eta) == 0
	@assert size(S) == (3, 3)
	@assert size(E) == (3, 3)
	@assert length(depsilon1_dt1) == 3

	println("[Ausdruck mit 16 temporären Dependencies substituiert]")

	println("[Evaluating 'tmpRes_0', Komplexität 0()]")
	tmpRes_0 = eta

	println("[Evaluating 'tmpRes_1', Komplexität 1(0)]")
	tmpRes_1 = (1/tmpRes_0)

	println("[Evaluating 'tmpRes_2', Komplexität 0()]")
	tmpRes_2 = eta

	println("[Evaluating 'tmpRes_3', Komplexität 1(0)]")
	tmpRes_3 = (1/tmpRes_2)

	println("[Evaluating 'tmpRes_4', Komplexität 0()]")
	tmpRes_4 = eta

	println("[Evaluating 'tmpRes_5', Komplexität 1(0)]")
	tmpRes_5 = (1/tmpRes_4)

	println("[Evaluating 'tmpRes_6', Komplexität 0()]")
	tmpRes_6 = eta

	println("[Evaluating 'tmpRes_7', Komplexität 1(0)]")
	tmpRes_7 = (1/tmpRes_6)

	println("[Evaluating 'tmpRes_8', Komplexität 18(4, 12)]")
	@tullio tmpRes_8[idx65, idx73] := (((tmpRes_3 * S[idx65, idx69]) * E[idx69, idx73]) + (0.5 * (((tmpRes_5 * S[idx65, idx77]) * E[idx77, idx81]) * ((tmpRes_7 * S[idx81, idx85]) * E[idx85, idx73]))))

	println("[Evaluating 'tmpRes_9', Komplexität 0()]")
	tmpRes_9 = eta

	println("[Evaluating 'tmpRes_10', Komplexität 1(0)]")
	tmpRes_10 = (1/tmpRes_9)

	println("[Evaluating 'tmpRes_11', Komplexität 0()]")
	tmpRes_11 = eta

	println("[Evaluating 'tmpRes_12', Komplexität 1(0)]")
	tmpRes_12 = (1/tmpRes_11)

	println("[Evaluating 'tmpRes_13', Komplexität 0()]")
	tmpRes_13 = eta

	println("[Evaluating 'tmpRes_14', Komplexität 1(0)]")
	tmpRes_14 = (1/tmpRes_13)

	println("[Evaluating 'tmpRes_15', Komplexität 16(10, 4)]")
	@tullio tmpRes_15[idx65, idx73] := ((((tmpRes_10 * S[idx65, idx93]) * E[idx93, idx97]) * ((tmpRes_12 * S[idx97, idx101]) * E[idx101, idx105])) * ((tmpRes_14 * S[idx105, idx109]) * E[idx109, idx73]))

	println("[Evaluating final Result, Komplexität 12(10, 0)]")
	@tullio res[idx60] := ((((tmpRes_1 * S[idx60, idx61]) * E[idx61, idx65]) * (tmpRes_8[idx65, idx73] + (0.166667 * tmpRes_15[idx65, idx73]))) * depsilon1_dt1[idx73])

	return res

end

start_time = time()
res = autodiff_epsilonv_TSM_t_dot(rand(), rand(3, 3), rand(3, 3), rand(3))
elapsed = time() - start_time
println("Laufzeit: ", elapsed, " s")
println("Ergebnis: ", res)