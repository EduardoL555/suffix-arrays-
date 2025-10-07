# C++

## Requisitos
- CMake 3.15+
- Compilador C++17
- VS Code con ms-vscode.cpptools (opcional)

## Compilar
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

## Ejecutar
```bash
# SA por Manber & Myers
./build/sa_app --algo mm --file ../data/libro1.txt

# SA por SA-IS (requiere texto con '$' al final)
./build/sa_app --algo sais --file ../data/libro1.txt

# FM-Index y búsqueda
./build/sa_app --algo fm --file ../data/libro1.txt --query "patron"
```
