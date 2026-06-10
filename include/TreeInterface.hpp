#pragma once
#include <cstdint>

/**
 * Interfaz común para AVL Tree y Splay Tree.
 * Permite al módulo de experimentos usar ambas estructuras de forma intercambiable.
 */
class TreeInterface {
public:
    virtual ~TreeInterface() = default;

    /**
     * Inserta una clave en el árbol.
     * @param key Clave a insertar
     */
    virtual void insert(uint32_t key) = 0;

    /**
     * Busca una clave en el árbol.
     * @param key Clave a buscar
     * @return true si la clave existe, false en caso contrario
     */
    virtual bool search(uint32_t key) = 0;
};