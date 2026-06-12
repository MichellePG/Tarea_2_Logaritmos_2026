#pragma once

#include <cstdint>

/**
 * Nodo de un AVL Tree.
 *
 * Cada nodo almacena una clave entera sin signo de 32 bits, punteros a sus
 * hijos izquierdo y derecho, y la altura del subárbol cuya raíz es este nodo.
 *
 * @param key    Clave almacenada en el nodo.
 * @param left   Puntero al hijo izquierdo. Contiene claves menores que key.
 * @param right  Puntero al hijo derecho. Contiene claves mayores que key.
 * @param height Altura del subárbol con raíz en este nodo.
 */
struct AVLNode {
    uint32_t key;
    AVLNode* left;
    AVLNode* right;
    int height;

    /**
     * Constructor de un nodo AVL.
     *
     * @param k Clave que se almacenará en el nodo.
     */
    explicit AVLNode(uint32_t k)
        : key(k), left(nullptr), right(nullptr), height(1) {}
};