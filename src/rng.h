// File that will control random utility based functions
#include <limits>
#include <random>
#include <algorithm>
#ifndef RNG_H
#define RNG_H

std::mt19937& rng();

struct AIRNG
{
    AIRNG() = default;

    // Edit after finding the real stats
    // Generate a random integer between a and b 
    int range(int a, int b) 
    {
        auto& gen = rng();
        std::uniform_int_distribution<int> dist(a,b);
        return dist(gen);
    }
    //For determining AI actions
    bool chance(int percent) 
    {
        auto& gen = rng();
        std::uniform_int_distribution<int> perc(1,100);
        return perc(gen) <= percent;
    }
};

inline AIRNG AIrng;

#endif