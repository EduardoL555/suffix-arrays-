#pragma once
#include <vector>
#include <cstdint>

// Bit vector simple sobre uint64_t
struct BitVector {
    std::vector<uint64_t> bits;
    size_t n = 0;

    BitVector() = default;
    explicit BitVector(size_t n_) : bits((n_ + 63) / 64, 0), n(n_) {}

    inline void set(size_t i, bool v) {
        size_t w = i >> 6, b = i & 63;
        if (v) bits[w] |= (uint64_t(1) << b);
        else   bits[w] &= ~(uint64_t(1) << b);
    }
    inline bool get(size_t i) const {
        size_t w = i >> 6, b = i & 63;
        return (bits[w] >> b) & 1ULL;
    }
};
