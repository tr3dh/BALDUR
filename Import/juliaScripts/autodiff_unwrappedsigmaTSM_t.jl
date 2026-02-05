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

	println("[Ausdruck mit 13 temporären Dependencies substituiert]")

	println("[Evaluating 'tmpRes_0', Komplexität 17(0, 15)]")
	@tullio tmpRes_0[idx776] := (epsilon[idx776] - (epsilonv[idx776] + (depsilonv1_dD1[idx776, idx789, idx790] * D[idx789, idx790]) + (0.5 * depsilonv2_dD2[idx776, idx800, idx801, idx802, idx803] * D[idx802, idx803] * D[idx800, idx801]) + (0.166667 * depsilonv3_dD3[idx776, idx820, idx821, idx822, idx823, idx824, idx825] * D[idx824, idx825] * D[idx822, idx823] * D[idx820, idx821])))

	println("[Evaluating 'tmpRes_1', Komplexität 17(0, 15)]")
	@tullio tmpRes_1[idx904] := (epsilon[idx904] - (epsilonv[idx904] + (depsilonv1_dD1[idx904, idx851, idx852] * D[idx851, idx852]) + (0.5 * depsilonv2_dD2[idx904, idx862, idx863, idx864, idx865] * D[idx864, idx865] * D[idx862, idx863]) + (0.166667 * depsilonv3_dD3[idx904, idx882, idx883, idx884, idx885, idx886, idx887] * D[idx886, idx887] * D[idx884, idx885] * D[idx882, idx883])))

	println("[Evaluating 'tmpRes_2', Komplexität 0()]")
	@tullio tmpRes_2[idx910, idx902, idx903] := depsilonv1_dD1[idx910, idx902, idx903]

	println("[Evaluating 'tmpRes_3', Komplexität 2(0, 0)]")
	@tullio tmpRes_3[idx910, idx902, idx903] := (depsilonv2_dD2[idx910, idx902, idx903, idx928, idx929] * D[idx928, idx929])

	println("[Evaluating 'tmpRes_4', Komplexität 2(0, 0)]")
	@tullio tmpRes_4[idx910, idx902, idx903] := (depsilonv1_dD1[idx910, idx943, idx944] * Identity_ord4_dm3333[idx943, idx944, idx902, idx903])

	println("[Evaluating 'tmpRes_5', Komplexität 4(0, 0, 0, 0)]")
	@tullio tmpRes_5[idx910, idx902, idx903] := (0.5 * depsilonv3_dD3[idx910, idx902, idx903, idx959, idx960, idx961, idx962] * D[idx961, idx962] * D[idx959, idx960])

	println("[Evaluating 'tmpRes_6', Komplexität 4(0, 0, 0, 0)]")
	@tullio tmpRes_6[idx910, idx902, idx903] := (0.5 * depsilonv2_dD2[idx910, idx902, idx903, idx979, idx980] * Identity_ord4_dm3333[idx979, idx980, idx987, idx988] * D[idx987, idx988])

	println("[Evaluating 'tmpRes_7', Komplexität 4(0, 0, 0, 0)]")
	@tullio tmpRes_7[idx910, idx902, idx903] := (0.5 * depsilonv2_dD2[idx910, idx999, idx1000, idx1001, idx1002] * D[idx1001, idx1002] * Identity_ord4_dm3333[idx999, idx1000, idx902, idx903])

	println("[Evaluating 'tmpRes_8', Komplexität 5(0, 0, 0, 0, 0)]")
	@tullio tmpRes_8[idx910, idx902, idx903] := (0.166667 * depsilonv4_dD4[idx910, idx902, idx903, idx1027, idx1028, idx1029, idx1030, idx1031, idx1032] * D[idx1031, idx1032] * D[idx1029, idx1030] * D[idx1027, idx1028])

	println("[Evaluating 'tmpRes_9', Komplexität 5(0, 0, 0, 0, 0)]")
	@tullio tmpRes_9[idx910, idx902, idx903] := (0.166667 * depsilonv3_dD3[idx910, idx902, idx903, idx1055, idx1056, idx1057, idx1058] * Identity_ord4_dm3333[idx1057, idx1058, idx1065, idx1066] * D[idx1065, idx1066] * D[idx1055, idx1056])

	println("[Evaluating 'tmpRes_10', Komplexität 5(0, 0, 0, 0, 0)]")
	@tullio tmpRes_10[idx910, idx902, idx903] := (0.166667 * depsilonv3_dD3[idx910, idx902, idx903, idx1085, idx1086, idx1087, idx1088] * D[idx1087, idx1088] * Identity_ord4_dm3333[idx1085, idx1086, idx1099, idx1100] * D[idx1099, idx1100])

	println("[Evaluating 'tmpRes_11', Komplexität 5(0, 0, 0, 0, 0)]")
	@tullio tmpRes_11[idx910, idx902, idx903] := (0.166667 * depsilonv3_dD3[idx910, idx1113, idx1114, idx1115, idx1116, idx1117, idx1118] * D[idx1117, idx1118] * D[idx1115, idx1116] * Identity_ord4_dm3333[idx1113, idx1114, idx902, idx903])

	println("[Evaluating 'tmpRes_12', Komplexität 19(2, 15)]")
	@tullio tmpRes_12[idx775, idx902, idx903] := ((Identity_ord4_dm3333[idx775, idx902, idx903, idx904] * tmpRes_1[idx904]) + ((E0[idx775, idx910] + D[idx775, idx910]) * -1 * (tmpRes_2[idx910, idx902, idx903] + tmpRes_3[idx910, idx902, idx903] + tmpRes_4[idx910, idx902, idx903] + tmpRes_5[idx910, idx902, idx903] + tmpRes_6[idx910, idx902, idx903] + tmpRes_7[idx910, idx902, idx903] + tmpRes_8[idx910, idx902, idx903] + tmpRes_9[idx910, idx902, idx903] + tmpRes_10[idx910, idx902, idx903] + tmpRes_11[idx910, idx902, idx903])))

	println("[Evaluating final Result, Komplexität 8(4, 2)]")
	@tullio res[idx775] := (((E0[idx775, idx776] + D[idx775, idx776]) * tmpRes_0[idx776]) + (tmpRes_12[idx775, idx902, idx903] * D[idx902, idx903]))

	return res

end

start_time = time()
res = autodiff_sigmaTSM_t(rand(3, 3), rand(3, 3), rand(3), rand(3), rand(3, 3, 3), rand(3, 3, 3, 3, 3), rand(3, 3, 3, 3, 3, 3, 3), rand(3, 3, 3, 3, 3, 3, 3, 3, 3))
elapsed = time() - start_time
println("Laufzeit: ", elapsed, " s")
println("Ergebnis: ", res)