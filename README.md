# FMI App (C++): Compresión con SA → BWT → MTF → RLE₀ → Huffman

Herramienta de línea de comando para **comprimir** y **descomprimir** texto usando:
**Suffix Array (Manber–Myers) → Burrows–Wheeler (BWT) → Move-To-Front (MTF) → Run-Length de ceros (RLE₀) → Huffman**, y su inversa.  
El contenedor **`.fmi`** almacena metadatos mínimos para recuperar el archivo original **exacto**.

---

## Introducción
- Compresor/descompresor reproducible y rápido, orientado a textos.
- Decisiones clave:
  - Sentinela **0x00** para construir SA/BWT (seguro y único).
  - Inversa de BWT con **LF-mapping** (tablas `C[c]` y `rank` por símbolo).
  - Huffman **MSB-first** con empaquetado correcto: primero bytes completos, al final byte parcial con padding.
- Exactitud verificada por hash del archivo original vs. descomprimido.

---

## Dependencias
- **CMake ≥ 3.16**
- **Compilador C++17**
  - Windows: **MSYS2/MinGW-w64** (recomendado) o **MSVC (Visual Studio)**
- (Opcional) PowerShell para los ejemplos de ejecución.

---

## Instalación y compilación

### Opción A — MSYS2/MinGW (recomendada)
    cd cpp
    $env:Path = "C:\msys64\mingw64\bin;$env:Path"
    cmake -S . -B build-mingw -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
    cmake --build build-mingw -j
    # Ejecutable:
    $exe = ".\build-mingw\fmi_app.exe"

### Opción B — Visual Studio (MSVC)
    cd cpp
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build -j
    # Ejecutable típico:
    $exe = ".\build\Release\fmi_app.exe"

---

## Uso
    fmi_app compress <input.txt> -o <output.fmi>
    fmi_app decompress <input.fmi> -o <output.txt>

**Ejemplos**

*Prueba rápida (“toy”):*
    
    Set-Content -NoNewline ..\data\toy.txt "mississippi"
    $exe compress ..\data\toy.txt -o ..\data\toy.fmi
    $exe decompress ..\data\toy.fmi -o ..\data\toy_out.txt
    (Get-FileHash ..\data\toy.txt).Hash -eq (Get-FileHash ..\data\toy_out.txt).Hash  # True

*Archivo mayor:*
    
    $exe compress ..\data\libro1.txt -o ..\data\libro1.fmi
    $exe decompress ..\data\libro1.fmi -o ..\data\libro1_out.txt
    (Get-FileHash ..\data\libro1.txt).Hash -eq (Get-FileHash ..\data\libro1_out.txt).Hash  # True

---

## Detalles relevantes
- **`.fmi`** contiene:
  - `orig_len_no_sentinel` (u32), `had_sentinel` (u8),
  - `alphabet` (u8[] para MTF),
  - **codebook de Huffman** ((sym, len, code)),
  - `rle_len` (u32) y `bitstream` (bytes MSB-first).
- **Descompresión robusta:** tras BWT⁻¹, si el original no tenía sentinela, se recorta por `orig_len_no_sentinel`; si lo tenía, se valida longitud exacta.

---

## Créditos
- **Jesús Eduardo García Luna – A01739060**  
- **Victoria Iluminda Rosales García – A01734739**

---

## Licencia
Uso académico. Se distribuye “tal cual”, sin garantías. Propósito didáctico para acompañar contenidos de estructuras de datos y algoritmos.
