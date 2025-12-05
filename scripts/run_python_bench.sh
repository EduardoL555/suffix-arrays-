#!/bin/bash
# scripts/run_python_bench.sh

echo "Iniciando Benchmark: Manber-Myers vs SA-IS (Python)..."
echo "-----------------------------------------------------"

# Asegurarse de estar en el root o ajustar rutas
# Asumimos ejecución desde el root como ./scripts/run_python_bench.sh
if [ -f "scripts/bench.py" ]; then
    python3 scripts/bench.py
else
    echo "Error: No se encuentra scripts/bench.py. Ejecuta desde la raíz del proyecto."
    exit 1
fi