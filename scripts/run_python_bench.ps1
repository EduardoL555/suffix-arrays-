# scripts/run_python_bench.ps1
Write-Host "Iniciando Benchmark: Manber-Myers vs SA-IS (Python)..." -ForegroundColor Cyan
Write-Host "-----------------------------------------------------" -ForegroundColor Gray

if (Test-Path "scripts\bench.py") {
    python scripts\bench.py
} else {
    Write-Error "Error: No se encuentra scripts\bench.py. Ejecuta desde la raíz del proyecto."
}