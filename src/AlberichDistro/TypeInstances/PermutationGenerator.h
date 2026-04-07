#pragma once

#include "defines.h"

inline void generateIndexPermutations(size_t N, std::vector<bool>& used, std::vector<size_t>& current,
     std::vector<std::vector<size_t>>& allPermutations, size_t depth)
{
    if(depth == N){

        allPermutations.push_back(current);
        return;
    }

    for(size_t i = 0; i < N; i++){

        if(!used[i]){
            used[i] = true;
            current[depth] = i;
            generateIndexPermutations(N, used, current, allPermutations, depth + 1);
            used[i] = false;
        }
    }
}

inline void generateIndexCombinations(const std::vector<size_t>& indices, std::vector<bool>& used, std::vector<size_t>& current,
     std::vector<std::vector<size_t>>& allPermutations, size_t depth)
{
    if(depth == indices.size()){

        allPermutations.push_back(current);
        return;
    }

    for(size_t i = 0; i < indices.size(); i++){
        if(!used[i]){
            used[i] = true;
            current[depth] = indices[i];
            generateIndexCombinations(indices, used, current, allPermutations, depth + 1);
            used[i] = false;
        }
    }
}

inline std::vector<std::vector<int>> generateTensorIndexPermutations(const std::vector<int>& dimensions) {

    std::vector<std::vector<int>> result;
    
    // Berechne die Gesamtanzahl der Einträge
    int total_size = 1;
    for (int dim : dimensions) {
        total_size *= dim;
    }
    
    // Generiere alle Indizes
    std::vector<int> current(dimensions.size(), 0);
    
    for (int i = 0; i < total_size; ++i) {
        result.push_back(current);
        
        // Inkrementiere die Indizes (wie bei einem Odometer)
        for (int j = dimensions.size() - 1; j >= 0; --j) {
            current[j]++;
            if (current[j] < dimensions[j]) {
                break;
            }
            current[j] = 0;
        }
    }
    
    return result;
}