# Suffix Array Project (Manber & Myers + SA-IS + FM-Index)

Repositorio listo para **VS Code** con implementaciones en **Python** y **C++**, pruebas con libros, medición de **tiempo** y **memoria**, y búsqueda exacta vía **BWT + FM-Index**.

> **Nota**: Este repo incluye una **plantilla de reporte** en `docs/report_template.md` y una guía de **perfilado de memoria** para Python y C++ en `docs/profiling.md`.

## Estructura

```
.
├── README.md
├── .gitignore
├── docs/
│   ├── report_template.md
│   └── profiling.md
├── data/                # coloca aquí los libros (.txt)
├── python/
│   ├── README.md
│   ├── bench.py         # tiempos y memoria (tracemalloc) para SA y FM-Index
│   ├── manber_myers_fixed.py
│   ├── sais_fixed.py
│   └── fm_index.py
├── cpp/
│   ├── README.md
│   ├── CMakeLists.txt
│   ├── include/
│   │   ├── bitvector.hpp
│   │   ├── suffix_array_mm.hpp
│   │   ├── sais.hpp
│   │   └── fm_index.hpp
│   └── src/
│       ├── main.cpp
│       ├── suffix_array_mm.cpp
│       ├── sais.cpp
│       └── fm_index.cpp
└── scripts/
    ├── run_python_bench.sh
    └── run_python_bench.ps1
```

## Paso a paso (resumen)

1) **Crear repo privado** en GitHub/Bitbucket y subir este contenido.  
2) **Colocar 4 libros** `.txt` en `data/`. (Ej. de Project Gutenberg).  
3) **Python (sin librerías externas)**:
   - Construir SA con **Manber&Myers** y **SA-IS**.
   - Construir **FM-Index** y hacer búsquedas exactas.
   - Medir tiempo y memoria con `bench.py` (usa `tracemalloc`).  
4) **C++ (VS Code + CMake)**:
   - Compilar y ejecutar, medir tiempo con `chrono`.  
   - Para memoria: usar **Valgrind/Massif** (Linux) o **Windows Performance Analyzer / VS Diagnostic Tools** (Windows).  
5) **Reporte** en `docs/report_template.md`, incluyendo discusión, complejidades y comparación.

---

## Comandos rápidos

### Python
```bash
cd python
# Construye SA con ambos algoritmos y FM-Index; mide tiempo y memoria
python bench.py --books ../data/libro1.txt ../data/libro2.txt ../data/libro3.txt ../data/libro4.txt --queries "the" "and" "lorem"
```

### C++
```bash
cd cpp
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
# SA con Manber&Myers
./build/sa_app --algo mm  --file ../data/libro1.txt
# SA con SA-IS
./build/sa_app --algo sais --file ../data/libro1.txt
# Búsqueda exacta con FM-Index
./build/sa_app --algo fm --file ../data/libro1.txt --query "pattern"
```

> Para perfilado de memoria: ver `docs/profiling.md`.

---

## Equipo y commits
- Trabajo en equipos de **3** personas.  
- Crear repo **privado** y **hacer commits frecuentes** mostrando la evolución (mensajes claros: `fix(mm): rank loop off-by-one`, `feat: fm-index search`, `docs: complejidad SA-IS`, etc.).
