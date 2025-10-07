# Manual de Usuario — Arreglo de Sufijos (MM / SA-IS) y FM-Index

Este manual explica cómo **compilar** y **ejecutar** las implementaciones en **Python** y **C++**, cómo **medir** tiempos/memoria y cómo **buscar** patrones con el **FM-Index**.

---

## 1) Requisitos

- **Windows 10/11** con **Visual Studio Code**
- **Python 3.10+** (sin librerías adicionales)
- **CMake** y un compilador **C++**
  - Opción A: **Visual Studio 2022** (Desktop development with C++)
  - Opción B: **MinGW-w64** (g++)
- 4 archivos `.txt` en `data/` (por ejemplo, de Project Gutenberg / Internet Archive)

> Asegura agregar `cmake` (y `g++` si usas MinGW) al **PATH** del sistema.

---

## 2) Estructura del proyecto (sugerida)

```
Act_Integradora/
├─ cpp/
│  ├─ CMakeLists.txt
│  ├─ include/                # suffix_array_mm.hpp, sais.hpp, fm_index.hpp
│  └─ src/                    # main.cpp, *.cpp
├─ python/
│  ├─ manber_myers_fixed.py
│  ├─ sais_fixed.py
│  ├─ fm_index.py
│  └─ bench.py
└─ data/                      # libros .txt 
```

---

## 3) Preparar los datos

Coloca los 4 libros `.txt` en `data/`:

```
data/libro1.txt  data/libro2.txt  data/libro3.txt  data/libro4.txt
```

> El software **añade** el sentinela `$` al final si no está presente.

---

## 4) Ejecución en **Python**

Ejecuta estos comandos **desde** la carpeta `python/`.

### 4.1 Bench **rápido** (MM + FM-Index)

```powershell
cd python
python bench.py --books ../data/libro1.txt ../data/libro2.txt ../data/libro3.txt ../data/libro4.txt --queries "the" "and" " de " " la " --status --skip-sais
```

**Qué hace:**
- Construye el **SA** con **Manber & Myers (MM)** por libro y mide **tiempo** + **pico de memoria (KB)**.
- Construye el **FM-Index** y ejecuta consultas de ejemplo (`--queries`).

### 4.2 SA-IS (Python) con truncado (recomendado)

SA-IS en Python puro es costoso para textos grandes. Ejecuta **por libro** con límite de **200k** caracteres:

```powershell
python bench.py --books ../data/libro1.txt --status --skip-mm --skip-fm --max_chars 200000
python bench.py --books ../data/libro2.txt --status --skip-mm --skip-fm --max_chars 200000
python bench.py --books ../data/libro3.txt --status --skip-mm --skip-fm --max_chars 200000
python bench.py --books ../data/libro4.txt --status --skip-mm --skip-fm --max_chars 200000
```

**Opciones útiles de `bench.py`:**

- `--books <paths...>`: rutas a `.txt`  
- `--queries <q1 q2 ...>`: patrones para FM-Index  
- `--status`: progreso por fases  
- `--skip-sais | --skip-mm | --skip-fm`: omite fases  
- `--max_chars N`: trunca cada libro a **N** caracteres

---

## 5) Compilación y ejecución en **C++**

Ejecuta estos pasos **desde** la carpeta `cpp/`.

### 5.1 Compilar con CMake

**Si ya tienes configurado `build/`:**
```powershell
cmake --build build --config Release
```

**Primera vez (Visual Studio 2022 x64):**
```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

*(Si usas MinGW-w64, puedes usar `-G "MinGW Makefiles"` y `mingw32-make`.)*

### 5.2 Seleccionar el ejecutable (PowerShell 5.1)

```powershell
if (Test-Path .\build\Release\sa_app.exe) {
  $exe = ".\build\Release\sa_app.exe"
} elseif (Test-Path .\build\sa_app.exe) {
  $exe = ".\build\sa_app.exe"
} else {
  throw "No se encontró sa_app.exe. Compila primero."
}
```

> En **PowerShell 7** (`pwsh`) puedes usar el operador ternario `? :`; en PS 5.1 no.

### 5.3 Ejecutar

**Manber & Myers**
```powershell
& $exe --algo mm --file ..\data\libro1.txt
```

**SA-IS**
```powershell
& $exe --algo sais --file ..\data\libro1.txt
```

**FM-Index (conteo + posiciones)**
```powershell
& $exe --algo fm --file ..\data\libro1.txt --query "the" --print-positions > ..\data\occ_the_libro1.txt
```

**Opciones de `sa_app`:**

- `--algo {mm|sais|fm}`: Manber-Myers, SA-IS o FM-Index  
- `--file <ruta>`: texto de entrada (`.txt`)  
- `--query <patrón>`: patrón a buscar (solo con `--algo fm`)  
- `--print-positions`: imprime **todas** las posiciones (recomendado redirigir a archivo)

---

## 6) Verificación rápida (toy test)

```powershell
# Crear archivo pequeño
Set-Content ..\data\toy.txt "mississippi"

# Probar MM / SA-IS / FM
& $exe --algo mm   --file ..\data\toy.txt
& $exe --algo sais --file ..\data\toy.txt
& $exe --algo fm   --file ..\data\toy.txt --query "issi" --print-positions
```

**Esperado:** SA correcto; FM-Index encuentra `"issi"` en dos posiciones.

---

## 7) Problemas comunes (y solución)

- **`cmake` no se reconoce**  
  Instala CMake y agrega `<CMake>\bin` al **PATH**.

- **PowerShell 5.1 no acepta `? :`**  
  Usa el bloque `if / elseif` para asignar `$exe` (mostrado arriba).

- **SA-IS (Python) tarda mucho**  
  Usa `--max_chars` (100k–200k) y ejecuta libro por libro con `--status`.

- **Salidas muy grandes de posiciones**  
  Redirige a archivo con `>` (ej.: `..\data\occ_the_libro1.txt`).

- **Rutas relativas**  
  Corre los comandos **desde** `python/` o `cpp/` según corresponda.

---

## 8) Ejemplos útiles

**FM-Index con varias consultas:**
```powershell
& $exe --algo fm --file ..\data\libro2.txt --query "and"
& $exe --algo fm --file ..\data\libro2.txt --query " the " --print-positions > ..\data\occ_the_libro2.txt
```

**Bench Python saltando fases costosas:**
```powershell
python bench.py --books ../data/libro1.txt ../data/libro2.txt --queries "the" "and" --status --skip-sais
```

---

### 