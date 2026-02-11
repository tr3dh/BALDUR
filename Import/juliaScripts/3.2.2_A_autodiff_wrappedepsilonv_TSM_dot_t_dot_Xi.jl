# Julia Skript
#
# unique external nodes :
# | arg 'depsilonv1_dXi1', order [3], dimensions {3, 3, 3}
# | arg 'depsilonv2_dXi2', order [5], dimensions {3, 3, 3, 3, 3}
# | arg 'Xi', order [2], dimensions {3, 3}
# | arg 'Identity_ord4_dm3333', order [4], dimensions {3, 3, 3, 3}
# | arg 'depsilonv3_dXi3', order [7], dimensions {3, 3, 3, 3, 3, 3, 3}
# | arg 'depsilonv4_dXi4', order [9], dimensions {3, 3, 3, 3, 3, 3, 3, 3, 3}

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


function autodiff_epsilonv_TSM_dot_t_dot_Xi(depsilonv1_dXi1, depsilonv2_dXi2, Xi, depsilonv3_dXi3, depsilonv4_dXi4)

	@assert size(depsilonv1_dXi1) == (3, 3, 3)
	@assert size(depsilonv2_dXi2) == (3, 3, 3, 3, 3)
	@assert size(Xi) == (3, 3)
	Identity_ord4_dm3333 = create_Identity(3, 3, 3, 3)
	@assert size(depsilonv3_dXi3) == (3, 3, 3, 3, 3, 3, 3)
	@assert size(depsilonv4_dXi4) == (3, 3, 3, 3, 3, 3, 3, 3, 3)

	println("[Ausdruck mit 4 temporären Dependencies substituiert]")

	println("[Evaluating 'tmpRes_0', Komplexität 16(8, 6)]")
	@tullio tmpRes_0[idx237, idx238, idx239] := (((depsilonv1_dXi1[idx237, idx238, idx239] + (depsilonv2_dXi2[idx237, idx238, idx239, idx233, idx234] * Xi[idx233, idx234])) + (depsilonv1_dXi1[idx237, idx248, idx249] * Identity_ord4_dm3333[idx248, idx249, idx238, idx239])) + (((0.5 * depsilonv3_dXi3[idx237, idx238, idx239, idx264, idx265, idx266, idx267]) * Xi[idx266, idx267]) * Xi[idx264, idx265]))

	println("[Evaluating 'tmpRes_1', Komplexität 16(8, 6)]")
	@tullio tmpRes_1[idx237, idx238, idx239] := ((tmpRes_0[idx237, idx238, idx239] + (((0.5 * depsilonv2_dXi2[idx237, idx238, idx239, idx284, idx285]) * Identity_ord4_dm3333[idx284, idx285, idx292, idx293]) * Xi[idx292, idx293])) + (((0.5 * depsilonv2_dXi2[idx237, idx304, idx305, idx306, idx307]) * Xi[idx306, idx307]) * Identity_ord4_dm3333[idx304, idx305, idx238, idx239]))

	println("[Evaluating 'tmpRes_2', Komplexität 20(10, 8)]")
	@tullio tmpRes_2[idx237, idx238, idx239] := ((tmpRes_1[idx237, idx238, idx239] + ((((0.166667 * depsilonv4_dXi4[idx237, idx238, idx239, idx332, idx333, idx334, idx335, idx336, idx337]) * Xi[idx336, idx337]) * Xi[idx334, idx335]) * Xi[idx332, idx333])) + ((((0.166667 * depsilonv3_dXi3[idx237, idx238, idx239, idx360, idx361, idx362, idx363]) * Identity_ord4_dm3333[idx362, idx363, idx370, idx371]) * Xi[idx370, idx371]) * Xi[idx360, idx361]))

	println("[Evaluating 'tmpRes_3', Komplexität 20(10, 8)]")
	@tullio tmpRes_3[idx237, idx238, idx239] := ((tmpRes_2[idx237, idx238, idx239] + ((((0.166667 * depsilonv3_dXi3[idx237, idx238, idx239, idx390, idx391, idx392, idx393]) * Xi[idx392, idx393]) * Identity_ord4_dm3333[idx390, idx391, idx404, idx405]) * Xi[idx404, idx405])) + ((((0.166667 * depsilonv3_dXi3[idx237, idx418, idx419, idx420, idx421, idx422, idx423]) * Xi[idx422, idx423]) * Xi[idx420, idx421]) * Identity_ord4_dm3333[idx418, idx419, idx238, idx239]))

	println("[Evaluating final Result, Komplexität 0()]")
	@tullio res[idx237, idx238, idx239] := tmpRes_3[idx237, idx238, idx239]

	return res

end

start_time = time()
res = autodiff_epsilonv_TSM_dot_t_dot_Xi(rand(3, 3, 3), rand(3, 3, 3, 3, 3), rand(3, 3), rand(3, 3, 3, 3, 3, 3, 3), rand(3, 3, 3, 3, 3, 3, 3, 3, 3))
elapsed = time() - start_time
println("Laufzeit: ", elapsed, " s")
println("Ergebnis: ", res)