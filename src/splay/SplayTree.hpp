#pragma once
#include "SplayNode.hpp"
#include "../../include/TreeInterface.hpp"

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

private:
    SplayNode* root;

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