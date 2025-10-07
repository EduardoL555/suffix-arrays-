#include "fm_index.hpp"
#include "suffix_array_mm.hpp"
#include <algorithm>
#include <cstring>

static std::string bwt_from_sa(const std::string& s, const std::vector<int>& sa) {
    int n = (int)s.size();
    std::string bwt(n, '\0');
    for (int i = 0; i < n; ++i) {
        int idx = sa[i];
        bwt[i] = (idx == 0) ? s[n-1] : s[idx-1];
    }
    return bwt;
}

FMIndex::FMIndex(const std::string& s, const std::vector<int>& sa_, int step_)
: sa(sa_), step(step_), n((int)s.size()) {
    bwt = bwt_from_sa(s, sa);
    C.assign(256, 0);
    std::vector<int> cnt(256, 0);
    for (char ch : bwt) cnt[(unsigned char)ch]++;
    int tot = 0;
    for (int c = 0; c < 256; ++c) {
        C[c] = tot;
        tot += cnt[c];
    }
    int blocks = (n + step - 1) / step;
    checkpoints.assign(256, std::vector<int>(blocks+1, 0));
    std::vector<int> run(256, 0);
    for (int i = 1; i <= n; ++i) {
        unsigned char ch = (unsigned char)bwt[i-1];
        run[ch]++;
        if (i % step == 0) {
            int b = i / step;
            for (int c = 0; c < 256; ++c) checkpoints[c][b] = run[c];
        }
    }
    // almacenar totales al final en checkpoints[][blocks]
    for (int c = 0; c < 256; ++c) checkpoints[c][blocks] = run[c];
}

int FMIndex::occ(unsigned char c, int i) const {
    if (i <= 0) return 0;
    int block = i / step;
    int offset = i % step;
    int base = checkpoints[c][block];
    int start = block * step;
    int end = std::min(start + offset, n);
    for (int j = start; j < end; ++j) {
        if ((unsigned char)bwt[j] == c) base++;
    }
    return base;
}

std::vector<int> FMIndex::search(const std::string& pat) const {
    if (pat.empty()) return {};
    int l = 0, r = n;
    for (int k = (int)pat.size()-1; k >= 0; --k) {
        unsigned char c = (unsigned char)pat[k];
        int cbase = C[c];
        l = cbase + occ(c, l);
        r = cbase + occ(c, r);
        if (l >= r) return {};
    }
    std::vector<int> out;
    out.reserve(r - l);
    for (int i = l; i < r; ++i) out.push_back(sa[i]);
    return out;
}
