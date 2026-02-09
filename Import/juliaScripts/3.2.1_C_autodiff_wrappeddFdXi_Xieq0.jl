# Julia Skript
#
# unique external nodes :
# | arg 'ddeltaEpsilon1_dXi1', order [3], dimensions {3, 3, 3}
# | arg 'Identity_ord4_dm3333', order [4], dimensions {3, 3, 3, 3}
# | arg 'E0', order [2], dimensions {3, 3}
# | arg 'epsilon', order [1], dimensions {3}
# | arg 'deltaEpsilon', order [1], dimensions {3}
# | arg 'depsilon1_dXi1', order [3], dimensions {3, 3, 3}
# | arg 'ddeltaU1_dXi1', order [3], dimensions {3, 3, 3}
# | arg 'b', order [1], dimensions {3}
# | arg 't', order [1], dimensions {3}

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


function autodiff_dFdXi_Xieq0(ddeltaEpsilon1_dXi1, E0, epsilon, deltaEpsilon, depsilon1_dXi1, ddeltaU1_dXi1, b, t)

	@assert size(ddeltaEpsilon1_dXi1) == (3, 3, 3)
	Identity_ord4_dm3333 = create_Identity(3, 3, 3, 3)
	@assert size(E0) == (3, 3)
	@assert length(epsilon) == 3
	@assert length(deltaEpsilon) == 3
	@assert size(depsilon1_dXi1) == (3, 3, 3)
	@assert size(ddeltaU1_dXi1) == (3, 3, 3)
	@assert length(b) == 3
	@assert length(t) == 3

	println("[Ausdruck mit 3 temporären Dependencies substituiert]")

	println("[Evaluating 'tmpRes_0', Komplexität 14(8, 4)]")
	@tullio tmpRes_0[idx125, idx126] := ((((ddeltaEpsilon1_dXi1[idx125, idx126, idx127] + (ddeltaEpsilon1_dXi1[idx125, idx119, idx120] * Identity_ord4_dm3333[idx119, idx120, idx126, idx127])) * E0[idx127, idx131]) * epsilon[idx131]) + ((deltaEpsilon[idx139] * Identity_ord4_dm3333[idx139, idx125, idx126, idx143]) * epsilon[idx143]))

	println("[Evaluating 'tmpRes_1', Komplexität 8(2, 4)]")
	@tullio tmpRes_1[idx125, idx126] := ((deltaEpsilon[idx149] * E0[idx149, idx151]) * (depsilon1_dXi1[idx151, idx125, idx126] + (depsilon1_dXi1[idx151, idx163, idx164] * Identity_ord4_dm3333[idx163, idx164, idx125, idx126])))

	println("[Evaluating 'tmpRes_2', Komplexität 18(10, 6)]")
	@tullio tmpRes_2[idx125, idx126] := (((tmpRes_0[idx125, idx126] + tmpRes_1[idx125, idx126]) - ((ddeltaU1_dXi1[idx125, idx126, idx191] + (ddeltaU1_dXi1[idx125, idx183, idx184] * Identity_ord4_dm3333[idx183, idx184, idx126, idx191])) * b[idx191])) - ((ddeltaU1_dXi1[idx125, idx126, idx213] + (ddeltaU1_dXi1[idx125, idx205, idx206] * Identity_ord4_dm3333[idx205, idx206, idx126, idx213])) * t[idx213]))

	println("[Evaluating final Result, Komplexität 0()]")
	@tullio res[idx125, idx126] := tmpRes_2[idx125, idx126]

	return res

end

start_time = time()
res = autodiff_dFdXi_Xieq0(rand(3, 3, 3), rand(3, 3), rand(3), rand(3), rand(3, 3, 3), rand(3, 3, 3), rand(3), rand(3))
elapsed = time() - start_time
println("Laufzeit: ", elapsed, " s")
println("Ergebnis: ", res)