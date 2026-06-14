#include "Experiments.hpp"

#include "../avl/AVLTree.hpp"
#include "../splay/SplayTree.hpp"
#include "../../include/TreeInterface.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <numeric>
#include <string>

namespace experiments {

namespace {

using Clock = std::chrono::steady_clock;

// Devuelve el tiempo transcurrido entre dos instantes en nanosegundos.
long long elapsedNs(Clock::time_point a, Clock::time_point b) {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(b - a).count();
}

// Crea el directorio de resultados si no existe y abre un CSV para escritura.
std::ofstream openCsv(const std::string& name) {
    std::filesystem::create_directories("outputs/results");
    std::ofstream out("outputs/results/" + name);
    if (!out) {
        std::cerr << "[error] no se pudo abrir outputs/results/" << name << "\n";
    }
    return out;
}

// Inserta todas las claves en el árbol y retorna el tiempo total (ns).
long long insertAll(TreeInterface* tree, const std::vector<uint32_t>& keys) {
    auto start = Clock::now();
    for (uint32_t k : keys) tree->insert(k);
    auto end = Clock::now();
    return elapsedNs(start, end);
}

// Resultado agregado de una secuencia de búsquedas.
struct SearchResult {
    long long timeNs = 0;            // tiempo total de la secuencia
    unsigned long long totalCost = 0; // suma de nodos visitados
};

// Ejecuta una secuencia de búsquedas midiendo tiempo y costo total.
// Si perCost != nullptr, además guarda el costo de cada búsqueda individual.
SearchResult searchAll(TreeInterface* tree,
                       const std::vector<uint32_t>& keys,
                       std::vector<unsigned long long>* perCost = nullptr) {
    SearchResult res;
    if (perCost) perCost->reserve(keys.size());
    auto start = Clock::now();
    for (uint32_t k : keys) {
        tree->search(k);
        unsigned long long c = tree->getLastCost();
        res.totalCost += c;
        if (perCost) perCost->push_back(c);
    }
    auto end = Clock::now();
    res.timeNs = elapsedNs(start, end);
    return res;
}

// Serie de búsquedas agrupada en "buckets" consecutivos. Para cada bucket se
// mide el tiempo de pared y se promedia por búsqueda, lo que reduce el ruido
// de la medición a nivel de nanosegundo y permite graficar el tiempo de
// ejecución por búsqueda a lo largo de toda la secuencia (sección 7.2).
struct Bucketed {
    std::vector<size_t> idx;            // índice de inicio de cada bucket
    std::vector<double> timePerSearch;  // ns promedio por búsqueda en el bucket
    std::vector<double> costPerSearch;  // nodos visitados promedio por búsqueda
    long long totalTimeNs = 0;
    unsigned long long totalCost = 0;
};

// Ejecuta las búsquedas en buckets de tamaño ~M/nBuckets, midiendo tiempo y
// costo por bucket. Si M <= nBuckets el bucket es de tamaño 1 (por búsqueda).
Bucketed searchBucketed(TreeInterface* tree, const std::vector<uint32_t>& queries,
                        size_t nBuckets) {
    Bucketed b;
    const size_t M = queries.size();
    if (M == 0) return b;
    const size_t bucket = std::max<size_t>(1, (M + nBuckets - 1) / nBuckets);
    for (size_t start = 0; start < M; start += bucket) {
        const size_t end = std::min(M, start + bucket);
        unsigned long long costSum = 0;
        auto t0 = Clock::now();
        for (size_t i = start; i < end; ++i) {
            tree->search(queries[i]);
            costSum += tree->getLastCost();
        }
        auto t1 = Clock::now();
        const long long ns = elapsedNs(t0, t1);
        const size_t cnt = end - start;
        b.idx.push_back(start);
        b.timePerSearch.push_back(static_cast<double>(ns) / cnt);
        b.costPerSearch.push_back(static_cast<double>(costSum) / cnt);
        b.totalTimeNs += ns;
        b.totalCost += costSum;
    }
    return b;
}

// Construye una secuencia de búsquedas uniformes a partir del dataset.
std::vector<uint32_t> buildUniformQueries(const std::vector<uint32_t>& dataset,
                                          size_t m, std::mt19937_64& rng) {
    std::uniform_int_distribution<size_t> dist(0, dataset.size() - 1);
    std::vector<uint32_t> q;
    q.reserve(m);
    for (size_t i = 0; i < m; ++i) q.push_back(dataset[dist(rng)]);
    return q;
}

// Construye una secuencia de búsquedas sesgadas según P(i) sobre el dataset.
std::vector<uint32_t> buildBiasedQueries(const std::vector<uint32_t>& dataset,
                                         size_t m, const BiasedSampler& sampler,
                                         std::mt19937_64& rng) {
    std::vector<uint32_t> q;
    q.reserve(m);
    for (size_t i = 0; i < m; ++i) q.push_back(dataset[sampler.sample(rng)]);
    return q;
}

const char* CONFIG_NAMES[4] = {"a", "b", "c", "d"};

} // namespace

// ---------------------------------------------------------------------------
// Generación de datos
// ---------------------------------------------------------------------------

std::vector<uint32_t> generateDataset(size_t n, uint64_t seed) {
    std::mt19937_64 rng(seed);
    std::uniform_int_distribution<uint32_t> dist(0u, 0xFFFFFFFFu);
    std::vector<uint32_t> data;
    data.reserve(n);
    for (size_t i = 0; i < n; ++i) data.push_back(dist(rng));
    return data;
}

BiasedSampler::BiasedSampler(size_t n, double lambda)
    : n_(n), lambda_(lambda) {
    denom_ = 1.0 - std::exp(-lambda_ * static_cast<double>(n_));
    if (denom_ <= 0.0) denom_ = 1.0; // saturación numérica para n grande
}

size_t BiasedSampler::sample(std::mt19937_64& rng) const {
    if (lambda_ <= 0.0) {
        std::uniform_int_distribution<size_t> u(0, n_ - 1);
        return u(rng);
    }
    std::uniform_real_distribution<double> u(0.0, 1.0);
    double r = u(rng);
    // CDF inversa de la geométrica truncada.
    double val = std::log(1.0 - r * denom_) / (-lambda_);
    size_t idx = static_cast<size_t>(val);
    if (idx >= n_) idx = n_ - 1;
    return idx;
}

// ---------------------------------------------------------------------------
// 7.2  Escenarios base
// ---------------------------------------------------------------------------

void runBaseScenarios() {
    std::cerr << "== Escenarios base (7.2), c = " << C_PARAM
              << " => M = 10^c * N ==\n";

    std::ofstream totals = openCsv("base_totals.csv");
    totals << "config,N,struct,insert_time_ns,search_time_ns,"
              "total_search_cost,avg_search_cost\n";

    for (int e = BASE_EXP_MIN; e <= BASE_EXP_MAX; ++e) {
        const size_t N = size_t(1) << e;
        const size_t M = static_cast<size_t>(std::pow(10.0, C_PARAM)) * N;

        // Dataset asociado a este N.
        std::vector<uint32_t> dataset =
            generateDataset(N, BASE_SEED + static_cast<uint64_t>(e));

        std::vector<uint32_t> sorted = dataset;
        std::sort(sorted.begin(), sorted.end());

        BiasedSampler sampler(N, LAMBDA);

        for (int cfg = 0; cfg < 4; ++cfg) {
            // Orden de inserción: a,b aleatorio; c,d ordenado.
            const bool sortedInsert = (cfg == 2 || cfg == 3);
            const std::vector<uint32_t>& insertKeys = sortedInsert ? sorted : dataset;

            // Tipo de búsqueda: a,c uniforme; b,d sesgada.
            const bool biased = (cfg == 1 || cfg == 3);

            std::mt19937_64 rng(BASE_SEED ^ (0x1000ULL * e + cfg));
            std::vector<uint32_t> queries =
                biased ? buildBiasedQueries(dataset, M, sampler, rng)
                       : buildUniformQueries(dataset, M, rng);

            // Ambas estructuras se miden sobre la misma secuencia de búsquedas,
            // registrando el tiempo por búsqueda a lo largo de toda la secuencia.

            // --- AVL ---
            long long avlInsNs;
            Bucketed avlB;
            {
                AVLTree avl;
                avlInsNs = insertAll(&avl, insertKeys);
                avlB = searchBucketed(&avl, queries, MAX_PERSEARCH_SAMPLES);
            }

            // --- Splay ---
            long long splayInsNs;
            Bucketed splayB;
            {
                SplayTree splay;
                splayInsNs = insertAll(&splay, insertKeys);
                splayB = searchBucketed(&splay, queries, MAX_PERSEARCH_SAMPLES);
            }

            totals << CONFIG_NAMES[cfg] << ',' << N << ",AVL," << avlInsNs << ','
                   << avlB.totalTimeNs << ',' << avlB.totalCost << ','
                   << (double)avlB.totalCost / M << '\n';
            totals << CONFIG_NAMES[cfg] << ',' << N << ",Splay," << splayInsNs << ','
                   << splayB.totalTimeNs << ',' << splayB.totalCost << ','
                   << (double)splayB.totalCost / M << '\n';

            std::cerr << "  cfg " << CONFIG_NAMES[cfg] << "  N=" << N
                      << "  M=" << M
                      << "  AVL avg=" << (double)avlB.totalCost / M
                      << "  Splay avg=" << (double)splayB.totalCost / M << '\n';

            // Serie por-búsqueda (tiempo y costo) de ambas estructuras para los
            // gráficos de línea de 7.2. Un archivo por (configuración, N).
            std::ofstream ps = openCsv(std::string("base_persearch_") +
                                       CONFIG_NAMES[cfg] + "_N" +
                                       std::to_string(N) + ".csv");
            ps << "idx,avl_time_ns,splay_time_ns,avl_cost,splay_cost\n";
            for (size_t k = 0; k < avlB.idx.size(); ++k) {
                ps << avlB.idx[k] << ',' << avlB.timePerSearch[k] << ','
                   << splayB.timePerSearch[k] << ',' << avlB.costPerSearch[k] << ','
                   << splayB.costPerSearch[k] << '\n';
            }
        }
    }
    std::cerr << "   -> outputs/results/base_totals.csv\n";
}

// ---------------------------------------------------------------------------
// 7.3.1.a  Sequential Access Theorem
// ---------------------------------------------------------------------------

void runSequentialAccess(int exp) {
    const size_t N = size_t(1) << exp;
    std::cerr << "== Sequential Access Theorem (7.3.1.a), N = 2^" << exp
              << " = " << N << " ==\n";

    // Dataset N y su versión ordenada y sin duplicados (para claves crecientes).
    std::vector<uint32_t> dataset = generateDataset(N, BASE_SEED + 100);
    std::vector<uint32_t> sortedUnique = dataset;
    std::sort(sortedUnique.begin(), sortedUnique.end());
    sortedUnique.erase(std::unique(sortedUnique.begin(), sortedUnique.end()),
                       sortedUnique.end());

    // Máximo m = N/10. Construimos una secuencia creciente de ese largo
    // muestreando claves equiespaciadas (no necesariamente consecutivas).
    const size_t maxM = N / 10;
    std::vector<uint32_t> seq;
    seq.reserve(maxM);
    const size_t stride = std::max<size_t>(1, sortedUnique.size() / maxM);
    for (size_t i = 0; i < sortedUnique.size() && seq.size() < maxM; i += stride) {
        seq.push_back(sortedUnique[i]);
    }
    const size_t L = seq.size();

    // Puntos de medición m_j = j * N/100, j = 1..10.
    std::vector<size_t> breakpoints;
    for (int j = 1; j <= 10; ++j) {
        size_t m = std::min<size_t>(L, (static_cast<size_t>(j) * N) / 100);
        if (m > 0) breakpoints.push_back(m);
    }

    // Mide la secuencia sobre un árbol, registrando tiempo y costo acumulados
    // en cada breakpoint. La secuencia se recorre una sola vez.
    auto measure = [&](TreeInterface* tree, std::vector<long long>& times,
                       std::vector<unsigned long long>& costs) {
        size_t bpIdx = 0;
        unsigned long long cost = 0;
        auto start = Clock::now();
        for (size_t i = 0; i < L; ++i) {
            tree->search(seq[i]);
            cost += tree->getLastCost();
            if (bpIdx < breakpoints.size() && (i + 1) == breakpoints[bpIdx]) {
                times.push_back(elapsedNs(start, Clock::now()));
                costs.push_back(cost);
                ++bpIdx;
            }
        }
    };

    std::vector<long long> avlT, splayT;
    std::vector<unsigned long long> avlC, splayC;

    {
        std::cerr << "  construyendo AVL...\n";
        AVLTree avl;
        for (uint32_t k : dataset) avl.insert(k);
        std::cerr << "  midiendo AVL...\n";
        measure(&avl, avlT, avlC);
    }
    {
        std::cerr << "  construyendo Splay...\n";
        SplayTree splay;
        for (uint32_t k : dataset) splay.insert(k);
        std::cerr << "  midiendo Splay...\n";
        measure(&splay, splayT, splayC);
    }

    std::ofstream out = openCsv("seq_access.csv");
    out << "m,n,avl_time_ns,splay_time_ns,avl_cost,splay_cost\n";
    for (size_t j = 0; j < breakpoints.size(); ++j) {
        out << breakpoints[j] << ',' << N << ',' << avlT[j] << ',' << splayT[j]
            << ',' << avlC[j] << ',' << splayC[j] << '\n';
    }
    std::cerr << "   -> outputs/results/seq_access.csv\n";
}

// ---------------------------------------------------------------------------
// 7.3.1.b  Working Set Theorem
// ---------------------------------------------------------------------------

void runWorkingSet(int exp) {
    const size_t N = size_t(1) << exp;
    const size_t M = static_cast<size_t>(std::pow(10.0, C_PARAM)) * N;
    std::cerr << "== Working Set Theorem (7.3.1.b), N = 2^" << exp << " = " << N
              << ", M = " << M << " ==\n";

    std::vector<uint32_t> dataset = generateDataset(N, BASE_SEED + 200);

    const std::vector<size_t> Ws = {10, 100, 1000, 10000, 100000, 1000000};

    std::ofstream out = openCsv("working_set.csv");
    out << "W,n,avl_time_ns,splay_time_ns,avl_avg_cost,splay_avg_cost\n";

    // El AVL no se modifica al buscar: se construye una sola vez.
    std::cerr << "  construyendo AVL (una vez)...\n";
    AVLTree avl;
    for (uint32_t k : dataset) avl.insert(k);

    std::mt19937_64 rng(BASE_SEED + 201);

    for (size_t W : Ws) {
        if (W > N) break;

        // Subconjunto aleatorio de tamaño W (working set).
        std::vector<uint32_t> ws;
        ws.reserve(W);
        {
            std::vector<size_t> idx(N);
            std::iota(idx.begin(), idx.end(), 0);
            std::shuffle(idx.begin(), idx.end(), rng);
            for (size_t i = 0; i < W; ++i) ws.push_back(dataset[idx[i]]);
        }

        // Secuencia de M búsquedas uniformes dentro del working set.
        std::vector<uint32_t> queries = buildUniformQueries(ws, M, rng);

        // AVL (reutilizado).
        SearchResult avlRes = searchAll(&avl, queries);

        // Splay: se reconstruye por cada W para aislar el estado inicial.
        SearchResult splayRes;
        {
            SplayTree splay;
            for (uint32_t k : dataset) splay.insert(k);
            splayRes = searchAll(&splay, queries);
        }

        out << W << ',' << N << ',' << avlRes.timeNs << ',' << splayRes.timeNs
            << ',' << (double)avlRes.totalCost / M << ','
            << (double)splayRes.totalCost / M << '\n';
        out.flush();

        std::cerr << "  W=" << W
                  << "  AVL avg=" << (double)avlRes.totalCost / M
                  << "  Splay avg=" << (double)splayRes.totalCost / M << '\n';
    }
    std::cerr << "   -> outputs/results/working_set.csv\n";
}

// ---------------------------------------------------------------------------
// 7.4  Traversal Conjecture (bonus)
// ---------------------------------------------------------------------------

void runTraversalConjecture(int exp) {
    const size_t N = size_t(1) << exp;
    std::cerr << "== Traversal Conjecture (bonus 7.4), N = 2^" << exp << " = "
              << N << " ==\n";

    std::vector<uint32_t> dataset = generateDataset(N, BASE_SEED + 300);

    // Dos permutaciones distintas de los mismos elementos.
    std::vector<uint32_t> permA = dataset;
    std::vector<uint32_t> permB = dataset;
    std::mt19937_64 rngA(BASE_SEED + 301), rngB(BASE_SEED + 302);
    std::shuffle(permA.begin(), permA.end(), rngA);
    std::shuffle(permB.begin(), permB.end(), rngB);

    std::cerr << "  construyendo T1...\n";
    SplayTree t1;
    for (uint32_t k : permA) t1.insert(k);

    std::cerr << "  construyendo T2...\n";
    SplayTree t2;
    for (uint32_t k : permB) t2.insert(k);

    // Secuencia: preorden de T1.
    std::cerr << "  recorriendo T1 en preorden...\n";
    std::vector<uint32_t> seq = t1.preorder();
    const size_t L = seq.size();

    // Búsqueda de la secuencia en T2 con muestreo de costo/tiempo por búsqueda.
    std::ofstream out = openCsv("traversal.csv");
    out << "idx,cost,cum_time_ns\n";
    size_t stride = std::max<size_t>(1, L / MAX_PERSEARCH_SAMPLES);

    unsigned long long totalCost = 0;
    std::cerr << "  buscando secuencia en T2...\n";
    auto start = Clock::now();
    for (size_t i = 0; i < L; ++i) {
        t2.search(seq[i]);
        unsigned long long c = t2.getLastCost();
        totalCost += c;
        if (i % stride == 0) {
            out << i << ',' << c << ',' << elapsedNs(start, Clock::now()) << '\n';
        }
    }
    long long totalNs = elapsedNs(start, Clock::now());

    std::cerr << "  costo total = " << totalCost << " nodos visitados ("
              << (double)totalCost / L << " por búsqueda, n=" << L << ")\n";
    std::cerr << "  tiempo total = " << totalNs / 1e6 << " ms\n";
    std::cerr << "  costo/n = " << (double)totalCost / L
              << "  (la conjetura postula costo total O(n), i.e. costo/n acotado)\n";
    std::cerr << "   -> outputs/results/traversal.csv\n";
}

} // namespace experiments
