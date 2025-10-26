// src/huffman.cpp
#include "huffman.hpp"
#include <queue>
#include <array>
#include <stdexcept>
#include <cstdint>

namespace {

struct BitWriter {
    uint8_t buf{0};
    int bits{0};
    std::vector<uint8_t> out;

    void write(uint32_t code, uint8_t length) {
        if (length == 0) return;
        for (int k = length - 1; k >= 0; --k) {
            uint8_t bit = static_cast<uint8_t>((code >> k) & 1u);
            buf = static_cast<uint8_t>((buf << 1) | bit);
            ++bits;
            if (bits == 8) {
                out.push_back(buf);
                buf = 0;
                bits = 0;
            }
        }
    }
    void finish(std::vector<uint8_t>& dst) {
        if (!out.empty()) {
            dst.insert(dst.end(), out.begin(), out.end());
            out.clear();
        }
        if (bits > 0) {
            dst.push_back(static_cast<uint8_t>(buf << (8 - bits)));
            buf = 0;
            bits = 0;
        }
    }
};

struct BitReader {
    const std::vector<uint8_t>& data;
    size_t i{0};
    uint8_t buf{0};
    int bits{0};

    explicit BitReader(const std::vector<uint8_t>& d) : data(d) {}

    int read_bit() {
        if (bits == 0) {
            if (i >= data.size()) return -1;
            buf = data[i++];
            bits = 8;
        }
        --bits;
        return (buf >> bits) & 1u;
    }
};

struct Node {
    int freq;
    int uid;
    int sym;
    Node* left;
    Node* right;
    Node(int f, int id, int s=-1, Node* L=nullptr, Node* R=nullptr)
        : freq(f), uid(id), sym(s), left(L), right(R) {}
};

struct Cmp {
    bool operator()(const Node* a, const Node* b) const {
        if (a->freq != b->freq) return a->freq > b->freq;
        return a->uid > b->uid;
    }
};

void free_tree(Node* n) {
    if (!n) return;
    free_tree(n->left);
    free_tree(n->right);
    delete n;
}

void assign_codes(Node* n, uint32_t code, uint8_t len,
                  std::unordered_map<uint8_t, std::pair<uint32_t,uint8_t>>& out) {
    if (!n) return;
    if (n->sym >= 0 && !n->left && !n->right) {
        out[static_cast<uint8_t>(n->sym)] = std::make_pair(code, (len==0 ? 1 : len));
        return;
    }
    assign_codes(n->left,  (code << 1) | 0u, static_cast<uint8_t>(len + 1), out);
    assign_codes(n->right, (code << 1) | 1u, static_cast<uint8_t>(len + 1), out);
}

struct Trie {
    struct TNode { int sym{-1}; int next[2]{-1, -1}; };
    std::vector<TNode> nodes;
    Trie() { nodes.emplace_back(); }

    void insert(uint32_t code, uint8_t len, int sym) {
        int cur = 0;
        if (len == 0) { len = 1; }
        for (int k = len - 1; k >= 0; --k) {
            int bit = (code >> k) & 1u;
            int nx = nodes[cur].next[bit];
            if (nx < 0) {
                nodes[cur].next[bit] = static_cast<int>(nodes.size());
                nodes.emplace_back();
                cur = static_cast<int>(nodes.size()) - 1;
            } else {
                cur = nx;
            }
        }
        nodes[cur].sym = sym;
    }
};

}

void huffman_encode(const std::vector<uint8_t>& data,
                    std::unordered_map<uint8_t, std::pair<uint32_t,uint8_t>>& codes_out,
                    std::vector<uint8_t>& bitstream_out)
{
    codes_out.clear();
    bitstream_out.clear();
    if (data.empty()) return;

    std::array<int, 256> freq{};
    for (uint8_t b : data) ++freq[b];

    std::priority_queue<Node*, std::vector<Node*>, Cmp> pq;
    int uid = 0;
    for (int s = 0; s < 256; ++s) {
        if (freq[s] > 0) {
            pq.push(new Node(freq[s], uid++, s, nullptr, nullptr));
        }
    }
    if (pq.empty()) return;

    Node* root = nullptr;
    if (pq.size() == 1) {
        root = pq.top(); pq.pop();
    } else {
        while (pq.size() > 1) {
            Node* a = pq.top(); pq.pop();
            Node* b = pq.top(); pq.pop();
            Node* p = new Node(a->freq + b->freq, uid++, -1, a, b);
            pq.push(p);
        }
        root = pq.top(); pq.pop();
    }

    assign_codes(root, 0u, 0, codes_out);

    BitWriter bw;
    for (uint8_t b : data) {
        auto it = codes_out.find(b);
        if (it == codes_out.end()) {
            free_tree(root);
            throw std::runtime_error("huffman_encode: símbolo sin código");
        }
        uint32_t code = it->second.first;
        uint8_t  len  = it->second.second;
        bw.write(code, len);
    }
    bw.finish(bitstream_out);

    free_tree(root);
}

std::vector<uint8_t> huffman_decode(const std::vector<uint8_t>& bitstream,
    const std::unordered_map<uint8_t, std::pair<uint32_t,uint8_t>>& codes,
    uint32_t out_len)
{
    std::vector<uint8_t> out;
    out.reserve(out_len);

    if (out_len == 0) return out;
    if (codes.empty()) throw std::runtime_error("huffman_decode: codebook vacío");

    struct Trie { struct TNode { int sym{-1}; int next[2]{-1,-1}; }; std::vector<TNode> nodes; Trie(){nodes.emplace_back();} 
        void insert(uint32_t code, uint8_t len, int sym){ int cur=0; if(len==0) len=1; for(int k=len-1;k>=0;--k){int bit=(code>>k)&1u; int nx=nodes[cur].next[bit]; if(nx<0){nodes[cur].next[bit]=(int)nodes.size(); nodes.emplace_back(); cur=(int)nodes.size()-1;} else cur=nx;} nodes[cur].sym=sym; } 
    } trie;
    for (const auto& kv : codes) {
        trie.insert(kv.second.first, kv.second.second, static_cast<int>(kv.first));
    }

    BitReader br(bitstream);
    while (out.size() < out_len) {
        int cur = 0; // raíz
        while (true) {
            int sym = trie.nodes[cur].sym;
            if (sym >= 0) { out.push_back((uint8_t)sym); break; }
            int bit = br.read_bit();
            if (bit < 0) throw std::runtime_error("huffman_decode: bitstream insuficiente");
            int nx = trie.nodes[cur].next[bit];
            if (nx < 0) throw std::runtime_error("huffman_decode: código inválido");
            cur = nx;
        }
    }
    return out;
}
