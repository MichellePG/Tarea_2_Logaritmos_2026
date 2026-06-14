#include "src/experiments/Experiments.hpp"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

/**
 * Punto de entrada de la batería experimental de la Tarea 2.
 *
 * Uso:
 *   ./tarea2 [bateria] [exp]
 *
 *   bateria : base | seq | ws | traversal | all   (por defecto: all)
 *   exp     : exponente de N para los experimentos de teoremas (7.3, 7.4).
 *             Por defecto 25 (N = 2^25, como pide el enunciado).
 *             Conviene reducirlo (p.ej. 18) para pruebas rápidas.
 *
 * Ejemplos:
 *   ./tarea2                 # corre todo con N = 2^25 (pesado, ~4 GB RAM)
 *   ./tarea2 base            # solo escenarios base (7.2)
 *   ./tarea2 seq 20          # Sequential Access con N = 2^20
 *   ./tarea2 all 18          # toda la batería en modo liviano
 */
static void printUsage(const char* prog) {
    std::cerr << "Uso: " << prog << " [base|seq|ws|traversal|all] [exp]\n"
              << "  bateria: conjunto de experimentos a ejecutar (def: all)\n"
              << "  exp    : exponente de N para teoremas/bonus (def: "
              << experiments::THEOREM_EXP_DEFAULT << ")\n";
}

int main(int argc, char** argv) {
    std::string which = "all";
    int exp = experiments::THEOREM_EXP_DEFAULT;

    if (argc >= 2) {
        which = argv[1];
        if (which == "-h" || which == "--help") {
            printUsage(argv[0]);
            return 0;
        }
    }
    if (argc >= 3) {
        exp = std::atoi(argv[2]);
        if (exp < 4 || exp > 30) {
            std::cerr << "[error] exp fuera de rango razonable [4, 30]\n";
            return 1;
        }
    }

    const bool runAll = (which == "all");

    if (runAll || which == "base") {
        experiments::runBaseScenarios();
    }
    if (runAll || which == "seq") {
        experiments::runSequentialAccess(exp);
    }
    if (runAll || which == "ws") {
        experiments::runWorkingSet(exp);
    }
    if (runAll || which == "traversal") {
        experiments::runTraversalConjecture(exp);
    }

    if (!runAll && which != "base" && which != "seq" && which != "ws" &&
        which != "traversal") {
        std::cerr << "[error] batería desconocida: " << which << "\n";
        printUsage(argv[0]);
        return 1;
    }

    std::cerr << "\nListo. Resultados en outputs/results/. "
                 "Genere los gráficos con: uv run scripts/plot.py\n";
    return 0;
}
