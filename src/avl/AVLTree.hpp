#pragma once

#include "AVLNode.hpp"
#include "../../include/TreeInterface.hpp"

/**
 * Árbol AVL que implementa inserción y búsqueda.
 */
class AVLTree : public TreeInterface {
public:
    /**
     * Crea un árbol AVL vacío.
     */
    AVLTree();

    /**
     * Libera todos los nodos del árbol.
     */
    ~AVLTree();

    /**
     * Inserta una clave en el árbol.
     *
     * @param key Clave a insertar.
     */
    void insert(uint32_t key) override;

    /**
     * Busca una clave en el árbol.
     *
     * @param key Clave a buscar.
     * @return true si la clave está en el árbol, false en caso contrario.
     */
    bool search(uint32_t key) override;

private:
    AVLNode* root;

    /**
     * Inserta una clave en el subárbol indicado y retorna su nueva raíz.
     */
    AVLNode* insertRec(AVLNode* node, uint32_t key);

    /**
     * Busca una clave en el subárbol indicado.
     */
    bool searchRec(AVLNode* node, uint32_t key) const;

    /**
     * Retorna la altura del nodo, o 0 si es nulo.
     */
    int height(AVLNode* node) const;

    /**
     * Calcula BF = altura(izquierdo) - altura(derecho).
     */
    int balanceFactor(AVLNode* node) const;

    /**
     * Actualiza la altura de un nodo según sus hijos.
     */
    void updateHeight(AVLNode* node);

    /**
     * Rotación simple a la derecha.
     */
    AVLNode* rotateRight(AVLNode* y);

    /**
     * Rotación simple a la izquierda.
     */
    AVLNode* rotateLeft(AVLNode* y);

    /**
     * Rebalancea un subárbol si es necesario.
     */
    AVLNode* balance(AVLNode* node);

    /**
     * Libera recursivamente los nodos del subárbol.
     */
    void destroy(AVLNode* node);
};