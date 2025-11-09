/*  This file stores the rng() generator. to use:
    std::uniform_int_distribution<int> d20(1,20), d6(1,6);
    randomvalue = d20(rng());
*/
#include "rng.h"

std::mt19937& rng() {
    static std::mt19937 gen(std::random_device{}());
    return gen;
}