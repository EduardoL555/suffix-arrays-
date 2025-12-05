import sys
import time
import tracemalloc

# ===========================================================
# MANBER-MYERS (O(n log n))
# Implementación usando clases para el ranking de sufijos.
# ===========================================================

class RankPair:
    def __init__(self, left=0, right=0, index=0):
        self.left = left
        self.right = right
        self.index = index

def compute_new_ranks(pairs, n):
    rank = 1
    new_rank = [-1] * n
    new_rank[pairs[0].index] = rank

    for i in range(1, n):
        # Si el par actual es diferente al anterior, incrementamos rango
        if pairs[i].left != pairs[i - 1].left or pairs[i].right != pairs[i - 1].right:
            rank += 1
        new_rank[pairs[i].index] = rank

    return new_rank

def manber_myers(text):
    n = len(text)
    pairs = []

    # Inicialización con caracteres individuales
    for i in range(n):
        right = ord(text[i + 1]) if i < n - 1 else -1
        pairs.append(RankPair(ord(text[i]), right, i))

    pairs.sort(key=lambda x: (x.left, x.right))

    length = 2
    while length < n:
        rank = compute_new_ranks(pairs, n)
        new_list = []

        for i in range(n):
            next_rank = rank[i + length] if i + length < n else -1
            new_list.append(RankPair(rank[i], next_rank, i))

        # Ordenar por el nuevo par de rangos
        pairs = sorted(new_list, key=lambda x: (x.left, x.right))
        length *= 2

    # El Suffix Array son los índices ordenados
    return [p.index for p in pairs]

# ===========================================================
# FM-INDEX & BWT
# ===========================================================

def build_bwt(text, sa):
    bwt = []
    n = len(sa)
    for i in range(n):
        pos = sa[i] - 1
        ch = text[pos] if pos >= 0 else text[-1]
        bwt.append(ch)
    return "".join(bwt)

def build_tables(bwt):
    alphabet = sorted(set(bwt))
    Occ = {c: [0] * (len(bwt) + 1) for c in alphabet}
    C = {c: 0 for c in alphabet}

    # Tabla de Ocurrencias (Occ)
    for i, char in enumerate(bwt):
        for c in alphabet:
            Occ[c][i + 1] = Occ[c][i] + (1 if char == c else 0)

    # Tabla de Conteo Acumulado (C)
    total = 0
    for c in alphabet:
        C[c] = total
        total += Occ[c][len(bwt)]

    return C, Occ

def fm_search(pattern, C, Occ, bwt):
    l, r = 0, len(bwt) - 1

    for c in reversed(pattern):
        if c not in C:
            return 0
        l = C[c] + Occ[c][l]
        r = C[c] + Occ[c][r + 1] - 1
        if l > r:
            return 0
    return r - l + 1

# ===========================================================
# MAIN
# ===========================================================

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Uso: python manber_myers.py <archivo.txt>")
        print("Usando 'Hamlet.txt' por defecto...")
        filename = "Hamlet.txt"
    else:
        filename = sys.argv[1]

    try:
        with open(filename, "r", encoding="utf-8") as f:
            text = f.read().strip() + "$"
    except FileNotFoundError:
        print(f"Error: No se encontró {filename}")
        sys.exit(1)

    print(f"\nProcesando con Manber-Myers: {filename}")
    
    tracemalloc.start()
    start = time.time()

    # 1. Construir SA
    sa = manber_myers(text)
    
    # 2. Índices de búsqueda
    bwt = build_bwt(text, sa)
    C, Occ = build_tables(bwt)

    end = time.time()
    _, peak = tracemalloc.get_traced_memory()
    tracemalloc.stop()

    print(f"Tiempo total: {end - start:.6f} s")
    print(f"Pico de memoria: {peak / 1024 / 1024:.4f} MB")

    pattern = input("\nBuscar patrón: ").strip()
    if pattern:
        count = fm_search(pattern, C, Occ, bwt)
        print(f"Ocurrencias encontradas: {count}")
