// include/rle0.hpp
#pragma once
#include <vector>
#include <cstdint>

std::vector<uint8_t> rle0_encode(const std::vector<uint8_t>& mtf);

std::vector<uint8_t> rle0_decode(const std::vector<uint8_t>& rle);
