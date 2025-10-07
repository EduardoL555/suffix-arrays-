# python/sais_fixed.py
from typing import List, Tuple

# -------------------- Utilidades internas --------------------

def _ls_types(T: List[int]) -> List[bool]:
    """
    S[i] = True si el sufijo en i es de tipo S; False si es L.
    Reglas:
      - S[n-1] = True
      - Para i<n-1: S[i] = (T[i] < T[i+1]) or (T[i] == T[i+1] and S[i+1])
    """
    n = len(T)
    S = [False] * n
    S[-1] = True
    for i in range(n - 2, -1, -1):
        if T[i] < T[i + 1] or (T[i] == T[i + 1] and S[i + 1]):
            S[i] = True
    return S

def _is_LMS(S: List[bool], i: int) -> bool:
    return i > 0 and S[i] and not S[i - 1]

def _lms_positions(S: List[bool]) -> List[int]:
    n = len(S)
    L = [i for i in range(1, n) if S[i] and not S[i - 1]]
    # asegurar que n-1 esté exactamente una vez
    if not L or L[-1] != n - 1:
        L.append(n - 1)
    return L

def _buckets(T: List[int], K: int) -> Tuple[List[int], List[int]]:
    """
    head[c] = índice inicial (inclusive) del bucket de c
    tail[c] = índice final (exclusivo) del bucket de c
    """
    cnt = [0] * K
    for c in T:
        cnt[c] += 1
    head = [0] * K
    tail = [0] * K
    s = 0
    for c in range(K):
        head[c] = s
        s += cnt[c]
        tail[c] = s
    return head, tail

def _place_LMS(T: List[int], S: List[bool], K: int, LMS: List[int]) -> List[int]:
    """
    Coloca los LMS en la cola de cada bucket (respetando límites).
    """
    n = len(T)
    SA = [-1] * n
    head, tail = _buckets(T, K)
    tailp = tail[:]
    for i in reversed(LMS):
        c = T[i]
        # insertar en la cola del bucket de c
        start = head[c]
        p = tailp[c]
        # p es exclusivo; el slot real es p-1
        while p > start and SA[p - 1] != -1:
            p -= 1
        if p > start:
            p -= 1
            SA[p] = i
            tailp[c] = p
    return SA

def _induce_L(T: List[int], S: List[bool], K: int, SA: List[int]) -> None:
    """
    Induce sufijos L en la cabeza de sus buckets.
    Con límites de bucket para evitar sobrescrituras fuera de rango.
    """
    head, tail = _buckets(T, K)
    headp = head[:]   # punteros de cabeza
    n = len(SA)
    for pos in range(n):
        j = SA[pos]
        if j > 0:
            i = j - 1
            if not S[i]:  # L-type
                c = T[i]
                if c < 0 or c >= K:
                    continue
                limit = tail[c]   # exclusivo
                p = headp[c]
                while p < limit and SA[p] != -1:
                    p += 1
                if p < limit:
                    SA[p] = i
                    headp[c] = p + 1

def _induce_S(T: List[int], S: List[bool], K: int, SA: List[int]) -> None:
    """
    Induce sufijos S en la cola de sus buckets.
    Con límites de bucket para evitar sobrescrituras fuera de rango.
    """
    head, tail = _buckets(T, K)
    tailp = tail[:]   # punteros de cola (exclusivo)
    n = len(SA)
    for pos in range(n - 1, -1, -1):
        j = SA[pos]
        if j > 0:
            i = j - 1
            if S[i]:  # S-type
                c = T[i]
                if c < 0 or c >= K:
                    continue
                start = head[c]  # inclusivo
                p = tailp[c]    # exclusivo
                while p > start and SA[p - 1] != -1:
                    p -= 1
                if p > start:
                    p -= 1
                    SA[p] = i
                    tailp[c] = p

def _lms_equal(T: List[int], S: List[bool], i: int, j: int) -> bool:
    """
    Compara LMS-substrings iniciando en i y j.
    Iguales si las secuencias coinciden y ambos terminan exactamente en un siguiente LMS.
    """
    n = len(T)
    k = 0
    while True:
        # límites de seguridad
        if i + k >= n or j + k >= n:
            return False
        if T[i + k] != T[j + k]:
            return False
        i_end = (i + k + 1 < n) and _is_LMS(S, i + k + 1)
        j_end = (j + k + 1 < n) and _is_LMS(S, j + k + 1)
        if i_end and j_end:
            return True
        if i_end != j_end:
            return False
        k += 1

# -------------------- SA-IS principal --------------------

def sais(T: List[int], K: int = None) -> List[int]:
    """
    Construye el SA de T (lista de enteros en [0..K)) usando SA-IS.
    Requiere sentinela mínimo 0 al final: T[-1] == 0.
    """
    n = len(T)
    assert n > 0 and T[-1] == 0, "T debe terminar en sentinela mínimo (0)."
    if K is None:
        K = max(T) + 1

    # Tipado L/S y LMS
    S = _ls_types(T)
    LMS = _lms_positions(S)

    # 1) Primera pasada: colocar LMS y hacer induced sorting
    SA = _place_LMS(T, S, K, LMS)
    _induce_L(T, S, K, SA)
    _induce_S(T, S, K, SA)

    # 2) Asignar nombres a LMS-substrings en el orden en que aparecen en SA
    lms_set = set(LMS)
    lms_in_SA = [p for p in SA if p >= 0 and p in lms_set]

    name = -1
    names = [-1] * n
    prev = -1
    for p in lms_in_SA:
        if prev == -1:
            name = 0
            names[p] = name
            prev = p
        else:
            if _lms_equal(T, S, prev, p):
                names[p] = name
            else:
                name += 1
                names[p] = name
                prev = p

    # Asegurar que el LMS del sentinela (n-1) tenga nombre 0
    sentinel_idx = LMS[-1]  # por construcción es n-1
    sentinel_name = names[sentinel_idx]
    if sentinel_name != 0:
        uniq = sorted(set(x for x in names if x != -1))
        remap = {sentinel_name: 0}
        nxt = 1
        for x in uniq:
            if x == sentinel_name:
                continue
            remap[x] = nxt
            nxt += 1
        for i in range(n):
            if names[i] != -1:
                names[i] = remap[names[i]]

    # Secuencia reducida en orden de aparición de LMS en el texto
    reduced = [names[i] for i in LMS if names[i] != -1]

    # Garantizar que la reducida termine con 0
    if not reduced or reduced[-1] != 0:
        if reduced:
            last_val = reduced[-1]
            uniq = []
            seen = set()
            for x in reduced[:-1]:
                if x not in seen:
                    uniq.append(x); seen.add(x)
            newmap = {last_val: 0}
            nxt = 1
            for x in sorted(uniq):
                if x == last_val:
                    continue
                newmap[x] = nxt; nxt += 1
            reduced = [newmap[x] for x in reduced]
        else:
            reduced = [0]

    # 3) Recurse si hay nombres repetidos
    maxname = max(reduced) if reduced else 0
    if maxname + 1 < len(reduced):
        K2 = maxname + 1
        SA_reduced = sais(reduced, K=K2)  # la reducida termina en 0
        ordered_LMS = [LMS[i] for i in SA_reduced]
    else:
        # Todos únicos: el orden es por nombre
        ordered_LMS = [x for _, x in sorted((names[i], i) for i in LMS)]

    # 4) Induced sorting final usando LMS ya ordenados
    SA = [-1] * n
    head, tail = _buckets(T, K)
    tailp = tail[:]
    for i in reversed(ordered_LMS):
        c = T[i]
        start = head[c]
        p = tailp[c]
        while p > start and SA[p - 1] != -1:
            p -= 1
        if p > start:
            p -= 1
            SA[p] = i
            tailp[c] = p

    _induce_L(T, S, K, SA)
    _induce_S(T, S, K, SA)

    return SA

# -------------------- Prueba rápida --------------------
if __name__ == "__main__":
    s = "mississippi$"
    # mapear '$' a 0 y demás a 1.. (ASCII simple)
    code = {"$": 0}
    nxt = 1
    for ch in sorted(set(s)):
        if ch == "$": continue
        code[ch] = nxt; nxt += 1
    T = [code[ch] for ch in s]
    SA = sais(T, K=max(T)+1)
    print(SA)
