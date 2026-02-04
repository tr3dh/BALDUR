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

	println("[Ausdruck mit 8 temporären Dependencies substituiert]")

	println("[Evaluating 'tmpRes_0', Komplexität 24(16, 6)]")
	@tullio tmpRes_0[idx87, idx79, idx80] := ((((depsilonv1_dD1[idx87, idx79, idx80]+(depsilonv2_dD2[idx87, idx79, idx80, idx105, idx106]*D[idx105, idx106]))+(depsilonv1_dD1[idx87, idx120, idx121]*Identity_ord4_dm3333[idx120, idx121, idx79, idx80]))+(((0.5*depsilonv3_dD3[idx87, idx79, idx80, idx136, idx137, idx138, idx139])*D[idx138, idx139])*D[idx136, idx137]))+(((0.5*depsilonv2_dD2[idx87, idx79, idx80, idx156, idx157])*Identity_ord4_dm3333[idx156, idx157, idx164, idx165])*D[idx164, idx165]))

	println("[Evaluating 'tmpRes_1', Komplexität 32(16, 14)]")
	@tullio tmpRes_1[idx4, idx79, idx80] := ((Identity_ord4_dm3333[idx4, idx79, idx80, idx81]*(epsilon[idx81]-((epsilonv[idx81]+(depsilonv1_dD1[idx81, idx54, idx55]*D[idx54, idx55]))+(((0.5*depsilonv2_dD2[idx81, idx65, idx66, idx67, idx68])*D[idx67, idx68])*D[idx65, idx66]))))+((E0[idx4, idx87]+D[idx4, idx87])*(-1*(tmpRes_0[idx87, idx79, idx80]+(((0.5*depsilonv2_dD2[idx87, idx176, idx177, idx178, idx179])*D[idx178, idx179])*Identity_ord4_dm3333[idx176, idx177, idx79, idx80])))))

	println("[Evaluating 'tmpRes_2', Komplexität 22(18, 2)]")
	@tullio tmpRes_2[idx4] := (((E0[idx4, idx5]+D[idx4, idx5])*(epsilon[idx5]-((epsilonv[idx5]+(depsilonv1_dD1[idx5, idx18, idx19]*D[idx18, idx19]))+(((0.5*depsilonv2_dD2[idx5, idx29, idx30, idx31, idx32])*D[idx31, idx32])*D[idx29, idx30]))))+(tmpRes_1[idx4, idx79, idx80]*D[idx79, idx80]))

	println("[Evaluating 'tmpRes_3', Komplexität 24(16, 6)]")
	@tullio tmpRes_3[idx303, idx218, idx219] := ((((depsilonv1_dD1[idx303, idx218, idx219]+(depsilonv2_dD2[idx303, idx218, idx219, idx213, idx214]*D[idx213, idx214]))+(depsilonv1_dD1[idx303, idx228, idx229]*Identity_ord4_dm3333[idx228, idx229, idx218, idx219]))+(((0.5*depsilonv3_dD3[idx303, idx218, idx219, idx244, idx245, idx246, idx247])*D[idx246, idx247])*D[idx244, idx245]))+(((0.5*depsilonv2_dD2[idx303, idx218, idx219, idx264, idx265])*Identity_ord4_dm3333[idx264, idx265, idx272, idx273])*D[idx272, idx273]))

	println("[Evaluating 'tmpRes_4', Komplexität 28(20, 6)]")
	@tullio tmpRes_4[idx309, idx301, idx302, idx218, idx219] := (((((depsilonv2_dD2[idx309, idx301, idx302, idx218, idx219]+(depsilonv3_dD3[idx309, idx301, idx302, idx218, idx219, idx333, idx334]*D[idx333, idx334]))+(depsilonv2_dD2[idx309, idx301, idx302, idx354, idx355]*Identity_ord4_dm3333[idx354, idx355, idx218, idx219]))+(depsilonv2_dD2[idx309, idx301, idx302, idx372, idx373]*Identity_ord4_dm3333[idx372, idx373, idx218, idx219]))+(((0.5*depsilonv4_dD4[idx309, idx301, idx302, idx218, idx219, idx392, idx393, idx394, idx395])*D[idx394, idx395])*D[idx392, idx393]))+(((0.5*depsilonv3_dD3[idx309, idx301, idx302, idx218, idx219, idx416, idx417])*Identity_ord4_dm3333[idx416, idx417, idx424, idx425])*D[idx424, idx425]))

	println("[Evaluating 'tmpRes_5', Komplexität 24(16, 6)]")
	@tullio tmpRes_5[idx309, idx301, idx302, idx218, idx219] := (((tmpRes_4[idx309, idx301, idx302, idx218, idx219]+(((0.5*depsilonv3_dD3[idx309, idx301, idx302, idx440, idx441, idx442, idx443])*D[idx442, idx443])*Identity_ord4_dm3333[idx440, idx441, idx218, idx219]))+(((0.5*depsilonv3_dD3[idx309, idx301, idx302, idx218, idx219, idx468, idx469])*Identity_ord4_dm3333[idx468, idx469, idx476, idx477])*D[idx476, idx477]))+(((0.5*depsilonv2_dD2[idx309, idx301, idx302, idx490, idx491])*Identity_ord4_dm3333[idx490, idx491, idx498, idx499])*Identity_ord4_dm3333[idx498, idx499, idx218, idx219]))

	println("[Evaluating 'tmpRes_6', Komplexität 22(2, 18)]")
	@tullio tmpRes_6[idx4, idx301, idx302, idx218, idx219] := ((E0[idx4, idx309]+D[idx4, idx309])*(-1*((tmpRes_5[idx309, idx301, idx302, idx218, idx219]+(((0.5*depsilonv3_dD3[idx309, idx301, idx302, idx518, idx519, idx520, idx521])*D[idx520, idx521])*Identity_ord4_dm3333[idx518, idx519, idx218, idx219]))+(((0.5*depsilonv2_dD2[idx309, idx301, idx302, idx542, idx543])*Identity_ord4_dm3333[idx542, idx543, idx550, idx551])*Identity_ord4_dm3333[idx550, idx551, idx218, idx219]))))

	println("[Evaluating 'tmpRes_7', Komplexität 22(20, 0)]")
	@tullio tmpRes_7[idx4] := (((0.5*((2*(Identity_ord4_dm3333[idx4, idx301, idx302, idx303]*(-1*(tmpRes_3[idx303, idx218, idx219]+(((0.5*depsilonv2_dD2[idx303, idx284, idx285, idx286, idx287])*D[idx286, idx287])*Identity_ord4_dm3333[idx284, idx285, idx218, idx219])))))+tmpRes_6[idx4, idx301, idx302, idx218, idx219]))*D[idx218, idx219])*D[idx301, idx302])

	println("[Evaluating final Result, Komplexität 2(0, 0)]")
	@tullio res[idx4] := (tmpRes_2[idx4]+tmpRes_7[idx4])

	return res

end

start_time = time()
res = autodiff_sigmaTSM_t(rand(3, 3), rand(3, 3), rand(3), rand(3), rand(3, 3, 3), rand(3, 3, 3, 3, 3), rand(3, 3, 3, 3, 3, 3, 3), rand(3, 3, 3, 3, 3, 3, 3, 3, 3))
elapsed = time() - start_time
println("Laufzeit: ", elapsed, " s")
println("Ergebnis: ", res)