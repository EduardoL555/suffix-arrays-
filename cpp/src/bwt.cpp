// src/bwt.cpp
#include "bwt.hpp"
#include <algorithm>
#include <array>
#include <stdexcept>
#include <vector>

std::vector<uint8_t> bwt_from_sa(const std::vector<uint8_t>& T,
                                 const std::vector<int>& SA) {
    const int n = static_cast<int>(T.size());
    if (static_cast<int>(SA.size()) != n) {
        throw std::runtime_error("bwt_from_sa: SA y T tienen longitudes distintas");
    }
    std::vector<uint8_t> L(n);
    for (int i = 0; i < n; ++i) {
        int j = SA[i] - 1;
        if (j < 0) j += n;
        L[i] = T[j];
    }
    return L;
}

std::vector<uint8_t> alphabet_from_text(const std::vector<uint8_t>& T) {
    std::array<bool, 256> seen{}; 
    for (uint8_t c : T) seen[c] = true;
    std::vector<uint8_t> A;
    A.reserve(256);
    for (int c = 0; c < 256; ++c) if (seen[c]) A.push_back(static_cast<uint8_t>(c));
    return A;
}

std::vector<uint8_t> inverse_bwt(const std::vector<uint8_t>& L) {
    const int n = static_cast<int>(L.size());
    if (n == 0) return {};

    std::array<int, 256> freq{};
    for (uint8_t c : L) ++freq[c];

    std::array<int, 256> C{};
    int sum = 0;
    for (int c = 0; c < 256; ++c) {
        C[c] = sum;
        sum += freq[c];
    }

    std::array<int, 256> seen{};
    std::vector<int> rank(n);
    int sentinel_pos = -1;
    for (int i = 0; i < n; ++i) {
        uint8_t c = L[i];
        rank[i] = seen[c]++;
        if (c == 0x00) sentinel_pos = i;
    }
    if (sentinel_pos < 0) {
        throw std::runtime_error("inverse_bwt: no se encontró sentinela 0x00 en L");
    }

    std::vector<uint8_t> T(n);
    int p = sentinel_pos;
    for (int k = n - 1; k >= 0; --k) {
        uint8_t c = L[p];
        T[k] = c;
        p = C[c] + rank[p];
    }
    return T;
}
