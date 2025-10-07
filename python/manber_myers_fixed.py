from typing import List, Tuple

class SubstrRank:
    __slots__ = ("left_rank", "right_rank", "index")
    def __init__(self, left_rank: int = 0, right_rank: int = 0, index: int = 0):
        self.left_rank = left_rank
        self.right_rank = right_rank
        self.index = index

def make_ranks(sorted_triplets: List[SubstrRank], n: int) -> List[int]:
    """
    Recibe la lista de tripletas (rank_left, rank_right, index) **ordenada**,
    y devuelve el arreglo rank por índice de inicio del sufijo.
    Corrección clave: el bucle inicia en i=1 (no en 0) para evitar comparar con i-1 = -1.
    """
    rank = [-1] * n
    r = 0
    prev_left, prev_right = None, None

    for i, sr in enumerate(sorted_triplets):
        if i == 0 or sr.left_rank != prev_left or sr.right_rank != prev_right:
            r += 1
            prev_left, prev_right = sr.left_rank, sr.right_rank
        rank[sr.index] = r
    return rank

def suffix_array(text: str) -> List[int]:
    """
    Manber & Myers (doubling). Devuelve SA de `text`.
    Requiere que `text` termine con un sentinel menor a cualquier símbolo (p. ej. '$').
    """
    n = len(text)
    # Inicial: rank por primer carácter, rank derecho por siguiente carácter
    triplets = [SubstrRank(ord(text[i]), ord(text[i+1]) if i+1 < n else -1, i) for i in range(n)]
    triplets.sort(key=lambda sr: (sr.left_rank, sr.right_rank))

    k = 1
    while k < n:
        # Recalcular rank por índice
        rank = make_ranks(triplets, n)

        # Construir nuevas tripletas por **índice** (no por posición en la lista ordenada)
        new_triplets = [
            SubstrRank(rank[i], rank[i + k] if i + k < n else -1, i)
            for i in range(n)
        ]
        # Ordenar por (rank_left, rank_right)
        new_triplets.sort(key=lambda sr: (sr.left_rank, sr.right_rank))

        triplets = new_triplets
        k <<= 1

    # Extraer SA
    SA = [sr.index for sr in triplets]
    return SA

if __name__ == "__main__":
    s = "mississippi$"
    print(suffix_array(s))
