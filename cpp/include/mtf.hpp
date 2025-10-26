// include/mtf.hpp
#pragma once
#include <vector>
#include <cstdint>

std::vector<uint8_t> mtf_encode(const std::vector<uint8_t>& L,
                                const std::vector<uint8_t>& alphabet);

std::vector<uint8_t> mtf_decode(const std::vector<uint8_t>& mtf_seq,
                                const std::vector<uint8_t>& alphabet);
