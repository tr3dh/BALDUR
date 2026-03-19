include("juliaScripts/3.2.2_optimized_C_wrapped_tangent_epsilonvp_E_1.jl")
include("juliaScripts/3.2.2_optimized_C_wrapped_tangent_epsilonvp_Y_1.jl")

eta      = 20.0
delta_t  = 0.01
lambda   = 12.0
mu       = 8.0
sigmaY   = 0.05
eps      = [0.1, 0.0, 0.0, 0.0, 0.0, 0.0]
epsvp    = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0]

P = [
    2/3  -1/3  -1/3  0  0  0 ;
    -1/3   2/3  -1/3  0  0  0 ;
    -1/3  -1/3   2/3  0  0  0 ;
    0     0     0    1  0  0 ;
    0     0     0    0  1  0 ;
    0     0     0    0  0  1
]

E0=  [
    28.0  12.0  12.0  0.0  0.0  0.0;
    12.0  28.0  12.0  0.0  0.0  0.0;
    12.0  12.0  28.0  0.0  0.0  0.0;
    0.0   0.0   0.0  8.0  0.0  0.0;
    0.0   0.0   0.0  0.0  8.0  0.0;
    0.0   0.0   0.0  0.0  0.0  8.0;
]

print(eval_epsilonvp_Y_1(eta, P, E0, eps, epsvp, sigmaY, zeros(6)) * delta_t)
permutedims(eval_epsilonvp_E_1(eta, P, E0, eps, epsvp, sigmaY, zeros(6,6,6)), (1,3,2)) * delta_t