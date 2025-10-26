// include/bwt.hpp
#pragma once
#include <vector>
#include <cstdint>

std::vector<uint8_t> bwt_from_sa(const std::vector<uint8_t>& T,
                                 const std::vector<int>& SA);
std::vector<uint8_t> inverse_bwt(const std::vector<uint8_t>& L);

std::vector<uint8_t> alphabet_from_text(const std::vector<uint8_t>& T);
