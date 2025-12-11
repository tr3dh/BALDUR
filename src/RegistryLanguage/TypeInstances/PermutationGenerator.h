#pragma once

#include "defines.h"

void generateIndexPermutations(size_t N, std::vector<bool>& used, std::vector<size_t>& current,
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

void generateIndexCombinations(const std::vector<size_t>& indices, std::vector<bool>& used, std::vector<size_t>& current,
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