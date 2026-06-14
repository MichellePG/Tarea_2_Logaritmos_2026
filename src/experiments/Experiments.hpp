#pragma once

#include <cstdint>
#include <cstddef>
#include <random>
#include <vector>

/**
 * Módulo de experimentación
 *
 * Contiene la generación de datasets, el muestreo sesgado exponencial y las
 * secciones:
 *   - Escenarios base (7.2)
 *   - Sequential Access Theorem (7.3.1.a)
 *   - Working Set Theorem (7.3.1.b)
 *   - Traversal Conjecture, bonus (7.4)
 *
 * Todos los resultados se escriben como archivos CSV en outputs/results/, los
 * cuales son consumidos por scripts/plot.py para generar los gráficos.
 */
namespace experiments {
// Constante c del enunciado (7.2): el número de búsquedas es M = 10^c * N.
// Se fija c = 1 por factibilidad: con c = 1 ambas lecturas posibles del
// enunciado (10^c y 10*c) coinciden y los experimentos son ejecutables.
inline constexpr int C_PARAM = 1;

// Parámetro lambda de la distribución sesgada exponencial P(i) (7.1.2).
// Recomendado en [0.001, 0.05]; 0.01 entrega un sesgo claro pero no extremo.
inline constexpr double LAMBDA = 0.01;

// Semilla base para la generación de datasets y secuencias (reproducibilidad).
inline constexpr uint64_t BASE_SEED = 0xC0FFEEULL;

// Exponentes de N para los escenarios base: N ∈ {2^10, ..., 2^14}.
inline constexpr int BASE_EXP_MIN = 10;
inline constexpr int BASE_EXP_MAX = 14;

// Exponente de N por defecto para los experimentos de teoremas (7.3, 7.4).
// El enunciado pide 2^25; se puede reducir por línea de comandos para pruebas.
inline constexpr int THEOREM_EXP_DEFAULT = 25;

// Cota superior de muestras por-búsqueda que se vuelcan a CSV (para no generar
// archivos gigantes en el gráfico de naturaleza amortizada de 7.2).
inline constexpr size_t MAX_PERSEARCH_SAMPLES = 4000;


// Generación de datos

/**
 * Genera un dataset de n enteros uniformes en [0, 2^32 - 1].
 * Los valores pueden repetirse (universo uniforme, tal como pide 7.1.1).
 *
 * @param n    Cantidad de elementos a generar.
 * @param seed Semilla del generador (para reproducibilidad).
 * @return vector con n claves uint32_t uniformemente distribuidas.
 */
std::vector<uint32_t> generateDataset(size_t n, uint64_t seed);

/**
 * Muestreador de índices según la distribución sesgada exponencial (7.1.2):
 *   P(i) = e^{-λ i} (1 - e^{-λ}) / (1 - e^{-λ N}),  i = 0..N-1.
 * Es una distribución geométrica truncada: los índices pequeños son mucho
 * más probables. El muestreo se hace por CDF inversa en O(1).
 */
class BiasedSampler {
public:
    /**
     * @param n      Tamaño del universo de índices (0..n-1).
     * @param lambda Parámetro de sesgo λ.
     */
    BiasedSampler(size_t n, double lambda);

    /**
     * Muestrea un índice en [0, n-1] según P(i).
     * @param rng Generador de números pseudoaleatorios.
     * @return índice muestreado.
     */
    size_t sample(std::mt19937_64& rng) const;

private:
    size_t n_;
    double lambda_;
    double denom_; // 1 - e^{-λ n}
};

// ---------------------------------------------------------------------------
// Bloques experimentales
// ---------------------------------------------------------------------------

/**
 * Escenarios base 7.2: para cada N ∈ {2^10,...,2^14} y cada configuración
 * (a,b,c,d) inserta N elementos y realiza M = 10^c·N búsquedas en AVL y Splay,
 * midiendo tiempos totales y costos. Vuelca tablas y series por-búsqueda.
 */
void runBaseScenarios();

/**
 * Sequential Access Theorem 7.3.1.a: sobre un árbol con N = 2^exp elementos,
 * ejecuta una secuencia de claves estrictamente crecientes y reporta el costo
 * y tiempo para cada m ∈ {N/100, 2N/100, ..., N/10}.
 *
 * @param exp Exponente de N (por defecto THEOREM_EXP_DEFAULT).
 */
void runSequentialAccess(int exp);

/**
 * Working Set Theorem 7.3.1.b: sobre un árbol con N = 2^exp elementos, para
 * cada W ∈ {10, 10^2, ..., 10^6} realiza M = 10^c·N búsquedas restringidas a
 * un subconjunto aleatorio de tamaño W y reporta tiempo y costo medio.
 *
 * @param exp Exponente de N (por defecto THEOREM_EXP_DEFAULT).
 */
void runWorkingSet(int exp);

/**
 * Traversal Conjecture, bonus 7.4: construye dos Splay Trees con los mismos
 * elementos en permutaciones distintas, recorre T1 en preorden y busca esa
 * secuencia en T2, reportando el costo y tiempo de cada búsqueda.
 *
 * @param exp Exponente de N (por defecto THEOREM_EXP_DEFAULT).
 */
void runTraversalConjecture(int exp);

} // namespace experiments
