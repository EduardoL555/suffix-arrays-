// include/huffman.hpp
#pragma once
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <utility>

void huffman_encode(const std::vector<uint8_t>& data,
                    std::unordered_map<uint8_t, std::pair<uint32_t,uint8_t>>& codes_out,
                    std::vector<uint8_t>& bitstream_out);

std::vector<uint8_t> huffman_decode(const std::vector<uint8_t>& bitstream,
    const std::unordered_map<uint8_t, std::pair<uint32_t,uint8_t>>& codes,
    uint32_t out_len);
