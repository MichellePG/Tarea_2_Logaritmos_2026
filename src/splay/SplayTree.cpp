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
// x es hijo izquierdo de y. Rotación a la derecha sobre y.
void SplayTree::zig(SplayNode* x) {
    SplayNode* y = x->parent;
    SplayNode* B = x->right;
    
    // B pasa a ser hijo izquierdo de y
    y->left = B;
    if (B) B->parent = y;

    // x pasa a ser padre de y
    x->parent = y->parent;
    if (y->parent==nullptr) {
        root = x; // x se convierte en raíz
    } else if (y == y->parent->left) {
        y->parent->left = x; 
    } else {
        y->parent->right = x;
    }

    x->right = y;
    y->parent = x;

}

// Zag: y(A, x(B,C)) → x(y(A,B), C)
// x es hijo derecho de y. Rotación a la izquierda sobre y.
void SplayTree::zag(SplayNode* x) {
    SplayNode* y = x->parent;
    SplayNode* B = x->left;
    
    // B pasa a ser hijo derecho de y
    y->right = B;
    if (B) B->parent = y;

    // x pasa a ser padre de y
    x->parent = y->parent;
    if (y->parent==nullptr) {
        root = x; // x se convierte en raíz
    } else if (y == y->parent->left) {
        y->parent->left = x;
    } else {
        y->parent->right = x; 
    }

    x->left = y;
    y->parent = x;
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

            if (xIsLeft && yIsLeft) { // Zig-Zig
                zig(y); // Primero sobre y
                zig(x); // Luego sobre x
            } else if (!xIsLeft && !yIsLeft) { // Zag-Zag
                zag(y); // Primero sobre y
                zag(x); // Luego sobre x
            } else if (xIsLeft && !yIsLeft) { // Zag-Zig:
                zig(x); // Primero sobre x hacia la derecha
                zag(x); // Luego sobre x hacia la izquierda
            } else { // Zig-Zag
                zag(x); // Primero sobre x hacia la izquierda
                zig(x); // Luego sobre x hacia la derecha
            }
        }
    }
    root = x;
}


// ----------------------------------------------------------------
// Insert
// ----------------------------------------------------------------

void SplayTree::insert(uint32_t key) {
    SplayNode* curr = root;
    SplayNode* parent = nullptr;

    // Bajamos hasta encontrar la posición de inserción
    while (curr != nullptr) {
        parent = curr;
        if (key <= curr->key) {
            curr = curr->left;
        } else                  {
            curr = curr->right;
        }
    }

    // Creamos el nuevo nodo y lo enlazamos
    SplayNode* node = new SplayNode(key);
    node->parent = parent;

    if (parent == nullptr) {
        root = node;        // árbol vacío
    } else if (key <= parent->key) {
        parent->left = node;
    } else {
        parent->right = node;
    }

    // Llevamos el nuevo nodo a la raíz
    splay(node);
}

// ----------------------------------------------------------------
// Search
// ----------------------------------------------------------------

// Búsqueda BST clásica.
// Si se encuentra: splay(x), retorna true.
// Si no: splay del último nodo visitado, retorna false.
bool SplayTree::search(uint32_t key) {
    lastCost = 0;
    SplayNode* curr = root;
    SplayNode* last = nullptr;

    while (curr != nullptr) {
        ++lastCost;
        last = curr;
        if (key == curr->key) {
            splay(curr);
            return true;
        }
        if (key < curr->key) {
            curr = curr->left;
        } else {
            curr = curr->right;
        }
    }

    // Si no se encontró, splay del último nodo visitado (si existe)
    if (last) splay(last);
    return false;
}


// Recorrido en preorden (sino se desborda pila en arboles de N = 2^25 nodos).
std::vector<uint32_t> SplayTree::preorder() const {
    std::vector<uint32_t> keys;
    if (!root) return keys;

    std::vector<SplayNode*> stack;
    stack.push_back(root);
    while (!stack.empty()) {
        SplayNode* node = stack.back();
        stack.pop_back();
        keys.push_back(node->key);
        if (node->right) stack.push_back(node->right);
        if (node->left)  stack.push_back(node->left);
    }
    return keys;
}