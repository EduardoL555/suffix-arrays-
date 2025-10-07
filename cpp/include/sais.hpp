#pragma once
#include <vector>
#include <cstdint>

// SA-IS para texto entero (bytes) con sentinela 0 al final.
std::vector<int> sais(const std::vector<int>& T, int K);
