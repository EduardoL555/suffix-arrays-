# Python

## Requisitos
- Python 3.x **base** (no se requieren librerías externas).

## Uso
```bash
cd python
python bench.py --books ../data/libro1.txt ../data/libro2.txt ../data/libro3.txt ../data/libro4.txt --queries "the" "and" "lorem"
```
- Genera tiempos y picos de memoria para:
  - Suffix Array por **Manber & Myers**
  - Suffix Array por **SA-IS**
  - **FM-Index** (construcción + búsqueda exacta de queries)

Los resultados se imprimen en consola en formato de tabla.
