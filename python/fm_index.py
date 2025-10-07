from typing import List, Dict, Tuple

def suffix_array_slow(s: str) -> List[int]:
    """Solo para validar FM-Index si se necesita (no se usa en bench)."""
    return sorted(range(len(s)), key=lambda i: s[i:])

def bwt_from_sa(s: str, sa: List[int]) -> str:
    n = len(s)
    return "".join(s[i-1] if i != 0 else s[-1] for i in sa)

class FMIndex:
    """
    FM-Index con Occ muestreado para ahorrar memoria.
    - C[c]: número de caracteres < c
    - Occ(c, i): # de c en BWT[0:i]
    Muestreo cada 'step' entradas.
    """
    def __init__(self, s: str, sa: List[int], step: int = 128):
        assert s[-1] == "$", "El texto debe terminar con sentinela '$'."
        self.s = s
        self.sa = sa
        self.n = len(s)
        self.step = step

        self.bwt = bwt_from_sa(s, sa)
        # Alfabeto ordenado
        self.alpha = sorted(set(self.bwt))
        # C array
        counts = {c: 0 for c in self.alpha}
        for ch in self.bwt:
            counts[ch] += 1
        total = 0
        self.C = {}
        for c in sorted(self.alpha):
            self.C[c] = total
            total += counts[c]

        # Occ muestreado
        self.checkpoints: Dict[str, List[int]] = {c: [0] for c in self.alpha}
        running = {c: 0 for c in self.alpha}
        for i, ch in enumerate(self.bwt, 1):
            running[ch] += 1
            if i % self.step == 0:
                for c in self.alpha:
                    self.checkpoints[c].append(running[c])
        # Guardar cuentas finales para facilitar límite superior
        self.running_final = running

    def _occ(self, c: str, i: int) -> int:
        """# de c en BWT[0:i] (i exclusivo)."""
        if i <= 0:
            return 0
        block = i // self.step
        offset = i % self.step
        base = self.checkpoints[c][block] if block < len(self.checkpoints[c]) else self.running_final[c]
        # Escanear remainder
        start = block * self.step
        for j in range(start, start + offset):
            if j >= self.n: break
            if self.bwt[j] == c:
                base += 1
        return base

    def search(self, pattern: str) -> List[int]:
        """Retorna posiciones (en SA) de todas las ocurrencias de `pattern` por backward search."""
        if not pattern:
            return []
        l, r = 0, self.n
        for ch in reversed(pattern):
            if ch not in self.C:
                return []
            l = self.C[ch] + self._occ(ch, l)
            r = self.C[ch] + self._occ(ch, r)
            if l >= r:
                return []
        # mapear rango [l, r) a SA
        return [self.sa[i] for i in range(l, r)]
