// scripts/sais.go
package main

import (
	"bufio"
	"fmt"
	"io"
	"os"
	"runtime"
	"sort"
	"strings"
	"time"
)

// ========================================================================
// SA-IS (Suffix Array - Induced Sorting) - CANONICAL GO PORT
// Soporta recursión completa y alfabeto de enteros.
// ========================================================================

// Tipos de sufijos
const (
	L_TYPE = false // Large
	S_TYPE = true  // Small
)

// getBuckets calcula los inicios y finales de los buckets
func getBuckets(T []int, k int) ([]int, []int) {
	counts := make([]int, k)
	for _, c := range T {
		counts[c]++
	}

	bktStarts := make([]int, k)
	bktEnds := make([]int, k)
	sum := 0
	for i := 0; i < k; i++ {
		bktStarts[i] = sum
		sum += counts[i]
		bktEnds[i] = sum
	}
	return bktStarts, bktEnds
}

// induceSAL induce los tipos L (L-type)
func induceSAL(T []int, SA []int, types []bool, bktStarts []int) {
	n := len(T)
	// Copiamos bktStarts porque lo vamos a modificar
	bkt := make([]int, len(bktStarts))
	copy(bkt, bktStarts)

	for i := 0; i < n; i++ {
		j := SA[i] - 1
		if j >= 0 && types[j] == L_TYPE {
			c := T[j]
			SA[bkt[c]] = j
			bkt[c]++
		}
	}
}

// induceSAS induce los tipos S (S-type)
func induceSAS(T []int, SA []int, types []bool, bktEnds []int) {
	n := len(T)
	// Copiamos bktEnds
	bkt := make([]int, len(bktEnds))
	copy(bkt, bktEnds)

	for i := n - 1; i >= 0; i-- {
		j := SA[i] - 1
		if j >= 0 && types[j] == S_TYPE {
			c := T[j]
			bkt[c]-- // Decrementar antes de escribir (exclusivo -> inclusivo)
			SA[bkt[c]] = j
		}
	}
}

// saisCore es el núcleo recursivo
func saisCore(T []int, k int) []int {
	n := len(T)
	SA := make([]int, n)
	if n == 0 {
		return SA
	}
	if n == 1 {
		SA[0] = 0
		return SA
	}

	// 1. Clasificar sufijos (S/L)
	types := make([]bool, n)
	types[n-1] = S_TYPE
	for i := n - 2; i >= 0; i-- {
		if T[i] < T[i+1] {
			types[i] = S_TYPE
		} else if T[i] > T[i+1] {
			types[i] = L_TYPE
		} else {
			types[i] = types[i+1]
		}
	}

	// Identificar LMS (Left-Most S-type)
	isLMS := func(i int) bool {
		return i > 0 && types[i] == S_TYPE && types[i-1] == L_TYPE
	}

	// 2. Paso 1: LMS Tentativo
	// Inicializar SA con -1
	for i := 0; i < n; i++ {
		SA[i] = -1
	}

	_, bktEnds := getBuckets(T, k)
	bkt := make([]int, k)
	copy(bkt, bktEnds)

	// Insertar LMS al final de sus buckets S
	for i := 1; i < n; i++ {
		if isLMS(i) {
			c := T[i]
			bkt[c]--
			SA[bkt[c]] = i
		}
	}

	// 3. Paso 2: Inducir SA
	bktStarts, _ := getBuckets(T, k)
	induceSAL(T, SA, types, bktStarts)
	induceSAS(T, SA, types, bktEnds)

	// 4. Paso 3: Renombrar LMS
	// Compactar los índices LMS ordenados
	nLMS := 0
	for _, x := range SA {
		if isLMS(x) {
			SA[nLMS] = x
			nLMS++
		}
	}
	// Limpiar el resto de SA para reutilizar memoria si se desea,
	// pero aquí usaremos índices claros.
	lmsIdx := SA[:nLMS] // Slice de los LMS ordenados

	// Asignar nombres
	lmsNames := make([]int, n)
	for i := range lmsNames {
		lmsNames[i] = -1
	}

	currentName := 0
	lmsNames[lmsIdx[0]] = currentName

	for i := 1; i < nLMS; i++ {
		prev := lmsIdx[i-1]
		curr := lmsIdx[i]

		diff := false
		for d := 0; d < n; d++ {
			if prev+d >= n || curr+d >= n {
				diff = true
				break
			}
			if T[prev+d] != T[curr+d] {
				diff = true
				break
			}
			if d > 0 && (isLMS(prev+d) || isLMS(curr+d)) {
				// Fin de substring LMS
				break
			}
		}

		if diff {
			currentName++
		}
		lmsNames[curr] = currentName
	}

	// Construir T reducido (T1)
	var T1 []int
	var mapT1 []int // Mapeo de vuelta a índices originales
	for i := 0; i < n; i++ {
		if isLMS(i) {
			T1 = append(T1, lmsNames[i])
			mapT1 = append(mapT1, i)
		}
	}

	// 5. Recursión
	var SA1 []int
	if currentName < nLMS-1 {
		// Nombres no únicos -> Recursión
		SA1 = saisCore(T1, currentName+1)
	} else {
		// Nombres únicos -> Bucket sort directo
		SA1 = make([]int, nLMS)
		for i, name := range T1 {
			SA1[name] = i
		}
	}

	// 6. Inducción Final
	// Mapear SA1 de vuelta a índices originales
	lmsCorrect := make([]int, nLMS)
	for i, x := range SA1 {
		lmsCorrect[i] = mapT1[x]
	}

	// Reiniciar SA
	for i := 0; i < n; i++ {
		SA[i] = -1
	}

	// Insertar LMS ordenados correctamente
	copy(bkt, bktEnds) // Reiniciar punteros tail
	for i := nLMS - 1; i >= 0; i-- {
		pos := lmsCorrect[i]
		c := T[pos]
		bkt[c]--
		SA[bkt[c]] = pos
	}

	// Inducción final
	induceSAL(T, SA, types, bktStarts)
	induceSAS(T, SA, types, bktEnds)

	return SA
}

// Wrapper principal para Strings
func SAIS(text string) []int {
	// Convertir string a []int y añadir centinela virtual 0
	// Esto asegura corrección aunque existan caracteres menores que el final.
	n := len(text)
	T := make([]int, n+1)
	maxChar := 0
	for i := 0; i < n; i++ {
		val := int(text[i])
		T[i] = val
		if val > maxChar {
			maxChar = val
		}
	}
	T[n] = 0 // Centinela virtual (debe ser menor que cualquier char del texto)

	SA := saisCore(T, maxChar+1)

	// El resultado incluye el centinela en la posición 0 del SA.
	// Lo eliminamos para devolver el SA del texto original.
	return SA[1:]
}

// ========================================================================
// BWT & FM-Index Utils
// ========================================================================

func BuildBWT(text string, sa []int) string {
	var b strings.Builder
	n := len(sa)
	b.Grow(n)
	for _, idx := range sa {
		if idx == 0 {
			b.WriteByte('$')
		} else {
			b.WriteByte(text[idx-1])
		}
	}
	return b.String()
}

func BuildFMIndex(bwt string) (map[byte]int, map[byte][]int) {
	// Alfabeto
	counts := make(map[byte]int)
	for i := 0; i < len(bwt); i++ {
		counts[bwt[i]]++
	}

	// Tabla C (acumulada)
	C := make(map[byte]int)
	alphabet := make([]byte, 0, len(counts))
	for k := range counts {
		alphabet = append(alphabet, k)
	}
	sort.Slice(alphabet, func(i, j int) bool { return alphabet[i] < alphabet[j] })

	sum := 0
	for _, char := range alphabet {
		C[char] = sum
		sum += counts[char]
	}

	// Tabla Occ (Ocurrencias)
	// Optimizada: map de slices
	Occ := make(map[byte][]int)
	for _, char := range alphabet {
		Occ[char] = make([]int, len(bwt)+1)
	}

	for i := 0; i < len(bwt); i++ {
		char := bwt[i]
		for _, c := range alphabet {
			Occ[c][i+1] = Occ[c][i]
		}
		Occ[char][i+1]++
	}

	return C, Occ
}

func FMSearch(pattern string, C map[byte]int, Occ map[byte][]int, bwtLen int) int {
	l, r := 0, bwtLen
	for i := len(pattern) - 1; i >= 0; i-- {
		char := pattern[i]
		if _, ok := C[char]; !ok {
			return 0
		}
		l = C[char] + Occ[char][l]
		r = C[char] + Occ[char][r]
		if l >= r {
			return 0
		}
	}
	return r - l
}

// ========================================================================
// MAIN
// ========================================================================

func readFile(path string) (string, error) {
	f, err := os.Open(path)
	if err != nil {
		return "", err
	}
	defer f.Close()

	var sb strings.Builder
	reader := bufio.NewReader(f)
	for {
		chunk, err := reader.ReadString('\n')
		sb.WriteString(chunk)
		if err == io.EOF {
			break
		}
		if err != nil {
			return "", err
		}
	}
	// Simular el comportamiento del script de Python: trim + $
	return strings.TrimSpace(sb.String()) + "$", nil
}

func main() {
	if len(os.Args) < 2 {
		fmt.Println("Uso: go run sais.go <archivo.txt>")
		return
	}
	filename := os.Args[1]

	text, err := readFile(filename)
	if err != nil {
		fmt.Printf("Error leyendo %s: %v\n", filename, err)
		return
	}

	fmt.Printf("Procesando: %s (len=%d)\n", filename, len(text))

	// Medir memoria inicial
	var m1, m2 runtime.MemStats
	runtime.GC()
	runtime.ReadMemStats(&m1)

	start := time.Now()

	// 1. Construir SA
	sa := SAIS(text)

	// 2. Construir BWT & Index
	bwt := BuildBWT(text, sa)
	C, Occ := BuildFMIndex(bwt)

	duration := time.Since(start)

	runtime.GC()
	runtime.ReadMemStats(&m2)
	memUsed := float64(m2.TotalAlloc-m1.TotalAlloc) / 1024 / 1024

	fmt.Printf("Tiempo Total (Go): %.4fs | Memoria alloc: %.2f MB\n", duration.Seconds(), memUsed)

	// Búsqueda de prueba rápida para verificar integridad básica
	patterns := []string{"the", "and", "lorem"}
	fmt.Printf("Consultas rápidas: %v\n", patterns)
	for _, p := range patterns {
		count := FMSearch(p, C, Occ, len(bwt))
		fmt.Printf(" '%s': %d\n", p, count)
	}
}
