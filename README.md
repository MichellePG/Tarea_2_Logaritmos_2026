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
│       ├── ??
│       ├── ??
│       └── ??
│
├── main.cpp 
│
├── data/                       # Datos generados localmente
│
├── outputs/                    # Resultados experimentales
│   ├── results/
│   └── plots/
│
├── docs/                       # Informe y material auxiliar
│
├── .gitignore                 
├── Makefile                    # Compilación y comandos útiles
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

## Compilación

Para compilar los programas, ejecute:

Pendiente.

## Uso

Pendiente.

### Experimentación

Pendiente.

## Limpieza

Para limpiar compilación:
