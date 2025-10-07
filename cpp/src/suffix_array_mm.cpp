#include "suffix_array_mm.hpp"
#include <algorithm>
#include <tuple>

struct Trip {
    int l, r, i;
};

static std::vector<int> make_ranks(const std::vector<Trip>& trips, int n) {
    std::vector<int> rank(n, -1);
    int cur = 0;
    int prevL = -1, prevR = -1;
    for (size_t k = 0; k < trips.size(); ++k) {
        if (k == 0 || trips[k].l != prevL || trips[k].r != prevR) {
            ++cur;
            prevL = trips[k].l;
            prevR = trips[k].r;
        }
        rank[trips[k].i] = cur;
    }
    return rank;
}

std::vector<int> suffix_array_mm(const std::string& s) {
    int n = (int)s.size();
    std::vector<Trip> trips(n);
    for (int i = 0; i < n; ++i) {
        trips[i] = { (int)(unsigned char)s[i], (i+1<n)? (int)(unsigned char)s[i+1] : -1, i };
    }
    std::sort(trips.begin(), trips.end(), [](const Trip& a, const Trip& b){
        return std::tie(a.l,a.r,a.i) < std::tie(b.l,b.r,b.i);
    });

    for (int k = 1; k < n; k <<= 1) {
        auto rank = make_ranks(trips, n);
        std::vector<Trip> nt(n);
        for (int i = 0; i < n; ++i) {
            nt[i] = { rank[i], (i+k<n)? rank[i+k] : -1, i };
        }
        std::sort(nt.begin(), nt.end(), [](const Trip& a, const Trip& b){
            return std::tie(a.l,a.r,a.i) < std::tie(b.l,b.r,b.i);
        });
        trips.swap(nt);
    }
    std::vector<int> sa(n);
    for (int i = 0; i < n; ++i) sa[i] = trips[i].i;
    return sa;
}
