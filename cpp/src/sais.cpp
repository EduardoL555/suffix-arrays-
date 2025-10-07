#include "sais.hpp"
#include "bitvector.hpp"
#include <algorithm>
#include <numeric>

static void buckets(const std::vector<int>& T, int K, std::vector<int>& head, std::vector<int>& tail) {
    head.assign(K, 0);
    tail.assign(K, 0);
    for (int c : T) tail[c]++;
    int s = 0;
    for (int c = 0; c < K; ++c) {
        head[c] = s;
        s += tail[c];
        tail[c] = s;
    }
}

static std::vector<int> lms_positions(const BitVector& S) {
    int n = (int)S.n;
    std::vector<int> L;
    for (int i = 1; i < n; ++i) {
        if (S.get(i) && !S.get(i - 1)) L.push_back(i);
    }
    // Evitar duplicar el último índice si ya fue detectado como LMS
    if (L.empty() || L.back() != n - 1) L.push_back(n - 1);
    return L;
}


static std::vector<int> place_LMS(const std::vector<int>& T, const BitVector& S, int K, const std::vector<int>& LMS) {
    int n = (int)T.size();
    std::vector<int> SA(n, -1);
    std::vector<int> head, tail; buckets(T, K, head, tail);
    std::vector<int> tailp = tail;
    for (int idx = (int)LMS.size()-1; idx >= 0; --idx) {
        int i = LMS[idx];
        int c = T[i];
        SA[--tailp[c]] = i;
    }
    return SA;
}

static void induce_L(const std::vector<int>& T, const BitVector& S, int K, std::vector<int>& SA) {
    std::vector<int> head, tail; buckets(T, K, head, tail);
    std::vector<int> headp = head;
    for (int pos = 0; pos < (int)SA.size(); ++pos) {
        int j = SA[pos];
        if (j > 0) {
            int i = j - 1;
            if (!S.get(i)) {
                int c = T[i];
                // ⬇️ Saltar celdas ocupadas antes de escribir
                while (headp[c] < (int)SA.size() && SA[headp[c]] != -1) headp[c]++;
                if (headp[c] < (int)SA.size()) {
                    SA[headp[c]++] = i;
                }
            }
        }
    }
}


static void induce_S(const std::vector<int>& T, const BitVector& S, int K, std::vector<int>& SA) {
    std::vector<int> head, tail; buckets(T, K, head, tail);
    std::vector<int> tailp = tail;
    for (int pos = (int)SA.size()-1; pos >= 0; --pos) {
        int j = SA[pos];
        if (j > 0) {
            int i = j - 1;
            if (S.get(i)) {
                int c = T[i];
                // ⬇️ Retrocede hasta encontrar espacio libre
                while (tailp[c] > 0 && SA[tailp[c]-1] != -1) tailp[c]--;
                if (tailp[c] > 0) {
                    SA[--tailp[c]] = i;
                }
            }
        }
    }
}

static bool lms_equal(const std::vector<int>& T, const BitVector& S, int i, int j) {
    int n = (int)T.size();
    int k = 0;
    while (true) {
        bool i_end = (i+k+1 < n) && S.get(i+k+1) && !S.get(i+k);
        bool j_end = (j+k+1 < n) && S.get(j+k+1) && !S.get(j+k);

        // ⬇️ Seguridad de límites ANTES de acceder a T[]
        if (i+k >= n || j+k >= n) return false;

        if (T[i+k] != T[j+k] || i_end != j_end) return false;
        if (i_end && j_end) return true;
        ++k;
    }
}


std::vector<int> sais(const std::vector<int>& T, int K) {
    int n = (int)T.size();
    // Tipos L/S (S=true)
    BitVector S(n);
    S.set(n-1, true);
    for (int i = n-2; i >= 0; --i) {
        bool isS = (T[i] < T[i+1]) || (T[i]==T[i+1] && S.get(i+1));
        S.set(i, isS);
    }

    auto LMS = lms_positions(S);

    // Primera pasada
    auto SA = place_LMS(T, S, K, LMS);
    induce_L(T, S, K, SA);
    induce_S(T, S, K, SA);

    // Nombrar LMS
    std::vector<int> isLMS(n, 0);
    for (int p : LMS) isLMS[p] = 1;
    std::vector<int> lms_in_SA;
    for (int x : SA) if (x>=0 && isLMS[x]) lms_in_SA.push_back(x);

    std::vector<int> name(n, -1);
    int cur = -1;
    int prev = -1;
    for (int p : lms_in_SA) {
        if (prev == -1) { cur = 0; name[p] = cur; prev = p; }
        else {
            if (lms_equal(T, S, prev, p)) name[p] = cur;
            else { ++cur; name[p] = cur; prev = p; }
        }
    }

    std::vector<int> reduced;
    reduced.reserve(LMS.size());
    for (int p : LMS) if (name[p] != -1) reduced.push_back(name[p]);

    std::vector<int> ordered_LMS;
    if (cur + 1 < (int)reduced.size()) {
        // Recurse
        int KK = cur + 1;
        ordered_LMS.resize(reduced.size());
        auto SAred = sais(reduced, KK);
        for (size_t i = 0; i < SAred.size(); ++i) ordered_LMS[i] = LMS[SAred[i]];
    } else {
        // Directo
        std::vector<std::pair<int,int>> pairs;
        pairs.reserve(LMS.size());
        for (int p : LMS) pairs.emplace_back(name[p], p);
        std::sort(pairs.begin(), pairs.end());
        for (auto& pr : pairs) ordered_LMS.push_back(pr.second);
    }

    // Segunda pasada
    std::fill(SA.begin(), SA.end(), -1);
    std::vector<int> head, tail; buckets(T, K, head, tail);
    std::vector<int> tailp = tail;
    for (int idx = (int)ordered_LMS.size()-1; idx >= 0; --idx) {
        int i = ordered_LMS[idx];
        int c = T[i];
        SA[--tailp[c]] = i;
    }
    induce_L(T, S, K, SA);
    induce_S(T, S, K, SA);

    return SA;
}
