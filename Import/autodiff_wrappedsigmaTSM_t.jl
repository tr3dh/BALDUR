# Julia Skript
#
# unique external nodes :
# | arg '__INVALID__', order [0], dimensions {}
# | arg 'E0', order [2], dimensions {3, 3}
# | arg 'D', order [2], dimensions {3, 3}
# | arg 'epsilon', order [1], dimensions {3}
# | arg 'epsilonv', order [1], dimensions {3}
# | arg 'depsilonv1_dD1', order [3], dimensions {3, 3, 3}
# | arg 'depsilonv2_dD2', order [5], dimensions {3, 3, 3, 3, 3}
# | arg 'depsilonv3_dD3', order [7], dimensions {3, 3, 3, 3, 3, 3, 3}
# | arg 'zeros_ord4_dm3333', order [4], dimensions {3, 3, 3, 3}
# | arg 'Identity_ord4_dm3333', order [4], dimensions {3, 3, 3, 3}
# | arg 'zeros_ord3_dm333', order [3], dimensions {3, 3, 3}
# | arg 'depsilonv4_dD4', order [9], dimensions {3, 3, 3, 3, 3, 3, 3, 3, 3}
# | arg 'zeros_ord6_dm333333', order [6], dimensions {3, 3, 3, 3, 3, 3}
# | arg 'zeros_ord5_dm33333', order [5], dimensions {3, 3, 3, 3, 3}
# | arg 'depsilonv5_dD5', order [11], dimensions {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3}
# | arg 'zeros_ord8_dm33333333', order [8], dimensions {3, 3, 3, 3, 3, 3, 3, 3}
# | arg 'zeros_ord7_dm3333333', order [7], dimensions {3, 3, 3, 3, 3, 3, 3}
# | arg 'depsilonv6_dD6', order [13], dimensions {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3}

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


function autodiff_sigmaTSM_t(__INVALID__, E0, D, epsilon, epsilonv, depsilonv1_dD1, depsilonv2_dD2, depsilonv3_dD3, depsilonv4_dD4, depsilonv5_dD5, depsilonv6_dD6)

	@assert ndims(__INVALID__) == 0
	@assert size(E0) == (3, 3)
	@assert size(D) == (3, 3)
	@assert length(epsilon) == 3
	@assert length(epsilonv) == 3
	@assert size(depsilonv1_dD1) == (3, 3, 3)
	@assert size(depsilonv2_dD2) == (3, 3, 3, 3, 3)
	@assert size(depsilonv3_dD3) == (3, 3, 3, 3, 3, 3, 3)
	zeros_ord4_dm3333 = create_zeros(3, 3, 3, 3)
	Identity_ord4_dm3333 = create_Identity(3, 3, 3, 3)
	zeros_ord3_dm333 = create_zeros(3, 3, 3)
	@assert size(depsilonv4_dD4) == (3, 3, 3, 3, 3, 3, 3, 3, 3)
	zeros_ord6_dm333333 = create_zeros(3, 3, 3, 3, 3, 3)
	zeros_ord5_dm33333 = create_zeros(3, 3, 3, 3, 3)
	@assert size(depsilonv5_dD5) == (3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3)
	zeros_ord8_dm33333333 = create_zeros(3, 3, 3, 3, 3, 3, 3, 3)
	zeros_ord7_dm3333333 = create_zeros(3, 3, 3, 3, 3, 3, 3)
	@assert size(depsilonv6_dD6) == (3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3)

	println("[Ausdruck mit 153 temporären Dependencies substituiert]")

	println("[Evaluating 'tmpRes_0', Komplexität 16(8, 6)]")
	@tullio tmpRes_0[idx8863] := (((1*epsilonv[idx8863])+((1*depsilonv1_dD1[idx8863, idx8875, idx8876])*D[idx8875, idx8876]))+(((0.5*depsilonv2_dD2[idx8863, idx8887, idx8888, idx8889, idx8890])*D[idx8889, idx8890])*D[idx8887, idx8888]))

	println("[Evaluating 'tmpRes_1', Komplexität 16(2, 12)]")
	@tullio tmpRes_1[idx8862] := ((E0[idx8862, idx8863]+D[idx8862, idx8863])*(epsilon[idx8863]-(tmpRes_0[idx8863]+((((0.166667*depsilonv3_dD3[idx8863, idx8907, idx8908, idx8909, idx8910, idx8911, idx8912])*D[idx8911, idx8912])*D[idx8909, idx8910])*D[idx8907, idx8908]))))

	println("[Evaluating 'tmpRes_2', Komplexität 16(8, 6)]")
	@tullio tmpRes_2[idx8937] := (((1*epsilonv[idx8937])+((1*depsilonv1_dD1[idx8937, idx8953, idx8954])*D[idx8953, idx8954]))+(((0.5*depsilonv2_dD2[idx8937, idx8965, idx8966, idx8967, idx8968])*D[idx8967, idx8968])*D[idx8965, idx8966]))

	println("[Evaluating 'tmpRes_3', Komplexität 16(2, 12)]")
	@tullio tmpRes_3[idx8862, idx8935, idx8936] := ((zeros_ord4_dm3333[idx8862, idx8935, idx8936, idx8937]+Identity_ord4_dm3333[idx8862, idx8935, idx8936, idx8937])*(epsilon[idx8937]-(tmpRes_2[idx8937]+((((0.166667*depsilonv3_dD3[idx8937, idx8985, idx8986, idx8987, idx8988, idx8989, idx8990])*D[idx8989, idx8990])*D[idx8987, idx8988])*D[idx8985, idx8986]))))

	println("[Evaluating 'tmpRes_4', Komplexität 14(8, 4)]")
	@tullio tmpRes_4[idx9009, idx8935, idx8936] := (((1*depsilonv1_dD1[idx9009, idx8935, idx8936])+((1*depsilonv2_dD2[idx9009, idx8935, idx8936, idx9031, idx9032])*D[idx9031, idx9032]))+((1*depsilonv1_dD1[idx9009, idx9041, idx9042])*Identity_ord4_dm3333[idx9041, idx9042, idx8935, idx8936]))

	println("[Evaluating 'tmpRes_5', Komplexität 6(4, 0)]")
	@tullio tmpRes_5[idx9009, idx8935, idx8936] := (((0.5*depsilonv3_dD3[idx9009, idx8935, idx8936, idx9061, idx9062, idx9063, idx9064])*D[idx9063, idx9064])*D[idx9061, idx9062])

	println("[Evaluating 'tmpRes_6', Komplexität 18(10, 6)]")
	@tullio tmpRes_6[idx9009, idx8935, idx8936] := (((tmpRes_4[idx9009, idx8935, idx8936]+tmpRes_5[idx9009, idx8935, idx8936])+(((0.5*depsilonv2_dD2[idx9009, idx8935, idx8936, idx9081, idx9082])*Identity_ord4_dm3333[idx9081, idx9082, idx9089, idx9090])*D[idx9089, idx9090]))+(((0.5*depsilonv2_dD2[idx9009, idx9101, idx9102, idx9103, idx9104])*D[idx9103, idx9104])*Identity_ord4_dm3333[idx9101, idx9102, idx8935, idx8936]))

	println("[Evaluating 'tmpRes_7', Komplexität 20(10, 8)]")
	@tullio tmpRes_7[idx9009, idx8935, idx8936] := ((tmpRes_6[idx9009, idx8935, idx8936]+((((0.166667*depsilonv4_dD4[idx9009, idx8935, idx8936, idx9129, idx9130, idx9131, idx9132, idx9133, idx9134])*D[idx9133, idx9134])*D[idx9131, idx9132])*D[idx9129, idx9130]))+((((0.166667*depsilonv3_dD3[idx9009, idx8935, idx8936, idx9157, idx9158, idx9159, idx9160])*Identity_ord4_dm3333[idx9159, idx9160, idx9167, idx9168])*D[idx9167, idx9168])*D[idx9157, idx9158]))

	println("[Evaluating 'tmpRes_8', Komplexität 20(10, 8)]")
	@tullio tmpRes_8[idx9009, idx8935, idx8936] := ((tmpRes_7[idx9009, idx8935, idx8936]+((((0.166667*depsilonv3_dD3[idx9009, idx8935, idx8936, idx9187, idx9188, idx9189, idx9190])*D[idx9189, idx9190])*Identity_ord4_dm3333[idx9187, idx9188, idx9201, idx9202])*D[idx9201, idx9202]))+((((0.166667*depsilonv3_dD3[idx9009, idx9215, idx9216, idx9217, idx9218, idx9219, idx9220])*D[idx9219, idx9220])*D[idx9217, idx9218])*Identity_ord4_dm3333[idx9215, idx9216, idx8935, idx8936]))

	println("[Evaluating 'tmpRes_9', Komplexität 16(2, 12)]")
	@tullio tmpRes_9[idx8862] := ((1*tmpRes_1[idx8862])+((1*(tmpRes_3[idx8862, idx8935, idx8936]+((E0[idx8862, idx9009]+D[idx8862, idx9009])*(zeros_ord3_dm333[idx9009, idx8935, idx8936]-tmpRes_8[idx9009, idx8935, idx8936]))))*D[idx8935, idx8936]))

	println("[Evaluating 'tmpRes_10', Komplexität 16(8, 6)]")
	@tullio tmpRes_10[idx9261] := (((1*epsilonv[idx9261])+((1*depsilonv1_dD1[idx9261, idx9281, idx9282])*D[idx9281, idx9282]))+(((0.5*depsilonv2_dD2[idx9261, idx9293, idx9294, idx9295, idx9296])*D[idx9295, idx9296])*D[idx9293, idx9294]))

	println("[Evaluating 'tmpRes_11', Komplexität 16(2, 12)]")
	@tullio tmpRes_11[idx8862, idx9257, idx9258, idx9259, idx9260] := ((zeros_ord6_dm333333[idx8862, idx9257, idx9258, idx9259, idx9260, idx9261]+zeros_ord6_dm333333[idx8862, idx9257, idx9258, idx9259, idx9260, idx9261])*(epsilon[idx9261]-(tmpRes_10[idx9261]+((((0.166667*depsilonv3_dD3[idx9261, idx9313, idx9314, idx9315, idx9316, idx9317, idx9318])*D[idx9317, idx9318])*D[idx9315, idx9316])*D[idx9313, idx9314]))))

	println("[Evaluating 'tmpRes_12', Komplexität 14(8, 4)]")
	@tullio tmpRes_12[idx9343, idx9259, idx9260] := (((1*depsilonv1_dD1[idx9343, idx9259, idx9260])+((1*depsilonv2_dD2[idx9343, idx9259, idx9260, idx9369, idx9370])*D[idx9369, idx9370]))+((1*depsilonv1_dD1[idx9343, idx9379, idx9380])*Identity_ord4_dm3333[idx9379, idx9380, idx9259, idx9260]))

	println("[Evaluating 'tmpRes_13', Komplexität 6(4, 0)]")
	@tullio tmpRes_13[idx9343, idx9259, idx9260] := (((0.5*depsilonv3_dD3[idx9343, idx9259, idx9260, idx9399, idx9400, idx9401, idx9402])*D[idx9401, idx9402])*D[idx9399, idx9400])

	println("[Evaluating 'tmpRes_14', Komplexität 18(10, 6)]")
	@tullio tmpRes_14[idx9343, idx9259, idx9260] := (((tmpRes_12[idx9343, idx9259, idx9260]+tmpRes_13[idx9343, idx9259, idx9260])+(((0.5*depsilonv2_dD2[idx9343, idx9259, idx9260, idx9419, idx9420])*Identity_ord4_dm3333[idx9419, idx9420, idx9427, idx9428])*D[idx9427, idx9428]))+(((0.5*depsilonv2_dD2[idx9343, idx9439, idx9440, idx9441, idx9442])*D[idx9441, idx9442])*Identity_ord4_dm3333[idx9439, idx9440, idx9259, idx9260]))

	println("[Evaluating 'tmpRes_15', Komplexität 20(10, 8)]")
	@tullio tmpRes_15[idx9343, idx9259, idx9260] := ((tmpRes_14[idx9343, idx9259, idx9260]+((((0.166667*depsilonv4_dD4[idx9343, idx9259, idx9260, idx9467, idx9468, idx9469, idx9470, idx9471, idx9472])*D[idx9471, idx9472])*D[idx9469, idx9470])*D[idx9467, idx9468]))+((((0.166667*depsilonv3_dD3[idx9343, idx9259, idx9260, idx9495, idx9496, idx9497, idx9498])*Identity_ord4_dm3333[idx9497, idx9498, idx9505, idx9506])*D[idx9505, idx9506])*D[idx9495, idx9496]))

	println("[Evaluating 'tmpRes_16', Komplexität 20(10, 8)]")
	@tullio tmpRes_16[idx9343, idx9259, idx9260] := ((tmpRes_15[idx9343, idx9259, idx9260]+((((0.166667*depsilonv3_dD3[idx9343, idx9259, idx9260, idx9525, idx9526, idx9527, idx9528])*D[idx9527, idx9528])*Identity_ord4_dm3333[idx9525, idx9526, idx9539, idx9540])*D[idx9539, idx9540]))+((((0.166667*depsilonv3_dD3[idx9343, idx9553, idx9554, idx9555, idx9556, idx9557, idx9558])*D[idx9557, idx9558])*D[idx9555, idx9556])*Identity_ord4_dm3333[idx9553, idx9554, idx9259, idx9260]))

	println("[Evaluating 'tmpRes_17', Komplexität 14(8, 4)]")
	@tullio tmpRes_17[idx9589, idx9259, idx9260] := (((1*depsilonv1_dD1[idx9589, idx9259, idx9260])+((1*depsilonv2_dD2[idx9589, idx9259, idx9260, idx9615, idx9616])*D[idx9615, idx9616]))+((1*depsilonv1_dD1[idx9589, idx9625, idx9626])*Identity_ord4_dm3333[idx9625, idx9626, idx9259, idx9260]))

	println("[Evaluating 'tmpRes_18', Komplexität 6(4, 0)]")
	@tullio tmpRes_18[idx9589, idx9259, idx9260] := (((0.5*depsilonv3_dD3[idx9589, idx9259, idx9260, idx9645, idx9646, idx9647, idx9648])*D[idx9647, idx9648])*D[idx9645, idx9646])

	println("[Evaluating 'tmpRes_19', Komplexität 18(10, 6)]")
	@tullio tmpRes_19[idx9589, idx9259, idx9260] := (((tmpRes_17[idx9589, idx9259, idx9260]+tmpRes_18[idx9589, idx9259, idx9260])+(((0.5*depsilonv2_dD2[idx9589, idx9259, idx9260, idx9665, idx9666])*Identity_ord4_dm3333[idx9665, idx9666, idx9673, idx9674])*D[idx9673, idx9674]))+(((0.5*depsilonv2_dD2[idx9589, idx9685, idx9686, idx9687, idx9688])*D[idx9687, idx9688])*Identity_ord4_dm3333[idx9685, idx9686, idx9259, idx9260]))

	println("[Evaluating 'tmpRes_20', Komplexität 20(10, 8)]")
	@tullio tmpRes_20[idx9589, idx9259, idx9260] := ((tmpRes_19[idx9589, idx9259, idx9260]+((((0.166667*depsilonv4_dD4[idx9589, idx9259, idx9260, idx9713, idx9714, idx9715, idx9716, idx9717, idx9718])*D[idx9717, idx9718])*D[idx9715, idx9716])*D[idx9713, idx9714]))+((((0.166667*depsilonv3_dD3[idx9589, idx9259, idx9260, idx9741, idx9742, idx9743, idx9744])*Identity_ord4_dm3333[idx9743, idx9744, idx9751, idx9752])*D[idx9751, idx9752])*D[idx9741, idx9742]))

	println("[Evaluating 'tmpRes_21', Komplexität 20(10, 8)]")
	@tullio tmpRes_21[idx9589, idx9259, idx9260] := ((tmpRes_20[idx9589, idx9259, idx9260]+((((0.166667*depsilonv3_dD3[idx9589, idx9259, idx9260, idx9771, idx9772, idx9773, idx9774])*D[idx9773, idx9774])*Identity_ord4_dm3333[idx9771, idx9772, idx9785, idx9786])*D[idx9785, idx9786]))+((((0.166667*depsilonv3_dD3[idx9589, idx9799, idx9800, idx9801, idx9802, idx9803, idx9804])*D[idx9803, idx9804])*D[idx9801, idx9802])*Identity_ord4_dm3333[idx9799, idx9800, idx9259, idx9260]))

	println("[Evaluating 'tmpRes_22', Komplexität 16(8, 6)]")
	@tullio tmpRes_22[idx8862, idx9257, idx9258, idx9259, idx9260] := ((tmpRes_11[idx8862, idx9257, idx9258, idx9259, idx9260]+((zeros_ord4_dm3333[idx8862, idx9257, idx9258, idx9343]+Identity_ord4_dm3333[idx8862, idx9257, idx9258, idx9343])*(zeros_ord3_dm333[idx9343, idx9259, idx9260]-tmpRes_16[idx9343, idx9259, idx9260])))+((zeros_ord4_dm3333[idx8862, idx9257, idx9258, idx9589]+Identity_ord4_dm3333[idx8862, idx9257, idx9258, idx9589])*(zeros_ord3_dm333[idx9589, idx9259, idx9260]-tmpRes_21[idx9589, idx9259, idx9260])))

	println("[Evaluating 'tmpRes_23', Komplexität 20(14, 4)]")
	@tullio tmpRes_23[idx9829, idx9257, idx9258, idx9259, idx9260] := ((((1*depsilonv2_dD2[idx9829, idx9257, idx9258, idx9259, idx9260])+((1*depsilonv3_dD3[idx9829, idx9257, idx9258, idx9259, idx9260, idx9861, idx9862])*D[idx9861, idx9862]))+((1*depsilonv2_dD2[idx9829, idx9257, idx9258, idx9875, idx9876])*Identity_ord4_dm3333[idx9875, idx9876, idx9259, idx9260]))+((1*depsilonv2_dD2[idx9829, idx9257, idx9258, idx9893, idx9894])*Identity_ord4_dm3333[idx9893, idx9894, idx9259, idx9260]))

	println("[Evaluating 'tmpRes_24', Komplexität 14(6, 6)]")
	@tullio tmpRes_24[idx9829, idx9257, idx9258, idx9259, idx9260] := ((tmpRes_23[idx9829, idx9257, idx9258, idx9259, idx9260]+((1*depsilonv1_dD1[idx9829, idx9907, idx9908])*zeros_ord6_dm333333[idx9907, idx9908, idx9257, idx9258, idx9259, idx9260]))+(((0.5*depsilonv4_dD4[idx9829, idx9257, idx9258, idx9259, idx9260, idx9935, idx9936, idx9937, idx9938])*D[idx9937, idx9938])*D[idx9935, idx9936]))

	println("[Evaluating 'tmpRes_25', Komplexität 6(4, 0)]")
	@tullio tmpRes_25[idx9829, idx9257, idx9258, idx9259, idx9260] := (((0.5*depsilonv3_dD3[idx9829, idx9257, idx9258, idx9259, idx9260, idx9959, idx9960])*Identity_ord4_dm3333[idx9959, idx9960, idx9967, idx9968])*D[idx9967, idx9968])

	println("[Evaluating 'tmpRes_26', Komplexität 18(10, 6)]")
	@tullio tmpRes_26[idx9829, idx9257, idx9258, idx9259, idx9260] := (((tmpRes_24[idx9829, idx9257, idx9258, idx9259, idx9260]+tmpRes_25[idx9829, idx9257, idx9258, idx9259, idx9260])+(((0.5*depsilonv3_dD3[idx9829, idx9257, idx9258, idx9983, idx9984, idx9985, idx9986])*D[idx9985, idx9986])*Identity_ord4_dm3333[idx9983, idx9984, idx9259, idx9260]))+(((0.5*depsilonv3_dD3[idx9829, idx9257, idx9258, idx9259, idx9260, idx10011, idx10012])*Identity_ord4_dm3333[idx10011, idx10012, idx10019, idx10020])*D[idx10019, idx10020]))

	println("[Evaluating 'tmpRes_27', Komplexität 16(8, 6)]")
	@tullio tmpRes_27[idx9829, idx9257, idx9258, idx9259, idx9260] := ((tmpRes_26[idx9829, idx9257, idx9258, idx9259, idx9260]+(((0.5*depsilonv2_dD2[idx9829, idx9257, idx9258, idx10033, idx10034])*zeros_ord6_dm333333[idx10033, idx10034, idx9259, idx9260, idx10045, idx10046])*D[idx10045, idx10046]))+(((0.5*depsilonv2_dD2[idx9829, idx9257, idx9258, idx10059, idx10060])*Identity_ord4_dm3333[idx10059, idx10060, idx10067, idx10068])*Identity_ord4_dm3333[idx10067, idx10068, idx9259, idx9260]))

	println("[Evaluating 'tmpRes_28', Komplexität 16(8, 6)]")
	@tullio tmpRes_28[idx9829, idx9257, idx9258, idx9259, idx9260] := ((tmpRes_27[idx9829, idx9257, idx9258, idx9259, idx9260]+(((0.5*depsilonv3_dD3[idx9829, idx9257, idx9258, idx10087, idx10088, idx10089, idx10090])*D[idx10089, idx10090])*Identity_ord4_dm3333[idx10087, idx10088, idx9259, idx9260]))+(((0.5*depsilonv2_dD2[idx9829, idx9257, idx9258, idx10111, idx10112])*Identity_ord4_dm3333[idx10111, idx10112, idx10119, idx10120])*Identity_ord4_dm3333[idx10119, idx10120, idx9259, idx9260]))

	println("[Evaluating 'tmpRes_29', Komplexität 18(8, 8)]")
	@tullio tmpRes_29[idx9829, idx9257, idx9258, idx9259, idx9260] := ((tmpRes_28[idx9829, idx9257, idx9258, idx9259, idx9260]+(((0.5*depsilonv2_dD2[idx9829, idx10135, idx10136, idx10137, idx10138])*D[idx10137, idx10138])*zeros_ord6_dm333333[idx10135, idx10136, idx9257, idx9258, idx9259, idx9260]))+((((0.166667*depsilonv5_dD5[idx9829, idx9257, idx9258, idx9259, idx9260, idx10171, idx10172, idx10173, idx10174, idx10175, idx10176])*D[idx10175, idx10176])*D[idx10173, idx10174])*D[idx10171, idx10172]))

	println("[Evaluating 'tmpRes_30', Komplexität 20(10, 8)]")
	@tullio tmpRes_30[idx9829, idx9257, idx9258, idx9259, idx9260] := ((tmpRes_29[idx9829, idx9257, idx9258, idx9259, idx9260]+((((0.166667*depsilonv4_dD4[idx9829, idx9257, idx9258, idx9259, idx9260, idx10203, idx10204, idx10205, idx10206])*Identity_ord4_dm3333[idx10205, idx10206, idx10213, idx10214])*D[idx10213, idx10214])*D[idx10203, idx10204]))+((((0.166667*depsilonv4_dD4[idx9829, idx9257, idx9258, idx9259, idx9260, idx10237, idx10238, idx10239, idx10240])*D[idx10239, idx10240])*Identity_ord4_dm3333[idx10237, idx10238, idx10251, idx10252])*D[idx10251, idx10252]))

	println("[Evaluating 'tmpRes_31', Komplexität 20(10, 8)]")
	@tullio tmpRes_31[idx9829, idx9257, idx9258, idx9259, idx9260] := ((tmpRes_30[idx9829, idx9257, idx9258, idx9259, idx9260]+((((0.166667*depsilonv4_dD4[idx9829, idx9257, idx9258, idx10269, idx10270, idx10271, idx10272, idx10273, idx10274])*D[idx10273, idx10274])*D[idx10271, idx10272])*Identity_ord4_dm3333[idx10269, idx10270, idx9259, idx9260]))+((((0.166667*depsilonv4_dD4[idx9829, idx9257, idx9258, idx9259, idx9260, idx10305, idx10306, idx10307, idx10308])*Identity_ord4_dm3333[idx10307, idx10308, idx10315, idx10316])*D[idx10315, idx10316])*D[idx10305, idx10306]))

	println("[Evaluating 'tmpRes_32', Komplexität 20(10, 8)]")
	@tullio tmpRes_32[idx9829, idx9257, idx9258, idx9259, idx9260] := ((tmpRes_31[idx9829, idx9257, idx9258, idx9259, idx9260]+((((0.166667*depsilonv3_dD3[idx9829, idx9257, idx9258, idx9259, idx9260, idx10337, idx10338])*zeros_ord6_dm333333[idx10337, idx10338, idx10347, idx10348, idx10349, idx10350])*D[idx10349, idx10350])*D[idx10347, idx10348]))+((((0.166667*depsilonv3_dD3[idx9829, idx9257, idx9258, idx9259, idx9260, idx10371, idx10372])*Identity_ord4_dm3333[idx10371, idx10372, idx10379, idx10380])*Identity_ord4_dm3333[idx10379, idx10380, idx10387, idx10388])*D[idx10387, idx10388]))

	println("[Evaluating 'tmpRes_33', Komplexität 20(10, 8)]")
	@tullio tmpRes_33[idx9829, idx9257, idx9258, idx9259, idx9260] := ((tmpRes_32[idx9829, idx9257, idx9258, idx9259, idx9260]+((((0.166667*depsilonv3_dD3[idx9829, idx9257, idx9258, idx10403, idx10404, idx10405, idx10406])*Identity_ord4_dm3333[idx10405, idx10406, idx10413, idx10414])*D[idx10413, idx10414])*Identity_ord4_dm3333[idx10403, idx10404, idx9259, idx9260]))+((((0.166667*depsilonv4_dD4[idx9829, idx9257, idx9258, idx9259, idx9260, idx10441, idx10442, idx10443, idx10444])*D[idx10443, idx10444])*Identity_ord4_dm3333[idx10441, idx10442, idx10455, idx10456])*D[idx10455, idx10456]))

	println("[Evaluating 'tmpRes_34', Komplexität 20(10, 8)]")
	@tullio tmpRes_34[idx9829, idx9257, idx9258, idx9259, idx9260] := ((tmpRes_33[idx9829, idx9257, idx9258, idx9259, idx9260]+((((0.166667*depsilonv3_dD3[idx9829, idx9257, idx9258, idx9259, idx9260, idx10473, idx10474])*Identity_ord4_dm3333[idx10473, idx10474, idx10481, idx10482])*Identity_ord4_dm3333[idx10481, idx10482, idx10489, idx10490])*D[idx10489, idx10490]))+((((0.166667*depsilonv3_dD3[idx9829, idx9257, idx9258, idx10505, idx10506, idx10507, idx10508])*D[idx10507, idx10508])*zeros_ord6_dm333333[idx10505, idx10506, idx9259, idx9260, idx10523, idx10524])*D[idx10523, idx10524]))

	println("[Evaluating 'tmpRes_35', Komplexität 20(10, 8)]")
	@tullio tmpRes_35[idx9829, idx9257, idx9258, idx9259, idx9260] := ((tmpRes_34[idx9829, idx9257, idx9258, idx9259, idx9260]+((((0.166667*depsilonv3_dD3[idx9829, idx9257, idx9258, idx10539, idx10540, idx10541, idx10542])*D[idx10541, idx10542])*Identity_ord4_dm3333[idx10539, idx10540, idx10553, idx10554])*Identity_ord4_dm3333[idx10553, idx10554, idx9259, idx9260]))+((((0.166667*depsilonv4_dD4[idx9829, idx9257, idx9258, idx10575, idx10576, idx10577, idx10578, idx10579, idx10580])*D[idx10579, idx10580])*D[idx10577, idx10578])*Identity_ord4_dm3333[idx10575, idx10576, idx9259, idx9260]))

	println("[Evaluating 'tmpRes_36', Komplexität 20(10, 8)]")
	@tullio tmpRes_36[idx9829, idx9257, idx9258, idx9259, idx9260] := ((tmpRes_35[idx9829, idx9257, idx9258, idx9259, idx9260]+((((0.166667*depsilonv3_dD3[idx9829, idx9257, idx9258, idx10607, idx10608, idx10609, idx10610])*Identity_ord4_dm3333[idx10609, idx10610, idx10617, idx10618])*D[idx10617, idx10618])*Identity_ord4_dm3333[idx10607, idx10608, idx9259, idx9260]))+((((0.166667*depsilonv3_dD3[idx9829, idx9257, idx9258, idx10641, idx10642, idx10643, idx10644])*D[idx10643, idx10644])*Identity_ord4_dm3333[idx10641, idx10642, idx10655, idx10656])*Identity_ord4_dm3333[idx10655, idx10656, idx9259, idx9260]))

	println("[Evaluating 'tmpRes_37', Komplexität 16(2, 12)]")
	@tullio tmpRes_37[idx8862, idx9257, idx9258, idx9259, idx9260] := ((E0[idx8862, idx9829]+D[idx8862, idx9829])*(zeros_ord5_dm33333[idx9829, idx9257, idx9258, idx9259, idx9260]-(tmpRes_36[idx9829, idx9257, idx9258, idx9259, idx9260]+((((0.166667*depsilonv3_dD3[idx9829, idx10673, idx10674, idx10675, idx10676, idx10677, idx10678])*D[idx10677, idx10678])*D[idx10675, idx10676])*zeros_ord6_dm333333[idx10673, idx10674, idx9257, idx9258, idx9259, idx9260]))))

	println("[Evaluating 'tmpRes_38', Komplexität 16(8, 6)]")
	@tullio tmpRes_38[idx10735] := (((1*epsilonv[idx10735])+((1*depsilonv1_dD1[idx10735, idx10759, idx10760])*D[idx10759, idx10760]))+(((0.5*depsilonv2_dD2[idx10735, idx10771, idx10772, idx10773, idx10774])*D[idx10773, idx10774])*D[idx10771, idx10772]))

	println("[Evaluating 'tmpRes_39', Komplexität 16(2, 12)]")
	@tullio tmpRes_39[idx8862, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((zeros_ord8_dm33333333[idx8862, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734, idx10735]+zeros_ord8_dm33333333[idx8862, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734, idx10735])*(epsilon[idx10735]-(tmpRes_38[idx10735]+((((0.166667*depsilonv3_dD3[idx10735, idx10791, idx10792, idx10793, idx10794, idx10795, idx10796])*D[idx10795, idx10796])*D[idx10793, idx10794])*D[idx10791, idx10792]))))

	println("[Evaluating 'tmpRes_40', Komplexität 14(8, 4)]")
	@tullio tmpRes_40[idx10827, idx10733, idx10734] := (((1*depsilonv1_dD1[idx10827, idx10733, idx10734])+((1*depsilonv2_dD2[idx10827, idx10733, idx10734, idx10857, idx10858])*D[idx10857, idx10858]))+((1*depsilonv1_dD1[idx10827, idx10867, idx10868])*Identity_ord4_dm3333[idx10867, idx10868, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_41', Komplexität 6(4, 0)]")
	@tullio tmpRes_41[idx10827, idx10733, idx10734] := (((0.5*depsilonv3_dD3[idx10827, idx10733, idx10734, idx10887, idx10888, idx10889, idx10890])*D[idx10889, idx10890])*D[idx10887, idx10888])

	println("[Evaluating 'tmpRes_42', Komplexität 18(10, 6)]")
	@tullio tmpRes_42[idx10827, idx10733, idx10734] := (((tmpRes_40[idx10827, idx10733, idx10734]+tmpRes_41[idx10827, idx10733, idx10734])+(((0.5*depsilonv2_dD2[idx10827, idx10733, idx10734, idx10907, idx10908])*Identity_ord4_dm3333[idx10907, idx10908, idx10915, idx10916])*D[idx10915, idx10916]))+(((0.5*depsilonv2_dD2[idx10827, idx10927, idx10928, idx10929, idx10930])*D[idx10929, idx10930])*Identity_ord4_dm3333[idx10927, idx10928, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_43', Komplexität 20(10, 8)]")
	@tullio tmpRes_43[idx10827, idx10733, idx10734] := ((tmpRes_42[idx10827, idx10733, idx10734]+((((0.166667*depsilonv4_dD4[idx10827, idx10733, idx10734, idx10955, idx10956, idx10957, idx10958, idx10959, idx10960])*D[idx10959, idx10960])*D[idx10957, idx10958])*D[idx10955, idx10956]))+((((0.166667*depsilonv3_dD3[idx10827, idx10733, idx10734, idx10983, idx10984, idx10985, idx10986])*Identity_ord4_dm3333[idx10985, idx10986, idx10993, idx10994])*D[idx10993, idx10994])*D[idx10983, idx10984]))

	println("[Evaluating 'tmpRes_44', Komplexität 20(10, 8)]")
	@tullio tmpRes_44[idx10827, idx10733, idx10734] := ((tmpRes_43[idx10827, idx10733, idx10734]+((((0.166667*depsilonv3_dD3[idx10827, idx10733, idx10734, idx11013, idx11014, idx11015, idx11016])*D[idx11015, idx11016])*Identity_ord4_dm3333[idx11013, idx11014, idx11027, idx11028])*D[idx11027, idx11028]))+((((0.166667*depsilonv3_dD3[idx10827, idx11041, idx11042, idx11043, idx11044, idx11045, idx11046])*D[idx11045, idx11046])*D[idx11043, idx11044])*Identity_ord4_dm3333[idx11041, idx11042, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_45', Komplexität 14(8, 4)]")
	@tullio tmpRes_45[idx11083, idx10733, idx10734] := (((1*depsilonv1_dD1[idx11083, idx10733, idx10734])+((1*depsilonv2_dD2[idx11083, idx10733, idx10734, idx11113, idx11114])*D[idx11113, idx11114]))+((1*depsilonv1_dD1[idx11083, idx11123, idx11124])*Identity_ord4_dm3333[idx11123, idx11124, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_46', Komplexität 6(4, 0)]")
	@tullio tmpRes_46[idx11083, idx10733, idx10734] := (((0.5*depsilonv3_dD3[idx11083, idx10733, idx10734, idx11143, idx11144, idx11145, idx11146])*D[idx11145, idx11146])*D[idx11143, idx11144])

	println("[Evaluating 'tmpRes_47', Komplexität 18(10, 6)]")
	@tullio tmpRes_47[idx11083, idx10733, idx10734] := (((tmpRes_45[idx11083, idx10733, idx10734]+tmpRes_46[idx11083, idx10733, idx10734])+(((0.5*depsilonv2_dD2[idx11083, idx10733, idx10734, idx11163, idx11164])*Identity_ord4_dm3333[idx11163, idx11164, idx11171, idx11172])*D[idx11171, idx11172]))+(((0.5*depsilonv2_dD2[idx11083, idx11183, idx11184, idx11185, idx11186])*D[idx11185, idx11186])*Identity_ord4_dm3333[idx11183, idx11184, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_48', Komplexität 20(10, 8)]")
	@tullio tmpRes_48[idx11083, idx10733, idx10734] := ((tmpRes_47[idx11083, idx10733, idx10734]+((((0.166667*depsilonv4_dD4[idx11083, idx10733, idx10734, idx11211, idx11212, idx11213, idx11214, idx11215, idx11216])*D[idx11215, idx11216])*D[idx11213, idx11214])*D[idx11211, idx11212]))+((((0.166667*depsilonv3_dD3[idx11083, idx10733, idx10734, idx11239, idx11240, idx11241, idx11242])*Identity_ord4_dm3333[idx11241, idx11242, idx11249, idx11250])*D[idx11249, idx11250])*D[idx11239, idx11240]))

	println("[Evaluating 'tmpRes_49', Komplexität 20(10, 8)]")
	@tullio tmpRes_49[idx11083, idx10733, idx10734] := ((tmpRes_48[idx11083, idx10733, idx10734]+((((0.166667*depsilonv3_dD3[idx11083, idx10733, idx10734, idx11269, idx11270, idx11271, idx11272])*D[idx11271, idx11272])*Identity_ord4_dm3333[idx11269, idx11270, idx11283, idx11284])*D[idx11283, idx11284]))+((((0.166667*depsilonv3_dD3[idx11083, idx11297, idx11298, idx11299, idx11300, idx11301, idx11302])*D[idx11301, idx11302])*D[idx11299, idx11300])*Identity_ord4_dm3333[idx11297, idx11298, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_50', Komplexität 16(8, 6)]")
	@tullio tmpRes_50[idx8862, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_39[idx8862, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+((zeros_ord6_dm333333[idx8862, idx10729, idx10730, idx10731, idx10732, idx10827]+zeros_ord6_dm333333[idx8862, idx10729, idx10730, idx10731, idx10732, idx10827])*(zeros_ord3_dm333[idx10827, idx10733, idx10734]-tmpRes_44[idx10827, idx10733, idx10734])))+((zeros_ord6_dm333333[idx8862, idx10729, idx10730, idx10731, idx10732, idx11083]+zeros_ord6_dm333333[idx8862, idx10729, idx10730, idx10731, idx10732, idx11083])*(zeros_ord3_dm333[idx11083, idx10733, idx10734]-tmpRes_49[idx11083, idx10733, idx10734])))

	println("[Evaluating 'tmpRes_51', Komplexität 20(14, 4)]")
	@tullio tmpRes_51[idx11333, idx10731, idx10732, idx10733, idx10734] := ((((1*depsilonv2_dD2[idx11333, idx10731, idx10732, idx10733, idx10734])+((1*depsilonv3_dD3[idx11333, idx10731, idx10732, idx10733, idx10734, idx11369, idx11370])*D[idx11369, idx11370]))+((1*depsilonv2_dD2[idx11333, idx10731, idx10732, idx11383, idx11384])*Identity_ord4_dm3333[idx11383, idx11384, idx10733, idx10734]))+((1*depsilonv2_dD2[idx11333, idx10731, idx10732, idx11401, idx11402])*Identity_ord4_dm3333[idx11401, idx11402, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_52', Komplexität 14(6, 6)]")
	@tullio tmpRes_52[idx11333, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_51[idx11333, idx10731, idx10732, idx10733, idx10734]+((1*depsilonv1_dD1[idx11333, idx11415, idx11416])*zeros_ord6_dm333333[idx11415, idx11416, idx10731, idx10732, idx10733, idx10734]))+(((0.5*depsilonv4_dD4[idx11333, idx10731, idx10732, idx10733, idx10734, idx11443, idx11444, idx11445, idx11446])*D[idx11445, idx11446])*D[idx11443, idx11444]))

	println("[Evaluating 'tmpRes_53', Komplexität 6(4, 0)]")
	@tullio tmpRes_53[idx11333, idx10731, idx10732, idx10733, idx10734] := (((0.5*depsilonv3_dD3[idx11333, idx10731, idx10732, idx10733, idx10734, idx11467, idx11468])*Identity_ord4_dm3333[idx11467, idx11468, idx11475, idx11476])*D[idx11475, idx11476])

	println("[Evaluating 'tmpRes_54', Komplexität 18(10, 6)]")
	@tullio tmpRes_54[idx11333, idx10731, idx10732, idx10733, idx10734] := (((tmpRes_52[idx11333, idx10731, idx10732, idx10733, idx10734]+tmpRes_53[idx11333, idx10731, idx10732, idx10733, idx10734])+(((0.5*depsilonv3_dD3[idx11333, idx10731, idx10732, idx11491, idx11492, idx11493, idx11494])*D[idx11493, idx11494])*Identity_ord4_dm3333[idx11491, idx11492, idx10733, idx10734]))+(((0.5*depsilonv3_dD3[idx11333, idx10731, idx10732, idx10733, idx10734, idx11519, idx11520])*Identity_ord4_dm3333[idx11519, idx11520, idx11527, idx11528])*D[idx11527, idx11528]))

	println("[Evaluating 'tmpRes_55', Komplexität 16(8, 6)]")
	@tullio tmpRes_55[idx11333, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_54[idx11333, idx10731, idx10732, idx10733, idx10734]+(((0.5*depsilonv2_dD2[idx11333, idx10731, idx10732, idx11541, idx11542])*zeros_ord6_dm333333[idx11541, idx11542, idx10733, idx10734, idx11553, idx11554])*D[idx11553, idx11554]))+(((0.5*depsilonv2_dD2[idx11333, idx10731, idx10732, idx11567, idx11568])*Identity_ord4_dm3333[idx11567, idx11568, idx11575, idx11576])*Identity_ord4_dm3333[idx11575, idx11576, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_56', Komplexität 16(8, 6)]")
	@tullio tmpRes_56[idx11333, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_55[idx11333, idx10731, idx10732, idx10733, idx10734]+(((0.5*depsilonv3_dD3[idx11333, idx10731, idx10732, idx11595, idx11596, idx11597, idx11598])*D[idx11597, idx11598])*Identity_ord4_dm3333[idx11595, idx11596, idx10733, idx10734]))+(((0.5*depsilonv2_dD2[idx11333, idx10731, idx10732, idx11619, idx11620])*Identity_ord4_dm3333[idx11619, idx11620, idx11627, idx11628])*Identity_ord4_dm3333[idx11627, idx11628, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_57', Komplexität 18(8, 8)]")
	@tullio tmpRes_57[idx11333, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_56[idx11333, idx10731, idx10732, idx10733, idx10734]+(((0.5*depsilonv2_dD2[idx11333, idx11643, idx11644, idx11645, idx11646])*D[idx11645, idx11646])*zeros_ord6_dm333333[idx11643, idx11644, idx10731, idx10732, idx10733, idx10734]))+((((0.166667*depsilonv5_dD5[idx11333, idx10731, idx10732, idx10733, idx10734, idx11679, idx11680, idx11681, idx11682, idx11683, idx11684])*D[idx11683, idx11684])*D[idx11681, idx11682])*D[idx11679, idx11680]))

	println("[Evaluating 'tmpRes_58', Komplexität 20(10, 8)]")
	@tullio tmpRes_58[idx11333, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_57[idx11333, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv4_dD4[idx11333, idx10731, idx10732, idx10733, idx10734, idx11711, idx11712, idx11713, idx11714])*Identity_ord4_dm3333[idx11713, idx11714, idx11721, idx11722])*D[idx11721, idx11722])*D[idx11711, idx11712]))+((((0.166667*depsilonv4_dD4[idx11333, idx10731, idx10732, idx10733, idx10734, idx11745, idx11746, idx11747, idx11748])*D[idx11747, idx11748])*Identity_ord4_dm3333[idx11745, idx11746, idx11759, idx11760])*D[idx11759, idx11760]))

	println("[Evaluating 'tmpRes_59', Komplexität 20(10, 8)]")
	@tullio tmpRes_59[idx11333, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_58[idx11333, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv4_dD4[idx11333, idx10731, idx10732, idx11777, idx11778, idx11779, idx11780, idx11781, idx11782])*D[idx11781, idx11782])*D[idx11779, idx11780])*Identity_ord4_dm3333[idx11777, idx11778, idx10733, idx10734]))+((((0.166667*depsilonv4_dD4[idx11333, idx10731, idx10732, idx10733, idx10734, idx11813, idx11814, idx11815, idx11816])*Identity_ord4_dm3333[idx11815, idx11816, idx11823, idx11824])*D[idx11823, idx11824])*D[idx11813, idx11814]))

	println("[Evaluating 'tmpRes_60', Komplexität 20(10, 8)]")
	@tullio tmpRes_60[idx11333, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_59[idx11333, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv3_dD3[idx11333, idx10731, idx10732, idx10733, idx10734, idx11845, idx11846])*zeros_ord6_dm333333[idx11845, idx11846, idx11855, idx11856, idx11857, idx11858])*D[idx11857, idx11858])*D[idx11855, idx11856]))+((((0.166667*depsilonv3_dD3[idx11333, idx10731, idx10732, idx10733, idx10734, idx11879, idx11880])*Identity_ord4_dm3333[idx11879, idx11880, idx11887, idx11888])*Identity_ord4_dm3333[idx11887, idx11888, idx11895, idx11896])*D[idx11895, idx11896]))

	println("[Evaluating 'tmpRes_61', Komplexität 20(10, 8)]")
	@tullio tmpRes_61[idx11333, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_60[idx11333, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv3_dD3[idx11333, idx10731, idx10732, idx11911, idx11912, idx11913, idx11914])*Identity_ord4_dm3333[idx11913, idx11914, idx11921, idx11922])*D[idx11921, idx11922])*Identity_ord4_dm3333[idx11911, idx11912, idx10733, idx10734]))+((((0.166667*depsilonv4_dD4[idx11333, idx10731, idx10732, idx10733, idx10734, idx11949, idx11950, idx11951, idx11952])*D[idx11951, idx11952])*Identity_ord4_dm3333[idx11949, idx11950, idx11963, idx11964])*D[idx11963, idx11964]))

	println("[Evaluating 'tmpRes_62', Komplexität 20(10, 8)]")
	@tullio tmpRes_62[idx11333, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_61[idx11333, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv3_dD3[idx11333, idx10731, idx10732, idx10733, idx10734, idx11981, idx11982])*Identity_ord4_dm3333[idx11981, idx11982, idx11989, idx11990])*Identity_ord4_dm3333[idx11989, idx11990, idx11997, idx11998])*D[idx11997, idx11998]))+((((0.166667*depsilonv3_dD3[idx11333, idx10731, idx10732, idx12013, idx12014, idx12015, idx12016])*D[idx12015, idx12016])*zeros_ord6_dm333333[idx12013, idx12014, idx10733, idx10734, idx12031, idx12032])*D[idx12031, idx12032]))

	println("[Evaluating 'tmpRes_63', Komplexität 20(10, 8)]")
	@tullio tmpRes_63[idx11333, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_62[idx11333, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv3_dD3[idx11333, idx10731, idx10732, idx12047, idx12048, idx12049, idx12050])*D[idx12049, idx12050])*Identity_ord4_dm3333[idx12047, idx12048, idx12061, idx12062])*Identity_ord4_dm3333[idx12061, idx12062, idx10733, idx10734]))+((((0.166667*depsilonv4_dD4[idx11333, idx10731, idx10732, idx12083, idx12084, idx12085, idx12086, idx12087, idx12088])*D[idx12087, idx12088])*D[idx12085, idx12086])*Identity_ord4_dm3333[idx12083, idx12084, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_64', Komplexität 20(10, 8)]")
	@tullio tmpRes_64[idx11333, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_63[idx11333, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv3_dD3[idx11333, idx10731, idx10732, idx12115, idx12116, idx12117, idx12118])*Identity_ord4_dm3333[idx12117, idx12118, idx12125, idx12126])*D[idx12125, idx12126])*Identity_ord4_dm3333[idx12115, idx12116, idx10733, idx10734]))+((((0.166667*depsilonv3_dD3[idx11333, idx10731, idx10732, idx12149, idx12150, idx12151, idx12152])*D[idx12151, idx12152])*Identity_ord4_dm3333[idx12149, idx12150, idx12163, idx12164])*Identity_ord4_dm3333[idx12163, idx12164, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_65', Komplexität 16(2, 12)]")
	@tullio tmpRes_65[idx8862, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((zeros_ord4_dm3333[idx8862, idx10729, idx10730, idx11333]+Identity_ord4_dm3333[idx8862, idx10729, idx10730, idx11333])*(zeros_ord5_dm33333[idx11333, idx10731, idx10732, idx10733, idx10734]-(tmpRes_64[idx11333, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv3_dD3[idx11333, idx12181, idx12182, idx12183, idx12184, idx12185, idx12186])*D[idx12185, idx12186])*D[idx12183, idx12184])*zeros_ord6_dm333333[idx12181, idx12182, idx10731, idx10732, idx10733, idx10734]))))

	println("[Evaluating 'tmpRes_66', Komplexität 14(8, 4)]")
	@tullio tmpRes_66[idx12229, idx10733, idx10734] := (((1*depsilonv1_dD1[idx12229, idx10733, idx10734])+((1*depsilonv2_dD2[idx12229, idx10733, idx10734, idx12259, idx12260])*D[idx12259, idx12260]))+((1*depsilonv1_dD1[idx12229, idx12269, idx12270])*Identity_ord4_dm3333[idx12269, idx12270, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_67', Komplexität 6(4, 0)]")
	@tullio tmpRes_67[idx12229, idx10733, idx10734] := (((0.5*depsilonv3_dD3[idx12229, idx10733, idx10734, idx12289, idx12290, idx12291, idx12292])*D[idx12291, idx12292])*D[idx12289, idx12290])

	println("[Evaluating 'tmpRes_68', Komplexität 18(10, 6)]")
	@tullio tmpRes_68[idx12229, idx10733, idx10734] := (((tmpRes_66[idx12229, idx10733, idx10734]+tmpRes_67[idx12229, idx10733, idx10734])+(((0.5*depsilonv2_dD2[idx12229, idx10733, idx10734, idx12309, idx12310])*Identity_ord4_dm3333[idx12309, idx12310, idx12317, idx12318])*D[idx12317, idx12318]))+(((0.5*depsilonv2_dD2[idx12229, idx12329, idx12330, idx12331, idx12332])*D[idx12331, idx12332])*Identity_ord4_dm3333[idx12329, idx12330, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_69', Komplexität 20(10, 8)]")
	@tullio tmpRes_69[idx12229, idx10733, idx10734] := ((tmpRes_68[idx12229, idx10733, idx10734]+((((0.166667*depsilonv4_dD4[idx12229, idx10733, idx10734, idx12357, idx12358, idx12359, idx12360, idx12361, idx12362])*D[idx12361, idx12362])*D[idx12359, idx12360])*D[idx12357, idx12358]))+((((0.166667*depsilonv3_dD3[idx12229, idx10733, idx10734, idx12385, idx12386, idx12387, idx12388])*Identity_ord4_dm3333[idx12387, idx12388, idx12395, idx12396])*D[idx12395, idx12396])*D[idx12385, idx12386]))

	println("[Evaluating 'tmpRes_70', Komplexität 20(10, 8)]")
	@tullio tmpRes_70[idx12229, idx10733, idx10734] := ((tmpRes_69[idx12229, idx10733, idx10734]+((((0.166667*depsilonv3_dD3[idx12229, idx10733, idx10734, idx12415, idx12416, idx12417, idx12418])*D[idx12417, idx12418])*Identity_ord4_dm3333[idx12415, idx12416, idx12429, idx12430])*D[idx12429, idx12430]))+((((0.166667*depsilonv3_dD3[idx12229, idx12443, idx12444, idx12445, idx12446, idx12447, idx12448])*D[idx12447, idx12448])*D[idx12445, idx12446])*Identity_ord4_dm3333[idx12443, idx12444, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_71', Komplexität 20(14, 4)]")
	@tullio tmpRes_71[idx12479, idx10731, idx10732, idx10733, idx10734] := ((((1*depsilonv2_dD2[idx12479, idx10731, idx10732, idx10733, idx10734])+((1*depsilonv3_dD3[idx12479, idx10731, idx10732, idx10733, idx10734, idx12515, idx12516])*D[idx12515, idx12516]))+((1*depsilonv2_dD2[idx12479, idx10731, idx10732, idx12529, idx12530])*Identity_ord4_dm3333[idx12529, idx12530, idx10733, idx10734]))+((1*depsilonv2_dD2[idx12479, idx10731, idx10732, idx12547, idx12548])*Identity_ord4_dm3333[idx12547, idx12548, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_72', Komplexität 14(6, 6)]")
	@tullio tmpRes_72[idx12479, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_71[idx12479, idx10731, idx10732, idx10733, idx10734]+((1*depsilonv1_dD1[idx12479, idx12561, idx12562])*zeros_ord6_dm333333[idx12561, idx12562, idx10731, idx10732, idx10733, idx10734]))+(((0.5*depsilonv4_dD4[idx12479, idx10731, idx10732, idx10733, idx10734, idx12589, idx12590, idx12591, idx12592])*D[idx12591, idx12592])*D[idx12589, idx12590]))

	println("[Evaluating 'tmpRes_73', Komplexität 6(4, 0)]")
	@tullio tmpRes_73[idx12479, idx10731, idx10732, idx10733, idx10734] := (((0.5*depsilonv3_dD3[idx12479, idx10731, idx10732, idx10733, idx10734, idx12613, idx12614])*Identity_ord4_dm3333[idx12613, idx12614, idx12621, idx12622])*D[idx12621, idx12622])

	println("[Evaluating 'tmpRes_74', Komplexität 18(10, 6)]")
	@tullio tmpRes_74[idx12479, idx10731, idx10732, idx10733, idx10734] := (((tmpRes_72[idx12479, idx10731, idx10732, idx10733, idx10734]+tmpRes_73[idx12479, idx10731, idx10732, idx10733, idx10734])+(((0.5*depsilonv3_dD3[idx12479, idx10731, idx10732, idx12637, idx12638, idx12639, idx12640])*D[idx12639, idx12640])*Identity_ord4_dm3333[idx12637, idx12638, idx10733, idx10734]))+(((0.5*depsilonv3_dD3[idx12479, idx10731, idx10732, idx10733, idx10734, idx12665, idx12666])*Identity_ord4_dm3333[idx12665, idx12666, idx12673, idx12674])*D[idx12673, idx12674]))

	println("[Evaluating 'tmpRes_75', Komplexität 16(8, 6)]")
	@tullio tmpRes_75[idx12479, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_74[idx12479, idx10731, idx10732, idx10733, idx10734]+(((0.5*depsilonv2_dD2[idx12479, idx10731, idx10732, idx12687, idx12688])*zeros_ord6_dm333333[idx12687, idx12688, idx10733, idx10734, idx12699, idx12700])*D[idx12699, idx12700]))+(((0.5*depsilonv2_dD2[idx12479, idx10731, idx10732, idx12713, idx12714])*Identity_ord4_dm3333[idx12713, idx12714, idx12721, idx12722])*Identity_ord4_dm3333[idx12721, idx12722, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_76', Komplexität 16(8, 6)]")
	@tullio tmpRes_76[idx12479, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_75[idx12479, idx10731, idx10732, idx10733, idx10734]+(((0.5*depsilonv3_dD3[idx12479, idx10731, idx10732, idx12741, idx12742, idx12743, idx12744])*D[idx12743, idx12744])*Identity_ord4_dm3333[idx12741, idx12742, idx10733, idx10734]))+(((0.5*depsilonv2_dD2[idx12479, idx10731, idx10732, idx12765, idx12766])*Identity_ord4_dm3333[idx12765, idx12766, idx12773, idx12774])*Identity_ord4_dm3333[idx12773, idx12774, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_77', Komplexität 18(8, 8)]")
	@tullio tmpRes_77[idx12479, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_76[idx12479, idx10731, idx10732, idx10733, idx10734]+(((0.5*depsilonv2_dD2[idx12479, idx12789, idx12790, idx12791, idx12792])*D[idx12791, idx12792])*zeros_ord6_dm333333[idx12789, idx12790, idx10731, idx10732, idx10733, idx10734]))+((((0.166667*depsilonv5_dD5[idx12479, idx10731, idx10732, idx10733, idx10734, idx12825, idx12826, idx12827, idx12828, idx12829, idx12830])*D[idx12829, idx12830])*D[idx12827, idx12828])*D[idx12825, idx12826]))

	println("[Evaluating 'tmpRes_78', Komplexität 20(10, 8)]")
	@tullio tmpRes_78[idx12479, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_77[idx12479, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv4_dD4[idx12479, idx10731, idx10732, idx10733, idx10734, idx12857, idx12858, idx12859, idx12860])*Identity_ord4_dm3333[idx12859, idx12860, idx12867, idx12868])*D[idx12867, idx12868])*D[idx12857, idx12858]))+((((0.166667*depsilonv4_dD4[idx12479, idx10731, idx10732, idx10733, idx10734, idx12891, idx12892, idx12893, idx12894])*D[idx12893, idx12894])*Identity_ord4_dm3333[idx12891, idx12892, idx12905, idx12906])*D[idx12905, idx12906]))

	println("[Evaluating 'tmpRes_79', Komplexität 20(10, 8)]")
	@tullio tmpRes_79[idx12479, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_78[idx12479, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv4_dD4[idx12479, idx10731, idx10732, idx12923, idx12924, idx12925, idx12926, idx12927, idx12928])*D[idx12927, idx12928])*D[idx12925, idx12926])*Identity_ord4_dm3333[idx12923, idx12924, idx10733, idx10734]))+((((0.166667*depsilonv4_dD4[idx12479, idx10731, idx10732, idx10733, idx10734, idx12959, idx12960, idx12961, idx12962])*Identity_ord4_dm3333[idx12961, idx12962, idx12969, idx12970])*D[idx12969, idx12970])*D[idx12959, idx12960]))

	println("[Evaluating 'tmpRes_80', Komplexität 20(10, 8)]")
	@tullio tmpRes_80[idx12479, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_79[idx12479, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv3_dD3[idx12479, idx10731, idx10732, idx10733, idx10734, idx12991, idx12992])*zeros_ord6_dm333333[idx12991, idx12992, idx13001, idx13002, idx13003, idx13004])*D[idx13003, idx13004])*D[idx13001, idx13002]))+((((0.166667*depsilonv3_dD3[idx12479, idx10731, idx10732, idx10733, idx10734, idx13025, idx13026])*Identity_ord4_dm3333[idx13025, idx13026, idx13033, idx13034])*Identity_ord4_dm3333[idx13033, idx13034, idx13041, idx13042])*D[idx13041, idx13042]))

	println("[Evaluating 'tmpRes_81', Komplexität 20(10, 8)]")
	@tullio tmpRes_81[idx12479, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_80[idx12479, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv3_dD3[idx12479, idx10731, idx10732, idx13057, idx13058, idx13059, idx13060])*Identity_ord4_dm3333[idx13059, idx13060, idx13067, idx13068])*D[idx13067, idx13068])*Identity_ord4_dm3333[idx13057, idx13058, idx10733, idx10734]))+((((0.166667*depsilonv4_dD4[idx12479, idx10731, idx10732, idx10733, idx10734, idx13095, idx13096, idx13097, idx13098])*D[idx13097, idx13098])*Identity_ord4_dm3333[idx13095, idx13096, idx13109, idx13110])*D[idx13109, idx13110]))

	println("[Evaluating 'tmpRes_82', Komplexität 20(10, 8)]")
	@tullio tmpRes_82[idx12479, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_81[idx12479, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv3_dD3[idx12479, idx10731, idx10732, idx10733, idx10734, idx13127, idx13128])*Identity_ord4_dm3333[idx13127, idx13128, idx13135, idx13136])*Identity_ord4_dm3333[idx13135, idx13136, idx13143, idx13144])*D[idx13143, idx13144]))+((((0.166667*depsilonv3_dD3[idx12479, idx10731, idx10732, idx13159, idx13160, idx13161, idx13162])*D[idx13161, idx13162])*zeros_ord6_dm333333[idx13159, idx13160, idx10733, idx10734, idx13177, idx13178])*D[idx13177, idx13178]))

	println("[Evaluating 'tmpRes_83', Komplexität 20(10, 8)]")
	@tullio tmpRes_83[idx12479, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_82[idx12479, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv3_dD3[idx12479, idx10731, idx10732, idx13193, idx13194, idx13195, idx13196])*D[idx13195, idx13196])*Identity_ord4_dm3333[idx13193, idx13194, idx13207, idx13208])*Identity_ord4_dm3333[idx13207, idx13208, idx10733, idx10734]))+((((0.166667*depsilonv4_dD4[idx12479, idx10731, idx10732, idx13229, idx13230, idx13231, idx13232, idx13233, idx13234])*D[idx13233, idx13234])*D[idx13231, idx13232])*Identity_ord4_dm3333[idx13229, idx13230, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_84', Komplexität 20(10, 8)]")
	@tullio tmpRes_84[idx12479, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_83[idx12479, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv3_dD3[idx12479, idx10731, idx10732, idx13261, idx13262, idx13263, idx13264])*Identity_ord4_dm3333[idx13263, idx13264, idx13271, idx13272])*D[idx13271, idx13272])*Identity_ord4_dm3333[idx13261, idx13262, idx10733, idx10734]))+((((0.166667*depsilonv3_dD3[idx12479, idx10731, idx10732, idx13295, idx13296, idx13297, idx13298])*D[idx13297, idx13298])*Identity_ord4_dm3333[idx13295, idx13296, idx13309, idx13310])*Identity_ord4_dm3333[idx13309, idx13310, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_85', Komplexität 16(2, 12)]")
	@tullio tmpRes_85[idx8862, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((zeros_ord4_dm3333[idx8862, idx10729, idx10730, idx12479]+Identity_ord4_dm3333[idx8862, idx10729, idx10730, idx12479])*(zeros_ord5_dm33333[idx12479, idx10731, idx10732, idx10733, idx10734]-(tmpRes_84[idx12479, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv3_dD3[idx12479, idx13327, idx13328, idx13329, idx13330, idx13331, idx13332])*D[idx13331, idx13332])*D[idx13329, idx13330])*zeros_ord6_dm333333[idx13327, idx13328, idx10731, idx10732, idx10733, idx10734]))))

	println("[Evaluating 'tmpRes_86', Komplexität 20(14, 4)]")
	@tullio tmpRes_86[idx13369, idx10731, idx10732, idx10733, idx10734] := ((((1*depsilonv2_dD2[idx13369, idx10731, idx10732, idx10733, idx10734])+((1*depsilonv3_dD3[idx13369, idx10731, idx10732, idx10733, idx10734, idx13405, idx13406])*D[idx13405, idx13406]))+((1*depsilonv2_dD2[idx13369, idx10731, idx10732, idx13419, idx13420])*Identity_ord4_dm3333[idx13419, idx13420, idx10733, idx10734]))+((1*depsilonv2_dD2[idx13369, idx10731, idx10732, idx13437, idx13438])*Identity_ord4_dm3333[idx13437, idx13438, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_87', Komplexität 14(6, 6)]")
	@tullio tmpRes_87[idx13369, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_86[idx13369, idx10731, idx10732, idx10733, idx10734]+((1*depsilonv1_dD1[idx13369, idx13451, idx13452])*zeros_ord6_dm333333[idx13451, idx13452, idx10731, idx10732, idx10733, idx10734]))+(((0.5*depsilonv4_dD4[idx13369, idx10731, idx10732, idx10733, idx10734, idx13479, idx13480, idx13481, idx13482])*D[idx13481, idx13482])*D[idx13479, idx13480]))

	println("[Evaluating 'tmpRes_88', Komplexität 6(4, 0)]")
	@tullio tmpRes_88[idx13369, idx10731, idx10732, idx10733, idx10734] := (((0.5*depsilonv3_dD3[idx13369, idx10731, idx10732, idx10733, idx10734, idx13503, idx13504])*Identity_ord4_dm3333[idx13503, idx13504, idx13511, idx13512])*D[idx13511, idx13512])

	println("[Evaluating 'tmpRes_89', Komplexität 18(10, 6)]")
	@tullio tmpRes_89[idx13369, idx10731, idx10732, idx10733, idx10734] := (((tmpRes_87[idx13369, idx10731, idx10732, idx10733, idx10734]+tmpRes_88[idx13369, idx10731, idx10732, idx10733, idx10734])+(((0.5*depsilonv3_dD3[idx13369, idx10731, idx10732, idx13527, idx13528, idx13529, idx13530])*D[idx13529, idx13530])*Identity_ord4_dm3333[idx13527, idx13528, idx10733, idx10734]))+(((0.5*depsilonv3_dD3[idx13369, idx10731, idx10732, idx10733, idx10734, idx13555, idx13556])*Identity_ord4_dm3333[idx13555, idx13556, idx13563, idx13564])*D[idx13563, idx13564]))

	println("[Evaluating 'tmpRes_90', Komplexität 16(8, 6)]")
	@tullio tmpRes_90[idx13369, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_89[idx13369, idx10731, idx10732, idx10733, idx10734]+(((0.5*depsilonv2_dD2[idx13369, idx10731, idx10732, idx13577, idx13578])*zeros_ord6_dm333333[idx13577, idx13578, idx10733, idx10734, idx13589, idx13590])*D[idx13589, idx13590]))+(((0.5*depsilonv2_dD2[idx13369, idx10731, idx10732, idx13603, idx13604])*Identity_ord4_dm3333[idx13603, idx13604, idx13611, idx13612])*Identity_ord4_dm3333[idx13611, idx13612, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_91', Komplexität 16(8, 6)]")
	@tullio tmpRes_91[idx13369, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_90[idx13369, idx10731, idx10732, idx10733, idx10734]+(((0.5*depsilonv3_dD3[idx13369, idx10731, idx10732, idx13631, idx13632, idx13633, idx13634])*D[idx13633, idx13634])*Identity_ord4_dm3333[idx13631, idx13632, idx10733, idx10734]))+(((0.5*depsilonv2_dD2[idx13369, idx10731, idx10732, idx13655, idx13656])*Identity_ord4_dm3333[idx13655, idx13656, idx13663, idx13664])*Identity_ord4_dm3333[idx13663, idx13664, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_92', Komplexität 18(8, 8)]")
	@tullio tmpRes_92[idx13369, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_91[idx13369, idx10731, idx10732, idx10733, idx10734]+(((0.5*depsilonv2_dD2[idx13369, idx13679, idx13680, idx13681, idx13682])*D[idx13681, idx13682])*zeros_ord6_dm333333[idx13679, idx13680, idx10731, idx10732, idx10733, idx10734]))+((((0.166667*depsilonv5_dD5[idx13369, idx10731, idx10732, idx10733, idx10734, idx13715, idx13716, idx13717, idx13718, idx13719, idx13720])*D[idx13719, idx13720])*D[idx13717, idx13718])*D[idx13715, idx13716]))

	println("[Evaluating 'tmpRes_93', Komplexität 20(10, 8)]")
	@tullio tmpRes_93[idx13369, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_92[idx13369, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv4_dD4[idx13369, idx10731, idx10732, idx10733, idx10734, idx13747, idx13748, idx13749, idx13750])*Identity_ord4_dm3333[idx13749, idx13750, idx13757, idx13758])*D[idx13757, idx13758])*D[idx13747, idx13748]))+((((0.166667*depsilonv4_dD4[idx13369, idx10731, idx10732, idx10733, idx10734, idx13781, idx13782, idx13783, idx13784])*D[idx13783, idx13784])*Identity_ord4_dm3333[idx13781, idx13782, idx13795, idx13796])*D[idx13795, idx13796]))

	println("[Evaluating 'tmpRes_94', Komplexität 20(10, 8)]")
	@tullio tmpRes_94[idx13369, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_93[idx13369, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv4_dD4[idx13369, idx10731, idx10732, idx13813, idx13814, idx13815, idx13816, idx13817, idx13818])*D[idx13817, idx13818])*D[idx13815, idx13816])*Identity_ord4_dm3333[idx13813, idx13814, idx10733, idx10734]))+((((0.166667*depsilonv4_dD4[idx13369, idx10731, idx10732, idx10733, idx10734, idx13849, idx13850, idx13851, idx13852])*Identity_ord4_dm3333[idx13851, idx13852, idx13859, idx13860])*D[idx13859, idx13860])*D[idx13849, idx13850]))

	println("[Evaluating 'tmpRes_95', Komplexität 20(10, 8)]")
	@tullio tmpRes_95[idx13369, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_94[idx13369, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv3_dD3[idx13369, idx10731, idx10732, idx10733, idx10734, idx13881, idx13882])*zeros_ord6_dm333333[idx13881, idx13882, idx13891, idx13892, idx13893, idx13894])*D[idx13893, idx13894])*D[idx13891, idx13892]))+((((0.166667*depsilonv3_dD3[idx13369, idx10731, idx10732, idx10733, idx10734, idx13915, idx13916])*Identity_ord4_dm3333[idx13915, idx13916, idx13923, idx13924])*Identity_ord4_dm3333[idx13923, idx13924, idx13931, idx13932])*D[idx13931, idx13932]))

	println("[Evaluating 'tmpRes_96', Komplexität 20(10, 8)]")
	@tullio tmpRes_96[idx13369, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_95[idx13369, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv3_dD3[idx13369, idx10731, idx10732, idx13947, idx13948, idx13949, idx13950])*Identity_ord4_dm3333[idx13949, idx13950, idx13957, idx13958])*D[idx13957, idx13958])*Identity_ord4_dm3333[idx13947, idx13948, idx10733, idx10734]))+((((0.166667*depsilonv4_dD4[idx13369, idx10731, idx10732, idx10733, idx10734, idx13985, idx13986, idx13987, idx13988])*D[idx13987, idx13988])*Identity_ord4_dm3333[idx13985, idx13986, idx13999, idx14000])*D[idx13999, idx14000]))

	println("[Evaluating 'tmpRes_97', Komplexität 20(10, 8)]")
	@tullio tmpRes_97[idx13369, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_96[idx13369, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv3_dD3[idx13369, idx10731, idx10732, idx10733, idx10734, idx14017, idx14018])*Identity_ord4_dm3333[idx14017, idx14018, idx14025, idx14026])*Identity_ord4_dm3333[idx14025, idx14026, idx14033, idx14034])*D[idx14033, idx14034]))+((((0.166667*depsilonv3_dD3[idx13369, idx10731, idx10732, idx14049, idx14050, idx14051, idx14052])*D[idx14051, idx14052])*zeros_ord6_dm333333[idx14049, idx14050, idx10733, idx10734, idx14067, idx14068])*D[idx14067, idx14068]))

	println("[Evaluating 'tmpRes_98', Komplexität 20(10, 8)]")
	@tullio tmpRes_98[idx13369, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_97[idx13369, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv3_dD3[idx13369, idx10731, idx10732, idx14083, idx14084, idx14085, idx14086])*D[idx14085, idx14086])*Identity_ord4_dm3333[idx14083, idx14084, idx14097, idx14098])*Identity_ord4_dm3333[idx14097, idx14098, idx10733, idx10734]))+((((0.166667*depsilonv4_dD4[idx13369, idx10731, idx10732, idx14119, idx14120, idx14121, idx14122, idx14123, idx14124])*D[idx14123, idx14124])*D[idx14121, idx14122])*Identity_ord4_dm3333[idx14119, idx14120, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_99', Komplexität 20(10, 8)]")
	@tullio tmpRes_99[idx13369, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_98[idx13369, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv3_dD3[idx13369, idx10731, idx10732, idx14151, idx14152, idx14153, idx14154])*Identity_ord4_dm3333[idx14153, idx14154, idx14161, idx14162])*D[idx14161, idx14162])*Identity_ord4_dm3333[idx14151, idx14152, idx10733, idx10734]))+((((0.166667*depsilonv3_dD3[idx13369, idx10731, idx10732, idx14185, idx14186, idx14187, idx14188])*D[idx14187, idx14188])*Identity_ord4_dm3333[idx14185, idx14186, idx14199, idx14200])*Identity_ord4_dm3333[idx14199, idx14200, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_100', Komplexität 16(2, 12)]")
	@tullio tmpRes_100[idx8862, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((zeros_ord4_dm3333[idx8862, idx10729, idx10730, idx13369]+Identity_ord4_dm3333[idx8862, idx10729, idx10730, idx13369])*(zeros_ord5_dm33333[idx13369, idx10731, idx10732, idx10733, idx10734]-(tmpRes_99[idx13369, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv3_dD3[idx13369, idx14217, idx14218, idx14219, idx14220, idx14221, idx14222])*D[idx14221, idx14222])*D[idx14219, idx14220])*zeros_ord6_dm333333[idx14217, idx14218, idx10731, idx10732, idx10733, idx10734]))))

	println("[Evaluating 'tmpRes_101', Komplexität 20(14, 4)]")
	@tullio tmpRes_101[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((((1*depsilonv3_dD3[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734])+((1*depsilonv4_dD4[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734, idx14295, idx14296])*D[idx14295, idx14296]))+((1*depsilonv3_dD3[idx14253, idx10729, idx10730, idx10731, idx10732, idx14313, idx14314])*Identity_ord4_dm3333[idx14313, idx14314, idx10733, idx10734]))+((1*depsilonv3_dD3[idx14253, idx10729, idx10730, idx10731, idx10732, idx14335, idx14336])*Identity_ord4_dm3333[idx14335, idx14336, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_102', Komplexität 18(12, 4)]")
	@tullio tmpRes_102[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := (((tmpRes_101[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+((1*depsilonv2_dD2[idx14253, idx10729, idx10730, idx14353, idx14354])*zeros_ord6_dm333333[idx14353, idx14354, idx10731, idx10732, idx10733, idx10734]))+((1*depsilonv3_dD3[idx14253, idx10729, idx10730, idx10731, idx10732, idx14379, idx14380])*Identity_ord4_dm3333[idx14379, idx14380, idx10733, idx10734]))+((1*depsilonv2_dD2[idx14253, idx10729, idx10730, idx14397, idx14398])*zeros_ord6_dm333333[idx14397, idx14398, idx10731, idx10732, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_103', Komplexität 20(12, 6)]")
	@tullio tmpRes_103[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := (((tmpRes_102[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+((1*depsilonv2_dD2[idx14253, idx10729, idx10730, idx14419, idx14420])*zeros_ord6_dm333333[idx14419, idx14420, idx10731, idx10732, idx10733, idx10734]))+((1*depsilonv1_dD1[idx14253, idx14437, idx14438])*zeros_ord8_dm33333333[idx14437, idx14438, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]))+(((0.5*depsilonv5_dD5[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734, idx14473, idx14474, idx14475, idx14476])*D[idx14475, idx14476])*D[idx14473, idx14474]))

	println("[Evaluating 'tmpRes_104', Komplexität 16(8, 6)]")
	@tullio tmpRes_104[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_103[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+(((0.5*depsilonv4_dD4[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734, idx14501, idx14502])*Identity_ord4_dm3333[idx14501, idx14502, idx14509, idx14510])*D[idx14509, idx14510]))+(((0.5*depsilonv4_dD4[idx14253, idx10729, idx10730, idx10731, idx10732, idx14529, idx14530, idx14531, idx14532])*D[idx14531, idx14532])*Identity_ord4_dm3333[idx14529, idx14530, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_105', Komplexität 16(8, 6)]")
	@tullio tmpRes_105[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_104[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+(((0.5*depsilonv4_dD4[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734, idx14561, idx14562])*Identity_ord4_dm3333[idx14561, idx14562, idx14569, idx14570])*D[idx14569, idx14570]))+(((0.5*depsilonv3_dD3[idx14253, idx10729, idx10730, idx10731, idx10732, idx14587, idx14588])*zeros_ord6_dm333333[idx14587, idx14588, idx10733, idx10734, idx14599, idx14600])*D[idx14599, idx14600]))

	println("[Evaluating 'tmpRes_106', Komplexität 16(8, 6)]")
	@tullio tmpRes_106[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_105[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+(((0.5*depsilonv3_dD3[idx14253, idx10729, idx10730, idx10731, idx10732, idx14617, idx14618])*Identity_ord4_dm3333[idx14617, idx14618, idx14625, idx14626])*Identity_ord4_dm3333[idx14625, idx14626, idx10733, idx10734]))+(((0.5*depsilonv4_dD4[idx14253, idx10729, idx10730, idx10731, idx10732, idx14649, idx14650, idx14651, idx14652])*D[idx14651, idx14652])*Identity_ord4_dm3333[idx14649, idx14650, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_107', Komplexität 16(8, 6)]")
	@tullio tmpRes_107[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_106[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+(((0.5*depsilonv3_dD3[idx14253, idx10729, idx10730, idx10731, idx10732, idx14677, idx14678])*Identity_ord4_dm3333[idx14677, idx14678, idx14685, idx14686])*Identity_ord4_dm3333[idx14685, idx14686, idx10733, idx10734]))+(((0.5*depsilonv3_dD3[idx14253, idx10729, idx10730, idx14705, idx14706, idx14707, idx14708])*D[idx14707, idx14708])*zeros_ord6_dm333333[idx14705, idx14706, idx10731, idx10732, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_108', Komplexität 16(8, 6)]")
	@tullio tmpRes_108[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_107[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+(((0.5*depsilonv4_dD4[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734, idx14741, idx14742])*Identity_ord4_dm3333[idx14741, idx14742, idx14749, idx14750])*D[idx14749, idx14750]))+(((0.5*depsilonv3_dD3[idx14253, idx10729, idx10730, idx10731, idx10732, idx14767, idx14768])*zeros_ord6_dm333333[idx14767, idx14768, idx10733, idx10734, idx14779, idx14780])*D[idx14779, idx14780]))

	println("[Evaluating 'tmpRes_109', Komplexität 16(8, 6)]")
	@tullio tmpRes_109[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_108[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+(((0.5*depsilonv3_dD3[idx14253, idx10729, idx10730, idx10731, idx10732, idx14797, idx14798])*Identity_ord4_dm3333[idx14797, idx14798, idx14805, idx14806])*Identity_ord4_dm3333[idx14805, idx14806, idx10733, idx10734]))+(((0.5*depsilonv3_dD3[idx14253, idx10729, idx10730, idx10731, idx10732, idx14827, idx14828])*zeros_ord6_dm333333[idx14827, idx14828, idx10733, idx10734, idx14839, idx14840])*D[idx14839, idx14840]))

	println("[Evaluating 'tmpRes_110', Komplexität 16(8, 6)]")
	@tullio tmpRes_110[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_109[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+(((0.5*depsilonv2_dD2[idx14253, idx10729, idx10730, idx14853, idx14854])*zeros_ord8_dm33333333[idx14853, idx14854, idx10731, idx10732, idx10733, idx10734, idx14869, idx14870])*D[idx14869, idx14870]))+(((0.5*depsilonv2_dD2[idx14253, idx10729, idx10730, idx14883, idx14884])*zeros_ord6_dm333333[idx14883, idx14884, idx10731, idx10732, idx14895, idx14896])*Identity_ord4_dm3333[idx14895, idx14896, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_111', Komplexität 16(8, 6)]")
	@tullio tmpRes_111[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_110[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+(((0.5*depsilonv3_dD3[idx14253, idx10729, idx10730, idx10731, idx10732, idx14917, idx14918])*Identity_ord4_dm3333[idx14917, idx14918, idx14925, idx14926])*Identity_ord4_dm3333[idx14925, idx14926, idx10733, idx10734]))+(((0.5*depsilonv2_dD2[idx14253, idx10729, idx10730, idx14943, idx14944])*zeros_ord6_dm333333[idx14943, idx14944, idx10731, idx10732, idx14955, idx14956])*Identity_ord4_dm3333[idx14955, idx14956, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_112', Komplexität 16(8, 6)]")
	@tullio tmpRes_112[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_111[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+(((0.5*depsilonv2_dD2[idx14253, idx10729, idx10730, idx14973, idx14974])*Identity_ord4_dm3333[idx14973, idx14974, idx14981, idx14982])*zeros_ord6_dm333333[idx14981, idx14982, idx10731, idx10732, idx10733, idx10734]))+(((0.5*depsilonv4_dD4[idx14253, idx10729, idx10730, idx10731, idx10732, idx15009, idx15010, idx15011, idx15012])*D[idx15011, idx15012])*Identity_ord4_dm3333[idx15009, idx15010, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_113', Komplexität 16(8, 6)]")
	@tullio tmpRes_113[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_112[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+(((0.5*depsilonv3_dD3[idx14253, idx10729, idx10730, idx10731, idx10732, idx15037, idx15038])*Identity_ord4_dm3333[idx15037, idx15038, idx15045, idx15046])*Identity_ord4_dm3333[idx15045, idx15046, idx10733, idx10734]))+(((0.5*depsilonv3_dD3[idx14253, idx10729, idx10730, idx15065, idx15066, idx15067, idx15068])*D[idx15067, idx15068])*zeros_ord6_dm333333[idx15065, idx15066, idx10731, idx10732, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_114', Komplexität 16(8, 6)]")
	@tullio tmpRes_114[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_113[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+(((0.5*depsilonv3_dD3[idx14253, idx10729, idx10730, idx10731, idx10732, idx15097, idx15098])*Identity_ord4_dm3333[idx15097, idx15098, idx15105, idx15106])*Identity_ord4_dm3333[idx15105, idx15106, idx10733, idx10734]))+(((0.5*depsilonv2_dD2[idx14253, idx10729, idx10730, idx15123, idx15124])*zeros_ord6_dm333333[idx15123, idx15124, idx10731, idx10732, idx15135, idx15136])*Identity_ord4_dm3333[idx15135, idx15136, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_115', Komplexität 16(8, 6)]")
	@tullio tmpRes_115[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_114[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+(((0.5*depsilonv2_dD2[idx14253, idx10729, idx10730, idx15153, idx15154])*Identity_ord4_dm3333[idx15153, idx15154, idx15161, idx15162])*zeros_ord6_dm333333[idx15161, idx15162, idx10731, idx10732, idx10733, idx10734]))+(((0.5*depsilonv3_dD3[idx14253, idx10729, idx10730, idx15185, idx15186, idx15187, idx15188])*D[idx15187, idx15188])*zeros_ord6_dm333333[idx15185, idx15186, idx10731, idx10732, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_116', Komplexität 16(8, 6)]")
	@tullio tmpRes_116[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_115[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+(((0.5*depsilonv2_dD2[idx14253, idx10729, idx10730, idx15213, idx15214])*Identity_ord4_dm3333[idx15213, idx15214, idx15221, idx15222])*zeros_ord6_dm333333[idx15221, idx15222, idx10731, idx10732, idx10733, idx10734]))+(((0.5*depsilonv2_dD2[idx14253, idx15241, idx15242, idx15243, idx15244])*D[idx15243, idx15244])*zeros_ord8_dm33333333[idx15241, idx15242, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_117', Komplexität 20(10, 8)]")
	@tullio tmpRes_117[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_116[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv6_dD6[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734, idx15285, idx15286, idx15287, idx15288, idx15289, idx15290])*D[idx15289, idx15290])*D[idx15287, idx15288])*D[idx15285, idx15286]))+((((0.166667*depsilonv5_dD5[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734, idx15321, idx15322, idx15323, idx15324])*Identity_ord4_dm3333[idx15323, idx15324, idx15331, idx15332])*D[idx15331, idx15332])*D[idx15321, idx15322]))

	println("[Evaluating 'tmpRes_118', Komplexität 20(10, 8)]")
	@tullio tmpRes_118[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_117[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv5_dD5[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734, idx15359, idx15360, idx15361, idx15362])*D[idx15361, idx15362])*Identity_ord4_dm3333[idx15359, idx15360, idx15373, idx15374])*D[idx15373, idx15374]))+((((0.166667*depsilonv5_dD5[idx14253, idx10729, idx10730, idx10731, idx10732, idx15395, idx15396, idx15397, idx15398, idx15399, idx15400])*D[idx15399, idx15400])*D[idx15397, idx15398])*Identity_ord4_dm3333[idx15395, idx15396, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_119', Komplexität 20(10, 8)]")
	@tullio tmpRes_119[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_118[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv5_dD5[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734, idx15435, idx15436, idx15437, idx15438])*Identity_ord4_dm3333[idx15437, idx15438, idx15445, idx15446])*D[idx15445, idx15446])*D[idx15435, idx15436]))+((((0.166667*depsilonv4_dD4[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734, idx15471, idx15472])*zeros_ord6_dm333333[idx15471, idx15472, idx15481, idx15482, idx15483, idx15484])*D[idx15483, idx15484])*D[idx15481, idx15482]))

	println("[Evaluating 'tmpRes_120', Komplexität 20(10, 8)]")
	@tullio tmpRes_120[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_119[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv4_dD4[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734, idx15509, idx15510])*Identity_ord4_dm3333[idx15509, idx15510, idx15517, idx15518])*Identity_ord4_dm3333[idx15517, idx15518, idx15525, idx15526])*D[idx15525, idx15526]))+((((0.166667*depsilonv4_dD4[idx14253, idx10729, idx10730, idx10731, idx10732, idx15545, idx15546, idx15547, idx15548])*Identity_ord4_dm3333[idx15547, idx15548, idx15555, idx15556])*D[idx15555, idx15556])*Identity_ord4_dm3333[idx15545, idx15546, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_121', Komplexität 20(10, 8)]")
	@tullio tmpRes_121[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_120[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv5_dD5[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734, idx15587, idx15588, idx15589, idx15590])*D[idx15589, idx15590])*Identity_ord4_dm3333[idx15587, idx15588, idx15601, idx15602])*D[idx15601, idx15602]))+((((0.166667*depsilonv4_dD4[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734, idx15623, idx15624])*Identity_ord4_dm3333[idx15623, idx15624, idx15631, idx15632])*Identity_ord4_dm3333[idx15631, idx15632, idx15639, idx15640])*D[idx15639, idx15640]))

	println("[Evaluating 'tmpRes_122', Komplexität 20(10, 8)]")
	@tullio tmpRes_122[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_121[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv4_dD4[idx14253, idx10729, idx10730, idx10731, idx10732, idx15659, idx15660, idx15661, idx15662])*D[idx15661, idx15662])*zeros_ord6_dm333333[idx15659, idx15660, idx10733, idx10734, idx15677, idx15678])*D[idx15677, idx15678]))+((((0.166667*depsilonv4_dD4[idx14253, idx10729, idx10730, idx10731, idx10732, idx15697, idx15698, idx15699, idx15700])*D[idx15699, idx15700])*Identity_ord4_dm3333[idx15697, idx15698, idx15711, idx15712])*Identity_ord4_dm3333[idx15711, idx15712, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_123', Komplexität 20(10, 8)]")
	@tullio tmpRes_123[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_122[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv5_dD5[idx14253, idx10729, idx10730, idx10731, idx10732, idx15737, idx15738, idx15739, idx15740, idx15741, idx15742])*D[idx15741, idx15742])*D[idx15739, idx15740])*Identity_ord4_dm3333[idx15737, idx15738, idx10733, idx10734]))+((((0.166667*depsilonv4_dD4[idx14253, idx10729, idx10730, idx10731, idx10732, idx15773, idx15774, idx15775, idx15776])*Identity_ord4_dm3333[idx15775, idx15776, idx15783, idx15784])*D[idx15783, idx15784])*Identity_ord4_dm3333[idx15773, idx15774, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_124', Komplexität 20(10, 8)]")
	@tullio tmpRes_124[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_123[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv4_dD4[idx14253, idx10729, idx10730, idx10731, idx10732, idx15811, idx15812, idx15813, idx15814])*D[idx15813, idx15814])*Identity_ord4_dm3333[idx15811, idx15812, idx15825, idx15826])*Identity_ord4_dm3333[idx15825, idx15826, idx10733, idx10734]))+((((0.166667*depsilonv4_dD4[idx14253, idx10729, idx10730, idx15847, idx15848, idx15849, idx15850, idx15851, idx15852])*D[idx15851, idx15852])*D[idx15849, idx15850])*zeros_ord6_dm333333[idx15847, idx15848, idx10731, idx10732, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_125', Komplexität 20(10, 8)]")
	@tullio tmpRes_125[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_124[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv5_dD5[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734, idx15891, idx15892, idx15893, idx15894])*Identity_ord4_dm3333[idx15893, idx15894, idx15901, idx15902])*D[idx15901, idx15902])*D[idx15891, idx15892]))+((((0.166667*depsilonv4_dD4[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734, idx15927, idx15928])*zeros_ord6_dm333333[idx15927, idx15928, idx15937, idx15938, idx15939, idx15940])*D[idx15939, idx15940])*D[idx15937, idx15938]))

	println("[Evaluating 'tmpRes_126', Komplexität 20(10, 8)]")
	@tullio tmpRes_126[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_125[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv4_dD4[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734, idx15965, idx15966])*Identity_ord4_dm3333[idx15965, idx15966, idx15973, idx15974])*Identity_ord4_dm3333[idx15973, idx15974, idx15981, idx15982])*D[idx15981, idx15982]))+((((0.166667*depsilonv4_dD4[idx14253, idx10729, idx10730, idx10731, idx10732, idx16001, idx16002, idx16003, idx16004])*Identity_ord4_dm3333[idx16003, idx16004, idx16011, idx16012])*D[idx16011, idx16012])*Identity_ord4_dm3333[idx16001, idx16002, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_127', Komplexität 20(10, 8)]")
	@tullio tmpRes_127[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_126[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv4_dD4[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734, idx16041, idx16042])*zeros_ord6_dm333333[idx16041, idx16042, idx16051, idx16052, idx16053, idx16054])*D[idx16053, idx16054])*D[idx16051, idx16052]))+((((0.166667*depsilonv3_dD3[idx14253, idx10729, idx10730, idx10731, idx10732, idx16075, idx16076])*zeros_ord8_dm33333333[idx16075, idx16076, idx10733, idx10734, idx16089, idx16090, idx16091, idx16092])*D[idx16091, idx16092])*D[idx16089, idx16090]))

	println("[Evaluating 'tmpRes_128', Komplexität 20(10, 8)]")
	@tullio tmpRes_128[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_127[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv3_dD3[idx14253, idx10729, idx10730, idx10731, idx10732, idx16113, idx16114])*zeros_ord6_dm333333[idx16113, idx16114, idx10733, idx10734, idx16125, idx16126])*Identity_ord4_dm3333[idx16125, idx16126, idx16133, idx16134])*D[idx16133, idx16134]))+((((0.166667*depsilonv3_dD3[idx14253, idx10729, idx10730, idx10731, idx10732, idx16151, idx16152])*zeros_ord6_dm333333[idx16151, idx16152, idx16161, idx16162, idx16163, idx16164])*D[idx16163, idx16164])*Identity_ord4_dm3333[idx16161, idx16162, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_129', Komplexität 20(10, 8)]")
	@tullio tmpRes_129[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_128[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv4_dD4[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734, idx16193, idx16194])*Identity_ord4_dm3333[idx16193, idx16194, idx16201, idx16202])*Identity_ord4_dm3333[idx16201, idx16202, idx16209, idx16210])*D[idx16209, idx16210]))+((((0.166667*depsilonv3_dD3[idx14253, idx10729, idx10730, idx10731, idx10732, idx16227, idx16228])*zeros_ord6_dm333333[idx16227, idx16228, idx10733, idx10734, idx16239, idx16240])*Identity_ord4_dm3333[idx16239, idx16240, idx16247, idx16248])*D[idx16247, idx16248]))

	println("[Evaluating 'tmpRes_130', Komplexität 20(10, 8)]")
	@tullio tmpRes_130[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_129[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv3_dD3[idx14253, idx10729, idx10730, idx10731, idx10732, idx16265, idx16266])*Identity_ord4_dm3333[idx16265, idx16266, idx16273, idx16274])*zeros_ord6_dm333333[idx16273, idx16274, idx10733, idx10734, idx16285, idx16286])*D[idx16285, idx16286]))+((((0.166667*depsilonv3_dD3[idx14253, idx10729, idx10730, idx10731, idx10732, idx16303, idx16304])*Identity_ord4_dm3333[idx16303, idx16304, idx16311, idx16312])*Identity_ord4_dm3333[idx16311, idx16312, idx16319, idx16320])*Identity_ord4_dm3333[idx16319, idx16320, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_131', Komplexität 20(10, 8)]")
	@tullio tmpRes_131[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_130[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv4_dD4[idx14253, idx10729, idx10730, idx10731, idx10732, idx16343, idx16344, idx16345, idx16346])*Identity_ord4_dm3333[idx16345, idx16346, idx16353, idx16354])*D[idx16353, idx16354])*Identity_ord4_dm3333[idx16343, idx16344, idx10733, idx10734]))+((((0.166667*depsilonv3_dD3[idx14253, idx10729, idx10730, idx10731, idx10732, idx16379, idx16380])*zeros_ord6_dm333333[idx16379, idx16380, idx16389, idx16390, idx16391, idx16392])*D[idx16391, idx16392])*Identity_ord4_dm3333[idx16389, idx16390, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_132', Komplexität 20(10, 8)]")
	@tullio tmpRes_132[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_131[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv3_dD3[idx14253, idx10729, idx10730, idx10731, idx10732, idx16417, idx16418])*Identity_ord4_dm3333[idx16417, idx16418, idx16425, idx16426])*Identity_ord4_dm3333[idx16425, idx16426, idx16433, idx16434])*Identity_ord4_dm3333[idx16433, idx16434, idx10733, idx10734]))+((((0.166667*depsilonv3_dD3[idx14253, idx10729, idx10730, idx16453, idx16454, idx16455, idx16456])*Identity_ord4_dm3333[idx16455, idx16456, idx16463, idx16464])*D[idx16463, idx16464])*zeros_ord6_dm333333[idx16453, idx16454, idx10731, idx10732, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_133', Komplexität 20(10, 8)]")
	@tullio tmpRes_133[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_132[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv5_dD5[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734, idx16499, idx16500, idx16501, idx16502])*D[idx16501, idx16502])*Identity_ord4_dm3333[idx16499, idx16500, idx16513, idx16514])*D[idx16513, idx16514]))+((((0.166667*depsilonv4_dD4[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734, idx16535, idx16536])*Identity_ord4_dm3333[idx16535, idx16536, idx16543, idx16544])*Identity_ord4_dm3333[idx16543, idx16544, idx16551, idx16552])*D[idx16551, idx16552]))

	println("[Evaluating 'tmpRes_134', Komplexität 20(10, 8)]")
	@tullio tmpRes_134[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_133[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv4_dD4[idx14253, idx10729, idx10730, idx10731, idx10732, idx16571, idx16572, idx16573, idx16574])*D[idx16573, idx16574])*zeros_ord6_dm333333[idx16571, idx16572, idx10733, idx10734, idx16589, idx16590])*D[idx16589, idx16590]))+((((0.166667*depsilonv4_dD4[idx14253, idx10729, idx10730, idx10731, idx10732, idx16609, idx16610, idx16611, idx16612])*D[idx16611, idx16612])*Identity_ord4_dm3333[idx16609, idx16610, idx16623, idx16624])*Identity_ord4_dm3333[idx16623, idx16624, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_135', Komplexität 20(10, 8)]")
	@tullio tmpRes_135[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_134[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv4_dD4[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734, idx16649, idx16650])*Identity_ord4_dm3333[idx16649, idx16650, idx16657, idx16658])*Identity_ord4_dm3333[idx16657, idx16658, idx16665, idx16666])*D[idx16665, idx16666]))+((((0.166667*depsilonv3_dD3[idx14253, idx10729, idx10730, idx10731, idx10732, idx16683, idx16684])*zeros_ord6_dm333333[idx16683, idx16684, idx10733, idx10734, idx16695, idx16696])*Identity_ord4_dm3333[idx16695, idx16696, idx16703, idx16704])*D[idx16703, idx16704]))

	println("[Evaluating 'tmpRes_136', Komplexität 20(10, 8)]")
	@tullio tmpRes_136[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_135[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv3_dD3[idx14253, idx10729, idx10730, idx10731, idx10732, idx16721, idx16722])*Identity_ord4_dm3333[idx16721, idx16722, idx16729, idx16730])*zeros_ord6_dm333333[idx16729, idx16730, idx10733, idx10734, idx16741, idx16742])*D[idx16741, idx16742]))+((((0.166667*depsilonv3_dD3[idx14253, idx10729, idx10730, idx10731, idx10732, idx16759, idx16760])*Identity_ord4_dm3333[idx16759, idx16760, idx16767, idx16768])*Identity_ord4_dm3333[idx16767, idx16768, idx16775, idx16776])*Identity_ord4_dm3333[idx16775, idx16776, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_137', Komplexität 20(10, 8)]")
	@tullio tmpRes_137[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_136[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv4_dD4[idx14253, idx10729, idx10730, idx10731, idx10732, idx16799, idx16800, idx16801, idx16802])*D[idx16801, idx16802])*zeros_ord6_dm333333[idx16799, idx16800, idx10733, idx10734, idx16817, idx16818])*D[idx16817, idx16818]))+((((0.166667*depsilonv3_dD3[idx14253, idx10729, idx10730, idx10731, idx10732, idx16835, idx16836])*Identity_ord4_dm3333[idx16835, idx16836, idx16843, idx16844])*zeros_ord6_dm333333[idx16843, idx16844, idx10733, idx10734, idx16855, idx16856])*D[idx16855, idx16856]))

	println("[Evaluating 'tmpRes_138', Komplexität 20(10, 8)]")
	@tullio tmpRes_138[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_137[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv3_dD3[idx14253, idx10729, idx10730, idx16871, idx16872, idx16873, idx16874])*D[idx16873, idx16874])*zeros_ord8_dm33333333[idx16871, idx16872, idx10731, idx10732, idx10733, idx10734, idx16893, idx16894])*D[idx16893, idx16894]))+((((0.166667*depsilonv3_dD3[idx14253, idx10729, idx10730, idx16909, idx16910, idx16911, idx16912])*D[idx16911, idx16912])*zeros_ord6_dm333333[idx16909, idx16910, idx10731, idx10732, idx16927, idx16928])*Identity_ord4_dm3333[idx16927, idx16928, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_139', Komplexität 20(10, 8)]")
	@tullio tmpRes_139[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_138[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv4_dD4[idx14253, idx10729, idx10730, idx10731, idx10732, idx16951, idx16952, idx16953, idx16954])*D[idx16953, idx16954])*Identity_ord4_dm3333[idx16951, idx16952, idx16965, idx16966])*Identity_ord4_dm3333[idx16965, idx16966, idx10733, idx10734]))+((((0.166667*depsilonv3_dD3[idx14253, idx10729, idx10730, idx10731, idx10732, idx16987, idx16988])*Identity_ord4_dm3333[idx16987, idx16988, idx16995, idx16996])*Identity_ord4_dm3333[idx16995, idx16996, idx17003, idx17004])*Identity_ord4_dm3333[idx17003, idx17004, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_140', Komplexität 20(10, 8)]")
	@tullio tmpRes_140[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_139[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv3_dD3[idx14253, idx10729, idx10730, idx17023, idx17024, idx17025, idx17026])*D[idx17025, idx17026])*zeros_ord6_dm333333[idx17023, idx17024, idx10731, idx10732, idx17041, idx17042])*Identity_ord4_dm3333[idx17041, idx17042, idx10733, idx10734]))+((((0.166667*depsilonv3_dD3[idx14253, idx10729, idx10730, idx17061, idx17062, idx17063, idx17064])*D[idx17063, idx17064])*Identity_ord4_dm3333[idx17061, idx17062, idx17075, idx17076])*zeros_ord6_dm333333[idx17075, idx17076, idx10731, idx10732, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_141', Komplexität 20(10, 8)]")
	@tullio tmpRes_141[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_140[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv5_dD5[idx14253, idx10729, idx10730, idx10731, idx10732, idx17105, idx17106, idx17107, idx17108, idx17109, idx17110])*D[idx17109, idx17110])*D[idx17107, idx17108])*Identity_ord4_dm3333[idx17105, idx17106, idx10733, idx10734]))+((((0.166667*depsilonv4_dD4[idx14253, idx10729, idx10730, idx10731, idx10732, idx17141, idx17142, idx17143, idx17144])*Identity_ord4_dm3333[idx17143, idx17144, idx17151, idx17152])*D[idx17151, idx17152])*Identity_ord4_dm3333[idx17141, idx17142, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_142', Komplexität 20(10, 8)]")
	@tullio tmpRes_142[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_141[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv4_dD4[idx14253, idx10729, idx10730, idx10731, idx10732, idx17179, idx17180, idx17181, idx17182])*D[idx17181, idx17182])*Identity_ord4_dm3333[idx17179, idx17180, idx17193, idx17194])*Identity_ord4_dm3333[idx17193, idx17194, idx10733, idx10734]))+((((0.166667*depsilonv4_dD4[idx14253, idx10729, idx10730, idx17215, idx17216, idx17217, idx17218, idx17219, idx17220])*D[idx17219, idx17220])*D[idx17217, idx17218])*zeros_ord6_dm333333[idx17215, idx17216, idx10731, idx10732, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_143', Komplexität 20(10, 8)]")
	@tullio tmpRes_143[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_142[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv4_dD4[idx14253, idx10729, idx10730, idx10731, idx10732, idx17255, idx17256, idx17257, idx17258])*Identity_ord4_dm3333[idx17257, idx17258, idx17265, idx17266])*D[idx17265, idx17266])*Identity_ord4_dm3333[idx17255, idx17256, idx10733, idx10734]))+((((0.166667*depsilonv3_dD3[idx14253, idx10729, idx10730, idx10731, idx10732, idx17291, idx17292])*zeros_ord6_dm333333[idx17291, idx17292, idx17301, idx17302, idx17303, idx17304])*D[idx17303, idx17304])*Identity_ord4_dm3333[idx17301, idx17302, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_144', Komplexität 20(10, 8)]")
	@tullio tmpRes_144[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_143[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv3_dD3[idx14253, idx10729, idx10730, idx10731, idx10732, idx17329, idx17330])*Identity_ord4_dm3333[idx17329, idx17330, idx17337, idx17338])*Identity_ord4_dm3333[idx17337, idx17338, idx17345, idx17346])*Identity_ord4_dm3333[idx17345, idx17346, idx10733, idx10734]))+((((0.166667*depsilonv3_dD3[idx14253, idx10729, idx10730, idx17365, idx17366, idx17367, idx17368])*Identity_ord4_dm3333[idx17367, idx17368, idx17375, idx17376])*D[idx17375, idx17376])*zeros_ord6_dm333333[idx17365, idx17366, idx10731, idx10732, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_145', Komplexität 20(10, 8)]")
	@tullio tmpRes_145[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_144[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv4_dD4[idx14253, idx10729, idx10730, idx10731, idx10732, idx17407, idx17408, idx17409, idx17410])*D[idx17409, idx17410])*Identity_ord4_dm3333[idx17407, idx17408, idx17421, idx17422])*Identity_ord4_dm3333[idx17421, idx17422, idx10733, idx10734]))+((((0.166667*depsilonv3_dD3[idx14253, idx10729, idx10730, idx10731, idx10732, idx17443, idx17444])*Identity_ord4_dm3333[idx17443, idx17444, idx17451, idx17452])*Identity_ord4_dm3333[idx17451, idx17452, idx17459, idx17460])*Identity_ord4_dm3333[idx17459, idx17460, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_146', Komplexität 20(10, 8)]")
	@tullio tmpRes_146[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_145[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv3_dD3[idx14253, idx10729, idx10730, idx17479, idx17480, idx17481, idx17482])*D[idx17481, idx17482])*zeros_ord6_dm333333[idx17479, idx17480, idx10731, idx10732, idx17497, idx17498])*Identity_ord4_dm3333[idx17497, idx17498, idx10733, idx10734]))+((((0.166667*depsilonv3_dD3[idx14253, idx10729, idx10730, idx17517, idx17518, idx17519, idx17520])*D[idx17519, idx17520])*Identity_ord4_dm3333[idx17517, idx17518, idx17531, idx17532])*zeros_ord6_dm333333[idx17531, idx17532, idx10731, idx10732, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_147', Komplexität 20(10, 8)]")
	@tullio tmpRes_147[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_146[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv4_dD4[idx14253, idx10729, idx10730, idx17557, idx17558, idx17559, idx17560, idx17561, idx17562])*D[idx17561, idx17562])*D[idx17559, idx17560])*zeros_ord6_dm333333[idx17557, idx17558, idx10731, idx10732, idx10733, idx10734]))+((((0.166667*depsilonv3_dD3[idx14253, idx10729, idx10730, idx17593, idx17594, idx17595, idx17596])*Identity_ord4_dm3333[idx17595, idx17596, idx17603, idx17604])*D[idx17603, idx17604])*zeros_ord6_dm333333[idx17593, idx17594, idx10731, idx10732, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_148', Komplexität 20(10, 8)]")
	@tullio tmpRes_148[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((tmpRes_147[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+((((0.166667*depsilonv3_dD3[idx14253, idx10729, idx10730, idx17631, idx17632, idx17633, idx17634])*D[idx17633, idx17634])*Identity_ord4_dm3333[idx17631, idx17632, idx17645, idx17646])*zeros_ord6_dm333333[idx17645, idx17646, idx10731, idx10732, idx10733, idx10734]))+((((0.166667*depsilonv3_dD3[idx14253, idx17667, idx17668, idx17669, idx17670, idx17671, idx17672])*D[idx17671, idx17672])*D[idx17669, idx17670])*zeros_ord8_dm33333333[idx17667, idx17668, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_149', Komplexität 14(12, 0)]")
	@tullio tmpRes_149[idx8862, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((((tmpRes_50[idx8862, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+tmpRes_65[idx8862, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734])+((zeros_ord6_dm333333[idx8862, idx10729, idx10730, idx10731, idx10732, idx12229]+zeros_ord6_dm333333[idx8862, idx10729, idx10730, idx10731, idx10732, idx12229])*(zeros_ord3_dm333[idx12229, idx10733, idx10734]-tmpRes_70[idx12229, idx10733, idx10734])))+tmpRes_85[idx8862, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734])+tmpRes_100[idx8862, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734])

	println("[Evaluating 'tmpRes_150', Komplexität 6(2, 2)]")
	@tullio tmpRes_150[idx8862, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734] := ((E0[idx8862, idx14253]+D[idx8862, idx14253])*(zeros_ord7_dm3333333[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]-tmpRes_148[idx14253, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]))

	println("[Evaluating 'tmpRes_151', Komplexität 10(0, 8)]")
	@tullio tmpRes_151[idx8862] := (tmpRes_9[idx8862]+(((0.5*(tmpRes_22[idx8862, idx9257, idx9258, idx9259, idx9260]+tmpRes_37[idx8862, idx9257, idx9258, idx9259, idx9260]))*D[idx9259, idx9260])*D[idx9257, idx9258]))

	println("[Evaluating 'tmpRes_152', Komplexität 10(8, 0)]")
	@tullio tmpRes_152[idx8862] := ((((0.166667*(tmpRes_149[idx8862, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]+tmpRes_150[idx8862, idx10729, idx10730, idx10731, idx10732, idx10733, idx10734]))*D[idx10733, idx10734])*D[idx10731, idx10732])*D[idx10729, idx10730])

	println("[Evaluating final Result, Komplexität 2(0, 0)]")
	@tullio res[idx8862] := (tmpRes_151[idx8862]+tmpRes_152[idx8862])

	return res

end

start_time = time()
res = autodiff_sigmaTSM_t(rand(), rand(3, 3), rand(3, 3), rand(3), rand(3), rand(3, 3, 3), rand(3, 3, 3, 3, 3), rand(3, 3, 3, 3, 3, 3, 3), rand(3, 3, 3, 3, 3, 3, 3, 3, 3), rand(3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3), rand(3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3))
elapsed = time() - start_time
println("Laufzeit: ", elapsed, " s")
println("Ergebnis: ", res)