
# Suffix Array Construction: Manber-Myers vs SA-IS

Este repositorio contiene implementaciones y benchmarks de algoritmos de construcción de Arreglos de Sufijos (Suffix Arrays) en **Python** y **Go**.

El objetivo es comparar el rendimiento entre un algoritmo $O(n \log n)$ clásico (Manber-Myers) y el estado del arte $O(n)$ (SA-IS), demostrando la eficiencia de la inducción de sufijos y la recursión.

---

## Implementaciones

| Archivo | Algoritmo | Complejidad | Lenguaje | Descripción |
| :--- | :--- | :--- | :--- | :--- |
| `scripts/manber_myers.py` | Manber-Myers | $O(n \log n)$ | Python | Algoritmo de duplicación de rangos. Referencia de exactitud. |
| `scripts/sais.py` | SA-IS | $O(n)$ | Python | Induced Sorting con recursión y manejo de centinelas virtuales. |
| `scripts/sais.go` | SA-IS | $O(n)$ | Go | Port de alto rendimiento de SA-IS. Soporta recursión completa. |

---

## Estructura del Proyecto

```text
.
├── data/               # Archivos de texto para pruebas (libros)
├── scripts/
│   ├── bench.py        # Suite de benchmarking y validación (Python)
│   ├── manber_myers.py # Implementación Manber-Myers
│   ├── sais.py         # Implementación SA-IS (Python)
│   ├── sais.go         # Implementación SA-IS (Go)
│   ├── run_python_bench.ps1 # Lanzador automático (Windows)
│   └── run_python_bench.sh  # Lanzador automático (Linux/Mac)
└── README.md
````

---

## Cómo ejecutar

### 1. Benchmark Comparativo (Python)

Ejecuta la suite que compara Manber-Myers vs SA-IS en Python.
Verifica la exactitud comparando los Arrays resultantes byte a byte.

**Windows (PowerShell):**

```powershell
.\scripts\run_python_bench.ps1
```

**Linux / Mac:**

```bash
./scripts/run_python_bench.sh
```

---

### 2. Versión de Alto Rendimiento (Go)

Para procesar archivos grandes en segundos (aprox. 70x más rápido que Python).

```bash
# Ejecutar directamente
go run scripts/sais.go data/libro1.txt
```

---

## Detalles Técnicos

* **Validación:** El script `bench.py` asegura que `SA_ManberMyers == SA_SAIS`.
* **Recursión:** Las implementaciones de SA-IS manejan correctamente la recursión para resolver colisiones de nombres en subcadenas LMS.
* **Centinelas:** Se utiliza un centinela virtual `\0` para garantizar el ordenamiento correcto incluso si el texto contiene caracteres ASCII bajos (como espacios).

---

## Créditos

Jesús Eduardo García Luna
Victoria Iluminda Rosales García
