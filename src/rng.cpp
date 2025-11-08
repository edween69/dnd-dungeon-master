// rng.cpp
#include "rng.h"

std::mt19937& rng() {
    static std::mt19937 gen(std::random_device{}());  // or fixed seed: std::mt19937 gen(12345);
    return gen;
}