// src/mtf.cpp
#include "mtf.hpp"
#include <array>
#include <stdexcept>
#include <algorithm>

std::vector<uint8_t> mtf_encode(const std::vector<uint8_t>& L,
                                const std::vector<uint8_t>& alphabet) {
    std::vector<uint8_t> A = alphabet;
    std::array<int, 256> pos;
    pos.fill(-1);
    for (size_t i = 0; i < A.size(); ++i) {
        pos[A[i]] = static_cast<int>(i);
    }

    std::vector<uint8_t> out;
    out.reserve(L.size());

    for (uint8_t c : L) {
        int i = pos[c];
        if (i < 0) {
            throw std::runtime_error("mtf_encode: símbolo fuera del alfabeto");
        }
        out.push_back(static_cast<uint8_t>(i));
        if (i != 0) {
            uint8_t v = A[static_cast<size_t>(i)];
            for (int k = i; k > 0; --k) {
                A[static_cast<size_t>(k)] = A[static_cast<size_t>(k - 1)];
                pos[A[static_cast<size_t>(k)]] = k;
            }
            A[0] = v;
            pos[v] = 0;
        }
    }
    return out;
}

std::vector<uint8_t> mtf_decode(const std::vector<uint8_t>& mtf_seq,
                                const std::vector<uint8_t>& alphabet) {
    std::vector<uint8_t> A = alphabet;
    std::vector<uint8_t> out;
    out.reserve(mtf_seq.size());

    const size_t sigma = A.size();
    for (uint8_t idx : mtf_seq) {
        if (idx >= sigma) {
            throw std::runtime_error("mtf_decode: índice fuera de rango");
        }
        uint8_t v = A[static_cast<size_t>(idx)];
        out.push_back(v);
        if (idx != 0) {
            for (int k = static_cast<int>(idx); k > 0; --k) {
                A[static_cast<size_t>(k)] = A[static_cast<size_t>(k - 1)];
            }
            A[0] = v;
        }
    }
    return out;
}
