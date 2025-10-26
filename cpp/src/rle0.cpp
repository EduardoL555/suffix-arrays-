// src/rle0.cpp
#include "rle0.hpp"

std::vector<uint8_t> rle0_encode(const std::vector<uint8_t>& mtf) {
    std::vector<uint8_t> out;
    out.reserve(mtf.size());
    const size_t n = mtf.size();
    size_t i = 0;
    while (i < n) {
        uint8_t b = mtf[i];
        if (b != 0) {
            out.push_back(b);
            ++i;
        } else {
            size_t j = i;
            while (j < n && mtf[j] == 0) ++j;
            size_t run = j - i;
            while (run > 255) {
                out.push_back(0);
                out.push_back(255);
                run -= 255;
            }
            out.push_back(0);
            out.push_back(static_cast<uint8_t>(run));
            i = j;
        }
    }
    return out;
}

std::vector<uint8_t> rle0_decode(const std::vector<uint8_t>& rle) {
    std::vector<uint8_t> out;
    out.reserve(rle.size());
    const size_t n = rle.size();
    size_t i = 0;
    while (i < n) {
        uint8_t b = rle[i++];
        if (b != 0) {
            out.push_back(b);
        } else {
            if (i >= n) break;
            uint8_t run = rle[i++];
            out.insert(out.end(), run, static_cast<uint8_t>(0));
        }
    }
    return out;
}
