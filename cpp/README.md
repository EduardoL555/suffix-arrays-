# Manual de Usuario — Arreglo de Sufijos (MM/SA-IS) y FM-Index

Este documento explica cómo **compilar** y **ejecutar** las implementaciones en **Python** y **C++**, cómo **medir** tiempos/memoria y cómo **buscar** patrones con FM-Index.

---

## 1) Requisitos

- **Windows 10/11** con **VS Code**
- **Python 3.10+** (sin librerías adicionales)
- **CMake** y un compilador C++  
  - Visual Studio 2022 (Desktop development with C++) **o** MinGW-w64 (g++)
- 4 libros `.txt` en `data/` (p. ej., Project Gutenberg / Internet Archive)

> Tip: agrega `cmake` y `g++` (si usas MinGW) al **PATH** de Windows.

---

## 2) Estructura de proyecto (sugerida)

Act_Integradora/
├─ cpp/
│ ├─ CMakeLists.txt
│ ├─ include/ # headers (suffix_array_mm.hpp, sais.hpp, fm_index.hpp)
│ └─ src/ # fuentes (main.cpp, .cpp)
├─ python/
│ ├─ manber_myers_fixed.py
│ ├─ sais_fixed.py
│ ├─ fm_index.py
│ └─ bench.py
├─ data/ # libros .txt y salidas (p. ej., occ_.txt)
└─ docs/ # (reporte, este manual)

yaml
Copiar código

---

## 3) Preparar los datos

Coloca tus 4 libros `.txt` dentro de `data/`:

data/libro1.txt data/libro2.txt data/libro3.txt data/libro4.txt

yaml
Copiar código

> El software **agrega** el sentinela `$` al final si no está presente.

---

## 4) Ejecución en **Python**

Desde la carpeta `python/`.

### 4.1 Bench **rápido** (MM + FM-Index)

```powershell
cd python
python bench.py --books ../data/libro1.txt ../data/libro2.txt ../data/libro3.txt ../data/libro4.txt ^
  --queries "the" "and" " de " " la " --status --skip-sais
Qué hace:

Construye SA (MM) por libro y mide tiempo + pico de memoria (KB).

Construye FM-Index y ejecuta búsquedas exactas para las queries dadas.

4.2 SA-IS (Python) con truncado (recomendado)
SA-IS puro en Python es costoso para textos muy grandes. Ejecuta por libro con límite de 200k caracteres:

powershell
Copiar código
python bench.py --books ../data/libro1.txt --status --skip-mm --skip-fm --max_chars 200000
python bench.py --books ../data/libro2.txt --status --skip-mm --skip-fm --max_chars 200000
python bench.py --books ../data/libro3.txt --status --skip-mm --skip-fm --max_chars 200000
python bench.py --books ../data/libro4.txt --status --skip-mm --skip-fm --max_chars 200000
Opciones de bench.py
--books <paths...>: rutas a archivos .txt.

--queries <q1 q2 ...>: patrones para FM-Index (búsqueda exacta).

--status: imprime progreso por fases.

--skip-sais | --skip-mm | --skip-fm: omite fases costosas si no se requieren.

--max_chars N: trunca cada libro a N caracteres (para calibrar costos).

5) Compilación y ejecución en C++
Desde la carpeta cpp/.

5.1 Configurar y compilar (CMake)
Si ya tienes la carpeta build/ configurada:

powershell
Copiar código
cmake --build build --config Release
Primera vez (ejemplo con Visual Studio 2022 x64):

powershell
Copiar código
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
5.2 Seleccionar el ejecutable (PowerShell 5.1)
powershell
Copiar código
if (Test-Path .\build\Release\sa_app.exe) {
  $exe = ".\build\Release\sa_app.exe"
} elseif (Test-Path .\build\sa_app.exe) {
  $exe = ".\build\sa_app.exe"
} else {
  throw "No se encontró sa_app.exe. Compila primero."
}
En PowerShell 7 (pwsh) puedes usar el operador ternario ? :, pero en PS 5.1 no.

5.3 Ejecutar
Manber & Myers

powershell
Copiar código
& $exe --algo mm --file ..\data\libro1.txt
SA-IS

powershell
Copiar código
& $exe --algo sais --file ..\data\libro1.txt
FM-Index (conteo + posiciones)

powershell
Copiar código
& $exe --algo fm --file ..\data\libro1.txt --query "the" --print-positions > ..\data\occ_the_libro1.txt
Opciones de sa_app
--algo {mm|sais|fm}: selecciona Manber-Myers, SA-IS o FM-Index.

--file <ruta>: texto de entrada (.txt).

--query <patrón>: patrón a buscar (solo en --algo fm).

--print-positions: imprime todas las posiciones (recomendado redirigir a archivo).

6) Verificación rápida (toy test)
powershell
Copiar código
# Crear toy
Set-Content ..\data\toy.txt "mississippi"

# Probar MM / SA-IS / FM
& $exe --algo mm   --file ..\data\toy.txt
& $exe --algo sais --file ..\data\toy.txt
& $exe --algo fm   --file ..\data\toy.txt --query "issi" --print-positions
Esperado: SA correcto; FM-Index encuentra "issi" en dos posiciones.

7) Problemas comunes (y solución)
cmake no se reconoce
Instala CMake y añade su carpeta bin al PATH.

PowerShell 5.1 no acepta ? :
Usa el bloque if / elseif mostrado arriba para seleccionar $exe.

SA-IS (Python) tarda mucho
Usa --max_chars (p. ej., 100k–200k) y corre libro por libro con --status.

Salidas muy grandes de posiciones
Redirige con > a un archivo en data/ (p. ej., occ_the_libro1.txt).

Rutas relativas
Ejecuta los comandos desde python/ o cpp/ según corresponda.

8) Notas de rendimiento (referencia)
Python (MM + FM): produce tiempos y pico de memoria por libro; las búsquedas en FM suelen ser muy rápidas (≈ O(m)).

Python (SA-IS 200k): útil para medir tendencia sin costos excesivos.

C++ (full): SA-IS y FM-Index con tiempos significativamente menores que en Python.

9) Ejemplos útiles
FM-Index con varias consultas:

powershell
Copiar código
& $exe --algo fm --file ..\data\libro2.txt --query "and"
& $exe --algo fm --file ..\data\libro2.txt --query " the " --print-positions > ..\data\occ_the_libro2.txt
Bench Python saltando fases costosas:

powershell
Copiar código
python bench.py --books ../data/libro1.txt ../data/libro2.txt --queries "the" "and" --status --skip-sais
Fin del manual