# Reporte del Proyecto: Suffix Arrays + FM-Index

**Integrantes (3):**  
- Nombre 1 (Matrícula)
- Nombre 2 (Matrícula)
- Nombre 3 (Matrícula)

## 1. Propósito y Objetivo
Breve explicación del objetivo del proyecto y del flujo general (corrección, port a C++, pruebas, perfilado, búsqueda con FM-Index).

## 2. Descripción técnica
### 2.1 Algoritmo Manber & Myers (Python y C++)
- Idea general del método de *doubling*.
- Complejidad temporal y espacial.
- Principales bugs corregidos (Python) y cómo se detectaron.

### 2.2 Algoritmo SA-IS (Python y C++)
- Ideas clave (tipado L/S, LMS, *induced sorting*).
- Complejidad temporal y espacial.
- Diferencias de implementación entre Python y C++.
- Cambios de “paso por valor” a “paso por referencia” (Python): qué operaciones evitamos (slicing, copies), uso de listas mutables, etc.

### 2.3 FM-Index (BWT + C, Occ)
- Construcción de BWT a partir del SA.
- Tablas `C` y `Occ` (muestreo para ahorro de memoria).
- Búsqueda exacta por *backward search*.

## 3. Metodología de pruebas
- 4 libros de distintos tamaños (citar fuente).
- Parámetros de hardware/software (CPU, RAM, SO, Python/C++ versión).

## 4. Resultados
- **Tiempos** (tabla) y **pico de memoria** (Python via `tracemalloc`) por libro y por algoritmo.
- Gráficas opcionales.
- Correctitud: validaciones con cadenas pequeñas y casos adversos.

## 5. Discusión y aprendizajes
- Trade-offs de cada algoritmo (simplicidad vs desempeño).
- Impacto de usar bit vectors vs diccionarios en C++.
- Lecciones al portar de Python a C++.

## 6. Conclusiones

## 7. Anexos
- Comandos de compilación/ejecución.
- Extractos de la conversación con IA (prompts y correcciones pedidas).
