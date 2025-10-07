# Guía de perfilado de memoria y tiempo

## Python (sin librerías externas)
- We use `time.perf_counter()` para tiempo y `tracemalloc` para memoria.
- Ejemplo: `python bench.py --books ../data/libro1.txt ...`

## Python (opcional)
- `pip install memory_profiler`
- Ejecutar con `python -m memory_profiler bench.py ...` para ver línea a línea.

## C++
### Linux / WSL
- **Valgrind (Massif)**:
  ```bash
  valgrind --tool=massif ./build/sa_app --algo sais --file ../data/libro1.txt
  ms_print massif.out.<pid> | less
  ```
- **/proc**:
  - Leer `VmHWM` y `VmRSS` en `/proc/<pid>/status` desde el programa para pico de memoria (opcional).

### Windows
- **Visual Studio Diagnostic Tools** (desde VS / VS Code con CMake Tools).
- **Windows Performance Analyzer (WPA)**.
- Alternativa simple: medir tamaño de estructuras que creamos (SA, BWT, Occ) y estimar memoria usada.

## Buenas prácticas
- Ejecutar varias veces y promediar.
- Asegurar mismas condiciones (release build, cerrar apps pesadas).
