# Julia Skript
#
# unique external nodes :
# | arg 'E0', order [2], dimensions {3, 3}
# | arg 'D', order [2], dimensions {3, 3}
# | arg 'epsilon', order [1], dimensions {3}
# | arg 'epsilonv', order [1], dimensions {3}
# | arg 'depsilonv1_dD1', order [3], dimensions {3, 3, 3}
# | arg 'depsilonv2_dD2', order [5], dimensions {3, 3, 3, 3, 3}
# | arg 'Identity_ord4_dm3333', order [4], dimensions {3, 3, 3, 3}
# | arg 'depsilonv3_dD3', order [7], dimensions {3, 3, 3, 3, 3, 3, 3}
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
	Identity_ord4_dm3333 = create_Identity(3, 3, 3, 3)
	@assert size(depsilonv3_dD3) == (3, 3, 3, 3, 3, 3, 3)
	@assert size(depsilonv4_dD4) == (3, 3, 3, 3, 3, 3, 3, 3, 3)

	println("[Ausdruck mit 15 temporären Dependencies substituiert]")

	println("[Evaluating 'tmpRes_0', Komplexität 22(0, 2, 2, 4, 4, 4)]")
	@tullio tmpRes_0[idx708, idx700, idx701] := (depsilonv1_dD1[idx708, idx700, idx701]+(depsilonv2_dD2[idx708, idx700, idx701, idx726, idx727]*D[idx726, idx727])+(depsilonv1_dD1[idx708, idx741, idx742]*Identity_ord4_dm3333[idx741, idx742, idx700, idx701])+(0.5*depsilonv3_dD3[idx708, idx700, idx701, idx757, idx758, idx759, idx760]*D[idx759, idx760]*D[idx757, idx758])+(0.5*depsilonv2_dD2[idx708, idx700, idx701, idx777, idx778]*Identity_ord4_dm3333[idx777, idx778, idx785, idx786]*D[idx785, idx786])+(0.5*depsilonv2_dD2[idx708, idx797, idx798, idx799, idx800]*D[idx799, idx800]*Identity_ord4_dm3333[idx797, idx798, idx700, idx701]))

	println("[Evaluating 'tmpRes_1', Komplexität 22(20, 0)]")
	@tullio tmpRes_1[idx625] := (((Identity_ord4_dm3333[idx625, idx700, idx701, idx702]*(epsilon[idx702]-(epsilonv[idx702]+(depsilonv1_dD1[idx702, idx675, idx676]*D[idx675, idx676])+(0.5*depsilonv2_dD2[idx702, idx686, idx687, idx688, idx689]*D[idx688, idx689]*D[idx686, idx687]))))+((E0[idx625, idx708]+D[idx625, idx708])*-1*tmpRes_0[idx708, idx700, idx701]))*D[idx700, idx701])

	println("[Evaluating 'tmpRes_2', Komplexität 22(0, 2, 2, 4, 4, 4)]")
	@tullio tmpRes_2[idx924, idx839, idx840] := (depsilonv1_dD1[idx924, idx839, idx840]+(depsilonv2_dD2[idx924, idx839, idx840, idx834, idx835]*D[idx834, idx835])+(depsilonv1_dD1[idx924, idx849, idx850]*Identity_ord4_dm3333[idx849, idx850, idx839, idx840])+(0.5*depsilonv3_dD3[idx924, idx839, idx840, idx865, idx866, idx867, idx868]*D[idx867, idx868]*D[idx865, idx866])+(0.5*depsilonv2_dD2[idx924, idx839, idx840, idx885, idx886]*Identity_ord4_dm3333[idx885, idx886, idx893, idx894]*D[idx893, idx894])+(0.5*depsilonv2_dD2[idx924, idx905, idx906, idx907, idx908]*D[idx907, idx908]*Identity_ord4_dm3333[idx905, idx906, idx839, idx840]))

	println("[Evaluating 'tmpRes_3', Komplexität 2(0, 0)]")
	@tullio tmpRes_3[idx930, idx922, idx923, idx839, idx840] := (depsilonv3_dD3[idx930, idx922, idx923, idx839, idx840, idx954, idx955]*D[idx954, idx955])

	println("[Evaluating 'tmpRes_4', Komplexität 2(0, 0)]")
	@tullio tmpRes_4[idx930, idx922, idx923, idx839, idx840] := (depsilonv2_dD2[idx930, idx922, idx923, idx975, idx976]*Identity_ord4_dm3333[idx975, idx976, idx839, idx840])

	println("[Evaluating 'tmpRes_5', Komplexität 2(0, 0)]")
	@tullio tmpRes_5[idx930, idx922, idx923, idx839, idx840] := (depsilonv2_dD2[idx930, idx922, idx923, idx993, idx994]*Identity_ord4_dm3333[idx993, idx994, idx839, idx840])

	println("[Evaluating 'tmpRes_6', Komplexität 4(0, 0, 0, 0)]")
	@tullio tmpRes_6[idx930, idx922, idx923, idx839, idx840] := (0.5*depsilonv4_dD4[idx930, idx922, idx923, idx839, idx840, idx1013, idx1014, idx1015, idx1016]*D[idx1015, idx1016]*D[idx1013, idx1014])

	println("[Evaluating 'tmpRes_7', Komplexität 4(0, 0, 0, 0)]")
	@tullio tmpRes_7[idx930, idx922, idx923, idx839, idx840] := (0.5*depsilonv3_dD3[idx930, idx922, idx923, idx839, idx840, idx1037, idx1038]*Identity_ord4_dm3333[idx1037, idx1038, idx1045, idx1046]*D[idx1045, idx1046])

	println("[Evaluating 'tmpRes_8', Komplexität 4(0, 0, 0, 0)]")
	@tullio tmpRes_8[idx930, idx922, idx923, idx839, idx840] := (0.5*depsilonv3_dD3[idx930, idx922, idx923, idx1061, idx1062, idx1063, idx1064]*D[idx1063, idx1064]*Identity_ord4_dm3333[idx1061, idx1062, idx839, idx840])

	println("[Evaluating 'tmpRes_9', Komplexität 4(0, 0, 0, 0)]")
	@tullio tmpRes_9[idx930, idx922, idx923, idx839, idx840] := (0.5*depsilonv3_dD3[idx930, idx922, idx923, idx839, idx840, idx1089, idx1090]*Identity_ord4_dm3333[idx1089, idx1090, idx1097, idx1098]*D[idx1097, idx1098])

	println("[Evaluating 'tmpRes_10', Komplexität 4(0, 0, 0, 0)]")
	@tullio tmpRes_10[idx930, idx922, idx923, idx839, idx840] := (0.5*depsilonv2_dD2[idx930, idx922, idx923, idx1111, idx1112]*Identity_ord4_dm3333[idx1111, idx1112, idx1119, idx1120]*Identity_ord4_dm3333[idx1119, idx1120, idx839, idx840])

	println("[Evaluating 'tmpRes_11', Komplexität 4(0, 0, 0, 0)]")
	@tullio tmpRes_11[idx930, idx922, idx923, idx839, idx840] := (0.5*depsilonv3_dD3[idx930, idx922, idx923, idx1139, idx1140, idx1141, idx1142]*D[idx1141, idx1142]*Identity_ord4_dm3333[idx1139, idx1140, idx839, idx840])

	println("[Evaluating 'tmpRes_12', Komplexität 4(0, 0, 0, 0)]")
	@tullio tmpRes_12[idx930, idx922, idx923, idx839, idx840] := (0.5*depsilonv2_dD2[idx930, idx922, idx923, idx1163, idx1164]*Identity_ord4_dm3333[idx1163, idx1164, idx1171, idx1172]*Identity_ord4_dm3333[idx1171, idx1172, idx839, idx840])

	println("[Evaluating 'tmpRes_13', Komplexität 22(4, 16)]")
	@tullio tmpRes_13[idx625, idx922, idx923, idx839, idx840] := ((2*Identity_ord4_dm3333[idx625, idx922, idx923, idx924]*-1*tmpRes_2[idx924, idx839, idx840])+((E0[idx625, idx930]+D[idx625, idx930])*-1*(depsilonv2_dD2[idx930, idx922, idx923, idx839, idx840]+tmpRes_3[idx930, idx922, idx923, idx839, idx840]+tmpRes_4[idx930, idx922, idx923, idx839, idx840]+tmpRes_5[idx930, idx922, idx923, idx839, idx840]+tmpRes_6[idx930, idx922, idx923, idx839, idx840]+tmpRes_7[idx930, idx922, idx923, idx839, idx840]+tmpRes_8[idx930, idx922, idx923, idx839, idx840]+tmpRes_9[idx930, idx922, idx923, idx839, idx840]+tmpRes_10[idx930, idx922, idx923, idx839, idx840]+tmpRes_11[idx930, idx922, idx923, idx839, idx840]+tmpRes_12[idx930, idx922, idx923, idx839, idx840])))

	println("[Evaluating 'tmpRes_14', Komplexität 22(15, 0, 4)]")
	@tullio tmpRes_14[idx625] := (((E0[idx625, idx626]+D[idx625, idx626])*(epsilon[idx626]-(epsilonv[idx626]+(depsilonv1_dD1[idx626, idx639, idx640]*D[idx639, idx640])+(0.5*depsilonv2_dD2[idx626, idx650, idx651, idx652, idx653]*D[idx652, idx653]*D[idx650, idx651]))))+tmpRes_1[idx625]+(0.5*tmpRes_13[idx625, idx922, idx923, idx839, idx840]*D[idx839, idx840]*D[idx922, idx923]))

	println("[Evaluating final Result, Komplexität 0()]")
	@tullio res[idx625] := tmpRes_14[idx625]

	return res

end

start_time = time()
res = autodiff_sigmaTSM_t(rand(3, 3), rand(3, 3), rand(3), rand(3), rand(3, 3, 3), rand(3, 3, 3, 3, 3), rand(3, 3, 3, 3, 3, 3, 3), rand(3, 3, 3, 3, 3, 3, 3, 3, 3))
elapsed = time() - start_time
println("Laufzeit: ", elapsed, " s")
println("Ergebnis: ", res)