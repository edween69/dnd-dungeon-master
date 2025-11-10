/*  Author: Andrew
    This file stores the rng() generator. to use:
    std::uniform_int_distribution<int> d20(1,20), d6(1,6);
    randomvalue = d20(rng());

    any functions that are dependent on randomness should be put here.
*/
#include "rng.h"

// @author Andrew
// @brief seeds the random generator using random_device{} this is to limit the amount of imported libraries
std::mt19937& rng() {
    static std::mt19937 gen(std::random_device{}());
    return gen;
}