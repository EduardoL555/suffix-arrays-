#pragma once
#include <string>
#include <vector>

struct FMIndex {
    std::string bwt;
    std::vector<int> sa;
    std::vector<int> C;                 // tamaño 256
    int step = 128;
    std::vector<std::vector<int>> checkpoints; // 256 x ceil(n/step)
    int n = 0;

    FMIndex(const std::string& s, const std::vector<int>& sa_, int step_=128);
    int occ(unsigned char c, int i) const; // #c en BWT[0:i)
    std::vector<int> search(const std::string& pat) const;
};
