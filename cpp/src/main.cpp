// cpp/src/main.cpp
#include <bits/stdc++.h>
#include "suffix_array_mm.hpp"
#include "sais.hpp"
#include "fm_index.hpp"

using namespace std;

static string read_file(const string& path) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    ifstream in(path, ios::binary);
    if (!in) {
        throw runtime_error("No se pudo abrir el archivo: " + path);
    }
    ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

static void ensure_sentinel(string& s) {
    if (s.empty() || s.back() != '$') s.push_back('$');
}

// Mapea texto a enteros con '$' -> 0 y los demás a 1..σ-1 en orden estable
static vector<int> map_to_ints_with_sentinel(const string& s) {
    // recoger alfabeto
    array<bool, 256> seen{}; seen.fill(false);
    seen[(unsigned char)'$'] = true;
    for (unsigned char c : s) seen[c] = true;

    // asignar códigos
    vector<int> code(256, -1);
    code[(unsigned char)'$'] = 0;
    int nx = 1;
    for (int c = 0; c < 256; ++c) {
        if (c == (unsigned char)'$') continue;
        if (seen[c]) code[c] = nx++;
    }
    vector<int> T; T.reserve(s.size());
    for (unsigned char c : s) T.push_back(code[c]);
    return T;
}

static double secs_since(chrono::high_resolution_clock::time_point t0) {
    using namespace chrono;
    return duration_cast<duration<double>>(high_resolution_clock::now() - t0).count();
}

int main(int argc, char** argv) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // ---- Parseo simple de argumentos ----
    string algo, file, query;
    bool print_positions = false;

    for (int i = 1; i < argc; ++i) {
        string a = argv[i];
        if (a == "--algo" && i + 1 < argc) {
            algo = argv[++i];
        } else if (a == "--file" && i + 1 < argc) {
            file = argv[++i];
        } else if (a == "--query" && i + 1 < argc) {
            query = argv[++i];
        } else if (a == "--print-positions") {
            print_positions = true;
        } else if (a == "-h" || a == "--help") {
            cout <<
R"(Uso:
  sa_app --algo {mm|sais|fm} --file <ruta_txt> [--query <patron>] [--print-positions]

Ejemplos:
  sa_app --algo mm   --file ../data/libro1.txt
  sa_app --algo sais --file ../data/libro1.txt
  sa_app --algo fm   --file ../data/libro1.txt --query "the" --print-positions
)";
            return 0;
        } else {
            // ignora silenciosamente argumentos desconocidos
        }
    }

    if (algo.empty() || file.empty()) {
        cerr << "Error: falta --algo o --file (usa --help)." << '\n';
        return 1;
    }

    // ---- Carga de texto y sentinela ----
    string text;
    try {
        text = read_file(file);
    } catch (const exception& e) {
        cerr << e.what() << '\n';
        return 1;
    }
    ensure_sentinel(text);
    const int n = (int)text.size();

    try {
        if (algo == "mm") {
            // -------- Manber & Myers --------
            auto t0 = chrono::high_resolution_clock::now();
            vector<int> SA = suffix_array_mm(text); // API en suffix_array_mm.hpp/.cpp
            double t = secs_since(t0);
            cout << "SA (MM) construido en " << std::fixed << setprecision(6)
                 << t << " s. n=" << n << "\n";

        } else if (algo == "sais") {
            // -------- SA-IS --------
            vector<int> T = map_to_ints_with_sentinel(text);
            int K = 0;
            for (int v : T) if (v > K) K = v;
            K += 1;

            auto t0 = chrono::high_resolution_clock::now();
            vector<int> SA = sais(T, K);           // API en sais.hpp/.cpp
            double t = secs_since(t0);
            cout << "SA (SA-IS) construido en " << std::fixed << setprecision(6)
                 << t << " s. n=" << n << "\n";

        } else if (algo == "fm") {
            // -------- FM-Index (construido desde SA(MM)) --------
            auto t_sa0 = chrono::high_resolution_clock::now();
            vector<int> SA = suffix_array_mm(text);
            double t_sa = secs_since(t_sa0);

            auto t_fm0 = chrono::high_resolution_clock::now();
            FMIndex fm(text, SA);                  // API en fm_index.hpp/.cpp
            double t_fm = secs_since(t_fm0);

            cout << "SA (MM): " << std::fixed << setprecision(6)
                 << t_sa << " s, FM build: " << t_fm << " s\n";

            if (!query.empty()) {
                auto t_q0 = chrono::high_resolution_clock::now();
                vector<int> occ = fm.search(query);
                double t_q = secs_since(t_q0);
                cout << "Query '" << query << "': " << occ.size()
                     << " ocurrencias en " << std::scientific << setprecision(2)
                     << t_q << " s\n" << std::fixed;

                if (print_positions) {
                    cout << "Posiciones:";
                    for (int p : occ) cout << ' ' << p;
                    cout << "\n";
                }
            }
        } else {
            cerr << "Error: --algo debe ser mm | sais | fm\n";
            return 1;
        }

    } catch (const std::exception& e) {
        cerr << "Excepción: " << e.what() << '\n';
        return 2;
    } catch (...) {
        cerr << "Excepción desconocida.\n";
        return 3;
    }

    return 0;
}
