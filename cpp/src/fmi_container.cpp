// src/fmi_container.cpp
#include "fmi_container.hpp"

#include <fstream>
#include <stdexcept>
#include <cstring>
#include <string>

namespace {
    constexpr char MAGIC[4] = {'F','M','I','1'};

    template <typename T>
    void write_pod(std::ofstream& f, const T& v) {
        f.write(reinterpret_cast<const char*>(&v), sizeof(T));
    }
    template <typename T>
    void read_pod(std::ifstream& f, T& v) {
        f.read(reinterpret_cast<char*>(&v), sizeof(T));
    }
}

void write_fmi(const std::string& outPath,
               uint32_t orig_len_no_sentinel,
               bool had_sentinel,
               const std::vector<uint8_t>& alphabet,
               const std::unordered_map<uint8_t, std::pair<uint32_t,uint8_t>>& codes,
               uint32_t rle_len,
               const std::vector<uint8_t>& bitstream)
{
    std::ofstream f(outPath, std::ios::binary);
    if (!f) throw std::runtime_error("No se pudo abrir el archivo de salida .fmi");

    f.write(MAGIC, 4);

    write_pod<uint32_t>(f, orig_len_no_sentinel);
    uint8_t had = had_sentinel ? 1u : 0u;
    write_pod<uint8_t>(f, had);

    if (alphabet.size() > 0xFFFF) {
        throw std::runtime_error("Alfabeto demasiado grande para u16");
    }
    uint16_t alph_size = static_cast<uint16_t>(alphabet.size());
    write_pod<uint16_t>(f, alph_size);
    if (alph_size) {
        f.write(reinterpret_cast<const char*>(alphabet.data()), alph_size);
    }

    if (codes.size() > 0xFFFF) {
        throw std::runtime_error("Codebook demasiado grande para u16");
    }
    uint16_t n = static_cast<uint16_t>(codes.size());

    std::string blob;
    blob.resize(2 + n * 6);
    char* p = blob.data();
    std::memcpy(p, &n, 2); p += 2;
    for (const auto& kv : codes) {
        uint8_t sym = kv.first;
        uint8_t len = kv.second.second;
        uint32_t code = kv.second.first;
        *p++ = static_cast<char>(sym);
        *p++ = static_cast<char>(len);
        std::memcpy(p, &code, 4); p += 4;
    }

    uint32_t blob_len = static_cast<uint32_t>(blob.size());
    write_pod<uint32_t>(f, blob_len);
    if (blob_len) f.write(blob.data(), blob_len);

    write_pod<uint32_t>(f, rle_len);
    uint32_t bs_len = static_cast<uint32_t>(bitstream.size());
    write_pod<uint32_t>(f, bs_len);
    if (bs_len) {
        f.write(reinterpret_cast<const char*>(bitstream.data()), bs_len);
    }

    if (!f) throw std::runtime_error("Fallo al escribir .fmi");
}

FmiHeader read_fmi(const std::string& inPath)
{
    std::ifstream f(inPath, std::ios::binary);
    if (!f) throw std::runtime_error("No se pudo abrir el archivo .fmi");

    char magic[4] = {0,0,0,0};
    f.read(magic, 4);
    if (!f || std::string(magic, 4) != std::string(MAGIC, 4)) {
        throw std::runtime_error("Archivo .fmi inválido (MAGIC)");
    }

    FmiHeader H{};
    read_pod<uint32_t>(f, H.orig_len_no_sentinel);

    uint8_t had = 0;
    read_pod<uint8_t>(f, had);
    H.had_sentinel = (had != 0);

    uint16_t alph_size = 0;
    read_pod<uint16_t>(f, alph_size);
    H.alphabet.resize(alph_size);
    if (alph_size) {
        f.read(reinterpret_cast<char*>(H.alphabet.data()), alph_size);
    }

    uint32_t blob_len = 0;
    read_pod<uint32_t>(f, blob_len);
    std::string blob;
    blob.resize(blob_len);
    if (blob_len) {
        f.read(blob.data(), blob_len);
    }

    if (blob_len < 2) throw std::runtime_error("Codebook corrupto (len < 2)");
    const char* p = blob.data();
    const char* end = blob.data() + blob.size();

    uint16_t n = 0;
    std::memcpy(&n, p, 2); p += 2;

    for (uint16_t i = 0; i < n; ++i) {
        if (p + 6 > end) throw std::runtime_error("Codebook truncado");
        uint8_t sym = static_cast<uint8_t>(*p++);
        uint8_t len = static_cast<uint8_t>(*p++);
        uint32_t code = 0;
        std::memcpy(&code, p, 4); p += 4;
        H.codes[sym] = {code, len};
    }

    read_pod<uint32_t>(f, H.rle_len);

    uint32_t bs_len = 0;
    read_pod<uint32_t>(f, bs_len);
    H.bitstream.resize(bs_len);
    if (bs_len) {
        f.read(reinterpret_cast<char*>(H.bitstream.data()), bs_len);
    }

    if (!f) throw std::runtime_error("Fallo al leer .fmi");
    return H;
}
