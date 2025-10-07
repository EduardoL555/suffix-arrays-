
# Actividad Integradora — Suffix Arrays, SA-IS y FM-Index
**Equipo:** 3 integrantes · **Entorno:** Windows + VS Code · **Lenguajes:** Python y C++

---

## 1) Objetivo
Corregir e implementar **Manber & Myers** y **SA-IS**, portar a **C++**, y añadir **BWT + FM-Index** con búsqueda exacta.
Medir **tiempos** y **memoria** en 4 libros (.txt) y documentar el proceso, optimizaciones y aprendizajes.

---

## 2) Entorno y cómo correr
**Python (sin librerías extra):**
```powershell
cd python
python bench.py --books ../data/libro1.txt ../data/libro2.txt ../data/libro3.txt ../data/libro4.txt --queries "the" "and" " de " " la " --status --skip-sais
# SA-IS (Python) truncado a 200k para costo razonable
python bench.py --books ../data/libro1.txt --status --skip-mm --skip-fm --max_chars 200000
python bench.py --books ../data/libro2.txt --status --skip-mm --skip-fm --max_chars 200000
python bench.py --books ../data/libro3.txt --status --skip-mm --skip-fm --max_chars 200000
python bench.py --books ../data/libro4.txt --status --skip-mm --skip-fm --max_chars 200000
```

**C++ (CMake):**
```powershell
cd cpp
cmake --build build --config Release
# Selección de ejecutable en PowerShell 5.1:
if (Test-Path .\build\Release\sa_app.exe) { $exe = ".\build\Release\sa_app.exe" } elseif (Test-Path .\build\sa_app.exe) { $exe = ".\build\sa_app.exe" } else { throw "No se encontró sa_app.exe" }

# Ejecutar
& $exe --algo mm   --file ..\data\libro1.txt
& $exe --algo sais --file ..\data\libro1.txt
& $exe --algo fm   --file ..\data\libro1.txt --query "the" --print-positions  > ..\data\occ_the_libro1.txt
```

---

## 3) Correcciones clave

### 3.1 Manber & Myers (Python)
- **Bug:** *off-by-one* al asignar `rank` (comparaba con `i-1` desde `i=0`), y reconstrucción de tripletas con índice incorrecto.
- **Fix:** recalcular `rank` desde tripletas **ya ordenadas** y reconstruir por **índice real** del sufijo.

### 3.2 SA-IS (C++)
- **Causa del crash:** duplicación de `n−1` como LMS → sobre-decremento de cola de bucket y escritura fuera de rango.
- **Fixes:** (i) `lms_positions` sin duplicado; (ii) inducción L/S con **límites de bucket** y salto de celdas ocupadas;
  (iii) `_lms_equal` con chequeos de límites; (iv) `K = max(T)+1` dinámico.

### 3.3 SA-IS (Python)
- **Robustez:** igual que en C++ (no duplicar LMS final; inducción con límites; `lms_in_SA` sin recrear `set` y filtrando `-1`).
- **Recursión estable:** garantizamos que la **secuencia reducida termine en 0** (sentinela) antes de recursar.

### 3.4 FM-Index (C++)
- Se añadió `--print-positions` para **listar todas** las ocurrencias del patrón (además del conteo).

---

## 4) Paso de parámetros — “por valor” vs “por referencia”
- **Python:** nombres referencian objetos; **slicing** (`T[i:j]`) y `list(...)` **copian** (efecto “por valor”). Cambiamos a trabajo por **índices** e **in-place** para evitar copias.
- **C++:** pasaje por **referencia const** (`const std::vector<int>&`) y escritura controlada dentro de buckets; ahorro de copias y mejor localidad.

---

## 5) Bit Vector vs Diccionario (C++)
- Para el tipado L/S en SA-IS, reemplazamos diccionario por **bit vector**: 1 bit por entrada, memoria ≈ `n/8` bytes.
- Ej.: `n=448,930` → ~56 KB; `unordered_map<int,bool>` puede ocupar **decenas de MB**.
- Impacto: **mejor localidad de caché**, menos *heap churn* y **inducción** más estable.

---

## 6) Resultados — Python (MM + FM-Index)
*(del comando con `--skip-sais`)*

| Libro     | MM: Tiempo (s) | MM: Pico Mem (KB) | FM-build: Tiempo (s) | FM-build: Pico Mem (KB) | 'the' hits | 'and' hits | ' de ' hits | ' la ' hits |
|-----------|----------------:|------------------:|---------------------:|-------------------------:|-----------:|-----------:|------------:|------------:|
| libro1.txt|          45.6800|            144330 |               0.9617 |                    4544  |       5472 |       3313 |           2 |           0 |
| libro2.txt|          14.4588|             54011 |               0.3544 |                    1926  |       2312 |        955 |           0 |           0 |
| libro3.txt|          14.1229|             53344 |               0.3448 |                    1788  |       1389 |        658 |           0 |           0 |
| libro4.txt|          41.9891|            133869 |               0.8924 |                    4358  |       3705 |       1966 |           1 |           0 |

**Notas:** FM-Index hace *backward search* con tiempos sub-milisegundo por consulta y baja memoria adicional.

---

## 7) Resultados — Python (SA-IS, `--max_chars 200000`)
*(para costo razonable en Python puro)*

| Libro     | SA-IS: Tiempo (s) | Pico Mem (KB) |
|-----------|-------------------:|--------------:|
| libro1.txt|            218.8078|         31753 |
| libro2.txt|            135.6484|         29839 |
| libro3.txt|            104.4783|         29330 |
| libro4.txt|            188.7542|         32550 |

---

## 8) Resultados — C++ (full)
*(últimas corridas; libro1 actualizado a 43.12 s)*

| Libro     | n      | MM (s) | SA-IS (s) | SAIS/MM (x) | FM-build (s) |
|-----------|-------:|-------:|----------:|------------:|-------------:|
| libro1.txt| 448930 |  5.8312|    43.1227|        7.39 |      0.01693 |
| libro2.txt| 174356 |  1.9649|     3.8488|        1.96 |      0.00662 |
| libro3.txt| 169547 |  1.9367|     3.2309|        1.67 |      0.00653 |
| libro4.txt| 428252 |  5.5652|    22.1189|        3.97 |      0.01620 |

**Observación:** En nuestra implementación de referencia, **MM** supera a **SA-IS** en tiempo; SA-IS requiere optimizaciones cuidadosas para acercarse al teórico O(n) con constantes bajas.

---

## 9) Complejidad (síntesis)
- **Manber & Myers (doubling):** Tiempo **O(n log n)**, Memoria **O(n)**.
- **SA-IS:** Tiempo **O(n)**, Memoria **O(n)** (buckets + bit vector).
- **FM-Index:** Construcción **O(n)** (dado SA); búsqueda exacta **O(m)**; Memoria ≈ `|BWT| + |C| + σ·⌈n/k⌉`.

---

## 10) Uso de IA generativa (registro breve)
- Se usó IA para identificar y corregir: (i) *off-by-one* y reconstrucción de tripletas en MM (Python); (ii) duplicación de LMS final y sobrescrituras fuera de rango en SA-IS (C++/Python); (iii) robustez de recursión en SA-IS (reducida con sentinela 0).
- Verificación con *toy tests* (`mississippi$`, `banana$`) y con los 4 libros, más comparación cruzada MM vs SA-IS.

---

## 11) Reflexión y aprendizajes
- Las implementaciones “limpias” de SA-IS requieren **estricto control de buckets** y del **sentinela** para evitar desbordes en la inducción.
- En Python, evitar *slicing* y trabajar **in-place** reduce copias (efecto “por referencia”) y memoria.
- El **bit vector** en C++ minimiza memoria y mejora la localidad, favoreciendo los pases de inducción.
- **FM-Index** permite búsquedas rápidas independientemente del tamaño del texto, útil para concordancias masivas.

---

## 12) Anexos y artefactos
- **Posiciones** de `"the"` en `libro1.txt`: `data/occ_the_libro1.txt` (generado con `--print-positions`).
- Scripts y fuentes: `python/` y `cpp/` (CMake).
- Repositorio privado con **commits frecuentes** e invitación al docente.
