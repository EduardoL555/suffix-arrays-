# python/bench.py
import argparse, time, tracemalloc, os, sys
from typing import List, Tuple

# Import local modules
from manber_myers_fixed import suffix_array as mm_sa
from sais_fixed import sais as sais_sa
from fm_index import FMIndex

def read_text(path: str, max_chars: int | None = None) -> str:
    with open(path, "r", encoding="utf-8", errors="ignore") as f:
        s = f.read()
    if max_chars and max_chars > 0:
        # Reservar 1 para el sentinela
        s = s[:max_chars - 1] if len(s) >= max_chars else s
    if not s or s[-1] != "$":
        s += "$"  # asegurar sentinela
    return s

def to_ints_with_sentinel(s: str) -> List[int]:
    # mapear '$' a 0, el resto a 1.. (orden ASCII simple)
    code = {"$": 0}
    nxt = 1
    for c in sorted(set(s)):
        if c == "$": continue
        code[c] = nxt; nxt += 1
    return [code[c] for c in s]

def measure(func, *args, **kwargs) -> Tuple[float, int, object]:
    """
    Devuelve (seconds, peak_kb, result)
    """
    tracemalloc.start()
    t0 = time.perf_counter()
    res = func(*args, **kwargs)
    secs = time.perf_counter() - t0
    _, peak = tracemalloc.get_traced_memory()
    tracemalloc.stop()
    return secs, peak // 1024, res

def bench_books(books: List[str], queries: List[str], status=False,
                skip_mm=False, skip_sais=False, skip_fm=False, max_chars=None):
    header = f"{'Libro':30} | {'Algoritmo':10} | {'Tiempo (s)':>10} | {'Pico Mem (KB)':>13} | Extra"
    print(header, flush=True)
    print("-"*len(header), flush=True)

    for path in books:
        s = read_text(path, max_chars=max_chars)
        T = to_ints_with_sentinel(s)

        base = os.path.basename(path)

        # Manber & Myers
        if not skip_mm:
            if status: print(f"[{base}] → MM…", flush=True)
            t_mm, m_mm, SA_mm = measure(mm_sa, s)
            print(f"{base:30} | {'MM':10} | {t_mm:10.4f} | {m_mm:13d} | SA ok: {len(SA_mm)==len(s)}", flush=True)
        else:
            SA_mm = None

        # SA-IS
        if not skip_sais:
            if status: print(f"[{base}] → SA-IS…", flush=True)
            t_si, m_si, SA_si = measure(sais_sa, T)
            print(f"{base:30} | {'SA-IS':10} | {t_si:10.4f} | {m_si:13d} | SA ok: {len(SA_si)==len(s)}", flush=True)

        # FM-Index
        if not skip_fm:
            # construimos FM con el SA de MM si lo tenemos, o lo recomputamos
            if SA_mm is None:
                if status: print(f"[{base}] → MM (para FM)…", flush=True)
                _, _, SA_mm = measure(mm_sa, s)
            if status: print(f"[{base}] → FM-build…", flush=True)
            t_fm, m_fm, fm = measure(FMIndex, s, SA_mm)
            print(f"{base:30} | {'FM-build':10} | {t_fm:10.4f} | {m_fm:13d} | BWT len: {len(fm.bwt)}", flush=True)

            for q in queries:
                if status: print(f"[{base}] → FM-search '{q}'…", flush=True)
                t_q, m_q, occs = measure(fm.search, q)
                print(f"{base:30} | {'FM-search':10} | {t_q:10.6f} | {m_q:13d} | '{q}' -> {len(occs)} hits", flush=True)

if __name__ == "__main__":
    ap = argparse.ArgumentParser()
    ap.add_argument("--books", nargs="+", required=True, help="Rutas a libros .txt")
    ap.add_argument("--queries", nargs="*", default=[], help="Cadenas a buscar (exact match)")
    ap.add_argument("--status", action="store_true", help="Imprime progreso por fases")
    ap.add_argument("--skip-mm", action="store_true", help="Omitir Manber & Myers")
    ap.add_argument("--skip-sais", action="store_true", help="Omitir SA-IS")
    ap.add_argument("--skip-fm", action="store_true", help="Omitir FM-Index")
    ap.add_argument("--max_chars", type=int, default=0, help="Truncar cada libro a N chars (0 = completo)")
    args = ap.parse_args()

    bench_books(args.books, args.queries, status=args.status,
                skip_mm=args.skip_mm, skip_sais=args.skip_sais, skip_fm=args.skip_fm,
                max_chars=(args.max_chars if args.max_chars > 0 else None))
