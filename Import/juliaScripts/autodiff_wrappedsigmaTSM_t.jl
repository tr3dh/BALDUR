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
# | arg 'depsilonv5_dD5', order [11], dimensions {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3}
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


function autodiff_sigmaTSM_t(E0, D, epsilon, epsilonv, depsilonv1_dD1, depsilonv2_dD2, depsilonv3_dD3, depsilonv4_dD4, depsilonv5_dD5, depsilonv6_dD6)

	@assert size(E0) == (3, 3)
	@assert size(D) == (3, 3)
	@assert length(epsilon) == 3
	@assert length(epsilonv) == 3
	@assert size(depsilonv1_dD1) == (3, 3, 3)
	@assert size(depsilonv2_dD2) == (3, 3, 3, 3, 3)
	@assert size(depsilonv3_dD3) == (3, 3, 3, 3, 3, 3, 3)
	Identity_ord4_dm3333 = create_Identity(3, 3, 3, 3)
	@assert size(depsilonv4_dD4) == (3, 3, 3, 3, 3, 3, 3, 3, 3)
	@assert size(depsilonv5_dD5) == (3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3)
	@assert size(depsilonv6_dD6) == (3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3)

	println("[Ausdruck mit 89 temporären Dependencies substituiert]")

	println("[Evaluating 'tmpRes_0', Komplexität 12(4, 6)]")
	@tullio tmpRes_0[idx5429] := ((epsilonv[idx5429] + (depsilonv1_dD1[idx5429, idx5442, idx5443] * D[idx5442, idx5443])) + (((0.5 * depsilonv2_dD2[idx5429, idx5453, idx5454, idx5455, idx5456]) * D[idx5455, idx5456]) * D[idx5453, idx5454]))

	println("[Evaluating 'tmpRes_1', Komplexität 8(6, 0)]")
	@tullio tmpRes_1[idx5429] := ((((0.166667 * depsilonv3_dD3[idx5429, idx5473, idx5474, idx5475, idx5476, idx5477, idx5478]) * D[idx5477, idx5478]) * D[idx5475, idx5476]) * D[idx5473, idx5474])

	println("[Evaluating 'tmpRes_2', Komplexität 12(4, 6)]")
	@tullio tmpRes_2[idx5557] := ((epsilonv[idx5557] + (depsilonv1_dD1[idx5557, idx5504, idx5505] * D[idx5504, idx5505])) + (((0.5 * depsilonv2_dD2[idx5557, idx5515, idx5516, idx5517, idx5518]) * D[idx5517, idx5518]) * D[idx5515, idx5516]))

	println("[Evaluating 'tmpRes_3', Komplexität 8(6, 0)]")
	@tullio tmpRes_3[idx5557] := ((((0.166667 * depsilonv3_dD3[idx5557, idx5535, idx5536, idx5537, idx5538, idx5539, idx5540]) * D[idx5539, idx5540]) * D[idx5537, idx5538]) * D[idx5535, idx5536])

	println("[Evaluating 'tmpRes_4', Komplexität 16(8, 6)]")
	@tullio tmpRes_4[idx5563, idx5555, idx5556] := (((depsilonv1_dD1[idx5563, idx5555, idx5556] + (depsilonv2_dD2[idx5563, idx5555, idx5556, idx5581, idx5582] * D[idx5581, idx5582])) + (depsilonv1_dD1[idx5563, idx5596, idx5597] * Identity_ord4_dm3333[idx5596, idx5597, idx5555, idx5556])) + (((0.5 * depsilonv3_dD3[idx5563, idx5555, idx5556, idx5612, idx5613, idx5614, idx5615]) * D[idx5614, idx5615]) * D[idx5612, idx5613]))

	println("[Evaluating 'tmpRes_5', Komplexität 16(8, 6)]")
	@tullio tmpRes_5[idx5563, idx5555, idx5556] := ((tmpRes_4[idx5563, idx5555, idx5556] + (((0.5 * depsilonv2_dD2[idx5563, idx5555, idx5556, idx5632, idx5633]) * Identity_ord4_dm3333[idx5632, idx5633, idx5640, idx5641]) * D[idx5640, idx5641])) + (((0.5 * depsilonv2_dD2[idx5563, idx5652, idx5653, idx5654, idx5655]) * D[idx5654, idx5655]) * Identity_ord4_dm3333[idx5652, idx5653, idx5555, idx5556]))

	println("[Evaluating 'tmpRes_6', Komplexität 20(10, 8)]")
	@tullio tmpRes_6[idx5563, idx5555, idx5556] := ((tmpRes_5[idx5563, idx5555, idx5556] + ((((0.166667 * depsilonv4_dD4[idx5563, idx5555, idx5556, idx5680, idx5681, idx5682, idx5683, idx5684, idx5685]) * D[idx5684, idx5685]) * D[idx5682, idx5683]) * D[idx5680, idx5681])) + ((((0.166667 * depsilonv3_dD3[idx5563, idx5555, idx5556, idx5708, idx5709, idx5710, idx5711]) * Identity_ord4_dm3333[idx5710, idx5711, idx5718, idx5719]) * D[idx5718, idx5719]) * D[idx5708, idx5709]))

	println("[Evaluating 'tmpRes_7', Komplexität 20(10, 8)]")
	@tullio tmpRes_7[idx5563, idx5555, idx5556] := ((tmpRes_6[idx5563, idx5555, idx5556] + ((((0.166667 * depsilonv3_dD3[idx5563, idx5555, idx5556, idx5738, idx5739, idx5740, idx5741]) * D[idx5740, idx5741]) * Identity_ord4_dm3333[idx5738, idx5739, idx5752, idx5753]) * D[idx5752, idx5753])) + ((((0.166667 * depsilonv3_dD3[idx5563, idx5766, idx5767, idx5768, idx5769, idx5770, idx5771]) * D[idx5770, idx5771]) * D[idx5768, idx5769]) * Identity_ord4_dm3333[idx5766, idx5767, idx5555, idx5556]))

	println("[Evaluating 'tmpRes_8', Komplexität 16(14, 0)]")
	@tullio tmpRes_8[idx5428] := (((Identity_ord4_dm3333[idx5428, idx5555, idx5556, idx5557] * (epsilon[idx5557] - (tmpRes_2[idx5557] + tmpRes_3[idx5557]))) + ((E0[idx5428, idx5563] + D[idx5428, idx5563]) * (-1 * tmpRes_7[idx5563, idx5555, idx5556]))) * D[idx5555, idx5556])

	println("[Evaluating 'tmpRes_9', Komplexität 16(8, 6)]")
	@tullio tmpRes_9[idx6019, idx5814, idx5815] := (((depsilonv1_dD1[idx6019, idx5814, idx5815] + (depsilonv2_dD2[idx6019, idx5814, idx5815, idx5809, idx5810] * D[idx5809, idx5810])) + (depsilonv1_dD1[idx6019, idx5824, idx5825] * Identity_ord4_dm3333[idx5824, idx5825, idx5814, idx5815])) + (((0.5 * depsilonv3_dD3[idx6019, idx5814, idx5815, idx5840, idx5841, idx5842, idx5843]) * D[idx5842, idx5843]) * D[idx5840, idx5841]))

	println("[Evaluating 'tmpRes_10', Komplexität 16(8, 6)]")
	@tullio tmpRes_10[idx6019, idx5814, idx5815] := ((tmpRes_9[idx6019, idx5814, idx5815] + (((0.5 * depsilonv2_dD2[idx6019, idx5814, idx5815, idx5860, idx5861]) * Identity_ord4_dm3333[idx5860, idx5861, idx5868, idx5869]) * D[idx5868, idx5869])) + (((0.5 * depsilonv2_dD2[idx6019, idx5880, idx5881, idx5882, idx5883]) * D[idx5882, idx5883]) * Identity_ord4_dm3333[idx5880, idx5881, idx5814, idx5815]))

	println("[Evaluating 'tmpRes_11', Komplexität 20(10, 8)]")
	@tullio tmpRes_11[idx6019, idx5814, idx5815] := ((tmpRes_10[idx6019, idx5814, idx5815] + ((((0.166667 * depsilonv4_dD4[idx6019, idx5814, idx5815, idx5908, idx5909, idx5910, idx5911, idx5912, idx5913]) * D[idx5912, idx5913]) * D[idx5910, idx5911]) * D[idx5908, idx5909])) + ((((0.166667 * depsilonv3_dD3[idx6019, idx5814, idx5815, idx5936, idx5937, idx5938, idx5939]) * Identity_ord4_dm3333[idx5938, idx5939, idx5946, idx5947]) * D[idx5946, idx5947]) * D[idx5936, idx5937]))

	println("[Evaluating 'tmpRes_12', Komplexität 20(10, 8)]")
	@tullio tmpRes_12[idx6019, idx5814, idx5815] := ((tmpRes_11[idx6019, idx5814, idx5815] + ((((0.166667 * depsilonv3_dD3[idx6019, idx5814, idx5815, idx5966, idx5967, idx5968, idx5969]) * D[idx5968, idx5969]) * Identity_ord4_dm3333[idx5966, idx5967, idx5980, idx5981]) * D[idx5980, idx5981])) + ((((0.166667 * depsilonv3_dD3[idx6019, idx5994, idx5995, idx5996, idx5997, idx5998, idx5999]) * D[idx5998, idx5999]) * D[idx5996, idx5997]) * Identity_ord4_dm3333[idx5994, idx5995, idx5814, idx5815]))

	println("[Evaluating 'tmpRes_13', Komplexität 16(8, 6)]")
	@tullio tmpRes_13[idx6247, idx5814, idx5815] := (((depsilonv1_dD1[idx6247, idx5814, idx5815] + (depsilonv2_dD2[idx6247, idx5814, idx5815, idx6037, idx6038] * D[idx6037, idx6038])) + (depsilonv1_dD1[idx6247, idx6052, idx6053] * Identity_ord4_dm3333[idx6052, idx6053, idx5814, idx5815])) + (((0.5 * depsilonv3_dD3[idx6247, idx5814, idx5815, idx6068, idx6069, idx6070, idx6071]) * D[idx6070, idx6071]) * D[idx6068, idx6069]))

	println("[Evaluating 'tmpRes_14', Komplexität 16(8, 6)]")
	@tullio tmpRes_14[idx6247, idx5814, idx5815] := ((tmpRes_13[idx6247, idx5814, idx5815] + (((0.5 * depsilonv2_dD2[idx6247, idx5814, idx5815, idx6088, idx6089]) * Identity_ord4_dm3333[idx6088, idx6089, idx6096, idx6097]) * D[idx6096, idx6097])) + (((0.5 * depsilonv2_dD2[idx6247, idx6108, idx6109, idx6110, idx6111]) * D[idx6110, idx6111]) * Identity_ord4_dm3333[idx6108, idx6109, idx5814, idx5815]))

	println("[Evaluating 'tmpRes_15', Komplexität 20(10, 8)]")
	@tullio tmpRes_15[idx6247, idx5814, idx5815] := ((tmpRes_14[idx6247, idx5814, idx5815] + ((((0.166667 * depsilonv4_dD4[idx6247, idx5814, idx5815, idx6136, idx6137, idx6138, idx6139, idx6140, idx6141]) * D[idx6140, idx6141]) * D[idx6138, idx6139]) * D[idx6136, idx6137])) + ((((0.166667 * depsilonv3_dD3[idx6247, idx5814, idx5815, idx6164, idx6165, idx6166, idx6167]) * Identity_ord4_dm3333[idx6166, idx6167, idx6174, idx6175]) * D[idx6174, idx6175]) * D[idx6164, idx6165]))

	println("[Evaluating 'tmpRes_16', Komplexität 20(10, 8)]")
	@tullio tmpRes_16[idx6247, idx5814, idx5815] := ((tmpRes_15[idx6247, idx5814, idx5815] + ((((0.166667 * depsilonv3_dD3[idx6247, idx5814, idx5815, idx6194, idx6195, idx6196, idx6197]) * D[idx6196, idx6197]) * Identity_ord4_dm3333[idx6194, idx6195, idx6208, idx6209]) * D[idx6208, idx6209])) + ((((0.166667 * depsilonv3_dD3[idx6247, idx6222, idx6223, idx6224, idx6225, idx6226, idx6227]) * D[idx6226, idx6227]) * D[idx6224, idx6225]) * Identity_ord4_dm3333[idx6222, idx6223, idx5814, idx5815]))

	println("[Evaluating 'tmpRes_17', Komplexität 20(12, 6)]")
	@tullio tmpRes_17[idx6253, idx6017, idx6018, idx5814, idx5815] := ((((depsilonv2_dD2[idx6253, idx6017, idx6018, idx5814, idx5815] + (depsilonv3_dD3[idx6253, idx6017, idx6018, idx5814, idx5815, idx6277, idx6278] * D[idx6277, idx6278])) + (depsilonv2_dD2[idx6253, idx6017, idx6018, idx6298, idx6299] * Identity_ord4_dm3333[idx6298, idx6299, idx5814, idx5815])) + (depsilonv2_dD2[idx6253, idx6017, idx6018, idx6316, idx6317] * Identity_ord4_dm3333[idx6316, idx6317, idx5814, idx5815])) + (((0.5 * depsilonv4_dD4[idx6253, idx6017, idx6018, idx5814, idx5815, idx6336, idx6337, idx6338, idx6339]) * D[idx6338, idx6339]) * D[idx6336, idx6337]))

	println("[Evaluating 'tmpRes_18', Komplexität 16(8, 6)]")
	@tullio tmpRes_18[idx6253, idx6017, idx6018, idx5814, idx5815] := ((tmpRes_17[idx6253, idx6017, idx6018, idx5814, idx5815] + (((0.5 * depsilonv3_dD3[idx6253, idx6017, idx6018, idx5814, idx5815, idx6360, idx6361]) * Identity_ord4_dm3333[idx6360, idx6361, idx6368, idx6369]) * D[idx6368, idx6369])) + (((0.5 * depsilonv3_dD3[idx6253, idx6017, idx6018, idx6384, idx6385, idx6386, idx6387]) * D[idx6386, idx6387]) * Identity_ord4_dm3333[idx6384, idx6385, idx5814, idx5815]))

	println("[Evaluating 'tmpRes_19', Komplexität 16(8, 6)]")
	@tullio tmpRes_19[idx6253, idx6017, idx6018, idx5814, idx5815] := ((tmpRes_18[idx6253, idx6017, idx6018, idx5814, idx5815] + (((0.5 * depsilonv3_dD3[idx6253, idx6017, idx6018, idx5814, idx5815, idx6412, idx6413]) * Identity_ord4_dm3333[idx6412, idx6413, idx6420, idx6421]) * D[idx6420, idx6421])) + (((0.5 * depsilonv2_dD2[idx6253, idx6017, idx6018, idx6434, idx6435]) * Identity_ord4_dm3333[idx6434, idx6435, idx6442, idx6443]) * Identity_ord4_dm3333[idx6442, idx6443, idx5814, idx5815]))

	println("[Evaluating 'tmpRes_20', Komplexität 16(8, 6)]")
	@tullio tmpRes_20[idx6253, idx6017, idx6018, idx5814, idx5815] := ((tmpRes_19[idx6253, idx6017, idx6018, idx5814, idx5815] + (((0.5 * depsilonv3_dD3[idx6253, idx6017, idx6018, idx6462, idx6463, idx6464, idx6465]) * D[idx6464, idx6465]) * Identity_ord4_dm3333[idx6462, idx6463, idx5814, idx5815])) + (((0.5 * depsilonv2_dD2[idx6253, idx6017, idx6018, idx6486, idx6487]) * Identity_ord4_dm3333[idx6486, idx6487, idx6494, idx6495]) * Identity_ord4_dm3333[idx6494, idx6495, idx5814, idx5815]))

	println("[Evaluating 'tmpRes_21', Komplexität 20(10, 8)]")
	@tullio tmpRes_21[idx6253, idx6017, idx6018, idx5814, idx5815] := ((tmpRes_20[idx6253, idx6017, idx6018, idx5814, idx5815] + ((((0.166667 * depsilonv5_dD5[idx6253, idx6017, idx6018, idx5814, idx5815, idx6520, idx6521, idx6522, idx6523, idx6524, idx6525]) * D[idx6524, idx6525]) * D[idx6522, idx6523]) * D[idx6520, idx6521])) + ((((0.166667 * depsilonv4_dD4[idx6253, idx6017, idx6018, idx5814, idx5815, idx6552, idx6553, idx6554, idx6555]) * Identity_ord4_dm3333[idx6554, idx6555, idx6562, idx6563]) * D[idx6562, idx6563]) * D[idx6552, idx6553]))

	println("[Evaluating 'tmpRes_22', Komplexität 20(10, 8)]")
	@tullio tmpRes_22[idx6253, idx6017, idx6018, idx5814, idx5815] := ((tmpRes_21[idx6253, idx6017, idx6018, idx5814, idx5815] + ((((0.166667 * depsilonv4_dD4[idx6253, idx6017, idx6018, idx5814, idx5815, idx6586, idx6587, idx6588, idx6589]) * D[idx6588, idx6589]) * Identity_ord4_dm3333[idx6586, idx6587, idx6600, idx6601]) * D[idx6600, idx6601])) + ((((0.166667 * depsilonv4_dD4[idx6253, idx6017, idx6018, idx6618, idx6619, idx6620, idx6621, idx6622, idx6623]) * D[idx6622, idx6623]) * D[idx6620, idx6621]) * Identity_ord4_dm3333[idx6618, idx6619, idx5814, idx5815]))

	println("[Evaluating 'tmpRes_23', Komplexität 20(10, 8)]")
	@tullio tmpRes_23[idx6253, idx6017, idx6018, idx5814, idx5815] := ((tmpRes_22[idx6253, idx6017, idx6018, idx5814, idx5815] + ((((0.166667 * depsilonv4_dD4[idx6253, idx6017, idx6018, idx5814, idx5815, idx6654, idx6655, idx6656, idx6657]) * Identity_ord4_dm3333[idx6656, idx6657, idx6664, idx6665]) * D[idx6664, idx6665]) * D[idx6654, idx6655])) + ((((0.166667 * depsilonv3_dD3[idx6253, idx6017, idx6018, idx5814, idx5815, idx6686, idx6687]) * Identity_ord4_dm3333[idx6686, idx6687, idx6694, idx6695]) * Identity_ord4_dm3333[idx6694, idx6695, idx6702, idx6703]) * D[idx6702, idx6703]))

	println("[Evaluating 'tmpRes_24', Komplexität 20(10, 8)]")
	@tullio tmpRes_24[idx6253, idx6017, idx6018, idx5814, idx5815] := ((tmpRes_23[idx6253, idx6017, idx6018, idx5814, idx5815] + ((((0.166667 * depsilonv3_dD3[idx6253, idx6017, idx6018, idx6718, idx6719, idx6720, idx6721]) * Identity_ord4_dm3333[idx6720, idx6721, idx6728, idx6729]) * D[idx6728, idx6729]) * Identity_ord4_dm3333[idx6718, idx6719, idx5814, idx5815])) + ((((0.166667 * depsilonv4_dD4[idx6253, idx6017, idx6018, idx5814, idx5815, idx6756, idx6757, idx6758, idx6759]) * D[idx6758, idx6759]) * Identity_ord4_dm3333[idx6756, idx6757, idx6770, idx6771]) * D[idx6770, idx6771]))

	println("[Evaluating 'tmpRes_25', Komplexität 20(10, 8)]")
	@tullio tmpRes_25[idx6253, idx6017, idx6018, idx5814, idx5815] := ((tmpRes_24[idx6253, idx6017, idx6018, idx5814, idx5815] + ((((0.166667 * depsilonv3_dD3[idx6253, idx6017, idx6018, idx5814, idx5815, idx6788, idx6789]) * Identity_ord4_dm3333[idx6788, idx6789, idx6796, idx6797]) * Identity_ord4_dm3333[idx6796, idx6797, idx6804, idx6805]) * D[idx6804, idx6805])) + ((((0.166667 * depsilonv3_dD3[idx6253, idx6017, idx6018, idx6820, idx6821, idx6822, idx6823]) * D[idx6822, idx6823]) * Identity_ord4_dm3333[idx6820, idx6821, idx6834, idx6835]) * Identity_ord4_dm3333[idx6834, idx6835, idx5814, idx5815]))

	println("[Evaluating 'tmpRes_26', Komplexität 20(10, 8)]")
	@tullio tmpRes_26[idx6253, idx6017, idx6018, idx5814, idx5815] := ((tmpRes_25[idx6253, idx6017, idx6018, idx5814, idx5815] + ((((0.166667 * depsilonv4_dD4[idx6253, idx6017, idx6018, idx6856, idx6857, idx6858, idx6859, idx6860, idx6861]) * D[idx6860, idx6861]) * D[idx6858, idx6859]) * Identity_ord4_dm3333[idx6856, idx6857, idx5814, idx5815])) + ((((0.166667 * depsilonv3_dD3[idx6253, idx6017, idx6018, idx6888, idx6889, idx6890, idx6891]) * Identity_ord4_dm3333[idx6890, idx6891, idx6898, idx6899]) * D[idx6898, idx6899]) * Identity_ord4_dm3333[idx6888, idx6889, idx5814, idx5815]))

	println("[Evaluating 'tmpRes_27', Komplexität 16(2, 12)]")
	@tullio tmpRes_27[idx5428, idx6017, idx6018, idx5814, idx5815] := ((E0[idx5428, idx6253] + D[idx5428, idx6253]) * (-1 * (tmpRes_26[idx6253, idx6017, idx6018, idx5814, idx5815] + ((((0.166667 * depsilonv3_dD3[idx6253, idx6017, idx6018, idx6922, idx6923, idx6924, idx6925]) * D[idx6924, idx6925]) * Identity_ord4_dm3333[idx6922, idx6923, idx6936, idx6937]) * Identity_ord4_dm3333[idx6936, idx6937, idx5814, idx5815]))))

	println("[Evaluating 'tmpRes_28', Komplexität 16(14, 0)]")
	@tullio tmpRes_28[idx5428, idx6017, idx6018] := ((0.5 * (((Identity_ord4_dm3333[idx5428, idx6017, idx6018, idx6019] * (-1 * tmpRes_12[idx6019, idx5814, idx5815])) + (Identity_ord4_dm3333[idx5428, idx6017, idx6018, idx6247] * (-1 * tmpRes_16[idx6247, idx5814, idx5815]))) + tmpRes_27[idx5428, idx6017, idx6018, idx5814, idx5815])) * D[idx5814, idx5815])

	println("[Evaluating 'tmpRes_29', Komplexität 20(12, 6)]")
	@tullio tmpRes_29[idx7649, idx6982, idx6983, idx6984, idx6985] := ((((depsilonv2_dD2[idx7649, idx6982, idx6983, idx6984, idx6985] + (depsilonv3_dD3[idx7649, idx6982, idx6983, idx6984, idx6985, idx6977, idx6978] * D[idx6977, idx6978])) + (depsilonv2_dD2[idx7649, idx6982, idx6983, idx6998, idx6999] * Identity_ord4_dm3333[idx6998, idx6999, idx6984, idx6985])) + (depsilonv2_dD2[idx7649, idx6982, idx6983, idx7016, idx7017] * Identity_ord4_dm3333[idx7016, idx7017, idx6984, idx6985])) + (((0.5 * depsilonv4_dD4[idx7649, idx6982, idx6983, idx6984, idx6985, idx7036, idx7037, idx7038, idx7039]) * D[idx7038, idx7039]) * D[idx7036, idx7037]))

	println("[Evaluating 'tmpRes_30', Komplexität 16(8, 6)]")
	@tullio tmpRes_30[idx7649, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_29[idx7649, idx6982, idx6983, idx6984, idx6985] + (((0.5 * depsilonv3_dD3[idx7649, idx6982, idx6983, idx6984, idx6985, idx7060, idx7061]) * Identity_ord4_dm3333[idx7060, idx7061, idx7068, idx7069]) * D[idx7068, idx7069])) + (((0.5 * depsilonv3_dD3[idx7649, idx6982, idx6983, idx7084, idx7085, idx7086, idx7087]) * D[idx7086, idx7087]) * Identity_ord4_dm3333[idx7084, idx7085, idx6984, idx6985]))

	println("[Evaluating 'tmpRes_31', Komplexität 16(8, 6)]")
	@tullio tmpRes_31[idx7649, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_30[idx7649, idx6982, idx6983, idx6984, idx6985] + (((0.5 * depsilonv3_dD3[idx7649, idx6982, idx6983, idx6984, idx6985, idx7112, idx7113]) * Identity_ord4_dm3333[idx7112, idx7113, idx7120, idx7121]) * D[idx7120, idx7121])) + (((0.5 * depsilonv2_dD2[idx7649, idx6982, idx6983, idx7134, idx7135]) * Identity_ord4_dm3333[idx7134, idx7135, idx7142, idx7143]) * Identity_ord4_dm3333[idx7142, idx7143, idx6984, idx6985]))

	println("[Evaluating 'tmpRes_32', Komplexität 16(8, 6)]")
	@tullio tmpRes_32[idx7649, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_31[idx7649, idx6982, idx6983, idx6984, idx6985] + (((0.5 * depsilonv3_dD3[idx7649, idx6982, idx6983, idx7162, idx7163, idx7164, idx7165]) * D[idx7164, idx7165]) * Identity_ord4_dm3333[idx7162, idx7163, idx6984, idx6985])) + (((0.5 * depsilonv2_dD2[idx7649, idx6982, idx6983, idx7186, idx7187]) * Identity_ord4_dm3333[idx7186, idx7187, idx7194, idx7195]) * Identity_ord4_dm3333[idx7194, idx7195, idx6984, idx6985]))

	println("[Evaluating 'tmpRes_33', Komplexität 20(10, 8)]")
	@tullio tmpRes_33[idx7649, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_32[idx7649, idx6982, idx6983, idx6984, idx6985] + ((((0.166667 * depsilonv5_dD5[idx7649, idx6982, idx6983, idx6984, idx6985, idx7220, idx7221, idx7222, idx7223, idx7224, idx7225]) * D[idx7224, idx7225]) * D[idx7222, idx7223]) * D[idx7220, idx7221])) + ((((0.166667 * depsilonv4_dD4[idx7649, idx6982, idx6983, idx6984, idx6985, idx7252, idx7253, idx7254, idx7255]) * Identity_ord4_dm3333[idx7254, idx7255, idx7262, idx7263]) * D[idx7262, idx7263]) * D[idx7252, idx7253]))

	println("[Evaluating 'tmpRes_34', Komplexität 20(10, 8)]")
	@tullio tmpRes_34[idx7649, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_33[idx7649, idx6982, idx6983, idx6984, idx6985] + ((((0.166667 * depsilonv4_dD4[idx7649, idx6982, idx6983, idx6984, idx6985, idx7286, idx7287, idx7288, idx7289]) * D[idx7288, idx7289]) * Identity_ord4_dm3333[idx7286, idx7287, idx7300, idx7301]) * D[idx7300, idx7301])) + ((((0.166667 * depsilonv4_dD4[idx7649, idx6982, idx6983, idx7318, idx7319, idx7320, idx7321, idx7322, idx7323]) * D[idx7322, idx7323]) * D[idx7320, idx7321]) * Identity_ord4_dm3333[idx7318, idx7319, idx6984, idx6985]))

	println("[Evaluating 'tmpRes_35', Komplexität 20(10, 8)]")
	@tullio tmpRes_35[idx7649, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_34[idx7649, idx6982, idx6983, idx6984, idx6985] + ((((0.166667 * depsilonv4_dD4[idx7649, idx6982, idx6983, idx6984, idx6985, idx7354, idx7355, idx7356, idx7357]) * Identity_ord4_dm3333[idx7356, idx7357, idx7364, idx7365]) * D[idx7364, idx7365]) * D[idx7354, idx7355])) + ((((0.166667 * depsilonv3_dD3[idx7649, idx6982, idx6983, idx6984, idx6985, idx7386, idx7387]) * Identity_ord4_dm3333[idx7386, idx7387, idx7394, idx7395]) * Identity_ord4_dm3333[idx7394, idx7395, idx7402, idx7403]) * D[idx7402, idx7403]))

	println("[Evaluating 'tmpRes_36', Komplexität 20(10, 8)]")
	@tullio tmpRes_36[idx7649, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_35[idx7649, idx6982, idx6983, idx6984, idx6985] + ((((0.166667 * depsilonv3_dD3[idx7649, idx6982, idx6983, idx7418, idx7419, idx7420, idx7421]) * Identity_ord4_dm3333[idx7420, idx7421, idx7428, idx7429]) * D[idx7428, idx7429]) * Identity_ord4_dm3333[idx7418, idx7419, idx6984, idx6985])) + ((((0.166667 * depsilonv4_dD4[idx7649, idx6982, idx6983, idx6984, idx6985, idx7456, idx7457, idx7458, idx7459]) * D[idx7458, idx7459]) * Identity_ord4_dm3333[idx7456, idx7457, idx7470, idx7471]) * D[idx7470, idx7471]))

	println("[Evaluating 'tmpRes_37', Komplexität 20(10, 8)]")
	@tullio tmpRes_37[idx7649, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_36[idx7649, idx6982, idx6983, idx6984, idx6985] + ((((0.166667 * depsilonv3_dD3[idx7649, idx6982, idx6983, idx6984, idx6985, idx7488, idx7489]) * Identity_ord4_dm3333[idx7488, idx7489, idx7496, idx7497]) * Identity_ord4_dm3333[idx7496, idx7497, idx7504, idx7505]) * D[idx7504, idx7505])) + ((((0.166667 * depsilonv3_dD3[idx7649, idx6982, idx6983, idx7520, idx7521, idx7522, idx7523]) * D[idx7522, idx7523]) * Identity_ord4_dm3333[idx7520, idx7521, idx7534, idx7535]) * Identity_ord4_dm3333[idx7534, idx7535, idx6984, idx6985]))

	println("[Evaluating 'tmpRes_38', Komplexität 20(10, 8)]")
	@tullio tmpRes_38[idx7649, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_37[idx7649, idx6982, idx6983, idx6984, idx6985] + ((((0.166667 * depsilonv4_dD4[idx7649, idx6982, idx6983, idx7556, idx7557, idx7558, idx7559, idx7560, idx7561]) * D[idx7560, idx7561]) * D[idx7558, idx7559]) * Identity_ord4_dm3333[idx7556, idx7557, idx6984, idx6985])) + ((((0.166667 * depsilonv3_dD3[idx7649, idx6982, idx6983, idx7588, idx7589, idx7590, idx7591]) * Identity_ord4_dm3333[idx7590, idx7591, idx7598, idx7599]) * D[idx7598, idx7599]) * Identity_ord4_dm3333[idx7588, idx7589, idx6984, idx6985]))

	println("[Evaluating 'tmpRes_39', Komplexität 20(12, 6)]")
	@tullio tmpRes_39[idx8345, idx6982, idx6983, idx6984, idx6985] := ((((depsilonv2_dD2[idx8345, idx6982, idx6983, idx6984, idx6985] + (depsilonv3_dD3[idx8345, idx6982, idx6983, idx6984, idx6985, idx7673, idx7674] * D[idx7673, idx7674])) + (depsilonv2_dD2[idx8345, idx6982, idx6983, idx7694, idx7695] * Identity_ord4_dm3333[idx7694, idx7695, idx6984, idx6985])) + (depsilonv2_dD2[idx8345, idx6982, idx6983, idx7712, idx7713] * Identity_ord4_dm3333[idx7712, idx7713, idx6984, idx6985])) + (((0.5 * depsilonv4_dD4[idx8345, idx6982, idx6983, idx6984, idx6985, idx7732, idx7733, idx7734, idx7735]) * D[idx7734, idx7735]) * D[idx7732, idx7733]))

	println("[Evaluating 'tmpRes_40', Komplexität 16(8, 6)]")
	@tullio tmpRes_40[idx8345, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_39[idx8345, idx6982, idx6983, idx6984, idx6985] + (((0.5 * depsilonv3_dD3[idx8345, idx6982, idx6983, idx6984, idx6985, idx7756, idx7757]) * Identity_ord4_dm3333[idx7756, idx7757, idx7764, idx7765]) * D[idx7764, idx7765])) + (((0.5 * depsilonv3_dD3[idx8345, idx6982, idx6983, idx7780, idx7781, idx7782, idx7783]) * D[idx7782, idx7783]) * Identity_ord4_dm3333[idx7780, idx7781, idx6984, idx6985]))

	println("[Evaluating 'tmpRes_41', Komplexität 16(8, 6)]")
	@tullio tmpRes_41[idx8345, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_40[idx8345, idx6982, idx6983, idx6984, idx6985] + (((0.5 * depsilonv3_dD3[idx8345, idx6982, idx6983, idx6984, idx6985, idx7808, idx7809]) * Identity_ord4_dm3333[idx7808, idx7809, idx7816, idx7817]) * D[idx7816, idx7817])) + (((0.5 * depsilonv2_dD2[idx8345, idx6982, idx6983, idx7830, idx7831]) * Identity_ord4_dm3333[idx7830, idx7831, idx7838, idx7839]) * Identity_ord4_dm3333[idx7838, idx7839, idx6984, idx6985]))

	println("[Evaluating 'tmpRes_42', Komplexität 16(8, 6)]")
	@tullio tmpRes_42[idx8345, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_41[idx8345, idx6982, idx6983, idx6984, idx6985] + (((0.5 * depsilonv3_dD3[idx8345, idx6982, idx6983, idx7858, idx7859, idx7860, idx7861]) * D[idx7860, idx7861]) * Identity_ord4_dm3333[idx7858, idx7859, idx6984, idx6985])) + (((0.5 * depsilonv2_dD2[idx8345, idx6982, idx6983, idx7882, idx7883]) * Identity_ord4_dm3333[idx7882, idx7883, idx7890, idx7891]) * Identity_ord4_dm3333[idx7890, idx7891, idx6984, idx6985]))

	println("[Evaluating 'tmpRes_43', Komplexität 20(10, 8)]")
	@tullio tmpRes_43[idx8345, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_42[idx8345, idx6982, idx6983, idx6984, idx6985] + ((((0.166667 * depsilonv5_dD5[idx8345, idx6982, idx6983, idx6984, idx6985, idx7916, idx7917, idx7918, idx7919, idx7920, idx7921]) * D[idx7920, idx7921]) * D[idx7918, idx7919]) * D[idx7916, idx7917])) + ((((0.166667 * depsilonv4_dD4[idx8345, idx6982, idx6983, idx6984, idx6985, idx7948, idx7949, idx7950, idx7951]) * Identity_ord4_dm3333[idx7950, idx7951, idx7958, idx7959]) * D[idx7958, idx7959]) * D[idx7948, idx7949]))

	println("[Evaluating 'tmpRes_44', Komplexität 20(10, 8)]")
	@tullio tmpRes_44[idx8345, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_43[idx8345, idx6982, idx6983, idx6984, idx6985] + ((((0.166667 * depsilonv4_dD4[idx8345, idx6982, idx6983, idx6984, idx6985, idx7982, idx7983, idx7984, idx7985]) * D[idx7984, idx7985]) * Identity_ord4_dm3333[idx7982, idx7983, idx7996, idx7997]) * D[idx7996, idx7997])) + ((((0.166667 * depsilonv4_dD4[idx8345, idx6982, idx6983, idx8014, idx8015, idx8016, idx8017, idx8018, idx8019]) * D[idx8018, idx8019]) * D[idx8016, idx8017]) * Identity_ord4_dm3333[idx8014, idx8015, idx6984, idx6985]))

	println("[Evaluating 'tmpRes_45', Komplexität 20(10, 8)]")
	@tullio tmpRes_45[idx8345, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_44[idx8345, idx6982, idx6983, idx6984, idx6985] + ((((0.166667 * depsilonv4_dD4[idx8345, idx6982, idx6983, idx6984, idx6985, idx8050, idx8051, idx8052, idx8053]) * Identity_ord4_dm3333[idx8052, idx8053, idx8060, idx8061]) * D[idx8060, idx8061]) * D[idx8050, idx8051])) + ((((0.166667 * depsilonv3_dD3[idx8345, idx6982, idx6983, idx6984, idx6985, idx8082, idx8083]) * Identity_ord4_dm3333[idx8082, idx8083, idx8090, idx8091]) * Identity_ord4_dm3333[idx8090, idx8091, idx8098, idx8099]) * D[idx8098, idx8099]))

	println("[Evaluating 'tmpRes_46', Komplexität 20(10, 8)]")
	@tullio tmpRes_46[idx8345, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_45[idx8345, idx6982, idx6983, idx6984, idx6985] + ((((0.166667 * depsilonv3_dD3[idx8345, idx6982, idx6983, idx8114, idx8115, idx8116, idx8117]) * Identity_ord4_dm3333[idx8116, idx8117, idx8124, idx8125]) * D[idx8124, idx8125]) * Identity_ord4_dm3333[idx8114, idx8115, idx6984, idx6985])) + ((((0.166667 * depsilonv4_dD4[idx8345, idx6982, idx6983, idx6984, idx6985, idx8152, idx8153, idx8154, idx8155]) * D[idx8154, idx8155]) * Identity_ord4_dm3333[idx8152, idx8153, idx8166, idx8167]) * D[idx8166, idx8167]))

	println("[Evaluating 'tmpRes_47', Komplexität 20(10, 8)]")
	@tullio tmpRes_47[idx8345, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_46[idx8345, idx6982, idx6983, idx6984, idx6985] + ((((0.166667 * depsilonv3_dD3[idx8345, idx6982, idx6983, idx6984, idx6985, idx8184, idx8185]) * Identity_ord4_dm3333[idx8184, idx8185, idx8192, idx8193]) * Identity_ord4_dm3333[idx8192, idx8193, idx8200, idx8201]) * D[idx8200, idx8201])) + ((((0.166667 * depsilonv3_dD3[idx8345, idx6982, idx6983, idx8216, idx8217, idx8218, idx8219]) * D[idx8218, idx8219]) * Identity_ord4_dm3333[idx8216, idx8217, idx8230, idx8231]) * Identity_ord4_dm3333[idx8230, idx8231, idx6984, idx6985]))

	println("[Evaluating 'tmpRes_48', Komplexität 20(10, 8)]")
	@tullio tmpRes_48[idx8345, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_47[idx8345, idx6982, idx6983, idx6984, idx6985] + ((((0.166667 * depsilonv4_dD4[idx8345, idx6982, idx6983, idx8252, idx8253, idx8254, idx8255, idx8256, idx8257]) * D[idx8256, idx8257]) * D[idx8254, idx8255]) * Identity_ord4_dm3333[idx8252, idx8253, idx6984, idx6985])) + ((((0.166667 * depsilonv3_dD3[idx8345, idx6982, idx6983, idx8284, idx8285, idx8286, idx8287]) * Identity_ord4_dm3333[idx8286, idx8287, idx8294, idx8295]) * D[idx8294, idx8295]) * Identity_ord4_dm3333[idx8284, idx8285, idx6984, idx6985]))

	println("[Evaluating 'tmpRes_49', Komplexität 14(0, 12)]")
	@tullio tmpRes_49[idx5428, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] := (Identity_ord4_dm3333[idx5428, idx7647, idx7648, idx7649] * (-1 * (tmpRes_38[idx7649, idx6982, idx6983, idx6984, idx6985] + ((((0.166667 * depsilonv3_dD3[idx7649, idx6982, idx6983, idx7622, idx7623, idx7624, idx7625]) * D[idx7624, idx7625]) * Identity_ord4_dm3333[idx7622, idx7623, idx7636, idx7637]) * Identity_ord4_dm3333[idx7636, idx7637, idx6984, idx6985]))))

	println("[Evaluating 'tmpRes_50', Komplexität 14(0, 12)]")
	@tullio tmpRes_50[idx5428, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] := (Identity_ord4_dm3333[idx5428, idx7647, idx7648, idx8345] * (-1 * (tmpRes_48[idx8345, idx6982, idx6983, idx6984, idx6985] + ((((0.166667 * depsilonv3_dD3[idx8345, idx6982, idx6983, idx8318, idx8319, idx8320, idx8321]) * D[idx8320, idx8321]) * Identity_ord4_dm3333[idx8318, idx8319, idx8332, idx8333]) * Identity_ord4_dm3333[idx8332, idx8333, idx6984, idx6985]))))

	println("[Evaluating 'tmpRes_51', Komplexität 20(12, 6)]")
	@tullio tmpRes_51[idx9041, idx6982, idx6983, idx6984, idx6985] := ((((depsilonv2_dD2[idx9041, idx6982, idx6983, idx6984, idx6985] + (depsilonv3_dD3[idx9041, idx6982, idx6983, idx6984, idx6985, idx8369, idx8370] * D[idx8369, idx8370])) + (depsilonv2_dD2[idx9041, idx6982, idx6983, idx8390, idx8391] * Identity_ord4_dm3333[idx8390, idx8391, idx6984, idx6985])) + (depsilonv2_dD2[idx9041, idx6982, idx6983, idx8408, idx8409] * Identity_ord4_dm3333[idx8408, idx8409, idx6984, idx6985])) + (((0.5 * depsilonv4_dD4[idx9041, idx6982, idx6983, idx6984, idx6985, idx8428, idx8429, idx8430, idx8431]) * D[idx8430, idx8431]) * D[idx8428, idx8429]))

	println("[Evaluating 'tmpRes_52', Komplexität 16(8, 6)]")
	@tullio tmpRes_52[idx9041, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_51[idx9041, idx6982, idx6983, idx6984, idx6985] + (((0.5 * depsilonv3_dD3[idx9041, idx6982, idx6983, idx6984, idx6985, idx8452, idx8453]) * Identity_ord4_dm3333[idx8452, idx8453, idx8460, idx8461]) * D[idx8460, idx8461])) + (((0.5 * depsilonv3_dD3[idx9041, idx6982, idx6983, idx8476, idx8477, idx8478, idx8479]) * D[idx8478, idx8479]) * Identity_ord4_dm3333[idx8476, idx8477, idx6984, idx6985]))

	println("[Evaluating 'tmpRes_53', Komplexität 16(8, 6)]")
	@tullio tmpRes_53[idx9041, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_52[idx9041, idx6982, idx6983, idx6984, idx6985] + (((0.5 * depsilonv3_dD3[idx9041, idx6982, idx6983, idx6984, idx6985, idx8504, idx8505]) * Identity_ord4_dm3333[idx8504, idx8505, idx8512, idx8513]) * D[idx8512, idx8513])) + (((0.5 * depsilonv2_dD2[idx9041, idx6982, idx6983, idx8526, idx8527]) * Identity_ord4_dm3333[idx8526, idx8527, idx8534, idx8535]) * Identity_ord4_dm3333[idx8534, idx8535, idx6984, idx6985]))

	println("[Evaluating 'tmpRes_54', Komplexität 16(8, 6)]")
	@tullio tmpRes_54[idx9041, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_53[idx9041, idx6982, idx6983, idx6984, idx6985] + (((0.5 * depsilonv3_dD3[idx9041, idx6982, idx6983, idx8554, idx8555, idx8556, idx8557]) * D[idx8556, idx8557]) * Identity_ord4_dm3333[idx8554, idx8555, idx6984, idx6985])) + (((0.5 * depsilonv2_dD2[idx9041, idx6982, idx6983, idx8578, idx8579]) * Identity_ord4_dm3333[idx8578, idx8579, idx8586, idx8587]) * Identity_ord4_dm3333[idx8586, idx8587, idx6984, idx6985]))

	println("[Evaluating 'tmpRes_55', Komplexität 20(10, 8)]")
	@tullio tmpRes_55[idx9041, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_54[idx9041, idx6982, idx6983, idx6984, idx6985] + ((((0.166667 * depsilonv5_dD5[idx9041, idx6982, idx6983, idx6984, idx6985, idx8612, idx8613, idx8614, idx8615, idx8616, idx8617]) * D[idx8616, idx8617]) * D[idx8614, idx8615]) * D[idx8612, idx8613])) + ((((0.166667 * depsilonv4_dD4[idx9041, idx6982, idx6983, idx6984, idx6985, idx8644, idx8645, idx8646, idx8647]) * Identity_ord4_dm3333[idx8646, idx8647, idx8654, idx8655]) * D[idx8654, idx8655]) * D[idx8644, idx8645]))

	println("[Evaluating 'tmpRes_56', Komplexität 20(10, 8)]")
	@tullio tmpRes_56[idx9041, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_55[idx9041, idx6982, idx6983, idx6984, idx6985] + ((((0.166667 * depsilonv4_dD4[idx9041, idx6982, idx6983, idx6984, idx6985, idx8678, idx8679, idx8680, idx8681]) * D[idx8680, idx8681]) * Identity_ord4_dm3333[idx8678, idx8679, idx8692, idx8693]) * D[idx8692, idx8693])) + ((((0.166667 * depsilonv4_dD4[idx9041, idx6982, idx6983, idx8710, idx8711, idx8712, idx8713, idx8714, idx8715]) * D[idx8714, idx8715]) * D[idx8712, idx8713]) * Identity_ord4_dm3333[idx8710, idx8711, idx6984, idx6985]))

	println("[Evaluating 'tmpRes_57', Komplexität 20(10, 8)]")
	@tullio tmpRes_57[idx9041, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_56[idx9041, idx6982, idx6983, idx6984, idx6985] + ((((0.166667 * depsilonv4_dD4[idx9041, idx6982, idx6983, idx6984, idx6985, idx8746, idx8747, idx8748, idx8749]) * Identity_ord4_dm3333[idx8748, idx8749, idx8756, idx8757]) * D[idx8756, idx8757]) * D[idx8746, idx8747])) + ((((0.166667 * depsilonv3_dD3[idx9041, idx6982, idx6983, idx6984, idx6985, idx8778, idx8779]) * Identity_ord4_dm3333[idx8778, idx8779, idx8786, idx8787]) * Identity_ord4_dm3333[idx8786, idx8787, idx8794, idx8795]) * D[idx8794, idx8795]))

	println("[Evaluating 'tmpRes_58', Komplexität 20(10, 8)]")
	@tullio tmpRes_58[idx9041, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_57[idx9041, idx6982, idx6983, idx6984, idx6985] + ((((0.166667 * depsilonv3_dD3[idx9041, idx6982, idx6983, idx8810, idx8811, idx8812, idx8813]) * Identity_ord4_dm3333[idx8812, idx8813, idx8820, idx8821]) * D[idx8820, idx8821]) * Identity_ord4_dm3333[idx8810, idx8811, idx6984, idx6985])) + ((((0.166667 * depsilonv4_dD4[idx9041, idx6982, idx6983, idx6984, idx6985, idx8848, idx8849, idx8850, idx8851]) * D[idx8850, idx8851]) * Identity_ord4_dm3333[idx8848, idx8849, idx8862, idx8863]) * D[idx8862, idx8863]))

	println("[Evaluating 'tmpRes_59', Komplexität 20(10, 8)]")
	@tullio tmpRes_59[idx9041, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_58[idx9041, idx6982, idx6983, idx6984, idx6985] + ((((0.166667 * depsilonv3_dD3[idx9041, idx6982, idx6983, idx6984, idx6985, idx8880, idx8881]) * Identity_ord4_dm3333[idx8880, idx8881, idx8888, idx8889]) * Identity_ord4_dm3333[idx8888, idx8889, idx8896, idx8897]) * D[idx8896, idx8897])) + ((((0.166667 * depsilonv3_dD3[idx9041, idx6982, idx6983, idx8912, idx8913, idx8914, idx8915]) * D[idx8914, idx8915]) * Identity_ord4_dm3333[idx8912, idx8913, idx8926, idx8927]) * Identity_ord4_dm3333[idx8926, idx8927, idx6984, idx6985]))

	println("[Evaluating 'tmpRes_60', Komplexität 20(10, 8)]")
	@tullio tmpRes_60[idx9041, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_59[idx9041, idx6982, idx6983, idx6984, idx6985] + ((((0.166667 * depsilonv4_dD4[idx9041, idx6982, idx6983, idx8948, idx8949, idx8950, idx8951, idx8952, idx8953]) * D[idx8952, idx8953]) * D[idx8950, idx8951]) * Identity_ord4_dm3333[idx8948, idx8949, idx6984, idx6985])) + ((((0.166667 * depsilonv3_dD3[idx9041, idx6982, idx6983, idx8980, idx8981, idx8982, idx8983]) * Identity_ord4_dm3333[idx8982, idx8983, idx8990, idx8991]) * D[idx8990, idx8991]) * Identity_ord4_dm3333[idx8980, idx8981, idx6984, idx6985]))

	println("[Evaluating 'tmpRes_61', Komplexität 18(2, 14)]")
	@tullio tmpRes_61[idx5428, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_49[idx5428, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] + tmpRes_50[idx5428, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985]) + (Identity_ord4_dm3333[idx5428, idx7647, idx7648, idx9041] * (-1 * (tmpRes_60[idx9041, idx6982, idx6983, idx6984, idx6985] + ((((0.166667 * depsilonv3_dD3[idx9041, idx6982, idx6983, idx9014, idx9015, idx9016, idx9017]) * D[idx9016, idx9017]) * Identity_ord4_dm3333[idx9014, idx9015, idx9028, idx9029]) * Identity_ord4_dm3333[idx9028, idx9029, idx6984, idx6985])))))

	println("[Evaluating 'tmpRes_62', Komplexität 16(12, 2)]")
	@tullio tmpRes_62[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] := ((((depsilonv3_dD3[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] + (depsilonv4_dD4[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985, idx9077, idx9078] * D[idx9077, idx9078])) + (depsilonv3_dD3[idx9047, idx7647, idx7648, idx6982, idx6983, idx9104, idx9105] * Identity_ord4_dm3333[idx9104, idx9105, idx6984, idx6985])) + (depsilonv3_dD3[idx9047, idx7647, idx7648, idx6982, idx6983, idx9126, idx9127] * Identity_ord4_dm3333[idx9126, idx9127, idx6984, idx6985])) + (depsilonv3_dD3[idx9047, idx7647, idx7648, idx6982, idx6983, idx9148, idx9149] * Identity_ord4_dm3333[idx9148, idx9149, idx6984, idx6985]))

	println("[Evaluating 'tmpRes_63', Komplexität 16(8, 6)]")
	@tullio tmpRes_63[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_62[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] + (((0.5 * depsilonv5_dD5[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985, idx9172, idx9173, idx9174, idx9175]) * D[idx9174, idx9175]) * D[idx9172, idx9173])) + (((0.5 * depsilonv4_dD4[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985, idx9200, idx9201]) * Identity_ord4_dm3333[idx9200, idx9201, idx9208, idx9209]) * D[idx9208, idx9209]))

	println("[Evaluating 'tmpRes_64', Komplexität 16(8, 6)]")
	@tullio tmpRes_64[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_63[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] + (((0.5 * depsilonv4_dD4[idx9047, idx7647, idx7648, idx6982, idx6983, idx9228, idx9229, idx9230, idx9231]) * D[idx9230, idx9231]) * Identity_ord4_dm3333[idx9228, idx9229, idx6984, idx6985])) + (((0.5 * depsilonv4_dD4[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985, idx9260, idx9261]) * Identity_ord4_dm3333[idx9260, idx9261, idx9268, idx9269]) * D[idx9268, idx9269]))

	println("[Evaluating 'tmpRes_65', Komplexität 16(8, 6)]")
	@tullio tmpRes_65[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_64[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] + (((0.5 * depsilonv3_dD3[idx9047, idx7647, idx7648, idx6982, idx6983, idx9286, idx9287]) * Identity_ord4_dm3333[idx9286, idx9287, idx9294, idx9295]) * Identity_ord4_dm3333[idx9294, idx9295, idx6984, idx6985])) + (((0.5 * depsilonv4_dD4[idx9047, idx7647, idx7648, idx6982, idx6983, idx9318, idx9319, idx9320, idx9321]) * D[idx9320, idx9321]) * Identity_ord4_dm3333[idx9318, idx9319, idx6984, idx6985]))

	println("[Evaluating 'tmpRes_66', Komplexität 16(8, 6)]")
	@tullio tmpRes_66[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_65[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] + (((0.5 * depsilonv3_dD3[idx9047, idx7647, idx7648, idx6982, idx6983, idx9346, idx9347]) * Identity_ord4_dm3333[idx9346, idx9347, idx9354, idx9355]) * Identity_ord4_dm3333[idx9354, idx9355, idx6984, idx6985])) + (((0.5 * depsilonv4_dD4[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985, idx9380, idx9381]) * Identity_ord4_dm3333[idx9380, idx9381, idx9388, idx9389]) * D[idx9388, idx9389]))

	println("[Evaluating 'tmpRes_67', Komplexität 16(8, 6)]")
	@tullio tmpRes_67[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_66[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] + (((0.5 * depsilonv3_dD3[idx9047, idx7647, idx7648, idx6982, idx6983, idx9406, idx9407]) * Identity_ord4_dm3333[idx9406, idx9407, idx9414, idx9415]) * Identity_ord4_dm3333[idx9414, idx9415, idx6984, idx6985])) + (((0.5 * depsilonv3_dD3[idx9047, idx7647, idx7648, idx6982, idx6983, idx9436, idx9437]) * Identity_ord4_dm3333[idx9436, idx9437, idx9444, idx9445]) * Identity_ord4_dm3333[idx9444, idx9445, idx6984, idx6985]))

	println("[Evaluating 'tmpRes_68', Komplexität 16(8, 6)]")
	@tullio tmpRes_68[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_67[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] + (((0.5 * depsilonv4_dD4[idx9047, idx7647, idx7648, idx6982, idx6983, idx9468, idx9469, idx9470, idx9471]) * D[idx9470, idx9471]) * Identity_ord4_dm3333[idx9468, idx9469, idx6984, idx6985])) + (((0.5 * depsilonv3_dD3[idx9047, idx7647, idx7648, idx6982, idx6983, idx9496, idx9497]) * Identity_ord4_dm3333[idx9496, idx9497, idx9504, idx9505]) * Identity_ord4_dm3333[idx9504, idx9505, idx6984, idx6985]))

	println("[Evaluating 'tmpRes_69', Komplexität 18(8, 8)]")
	@tullio tmpRes_69[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_68[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] + (((0.5 * depsilonv3_dD3[idx9047, idx7647, idx7648, idx6982, idx6983, idx9526, idx9527]) * Identity_ord4_dm3333[idx9526, idx9527, idx9534, idx9535]) * Identity_ord4_dm3333[idx9534, idx9535, idx6984, idx6985])) + ((((0.166667 * depsilonv6_dD6[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985, idx9564, idx9565, idx9566, idx9567, idx9568, idx9569]) * D[idx9568, idx9569]) * D[idx9566, idx9567]) * D[idx9564, idx9565]))

	println("[Evaluating 'tmpRes_70', Komplexität 20(10, 8)]")
	@tullio tmpRes_70[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_69[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] + ((((0.166667 * depsilonv5_dD5[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985, idx9600, idx9601, idx9602, idx9603]) * Identity_ord4_dm3333[idx9602, idx9603, idx9610, idx9611]) * D[idx9610, idx9611]) * D[idx9600, idx9601])) + ((((0.166667 * depsilonv5_dD5[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985, idx9638, idx9639, idx9640, idx9641]) * D[idx9640, idx9641]) * Identity_ord4_dm3333[idx9638, idx9639, idx9652, idx9653]) * D[idx9652, idx9653]))

	println("[Evaluating 'tmpRes_71', Komplexität 20(10, 8)]")
	@tullio tmpRes_71[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_70[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] + ((((0.166667 * depsilonv5_dD5[idx9047, idx7647, idx7648, idx6982, idx6983, idx9674, idx9675, idx9676, idx9677, idx9678, idx9679]) * D[idx9678, idx9679]) * D[idx9676, idx9677]) * Identity_ord4_dm3333[idx9674, idx9675, idx6984, idx6985])) + ((((0.166667 * depsilonv5_dD5[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985, idx9714, idx9715, idx9716, idx9717]) * Identity_ord4_dm3333[idx9716, idx9717, idx9724, idx9725]) * D[idx9724, idx9725]) * D[idx9714, idx9715]))

	println("[Evaluating 'tmpRes_72', Komplexität 20(10, 8)]")
	@tullio tmpRes_72[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_71[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] + ((((0.166667 * depsilonv4_dD4[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985, idx9750, idx9751]) * Identity_ord4_dm3333[idx9750, idx9751, idx9758, idx9759]) * Identity_ord4_dm3333[idx9758, idx9759, idx9766, idx9767]) * D[idx9766, idx9767])) + ((((0.166667 * depsilonv4_dD4[idx9047, idx7647, idx7648, idx6982, idx6983, idx9786, idx9787, idx9788, idx9789]) * Identity_ord4_dm3333[idx9788, idx9789, idx9796, idx9797]) * D[idx9796, idx9797]) * Identity_ord4_dm3333[idx9786, idx9787, idx6984, idx6985]))

	println("[Evaluating 'tmpRes_73', Komplexität 20(10, 8)]")
	@tullio tmpRes_73[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_72[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] + ((((0.166667 * depsilonv5_dD5[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985, idx9828, idx9829, idx9830, idx9831]) * D[idx9830, idx9831]) * Identity_ord4_dm3333[idx9828, idx9829, idx9842, idx9843]) * D[idx9842, idx9843])) + ((((0.166667 * depsilonv4_dD4[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985, idx9864, idx9865]) * Identity_ord4_dm3333[idx9864, idx9865, idx9872, idx9873]) * Identity_ord4_dm3333[idx9872, idx9873, idx9880, idx9881]) * D[idx9880, idx9881]))

	println("[Evaluating 'tmpRes_74', Komplexität 20(10, 8)]")
	@tullio tmpRes_74[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_73[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] + ((((0.166667 * depsilonv4_dD4[idx9047, idx7647, idx7648, idx6982, idx6983, idx9900, idx9901, idx9902, idx9903]) * D[idx9902, idx9903]) * Identity_ord4_dm3333[idx9900, idx9901, idx9914, idx9915]) * Identity_ord4_dm3333[idx9914, idx9915, idx6984, idx6985])) + ((((0.166667 * depsilonv5_dD5[idx9047, idx7647, idx7648, idx6982, idx6983, idx9940, idx9941, idx9942, idx9943, idx9944, idx9945]) * D[idx9944, idx9945]) * D[idx9942, idx9943]) * Identity_ord4_dm3333[idx9940, idx9941, idx6984, idx6985]))

	println("[Evaluating 'tmpRes_75', Komplexität 20(10, 8)]")
	@tullio tmpRes_75[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_74[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] + ((((0.166667 * depsilonv4_dD4[idx9047, idx7647, idx7648, idx6982, idx6983, idx9976, idx9977, idx9978, idx9979]) * Identity_ord4_dm3333[idx9978, idx9979, idx9986, idx9987]) * D[idx9986, idx9987]) * Identity_ord4_dm3333[idx9976, idx9977, idx6984, idx6985])) + ((((0.166667 * depsilonv4_dD4[idx9047, idx7647, idx7648, idx6982, idx6983, idx10014, idx10015, idx10016, idx10017]) * D[idx10016, idx10017]) * Identity_ord4_dm3333[idx10014, idx10015, idx10028, idx10029]) * Identity_ord4_dm3333[idx10028, idx10029, idx6984, idx6985]))

	println("[Evaluating 'tmpRes_76', Komplexität 20(10, 8)]")
	@tullio tmpRes_76[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_75[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] + ((((0.166667 * depsilonv5_dD5[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985, idx10056, idx10057, idx10058, idx10059]) * Identity_ord4_dm3333[idx10058, idx10059, idx10066, idx10067]) * D[idx10066, idx10067]) * D[idx10056, idx10057])) + ((((0.166667 * depsilonv4_dD4[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985, idx10092, idx10093]) * Identity_ord4_dm3333[idx10092, idx10093, idx10100, idx10101]) * Identity_ord4_dm3333[idx10100, idx10101, idx10108, idx10109]) * D[idx10108, idx10109]))

	println("[Evaluating 'tmpRes_77', Komplexität 20(10, 8)]")
	@tullio tmpRes_77[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_76[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] + ((((0.166667 * depsilonv4_dD4[idx9047, idx7647, idx7648, idx6982, idx6983, idx10128, idx10129, idx10130, idx10131]) * Identity_ord4_dm3333[idx10130, idx10131, idx10138, idx10139]) * D[idx10138, idx10139]) * Identity_ord4_dm3333[idx10128, idx10129, idx6984, idx6985])) + ((((0.166667 * depsilonv4_dD4[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985, idx10168, idx10169]) * Identity_ord4_dm3333[idx10168, idx10169, idx10176, idx10177]) * Identity_ord4_dm3333[idx10176, idx10177, idx10184, idx10185]) * D[idx10184, idx10185]))

	println("[Evaluating 'tmpRes_78', Komplexität 20(10, 8)]")
	@tullio tmpRes_78[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_77[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] + ((((0.166667 * depsilonv3_dD3[idx9047, idx7647, idx7648, idx6982, idx6983, idx10202, idx10203]) * Identity_ord4_dm3333[idx10202, idx10203, idx10210, idx10211]) * Identity_ord4_dm3333[idx10210, idx10211, idx10218, idx10219]) * Identity_ord4_dm3333[idx10218, idx10219, idx6984, idx6985])) + ((((0.166667 * depsilonv4_dD4[idx9047, idx7647, idx7648, idx6982, idx6983, idx10242, idx10243, idx10244, idx10245]) * Identity_ord4_dm3333[idx10244, idx10245, idx10252, idx10253]) * D[idx10252, idx10253]) * Identity_ord4_dm3333[idx10242, idx10243, idx6984, idx6985]))

	println("[Evaluating 'tmpRes_79', Komplexität 20(10, 8)]")
	@tullio tmpRes_79[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_78[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] + ((((0.166667 * depsilonv3_dD3[idx9047, idx7647, idx7648, idx6982, idx6983, idx10278, idx10279]) * Identity_ord4_dm3333[idx10278, idx10279, idx10286, idx10287]) * Identity_ord4_dm3333[idx10286, idx10287, idx10294, idx10295]) * Identity_ord4_dm3333[idx10294, idx10295, idx6984, idx6985])) + ((((0.166667 * depsilonv5_dD5[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985, idx10322, idx10323, idx10324, idx10325]) * D[idx10324, idx10325]) * Identity_ord4_dm3333[idx10322, idx10323, idx10336, idx10337]) * D[idx10336, idx10337]))

	println("[Evaluating 'tmpRes_80', Komplexität 20(10, 8)]")
	@tullio tmpRes_80[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_79[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] + ((((0.166667 * depsilonv4_dD4[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985, idx10358, idx10359]) * Identity_ord4_dm3333[idx10358, idx10359, idx10366, idx10367]) * Identity_ord4_dm3333[idx10366, idx10367, idx10374, idx10375]) * D[idx10374, idx10375])) + ((((0.166667 * depsilonv4_dD4[idx9047, idx7647, idx7648, idx6982, idx6983, idx10394, idx10395, idx10396, idx10397]) * D[idx10396, idx10397]) * Identity_ord4_dm3333[idx10394, idx10395, idx10408, idx10409]) * Identity_ord4_dm3333[idx10408, idx10409, idx6984, idx6985]))

	println("[Evaluating 'tmpRes_81', Komplexität 20(10, 8)]")
	@tullio tmpRes_81[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_80[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] + ((((0.166667 * depsilonv4_dD4[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985, idx10434, idx10435]) * Identity_ord4_dm3333[idx10434, idx10435, idx10442, idx10443]) * Identity_ord4_dm3333[idx10442, idx10443, idx10450, idx10451]) * D[idx10450, idx10451])) + ((((0.166667 * depsilonv3_dD3[idx9047, idx7647, idx7648, idx6982, idx6983, idx10468, idx10469]) * Identity_ord4_dm3333[idx10468, idx10469, idx10476, idx10477]) * Identity_ord4_dm3333[idx10476, idx10477, idx10484, idx10485]) * Identity_ord4_dm3333[idx10484, idx10485, idx6984, idx6985]))

	println("[Evaluating 'tmpRes_82', Komplexität 20(10, 8)]")
	@tullio tmpRes_82[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_81[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] + ((((0.166667 * depsilonv4_dD4[idx9047, idx7647, idx7648, idx6982, idx6983, idx10508, idx10509, idx10510, idx10511]) * D[idx10510, idx10511]) * Identity_ord4_dm3333[idx10508, idx10509, idx10522, idx10523]) * Identity_ord4_dm3333[idx10522, idx10523, idx6984, idx6985])) + ((((0.166667 * depsilonv3_dD3[idx9047, idx7647, idx7648, idx6982, idx6983, idx10544, idx10545]) * Identity_ord4_dm3333[idx10544, idx10545, idx10552, idx10553]) * Identity_ord4_dm3333[idx10552, idx10553, idx10560, idx10561]) * Identity_ord4_dm3333[idx10560, idx10561, idx6984, idx6985]))

	println("[Evaluating 'tmpRes_83', Komplexität 20(10, 8)]")
	@tullio tmpRes_83[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_82[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] + ((((0.166667 * depsilonv5_dD5[idx9047, idx7647, idx7648, idx6982, idx6983, idx10586, idx10587, idx10588, idx10589, idx10590, idx10591]) * D[idx10590, idx10591]) * D[idx10588, idx10589]) * Identity_ord4_dm3333[idx10586, idx10587, idx6984, idx6985])) + ((((0.166667 * depsilonv4_dD4[idx9047, idx7647, idx7648, idx6982, idx6983, idx10622, idx10623, idx10624, idx10625]) * Identity_ord4_dm3333[idx10624, idx10625, idx10632, idx10633]) * D[idx10632, idx10633]) * Identity_ord4_dm3333[idx10622, idx10623, idx6984, idx6985]))

	println("[Evaluating 'tmpRes_84', Komplexität 20(10, 8)]")
	@tullio tmpRes_84[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_83[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] + ((((0.166667 * depsilonv4_dD4[idx9047, idx7647, idx7648, idx6982, idx6983, idx10660, idx10661, idx10662, idx10663]) * D[idx10662, idx10663]) * Identity_ord4_dm3333[idx10660, idx10661, idx10674, idx10675]) * Identity_ord4_dm3333[idx10674, idx10675, idx6984, idx6985])) + ((((0.166667 * depsilonv4_dD4[idx9047, idx7647, idx7648, idx6982, idx6983, idx10698, idx10699, idx10700, idx10701]) * Identity_ord4_dm3333[idx10700, idx10701, idx10708, idx10709]) * D[idx10708, idx10709]) * Identity_ord4_dm3333[idx10698, idx10699, idx6984, idx6985]))

	println("[Evaluating 'tmpRes_85', Komplexität 20(10, 8)]")
	@tullio tmpRes_85[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] := ((tmpRes_84[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] + ((((0.166667 * depsilonv3_dD3[idx9047, idx7647, idx7648, idx6982, idx6983, idx10734, idx10735]) * Identity_ord4_dm3333[idx10734, idx10735, idx10742, idx10743]) * Identity_ord4_dm3333[idx10742, idx10743, idx10750, idx10751]) * Identity_ord4_dm3333[idx10750, idx10751, idx6984, idx6985])) + ((((0.166667 * depsilonv4_dD4[idx9047, idx7647, idx7648, idx6982, idx6983, idx10774, idx10775, idx10776, idx10777]) * D[idx10776, idx10777]) * Identity_ord4_dm3333[idx10774, idx10775, idx10788, idx10789]) * Identity_ord4_dm3333[idx10788, idx10789, idx6984, idx6985]))

	println("[Evaluating 'tmpRes_86', Komplexität 16(2, 12)]")
	@tullio tmpRes_86[idx5428, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] := ((E0[idx5428, idx9047] + D[idx5428, idx9047]) * (-1 * (tmpRes_85[idx9047, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] + ((((0.166667 * depsilonv3_dD3[idx9047, idx7647, idx7648, idx6982, idx6983, idx10810, idx10811]) * Identity_ord4_dm3333[idx10810, idx10811, idx10818, idx10819]) * Identity_ord4_dm3333[idx10818, idx10819, idx10826, idx10827]) * Identity_ord4_dm3333[idx10826, idx10827, idx6984, idx6985]))))

	println("[Evaluating 'tmpRes_87', Komplexität 14(10, 2)]")
	@tullio tmpRes_87[idx5428] := ((((E0[idx5428, idx5429] + D[idx5428, idx5429]) * (epsilon[idx5429] - (tmpRes_0[idx5429] + tmpRes_1[idx5429]))) + tmpRes_8[idx5428]) + (tmpRes_28[idx5428, idx6017, idx6018] * D[idx6017, idx6018]))

	println("[Evaluating 'tmpRes_88', Komplexität 10(8, 0)]")
	@tullio tmpRes_88[idx5428] := ((((0.166667 * (tmpRes_61[idx5428, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985] + tmpRes_86[idx5428, idx7647, idx7648, idx6982, idx6983, idx6984, idx6985])) * D[idx6984, idx6985]) * D[idx6982, idx6983]) * D[idx7647, idx7648])

	println("[Evaluating final Result, Komplexität 2(0, 0)]")
	@tullio res[idx5428] := (tmpRes_87[idx5428] + tmpRes_88[idx5428])

	return res

end

start_time = time()
res = autodiff_sigmaTSM_t(rand(3, 3), rand(3, 3), rand(3), rand(3), rand(3, 3, 3), rand(3, 3, 3, 3, 3), rand(3, 3, 3, 3, 3, 3, 3), rand(3, 3, 3, 3, 3, 3, 3, 3, 3), rand(3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3), rand(3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3))
elapsed = time() - start_time
println("Laufzeit: ", elapsed, " s")
println("Ergebnis: ", res)