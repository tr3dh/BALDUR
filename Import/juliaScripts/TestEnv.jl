include("3.2.1_A_autodiff_wrappedsigma_t.jl")
include("3.2.1_A_autodiff_wrappedsigmaTSM_t.jl")

# Test mit vernünftigen Werten
E0 = diagm([1.0, 1.0, 1.0])
D = fill(0.0, 3,3)

epsilon  = [1, 1, 1]
epsilonv = fill(0.1, 3)

depsilonv1_dD1 = fill(0.0, 3,3,3)
depsilonv2_dD2 = fill(0.00, 3,3,3,3,3)
depsilonv3_dD3 = fill(0.000, 3,3,3,3,3,3,3)
depsilonv4_dD4 = fill(0.0000, 3,3,3,3,3,3,3,3,3)

res1 = autodiff_sigma_t(
    E0, D, epsilon, epsilonv
)

println("Ergebnis sigma_t:")
println(res1)

res2 = autodiff_sigmaTSM_t(
    E0, D, epsilon, epsilonv,
    depsilonv1_dD1, depsilonv2_dD2
)

println("Ergebnis sigmaTSM_t:")
println(res2)