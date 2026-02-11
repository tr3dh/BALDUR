# Julia Skript
#
# unique external nodes :
# | arg 'eta', order [0], dimensions {}
# | arg 'S', order [2], dimensions {3, 3}
# | arg 'E0', order [2], dimensions {3, 3}
# | arg 'Xi', order [2], dimensions {3, 3}
# | arg 'epsilon', order [1], dimensions {3}
# | arg 'Identity_ord4_dm3333', order [4], dimensions {3, 3, 3, 3}

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


function autodiff_epsilonv_TSM_t(eta, S, E0, Xi, epsilon)

	@assert ndims(eta) == 0
	@assert size(S) == (3, 3)
	@assert size(E0) == (3, 3)
	@assert size(Xi) == (3, 3)
	@assert length(epsilon) == 3
	Identity_ord4_dm3333 = create_Identity(3, 3, 3, 3)

	println("[Ausdruck mit 15 temporären Dependencies substituiert]")

	println("[Evaluating 'tmpRes_0', Komplexität 0()]")
	tmpRes_0 = eta

	println("[Evaluating 'tmpRes_1', Komplexität 1(0)]")
	tmpRes_1 = (1/tmpRes_0)

	println("[Evaluating 'tmpRes_2', Komplexität 0()]")
	tmpRes_2 = eta

	println("[Evaluating 'tmpRes_3', Komplexität 1(0)]")
	tmpRes_3 = (1/tmpRes_2)

	println("[Evaluating 'tmpRes_4', Komplexität 16(14, 0)]")
	@tullio tmpRes_4[idx92] := ((((tmpRes_1 * S[idx92, idx93]) * (E0[idx93, idx99] + Xi[idx93, idx99])) * ((tmpRes_3 * S[idx99, idx107]) * (E0[idx107, idx113] + Xi[idx107, idx113]))) * epsilon[idx113])

	println("[Evaluating 'tmpRes_5', Komplexität 0()]")
	tmpRes_5 = eta

	println("[Evaluating 'tmpRes_6', Komplexität 1(0)]")
	tmpRes_6 = (1/tmpRes_5)

	println("[Evaluating 'tmpRes_7', Komplexität 0()]")
	tmpRes_7 = eta

	println("[Evaluating 'tmpRes_8', Komplexität 1(0)]")
	tmpRes_8 = (1/tmpRes_7)

	println("[Evaluating 'tmpRes_9', Komplexität 0()]")
	tmpRes_9 = eta

	println("[Evaluating 'tmpRes_10', Komplexität 1(0)]")
	tmpRes_10 = (1/tmpRes_9)

	println("[Evaluating 'tmpRes_11', Komplexität 0()]")
	tmpRes_11 = eta

	println("[Evaluating 'tmpRes_12', Komplexität 1(0)]")
	tmpRes_12 = (1/tmpRes_11)

	println("[Evaluating 'tmpRes_13', Komplexität 14(12, 0)]")
	@tullio tmpRes_13[idx92, idx129, idx130] := ((((tmpRes_6 * S[idx92, idx123]) * Identity_ord4_dm3333[idx123, idx129, idx130, idx131]) * ((tmpRes_8 * S[idx131, idx135]) * (E0[idx135, idx141] + Xi[idx135, idx141]))) * epsilon[idx141])

	println("[Evaluating 'tmpRes_14', Komplexität 14(12, 0)]")
	@tullio tmpRes_14[idx92, idx129, idx130] := ((((tmpRes_10 * S[idx92, idx151]) * (E0[idx151, idx157] + Xi[idx151, idx157])) * ((tmpRes_12 * S[idx157, idx165]) * Identity_ord4_dm3333[idx165, idx129, idx130, idx173])) * epsilon[idx173])

	println("[Evaluating final Result, Komplexität 6(0, 4)]")
	@tullio res[idx92] := (tmpRes_4[idx92] + ((tmpRes_13[idx92, idx129, idx130] + tmpRes_14[idx92, idx129, idx130]) * Xi[idx129, idx130]))

	return res

end

start_time = time()
res = autodiff_epsilonv_TSM_t(rand(), rand(3, 3), rand(3, 3), rand(3, 3), rand(3))
elapsed = time() - start_time
println("Laufzeit: ", elapsed, " s")
println("Ergebnis: ", res)