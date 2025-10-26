// src/sa_mm.cpp
#include "sa_mm.hpp"
#include <algorithm>
#include <utility>

std::vector<int> suffix_array_mm(const std::vector<uint8_t>& T) {
    const int n = static_cast<int>(T.size());
    std::vector<int> SA(n), rank(n), tmp(n);

    if (n == 0) return SA;
    for (int i = 0; i < n; ++i) {
        SA[i] = i;
        rank[i] = static_cast<int>(T[i]);
    }

    for (int k = 1;; k <<= 1) {
        std::sort(SA.begin(), SA.end(), [&](int i, int j) {
            if (rank[i] != rank[j]) return rank[i] < rank[j];
            const int ri = (i + k < n) ? rank[i + k] : -1;
            const int rj = (j + k < n) ? rank[j + k] : -1;
            return ri < rj;
        });

        tmp[SA[0]] = 0;
        for (int i = 1; i < n; ++i) {
            const int a = SA[i - 1], b = SA[i];
            const bool diff_first  = (rank[a] != rank[b]);
            const int  ra2 = (a + k < n) ? rank[a + k] : -1;
            const int  rb2 = (b + k < n) ? rank[b + k] : -1;
            const bool diff_second = (ra2 != rb2);
            tmp[b] = tmp[a] + (diff_first || diff_second ? 1 : 0);
        }
        rank.swap(tmp);
        if (rank[SA.back()] == n - 1) break;
    }

    return SA;
}
