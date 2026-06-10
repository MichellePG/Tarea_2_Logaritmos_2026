#pragma once
#include <cstdint>

/**
 * Nodo de un Splay Tree.
 * Solo guarda clave, puntero al padre y a los dos hijos.
 *
 * @param key   Clave del nodo 
 * @param left  Hijo izquierdo
 * @param right Hijo derecho
 * @param parent Puntero al nodo padre 
 */
struct SplayNode {
    uint32_t key;
    SplayNode* left;
    SplayNode* right;
    SplayNode* parent;

    explicit SplayNode(uint32_t k)
        : key(k), left(nullptr), right(nullptr), parent(nullptr) {}
};