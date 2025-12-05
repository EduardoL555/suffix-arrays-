import argparse
import time
import tracemalloc
import os
import sys

# Importamos los módulos locales que ya tienes en la carpeta scripts/
import manber_myers
import sais

def benchmark(file_path, queries):
    print(f"\n{'='*60}")
    print(f"Procesando archivo: {os.path.basename(file_path)}")
    print(f"{'='*60}")
    
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            text = f.read().strip() + "$"
    except Exception as e:
        print(f"Error crítico leyendo {file_path}: {e}")
        return

    # ---------------------------------------------------------
    # 1. MANBER-MYERS (Referencia confiable O(n log n))
    # ---------------------------------------------------------
    print(f"Ejecutando Manber-Myers (Longitud: {len(text)})...")
    tracemalloc.start()
    start_time = time.time()
    
    sa_mm = manber_myers.manber_myers(text)
    bwt_mm = manber_myers.build_bwt(text, sa_mm)
    C_mm, Occ_mm = manber_myers.build_tables(bwt_mm)
    
    end_time = time.time()
    _, peak_mem = tracemalloc.get_traced_memory()
    tracemalloc.stop()
    
    mm_time = end_time - start_time
    print(f" -> [MM] Tiempo: {mm_time:.4f}s | RAM: {peak_mem / 1024 / 1024:.2f} MB")

    # ---------------------------------------------------------
    # 2. SA-IS (Algoritmo lineal O(n) - A REPARAR)
    # ---------------------------------------------------------
    print(f"Ejecutando SA-IS...")
    tracemalloc.start()
    start_time = time.time()
    
    T_sais = list(text) # SA-IS suele trabajar con listas de enteros/chars
    sa_sais = sais.sais(T_sais)
    bwt_sais = sais.build_bwt(T_sais, sa_sais)
    C_sais, Occ_sais = sais.build_fm_index(bwt_sais)
    
    end_time = time.time()
    _, peak_mem = tracemalloc.get_traced_memory()
    tracemalloc.stop()
    
    sais_time = end_time - start_time
    print(f" -> [SA-IS] Tiempo: {sais_time:.4f}s | RAM: {peak_mem / 1024 / 1024:.2f} MB")
    
    # ---------------------------------------------------------
    # 3. VERIFICACIÓN (El paso clave para detectar tu error)
    # ---------------------------------------------------------
    if sa_mm != sa_sais:
        print("\n❌ ALERTA CRÍTICA: Los Suffix Arrays NO coinciden.")
        print("   Esto confirma que SA-IS falla en textos complejos (falta recursión).")
        # Opcional: imprimir primeros 10 fallos si es corto
    else:
        print("\n✅ ÉXITO: Los Suffix Arrays son idénticos.")

    # ---------------------------------------------------------
    # 4. CONSULTAS (FM-Index)
    # ---------------------------------------------------------
    print(f"\nProbando consultas: {queries}")
    for q in queries:
        c_mm = manber_myers.fm_search(q, C_mm, Occ_mm, bwt_mm)
        c_sais = sais.fm_search(q, C_sais, Occ_sais, bwt_sais)
        match = "✅" if c_mm == c_sais else "❌"
        print(f"  '{q}': MM={c_mm} vs SAIS={c_sais} {match}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Benchmark de Suffix Arrays")
    parser.add_argument("--books", nargs='+', required=True, help="Rutas a archivos de texto")
    parser.add_argument("--queries", nargs='+', required=True, help="Patrones a buscar")
    
    args = parser.parse_args()
    
    for book in args.books:
        benchmark(book, args.queries)