# Julia Skript
#
# unique external nodes :
# | arg 'E0', order [2], dimensions {3, 3}
# | arg 'D', order [2], dimensions {3, 3}
# | arg 'epsilon', order [1], dimensions {3}
# | arg 'epsilonv', order [1], dimensions {3}
# | arg 'depsilonv1_dD1', order [3], dimensions {3, 3, 3}
# | arg 'depsilonv2_dD2', order [5], dimensions {3, 3, 3, 3, 3}
# | arg 'depsilonv3_dD3', order [7], dimensions {3, 3, 3, 3, 3, 3, 3}
# | arg 'Identity_ord4_dm3333', order [4], dimensions {3, 3, 3, 3}
# | arg 'depsilonv4_dD4', order [9], dimensions {3, 3, 3, 3, 3, 3, 3, 3, 3}

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


function autodiff_sigmaTSM_t(E0, D, epsilon, epsilonv, depsilonv1_dD1, depsilonv2_dD2, depsilonv3_dD3, depsilonv4_dD4)

	@assert size(E0) == (3, 3)
	@assert size(D) == (3, 3)
	@assert length(epsilon) == 3
	@assert length(epsilonv) == 3
	@assert size(depsilonv1_dD1) == (3, 3, 3)
	@assert size(depsilonv2_dD2) == (3, 3, 3, 3, 3)
	@assert size(depsilonv3_dD3) == (3, 3, 3, 3, 3, 3, 3)
	Identity_ord4_dm3333 = create_Identity(3, 3, 3, 3)
	@assert size(depsilonv4_dD4) == (3, 3, 3, 3, 3, 3, 3, 3, 3)

	println("[Ausdruck mit 9 temporären Dependencies substituiert]")

	println("[Evaluating 'tmpRes_0', Komplexität 12(4, 6)]")
	@tullio tmpRes_0[idx373] := ((epsilonv[idx373] + (depsilonv1_dD1[idx373, idx386, idx387] * D[idx386, idx387])) + (((0.5 * depsilonv2_dD2[idx373, idx397, idx398, idx399, idx400]) * D[idx399, idx400]) * D[idx397, idx398]))

	println("[Evaluating 'tmpRes_1', Komplexität 8(6, 0)]")
	@tullio tmpRes_1[idx373] := ((((0.166667 * depsilonv3_dD3[idx373, idx417, idx418, idx419, idx420, idx421, idx422]) * D[idx421, idx422]) * D[idx419, idx420]) * D[idx417, idx418])

	println("[Evaluating 'tmpRes_2', Komplexität 12(4, 6)]")
	@tullio tmpRes_2[idx501] := ((epsilonv[idx501] + (depsilonv1_dD1[idx501, idx448, idx449] * D[idx448, idx449])) + (((0.5 * depsilonv2_dD2[idx501, idx459, idx460, idx461, idx462]) * D[idx461, idx462]) * D[idx459, idx460]))

	println("[Evaluating 'tmpRes_3', Komplexität 8(6, 0)]")
	@tullio tmpRes_3[idx501] := ((((0.166667 * depsilonv3_dD3[idx501, idx479, idx480, idx481, idx482, idx483, idx484]) * D[idx483, idx484]) * D[idx481, idx482]) * D[idx479, idx480])

	println("[Evaluating 'tmpRes_4', Komplexität 16(8, 6)]")
	@tullio tmpRes_4[idx507, idx499, idx500] := (((depsilonv1_dD1[idx507, idx499, idx500] + (depsilonv2_dD2[idx507, idx499, idx500, idx525, idx526] * D[idx525, idx526])) + (depsilonv1_dD1[idx507, idx540, idx541] * Identity_ord4_dm3333[idx540, idx541, idx499, idx500])) + (((0.5 * depsilonv3_dD3[idx507, idx499, idx500, idx556, idx557, idx558, idx559]) * D[idx558, idx559]) * D[idx556, idx557]))

	println("[Evaluating 'tmpRes_5', Komplexität 16(8, 6)]")
	@tullio tmpRes_5[idx507, idx499, idx500] := ((tmpRes_4[idx507, idx499, idx500] + (((0.5 * depsilonv2_dD2[idx507, idx499, idx500, idx576, idx577]) * Identity_ord4_dm3333[idx576, idx577, idx584, idx585]) * D[idx584, idx585])) + (((0.5 * depsilonv2_dD2[idx507, idx596, idx597, idx598, idx599]) * D[idx598, idx599]) * Identity_ord4_dm3333[idx596, idx597, idx499, idx500]))

	println("[Evaluating 'tmpRes_6', Komplexität 20(10, 8)]")
	@tullio tmpRes_6[idx507, idx499, idx500] := ((tmpRes_5[idx507, idx499, idx500] + ((((0.166667 * depsilonv4_dD4[idx507, idx499, idx500, idx624, idx625, idx626, idx627, idx628, idx629]) * D[idx628, idx629]) * D[idx626, idx627]) * D[idx624, idx625])) + ((((0.166667 * depsilonv3_dD3[idx507, idx499, idx500, idx652, idx653, idx654, idx655]) * Identity_ord4_dm3333[idx654, idx655, idx662, idx663]) * D[idx662, idx663]) * D[idx652, idx653]))

	println("[Evaluating 'tmpRes_7', Komplexität 20(10, 8)]")
	@tullio tmpRes_7[idx507, idx499, idx500] := ((tmpRes_6[idx507, idx499, idx500] + ((((0.166667 * depsilonv3_dD3[idx507, idx499, idx500, idx682, idx683, idx684, idx685]) * D[idx684, idx685]) * Identity_ord4_dm3333[idx682, idx683, idx696, idx697]) * D[idx696, idx697])) + ((((0.166667 * depsilonv3_dD3[idx507, idx710, idx711, idx712, idx713, idx714, idx715]) * D[idx714, idx715]) * D[idx712, idx713]) * Identity_ord4_dm3333[idx710, idx711, idx499, idx500]))

	println("[Evaluating 'tmpRes_8', Komplexität 16(14, 0)]")
	@tullio tmpRes_8[idx372] := (((Identity_ord4_dm3333[idx372, idx499, idx500, idx501] * (epsilon[idx501] - (tmpRes_2[idx501] + tmpRes_3[idx501]))) + ((E0[idx372, idx507] + D[idx372, idx507]) * (-1 * tmpRes_7[idx507, idx499, idx500]))) * D[idx499, idx500])

	println("[Evaluating final Result, Komplexität 10(8, 0)]")
	@tullio res[idx372] := (((E0[idx372, idx373] + D[idx372, idx373]) * (epsilon[idx373] - (tmpRes_0[idx373] + tmpRes_1[idx373]))) + tmpRes_8[idx372])

	return res

end

start_time = time()
res = autodiff_sigmaTSM_t(rand(3, 3), rand(3, 3), rand(3), rand(3), rand(3, 3, 3), rand(3, 3, 3, 3, 3), rand(3, 3, 3, 3, 3, 3, 3), rand(3, 3, 3, 3, 3, 3, 3, 3, 3))
elapsed = time() - start_time
println("Laufzeit: ", elapsed, " s")
println("Ergebnis: ", res)