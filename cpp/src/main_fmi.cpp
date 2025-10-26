// src/main_fmi.cpp
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <fstream>
#include <iostream>

#include "sa_mm.hpp"
#include "bwt.hpp"
#include "mtf.hpp"
#include "rle0.hpp"
#include "huffman.hpp"
#include "fmi_container.hpp"

static std::vector<uint8_t> read_all(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) throw std::runtime_error("No se pudo abrir entrada: " + path);
    f.seekg(0, std::ios::end);
    std::streamoff n = f.tellg();
    f.seekg(0, std::ios::beg);
    std::vector<uint8_t> buf;
    buf.resize(static_cast<size_t>(n));
    if (n > 0) f.read(reinterpret_cast<char*>(buf.data()), n);
    return buf;
}
static void write_all(const std::string& path, const std::vector<uint8_t>& data) {
    std::ofstream f(path, std::ios::binary);
    if (!f) throw std::runtime_error("No se pudo abrir salida: " + path);
    if (!data.empty()) f.write(reinterpret_cast<const char*>(data.data()),
                               static_cast<std::streamsize>(data.size()));
}

static void cmd_compress(const std::string& in_txt, const std::string& out_fmi) {
    std::vector<uint8_t> T0 = read_all(in_txt);
    const bool had_sentinel = (!T0.empty() && T0.back() == 0x00);
    const uint32_t orig_len_no_sentinel = static_cast<uint32_t>(T0.size());

    std::vector<uint8_t> T = T0;
    if (!had_sentinel) T.push_back(0x00);

    auto SA = suffix_array_mm(T);
    auto L  = bwt_from_sa(T, SA);

    auto A   = alphabet_from_text(L);
    auto MTF = mtf_encode(L, A);
    auto RLE = rle0_encode(MTF);

    std::unordered_map<uint8_t, std::pair<uint32_t,uint8_t>> codes;
    std::vector<uint8_t> bitstream;
    huffman_encode(RLE, codes, bitstream);

    write_fmi(out_fmi, orig_len_no_sentinel, had_sentinel, A, codes,
              static_cast<uint32_t>(RLE.size()), bitstream);
}

static void cmd_decompress(const std::string& in_fmi, const std::string& out_txt) {
    FmiHeader H = read_fmi(in_fmi);

    auto RLE = huffman_decode(H.bitstream, H.codes, H.rle_len);
    auto MTF = rle0_decode(RLE);
    auto L   = mtf_decode(MTF, H.alphabet);
    auto T   = inverse_bwt(L);

    if (!H.had_sentinel) {
        if (T.size() < static_cast<size_t>(H.orig_len_no_sentinel)) {
            throw std::runtime_error("Descompresión inconsistente: texto más corto que el original");
        }
        T.resize(H.orig_len_no_sentinel);
    } else {
        if (T.size() != static_cast<size_t>(H.orig_len_no_sentinel)) {
            throw std::runtime_error("Descompresión inconsistente: longitud inesperada (had_sentinel=1)");
        }
    }

    write_all(out_txt, T);
}

static void print_usage() {
    std::cerr <<
        "Uso:\n"
        "  fmi_app compress <input.txt> -o <output.fmi>\n"
        "  fmi_app decompress <input.fmi> -o <output.txt>\n";
}
int main(int argc, char** argv) {
    try {
        if (argc < 2) { print_usage(); return 1; }
        std::string cmd = argv[1];
        if (cmd == "compress") {
            if (argc < 5) { print_usage(); return 1; }
            std::string input = argv[2], output;
            for (int i = 3; i + 1 < argc; ++i)
                if (std::string(argv[i]) == "-o") output = argv[i + 1];
            if (output.empty()) { print_usage(); return 1; }
            cmd_compress(input, output);
        } else if (cmd == "decompress") {
            if (argc < 5) { print_usage(); return 1; }
            std::string input = argv[2], output;
            for (int i = 3; i + 1 < argc; ++i)
                if (std::string(argv[i]) == "-o") output = argv[i + 1];
            if (output.empty()) { print_usage(); return 1; }
            cmd_decompress(input, output);
        } else {
            print_usage(); return 1;
        }
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 2;
    }
}
