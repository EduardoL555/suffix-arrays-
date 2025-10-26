// include/fmi_container.hpp
#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <unordered_map>
#include <utility>

struct FmiHeader {
    uint32_t orig_len_no_sentinel = 0;
    bool     had_sentinel = false;
    std::vector<uint8_t> alphabet;
    std::unordered_map<uint8_t, std::pair<uint32_t,uint8_t>> codes;
    uint32_t rle_len = 0;
    std::vector<uint8_t> bitstream;
};

void write_fmi(const std::string& outPath,
               uint32_t orig_len_no_sentinel,
               bool had_sentinel,
               const std::vector<uint8_t>& alphabet,
               const std::unordered_map<uint8_t, std::pair<uint32_t,uint8_t>>& codes,
               uint32_t rle_len,
               const std::vector<uint8_t>& bitstream);
FmiHeader read_fmi(const std::string& inPath);
