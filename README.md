# Tarea 2 – AVL Trees vs Splay Trees

Implementación y análisis experimental de árboles AVL y Splay Trees para la Tarea 2 de CC4102 – Diseño y Análisis de Algoritmos.

## Integrantes

* Andrés Franz M.
* Nahuel L. Sanhueza
* Michelle Pérez G.

## Descripción

Este proyecto implementa dos estructuras de datos de búsqueda binarias autobalanceables:

* AVL Trees
* Splay Trees

Además se desarrollará un conjunto de experimentos para comparar el desempeño de ambas estructuras bajo distintos escenarios de inserción y búsqueda, así como para evaluar experimentalmente propiedades teóricas de los Splay Trees.

## Estructura del proyecto

```text
TAREA_2_LOGARITMOS_2026/
│
├── include/                
│   └── TreeInterface.hpp       # Interfaz común
│
├── src/
│   ├── avl/                    # Implementación AVL
│   │   ├── AVLNode.hpp
│   │   ├── AVLTree.hpp
│   │   └── AVLTree.cpp
│   │
│   ├── splay/                  # Implementación Splay Tree
│   │   ├── SplayNode.hpp
│   │   ├── SplayTree.hpp
│   │   └── SplayTree.cpp
│   │
│   └── experiments/            # Experimentación y benchmarks
│       ├── Experiments.hpp      # Firmas y constantes de configuración
│       └── Experiments.cpp      # Implementación de los 4 bloques (7.2–7.4)
│
├── main.cpp                    # Batería de tests parametrizable
│
├── scripts/
│   └── plot.py                 # Generación de gráficos
│
├── outputs/                    # Resultados experimentales (generados)
│   ├── results/                # CSV de salida
│   └── plots/                  # Gráficos PNG
│
├── requirements.txt            # Dependencias de plot.py
├── .gitignore
├── makefile                    # Compilación y comandos útiles
└── README.md                   # Descripción del proyecto
```

## Implementación

### Interfaz común

Se definió una interfaz común `TreeInterface` que abstrae las operaciones básicas requeridas por los experimentos:

* `insert(uint32_t key)`
* `search(uint32_t key)`

Tanto `AVLTree` como `SplayTree` implementan esta interfaz, permitiendo ejecutar el mismo conjunto de experimentos sobre ambas estructuras de forma transparente.

### AVL Tree

La implementación del árbol AVL incluye:

* Nodo AVL (`AVLNode`) con clave, punteros a hijos y altura.
* Inserción recursiva.
* Búsqueda recursiva.
* Actualización de alturas.
* Cálculo del factor de balance.
* Rotaciones simples izquierda y derecha.
* Rebalanceo mediante los cuatro casos clásicos:

  * Left-Left (LL)
  * Left-Right (LR)
  * Right-Right (RR)
  * Right-Left (RL)

La memoria utilizada por el árbol se libera recursivamente mediante un destructor.

### Splay Tree

La implementación del Splay Tree incluye:

* Nodo Splay (`SplayNode`) con clave, punteros a hijos y puntero al padre.
* Inserción BST seguida de una operación `splay`.
* Búsqueda BST seguida de una operación `splay` sobre el nodo encontrado o sobre el último nodo visitado.
* Rotaciones primitivas:

  * Zig
  * Zag
* Operación `splay` mediante los casos:

  * Zig
  * Zag
  * Zig-Zig
  * Zag-Zag
  * Zig-Zag
  * Zag-Zig

### Experimentación

El módulo `experiments` implementa la sección 7 del enunciado. Para medir el
desempeño se instrumentó cada estructura con un contador de costo
(`getLastCost()`) que registra el número de nodos visitados en la última
búsqueda. Este costo es un proxy reproducible del costo teórico de acceso y se
reporta junto con el tiempo de reloj (`std::chrono::steady_clock`).

**Constantes de configuración** (en `src/experiments/Experiments.hpp`):

| Constante   | Valor    | Descripción                                              |
|-------------|----------|----------------------------------------------------------|
| `C_PARAM`   | `1`      | Constante `c`: número de búsquedas `M = 10^c · N`.       |
| `LAMBDA`    | `0.01`   | Parámetro `λ` de la distribución sesgada `P(i)` (7.1.2). |
| `BASE_SEED` | `0xC0FFEE` | Semilla base (reproducibilidad de datasets/secuencias).|

Bloques experimentales:

- **Escenarios base (7.2):** para `N ∈ {2^10,…,2^14}` y las 4 configuraciones
  (inserción aleatoria/ordenada × búsqueda uniforme/sesgada).
- **Sequential Access Theorem (7.3.1.a):** secuencia de claves crecientes.
- **Working Set Theorem (7.3.1.b):** `W ∈ {10, 10², …, 10⁶}`.
- **Traversal Conjecture (bonus 7.4):** preorden de `T₁` buscado en `T₂`.

## Compilación

```bash
make
```

Genera el ejecutable `tarea2` (requiere `g++` con soporte C++17).

## Uso

```bash
./tarea2 [bateria] [exp]
```

- `bateria`: `base` | `seq` | `ws` | `traversal` | `all` (por defecto `all`).
- `exp`: exponente de `N` para los experimentos de teoremas (7.3, 7.4).
  Por defecto `25` (`N = 2^25`, como pide el enunciado; ~4 GB de RAM).
  Conviene reducirlo para pruebas rápidas (p. ej. `18`).

Ejemplos:

```bash
./tarea2                 # toda la batería con N = 2^25 (pesado)
./tarea2 base            # solo escenarios base (7.2)
./tarea2 seq 20          # Sequential Access con N = 2^20
make run EXP=18          # toda la batería en modo liviano
```

Los resultados se escriben como CSV en `outputs/results/`.

## Gráficos

Se usa [`uv`](https://docs.astral.sh/uv/), que instala las dependencias de
Python automáticamente a partir de la metadata del propio script:

```bash
uv run scripts/plot.py     # o bien: make plots
```

Alternativa con `pip`:

```bash
pip install -r requirements.txt
python3 scripts/plot.py
```

Los gráficos PNG se generan en `outputs/plots/`.

## Limpieza

```bash
make clean
```
