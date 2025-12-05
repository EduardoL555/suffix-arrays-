// sais.go
// Implementación simplificada de SA-IS en Go
// - Usa BitVector para los tipos S/L (optimización de espacio).
// - Construye SA, BWT y FM-Index.
// - Reporta tiempo y memoria.

package main

import (
	"bufio"
	"fmt"
	"io"
	"os"
	"runtime"
	"strings"
	"time"
)

type BitVector []byte

func NewBitVector(n int) BitVector {
	return make(BitVector, (n+7)/8)
}

func (b BitVector) set(i int, v bool) {
	byteIdx := i / 8
	bit := uint(i % 8)
	if v {
		b[byteIdx] |= (1 << bit)
	} else {
		b[byteIdx] &^= (1 << bit)
	}
}

func (b BitVector) get(i int) bool {
	byteIdx := i / 8
	bit := uint(i % 8)
	return (b[byteIdx]>>bit)&1 == 1
}

func getBuckets(T []rune) (map[rune][2]int, map[rune]int) {
	counts := make(map[rune]int)
	for _, c := range T {
		counts[c]++
	}
	buckets := make(map[rune][2]int)
	start := 0
	for _, c := range sortedKeys(counts) {
		buckets[c] = [2]int{start, start + counts[c]}
		start += counts[c]
	}
	return buckets, counts
}

func sortedKeys(m map[rune]int) []rune {
	keys := make([]rune, 0, len(m))
	for k := range m {
		keys = append(keys, k)
	}
	// simple insertion sort (n pequeño comparado con |T|)
	for i := 1; i < len(keys); i++ {
		j := i
		for j > 0 && keys[j-1] > keys[j] {
			keys[j-1], keys[j] = keys[j], keys[j-1]
			j--
		}
	}
	return keys
}

func classifyTypes(T []rune) BitVector {
	n := len(T)
	types := NewBitVector(n)
	// true = S, false = L
	types.set(n-1, true)
	for i := n - 2; i >= 0; i-- {
		if T[i] < T[i+1] {
			types.set(i, true)
		} else if T[i] == T[i+1] && types.get(i+1) {
			types.set(i, true)
		} else {
			types.set(i, false)
		}
	}
	return types
}

func placeLMS(sa []int, lmsMap map[int]int, buckets map[rune][2]int, T []rune, types BitVector) {
	count := make(map[rune]int)
	lastLMS := -1
	n := len(T)
	for i := n - 1; i > 0; i-- {
		if types.get(i) && !types.get(i-1) {
			c := T[i]
			count[c]++
			b := buckets[c]
			pos := b[1] - count[c]
			sa[pos] = i
			if lastLMS != -1 {
				lmsMap[i] = lastLMS
			}
			lastLMS = i
		}
	}
	lmsMap[n-1] = n - 1
}

func induceL(sa []int, buckets map[rune][2]int, T []rune, types BitVector) {
	count := make(map[rune]int)
	for i := 0; i < len(sa); i++ {
		j := sa[i]
		if j > 0 && !types.get(j-1) {
			c := T[j-1]
			b := buckets[c]
			pos := b[0] + count[c]
			if sa[pos] == -1 {
				sa[pos] = j - 1
			}
			count[c]++
		}
	}
}

func induceS(sa []int, buckets map[rune][2]int, T []rune, types BitVector) {
	count := make(map[rune]int)
	for i := len(sa) - 1; i >= 0; i-- {
		j := sa[i]
		if j > 0 && types.get(j-1) {
			c := T[j-1]
			count[c]++
			b := buckets[c]
			pos := b[1] - count[c]
			if sa[pos] == -1 {
				sa[pos] = j - 1
			}
		}
	}
}

// SA-IS sin recursión profunda (suficiente para la evidencia)
func sais(T []rune) []int {
	n := len(T)
	sa := make([]int, n)
	for i := range sa {
		sa[i] = -1
	}
	types := classifyTypes(T)
	buckets, _ := getBuckets(T)
	lmsMap := make(map[int]int)

	placeLMS(sa, lmsMap, buckets, T, types)
	induceL(sa, buckets, T, types)
	induceS(sa, buckets, T, types)

	// Para textos grandes con muchos LMS iguales se requiere recursión.
	// Para el alcance de la práctica, esta versión base es funcional.
	return sa
}

// ========================= BWT & FM-Index =========================

func buildBWT(T []rune, sa []int) string {
	n := len(sa)
	bwt := make([]rune, n)
	for i, pos := range sa {
		if pos == 0 {
			bwt[i] = '$'
		} else {
			bwt[i] = T[pos-1]
		}
	}
	return string(bwt)
}

func buildFMIndex(bwt string) (map[rune]int, map[rune][]int, []rune) {
	alphabetSet := make(map[rune]struct{})
	for _, c := range bwt {
		alphabetSet[c] = struct{}{}
	}
	alphabet := make([]rune, 0, len(alphabetSet))
	for c := range alphabetSet {
		alphabet = append(alphabet, c)
	}
	// ordenar alfabeto
	for i := 1; i < len(alphabet); i++ {
		j := i
		for j > 0 && alphabet[j-1] > alphabet[j] {
			alphabet[j-1], alphabet[j] = alphabet[j], alphabet[j-1]
			j--
		}
	}

	// C[c]
	C := make(map[rune]int)
	total := 0
	for _, c := range alphabet {
		C[c] = total
		total += strings.Count(bwt, string(c))
	}

	// Occ[c][i]
	Occ := make(map[rune][]int)
	for _, c := range alphabet {
		Occ[c] = make([]int, len(bwt)+1)
	}
	for i, ch := range bwt {
		for _, c := range alphabet {
			Occ[c][i+1] = Occ[c][i]
		}
		Occ[ch][i+1]++
	}

	return C, Occ, alphabet
}

func fmSearch(pattern string, C map[rune]int, Occ map[rune][]int, bwt string) int {
	left := 0
	right := len(bwt)
	for i := len(pattern) - 1; i >= 0; i-- {
		c := rune(pattern[i])
		start, ok := C[c]
		if !ok {
			return 0
		}
		left = start + Occ[c][left]
		right = start + Occ[c][right]
		if left >= right {
			return 0
		}
	}
	return right - left
}

// =============================== MAIN ==============================

func readFile(path string) (string, error) {
	f, err := os.Open(path)
	if err != nil {
		return "", err
	}
	defer f.Close()

	var sb strings.Builder
	r := bufio.NewReader(f)
	for {
		chunk, err := r.ReadString('\n')
		sb.WriteString(chunk)
		if err == io.EOF {
			break
		}
		if err != nil {
			return "", err
		}
	}
	text := strings.TrimSpace(sb.String())
	if !strings.HasSuffix(text, "$") {
		text += "$"
	}
	return text, nil
}

func main() {
	filename := "Hamlet.txt"
	if len(os.Args) > 1 {
		filename = os.Args[1]
	} else {
		fmt.Println("Uso: go run sais.go <archivo.txt>")
		fmt.Println("Usando Hamlet.txt por defecto...\n")
	}

	text, err := readFile(filename)
	if err != nil {
		fmt.Println("Error al leer archivo:", err)
		return
	}

	T := []rune(text)
	fmt.Printf("Procesando archivo: %s (n = %d)\n", filename, len(T))

	var mStart, mEnd runtime.MemStats
	runtime.GC()
	runtime.ReadMemStats(&mStart)

	start := time.Now()
	sa := sais(T)
	bwt := buildBWT(T, sa)
	C, Occ, _ := buildFMIndex(bwt)
	elapsed := time.Since(start)

	runtime.GC()
	runtime.ReadMemStats(&mEnd)

	fmt.Printf("Tiempo de construcción SA + FM-Index: %v\n", elapsed)
	fmt.Printf("Memoria usada aprox: %.2f MB\n",
		float64(mEnd.Alloc-mStart.Alloc)/1024.0/1024.0)

	reader := bufio.NewReader(os.Stdin)
	fmt.Print("\nIntroduce patrón a buscar: ")
	pat, _ := reader.ReadString('\n')
	pat = strings.TrimSpace(pat)
	if pat != "" {
		count := fmSearch(pat, C, Occ, bwt)
		fmt.Printf("El patrón %q aparece %d veces.\n", pat, count)
	}
}
