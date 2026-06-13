#pragma once
#include "SplayNode.hpp"
#include "../../include/TreeInterface.hpp"
#include <vector>
#include <cstdint>

/**
 * Implementación de Splay Tree.
 * Árbol de búsqueda binaria autobalanceable que lleva el nodo accedido a la raíz mediante la operación splay().
 */
class SplayTree : public TreeInterface {
public:
    SplayTree();
    ~SplayTree();

    // Inserta la clave key en el árbol.
    void insert(uint32_t key) override;
    // Busca key, retorna true si existe. Siempre termina con splay().
    bool search(uint32_t key) override;

    /**
     * Costo (nodos visitados durante el descenso) del último search().
     * El trabajo del splay posterior es proporcional a esta profundidad, por
     * lo que sirve como proxy del costo amortizado de acceso.
     */
    unsigned long long getLastCost() const override { return lastCost; }

    /**
     * Recorrido en preorden de las claves del árbol (sin modificar la
     * estructura). Usado por el experimento de la Traversal Conjecture.
     * @return vector con las claves en orden de preorden (raíz, izq, der).
     */
    std::vector<uint32_t> preorder() const;

private:
    SplayNode* root;

    // Nodos visitados durante el descenso de la última búsqueda.
    unsigned long long lastCost = 0;

    // --- Rotaciones (sección 3.2 del enunciado) ---

    // Zig: rotación simple a la derecha sobre el nodo x.
    void zig(SplayNode* x);

    // Zag: rotación simple a la izquierda sobre el nodo x.
    void zag(SplayNode* x);

    // --- Operación central ---

    /**
     * Lleva el nodo x a la raíz del árbol mediante rotaciones.
     * Aplica zig/zag simples solo si el padre es la raíz, en otro caso aplica rotaciones dobles (zig-zig, zig-zag, etc.).
     * @param x Nodo a subir
     */
    void splay(SplayNode* x);

    // Libera recursivamente toda la memoria del subárbol.
    void destroy(SplayNode* node);
};