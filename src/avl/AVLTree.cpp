#include "AVLTree.hpp"

#include <algorithm>

/**
 * Crea un árbol AVL vacío.
 */
AVLTree::AVLTree() : root(nullptr) {}

/**
 * Libera la memoria usada por el árbol.
 */
AVLTree::~AVLTree() {
    destroy(root);
}

/**
 * Inserta una clave en el AVL.
 */
void AVLTree::insert(uint32_t key) {
    root = insertRec(root, key);
}

/**
 * Busca una clave en el AVL.
 */
bool AVLTree::search(uint32_t key) {
    return searchRec(root, key);
}

/**
 * Inserta una clave en un subárbol y retorna la nueva raíz.
 */
AVLNode* AVLTree::insertRec(AVLNode* node, uint32_t key) {
    if (node == nullptr) {
        return new AVLNode(key);
    }

    if (key < node->key) {
        node->left = insertRec(node->left, key);
    } else if (key > node->key) {
        node->right = insertRec(node->right, key);
    } else {
        return node;
    }

    updateHeight(node);
    return balance(node);
}

/**
 * Busca una clave dentro de un subárbol.
 */
bool AVLTree::searchRec(AVLNode* node, uint32_t key) const {
    if (node == nullptr) {
        return false;
    }

    if (key == node->key) {
        return true;
    }

    if (key < node->key) {
        return searchRec(node->left, key);
    }

    return searchRec(node->right, key);
}

/**
 * Retorna la altura de un nodo.
 */
int AVLTree::height(AVLNode* node) const {
    if (node == nullptr) {
        return 0;
    }

    return node->height;
}

/**
 * Calcula el factor de balance de un nodo.
 */
int AVLTree::balanceFactor(AVLNode* node) const {
    if (node == nullptr) {
        return 0;
    }

    return height(node->left) - height(node->right);
}

/**
 * Actualiza la altura de un nodo.
 */
void AVLTree::updateHeight(AVLNode* node) {
    if (node == nullptr) {
        return;
    }

    node->height = 1 + std::max(height(node->left), height(node->right));
}

/**
 * Rotación simple a la derecha.
 */
AVLNode* AVLTree::rotateRight(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* beta = x->right;

    x->right = y;
    y->left = beta;

    updateHeight(y);
    updateHeight(x);

    return x;
}

/**
 * Rotación simple a la izquierda.
 */
AVLNode* AVLTree::rotateLeft(AVLNode* y) {
    AVLNode* x = y->right;
    AVLNode* beta = x->left;

    x->left = y;
    y->right = beta;

    updateHeight(y);
    updateHeight(x);

    return x;
}

/**
 * Rebalancea un subárbol si quedó desbalanceado.
 */
AVLNode* AVLTree::balance(AVLNode* node) {
    if (node == nullptr) {
        return nullptr;
    }

    int bf = balanceFactor(node);

    // Caso Left Left.
    if (bf > 1 && balanceFactor(node->left) >= 0) {
        return rotateRight(node);
    }

    // Caso Left Right.
    if (bf > 1 && balanceFactor(node->left) < 0) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }

    // Caso Right Right.
    if (bf < -1 && balanceFactor(node->right) <= 0) {
        return rotateLeft(node);
    }

    // Caso Right Left.
    if (bf < -1 && balanceFactor(node->right) > 0) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
}

/**
 * Libera recursivamente los nodos de un subárbol.
 */
void AVLTree::destroy(AVLNode* node) {
    if (node == nullptr) {
        return;
    }

    destroy(node->left);
    destroy(node->right);
    delete node;
}