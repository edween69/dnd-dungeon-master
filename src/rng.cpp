/*  Author: Andrew
    Any functions that are dependent on randomness should be put here.
    To use random rolls just run call roll_d() with whatever die face you want.
*/
#include "rng.h"


namespace {
    // This function returns a reference to a single global-ish engine,
    // but it's hidden inside this file only.
    std::mt19937& engine() {
        static std::mt19937 eng{ std::random_device{}() };
        return eng;
    }
}

// @author: Andrew
// @brief: the function rolls a random number in the range 1 to a number specified
// @param: int sides - is effectively the number of sides of a dice so a 6 would be like rolling a 6 sided die
// @return: int dist(engine) -  is a random number in range 1 to specified number
int roll_d(int sides) {
    std::uniform_int_distribution<int> dist(1, sides);
    return dist(engine());
}