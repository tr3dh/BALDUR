# Julia Skript
#
# unique external nodes :
# | arg 'epsilon', order [1], dimensions {6}
# | arg 'scalar', order [0], dimensions {}

function autodiff(epsilon, scalar)

        @assert length(epsilon) == 6
        @assert ndims(scalar) == 0

        res = zeros(6)

        res[1] = epsilon[1] * scalar[]
        res[2] = epsilon[2] * scalar[]
        res[3] = epsilon[3] * scalar[]
        res[4] = epsilon[4] * scalar[]
        res[5] = epsilon[5] * scalar[]
        res[6] = epsilon[6] * scalar[]

        return res
end

# Test mit konkreten Werten
println("=== Test der autodiff Funktion ===\n")

# Erstelle Test-Matrizen und -Vektor
E0_test = [
    1.0  2.0  3.0  4.0  5.0  6.0;
    7.0  8.0  9.0  10.0 11.0 12.0;
    13.0 14.0 15.0 16.0 17.0 18.0;
    19.0 20.0 21.0 22.0 23.0 24.0;
    25.0 26.0 27.0 28.0 29.0 30.0;
    31.0 32.0 33.0 34.0 35.0 36.0
]

D_test = [
    0.1  0.2  0.3  0.4  0.5  0.6;
    0.7  0.8  0.9  1.0  1.1  1.2;
    1.3  1.4  1.5  1.6  1.7  1.8;
    1.9  2.0  2.1  2.2  2.3  2.4;
    2.5  2.6  2.7  2.8  2.9  3.0;
    3.1  3.2  3.3  3.4  3.5  3.6
]

epsilon_test = [0.5, 1.0, 1.5, 2.0, 2.5, 3.0]

s = fill(5.0)

# Funktion ausführen
result = autodiff(epsilon_test, s)

# Ergebnis ausgeben
println("Input E0:")
display(E0_test)
println("\n\nInput D:")
display(D_test)
println("\n\nInput epsilon:")
println(epsilon_test)
println("\n\nErgebnis:")
println(result)
println("\nEinzelne Komponenten:")
for i in 1:6
    println("res[$i] = $(result[i])")
end

print(s[])