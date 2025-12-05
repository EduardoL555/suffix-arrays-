import sys

# ===========================================================
# SA-IS (Suffix Array - Induced Sorting) - ROBUST
# Solución: Wrapper con centinela virtual \0 para consistencia
# ===========================================================

def get_counts(T):
    counts = {}
    for c in T:
        counts[c] = counts.get(c, 0) + 1
    return counts

def get_bucket_starts(counts):
    starts = {}
    total = 0
    for c in sorted(counts.keys()):
        starts[c] = total
        total += counts[c]
    return starts

def get_bucket_ends(counts):
    ends = {}
    total = 0
    for c in sorted(counts.keys()):
        total += counts[c]
        ends[c] = total
    return ends

def induce_sort(T, sa, types, counts):
    # 1. Inducir L-types (Izquierda -> Derecha)
    bkt = get_bucket_starts(counts)
    for i in range(len(sa)):
        j = sa[i] - 1
        if j >= 0 and types[j] == "L":
            c = T[j]
            sa[bkt[c]] = j
            bkt[c] += 1

    # 2. Inducir S-types (Derecha -> Izquierda)
    bkt = get_bucket_ends(counts)
    for i in range(len(sa) - 1, -1, -1):
        j = sa[i] - 1
        if j >= 0 and types[j] == "S":
            c = T[j]
            bkt[c] -= 1
            sa[bkt[c]] = j

def sais_core(T):
    n = len(T)
    
    # === Clasificación de Sufijos ===
    types = [""] * n
    types[n - 1] = "S"
    for i in range(n - 2, -1, -1):
        if T[i] < T[i + 1]:
            types[i] = "S"
        elif T[i] > T[i + 1]:
            types[i] = "L"
        else:
            types[i] = types[i + 1]

    counts = get_counts(T)
    
    # === PASO 1: LMS Tentativos ===
    sa = [-1] * n
    bkt = get_bucket_ends(counts)
    for i in range(n - 1, 0, -1):
        # is_lms check inline
        if types[i] == "S" and types[i - 1] == "L":
            c = T[i]
            bkt[c] -= 1
            sa[bkt[c]] = i
            
    # === PASO 2: Inducción Base ===
    induce_sort(T, sa, types, counts)
    
    # === PASO 3: Renombrar LMS ===
    lms_sorted = [x for x in sa if x > 0 and types[x] == "S" and types[x - 1] == "L"]
    
    lms_names = [-1] * n
    current_name = 0
    lms_names[lms_sorted[0]] = current_name
    
    for i in range(1, len(lms_sorted)):
        prev = lms_sorted[i - 1]
        curr = lms_sorted[i]
        
        diff = False
        for d in range(n):
            if prev + d >= n or curr + d >= n:
                diff = True; break
            if T[prev + d] != T[curr + d]:
                diff = True; break
            if d > 0:
                is_prev_lms = (types[prev+d] == "S" and types[prev+d-1] == "L")
                is_curr_lms = (types[curr+d] == "S" and types[curr+d-1] == "L")
                if is_prev_lms or is_curr_lms:
                    break
        
        if diff: current_name += 1
        lms_names[curr] = current_name
        
    # === PASO 4: Recursión ===
    lms_indices = [i for i in range(n) if i > 0 and types[i] == "S" and types[i - 1] == "L"]
    T1 = [lms_names[i] for i in lms_indices]
    
    if current_name < len(lms_indices) - 1:
        sa1 = sais_core(T1)
    else:
        sa1 = [-1] * len(T1)
        for i, val in enumerate(T1): sa1[val] = i
            
    # === PASO 5: Inducción Final ===
    lms_correct = [lms_indices[x] for x in sa1]
    
    sa = [-1] * n
    bkt = get_bucket_ends(counts)
    for i in range(len(lms_correct) - 1, -1, -1):
        idx = lms_correct[i]
        c = T[idx]
        bkt[c] -= 1
        sa[bkt[c]] = idx
        
    induce_sort(T, sa, types, counts)
    return sa

def sais(T):
    """
    Wrapper Principal:
    Convierte a enteros y añade un centinela 0 virtual para garantizar
    que el algoritmo SA-IS funcione correctamente incluso si el texto
    contiene caracteres menores que el último (ej. espacios < $).
    """
    # Convertir string a lista de enteros (safe mapping)
    if isinstance(T, str):
        T_ints = [ord(c) for c in T]
    else:
        T_ints = [ord(c) if isinstance(c, str) else c for c in T]
    
    # Añadir centinela estricto (0)
    T_ints.append(0)
    
    # Calcular SA sobre T + \0
    sa = sais_core(T_ints)
    
    # El resultado tendrá el centinela (índice len(T)) en la posición 0.
    # Lo removemos para devolver el SA del texto original.
    return sa[1:]

# ===========================================================
# FUNCIONES AUXILIARES
# ===========================================================

def build_bwt(T, sa):
    bwt = []
    for i in sa:
        if i == 0: bwt.append("$")
        else: bwt.append(T[i - 1])
    return "".join(bwt)

def build_fm_index(bwt):
    alphabet = sorted(set(bwt))
    C = {}
    total = 0
    for c in alphabet:
        C[c] = total
        total += bwt.count(c)
    Occ = {c: [0] for c in alphabet}
    for char in bwt:
        for c in alphabet:
            Occ[c].append(Occ[c][-1] + (1 if char == c else 0))
    return C, Occ

def fm_search(pattern, C, Occ, bwt):
    l, r = 0, len(bwt)
    for c in reversed(pattern):
        if c not in C: return 0
        l = C[c] + Occ[c][l]
        r = C[c] + Occ[c][r]
        if l >= r: return 0
    return r - l

if __name__ == "__main__":
    if len(sys.argv) < 2: sys.exit(1)
    try:
        with open(sys.argv[1], "r", encoding="utf-8") as f:
            text = f.read().strip() + "$"
        print(len(sais(text)))
    except: pass