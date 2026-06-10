#include "SplayTree.hpp"

// ----------------------------------------------------------------
// Constructor / Destructor
// ----------------------------------------------------------------

SplayTree::SplayTree() : root(nullptr) {}

SplayTree::~SplayTree() {
    destroy(root);
}

void SplayTree::destroy(SplayNode* node) {
    if (!node) return;
    destroy(node->left);
    destroy(node->right);
    delete node;
}

// ----------------------------------------------------------------
// Rotaciones primitivas
// ----------------------------------------------------------------


// Zig: y(x(A,B), C) → x(A, y(B,C))
void SplayTree::zig(SplayNode* x) {
    SplayNode* y = x->parent;
    // TODO
}

// Zag: y(A, x(B,C)) → x(y(A,B), C)
void SplayTree::zag(SplayNode* x) {
    SplayNode* y = x->parent;
    // TODO
}

// ----------------------------------------------------------------
// Splay
// ----------------------------------------------------------------

void SplayTree::splay(SplayNode* x) {
    // Mientras x no sea la raíz
    while (x->parent != nullptr) {
        SplayNode* y = x->parent;
        SplayNode* z = y->parent;

        if (z == nullptr) {
            // y es la raíz → rotación simple
            if (x == y->left) zig(x);   // Zig
            else zag(x);  // Zag
        } else {
            // Rotaciones dobles
            bool xIsLeft = (x == y->left);
            bool yIsLeft = (y == z->left);

            if (xIsLeft && yIsLeft) {
                // Zig-Zig: primero sobre y, luego sobre x.
                // TODO
            } else if (!xIsLeft && !yIsLeft) {
                // Zag-Zag: primero sobre y, luego sobre x.
                // TODO
            } else if (xIsLeft && !yIsLeft) {
                // Zag-Zig: primero sobre x hacia la izquierda, luego hacia la derecha.
                // TODO
            } else {
                // Zig-Zag: primero sobre x hacia la derecha, luego hacia la izquierda.
                // TODO
            }
        }
    }
    root = x;
}


// ----------------------------------------------------------------
// Insert
// ----------------------------------------------------------------

void SplayTree::insert(uint32_t key) {
    // Inserción BST clásica, luego splay del nuevo nodo
    // TODO
}

// ----------------------------------------------------------------
// Search
// ----------------------------------------------------------------

bool SplayTree::search(uint32_t key) {
    // Búsqueda BST clásica
    // Si se encuentra: splay(x), retorna true
    // Si no: splay del último nodo visitado, retorna false
    // TODO
    return false;
}