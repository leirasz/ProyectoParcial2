#include "BPlusTreeTitulares.h"
#include <iostream>

BPlusTreeTitulares::BPlusTreeTitulares(int grado_) : raiz(nullptr), grado(grado_) {}

BPlusTreeTitulares::~BPlusTreeTitulares() {
    liberarNodo(raiz);
}

void BPlusTreeTitulares::liberarNodo(NodoBPlus* nodo) {
    if (!nodo) return;
    if (!nodo->esHoja) {
        for (int i = 0; i <= nodo->numClaves; ++i)
            liberarNodo(nodo->hijos[i]);
    }
    delete nodo;
}

void BPlusTreeTitulares::insertar(const std::string& ci, Titular* titular) {
    if (!raiz) {
        raiz = new NodoBPlus(grado, true);
        raiz->claves[0] = ci;
        raiz->datos[0] = titular;
        raiz->numClaves = 1;
        return;
    }
    NodoBPlus* nuevoHijo = nullptr;
    std::string nuevaClave;
    insertarEnNodo(raiz, ci, titular, nuevoHijo, nuevaClave);
    if (nuevoHijo) {
        NodoBPlus* nuevaRaiz = new NodoBPlus(grado, false);
        nuevaRaiz->claves[0] = nuevaClave;
        nuevaRaiz->hijos[0] = raiz;
        nuevaRaiz->hijos[1] = nuevoHijo;
        nuevaRaiz->numClaves = 1;
        raiz = nuevaRaiz;
    }
}

void BPlusTreeTitulares::insertarEnNodo(NodoBPlus* nodo, const std::string& ci, Titular* titular, NodoBPlus*& nuevoHijo, std::string& nuevaClave) {
    int i = nodo->numClaves - 1;
    if (nodo->esHoja) {
        // Insertar en hoja
        while (i >= 0 && ci < nodo->claves[i]) {
            nodo->claves[i + 1] = nodo->claves[i];
            nodo->datos[i + 1] = nodo->datos[i];
            --i;
        }
        nodo->claves[i + 1] = ci;
        nodo->datos[i + 1] = titular;
        nodo->numClaves++;

        // Split si es necesario
        if (nodo->numClaves == grado) {
            int mitad = grado / 2;
            NodoBPlus* nuevo = new NodoBPlus(grado, true);
            nuevo->numClaves = nodo->numClaves - mitad;
            for (int j = 0; j < nuevo->numClaves; ++j) {
                nuevo->claves[j] = nodo->claves[mitad + j];
                nuevo->datos[j] = nodo->datos[mitad + j];
            }
            nodo->numClaves = mitad;
            nuevo->siguiente = nodo->siguiente;
            nodo->siguiente = nuevo;
            nuevaClave = nuevo->claves[0];
            nuevoHijo = nuevo;
        } else {
            nuevoHijo = nullptr;
        }
    } else {
        // Insertar en hijo adecuado
        while (i >= 0 && ci < nodo->claves[i]) --i;
        ++i;
        NodoBPlus* hijoNuevo = nullptr;
        std::string claveNueva;
        insertarEnNodo(nodo->hijos[i], ci, titular, hijoNuevo, claveNueva);
        if (hijoNuevo) {
            // Insertar nueva clave y puntero en este nodo
            for (int j = nodo->numClaves; j > i; --j) {
                nodo->claves[j] = nodo->claves[j - 1];
                nodo->hijos[j + 1] = nodo->hijos[j];
            }
            nodo->claves[i] = claveNueva;
            nodo->hijos[i + 1] = hijoNuevo;
            nodo->numClaves++;
            // Split si es necesario
            if (nodo->numClaves == grado) {
                int mitad = grado / 2;
                NodoBPlus* nuevo = new NodoBPlus(grado, false);
                nuevo->numClaves = nodo->numClaves - mitad - 1;
                for (int j = 0; j < nuevo->numClaves; ++j) {
                    nuevo->claves[j] = nodo->claves[mitad + 1 + j];
                    nuevo->hijos[j] = nodo->hijos[mitad + 1 + j];
                }
                nuevo->hijos[nuevo->numClaves] = nodo->hijos[grado];
                nuevaClave = nodo->claves[mitad];
                nodo->numClaves = mitad;
                nuevoHijo = nuevo;
            } else {
                nuevoHijo = nullptr;
            }
        } else {
            nuevoHijo = nullptr;
        }
    }
}

Titular* BPlusTreeTitulares::buscar(const std::string& ci) const {
    return buscarEnNodo(raiz, ci);
}

Titular* BPlusTreeTitulares::buscarEnNodo(NodoBPlus* nodo, const std::string& ci) const {
    if (!nodo) return nullptr;
    int i = 0;
    while (i < nodo->numClaves && ci > nodo->claves[i]) ++i;
    if (nodo->esHoja) {
        if (i < nodo->numClaves && nodo->claves[i] == ci)
            return nodo->datos[i];
        return nullptr;
    } else {
        return buscarEnNodo(nodo->hijos[i], ci);
    }
}

void BPlusTreeTitulares::imprimir() const {
    imprimirNodo(raiz, 0, true);
}

void BPlusTreeTitulares::imprimirNodo(NodoBPlus* nodo, int nivel,bool esRaiz) const {
    if (!nodo) return;
    if (esRaiz) {
        std::cout << "Raiz: ";
    } else {
        std::cout << std::string(nivel * 2, ' ') << (nodo->esHoja ? "[Hoja] " : "[Interno] ");
    }
    for (int i = 0; i < nodo->numClaves; ++i)
        std::cout << nodo->claves[i] << " ";
    std::cout << std::endl;
    if (!nodo->esHoja) {
        for (int i = 0; i <= nodo->numClaves; ++i)
            imprimirNodo(nodo->hijos[i], nivel + 1);
    }
}

void BPlusTreeTitulares::construirDesdeLista(NodoDoble<Titular*>* cabeza) {
    if (!cabeza) return;
    NodoDoble<Titular*>* actual = cabeza;
    do {
        Titular* t = actual->dato;
        insertar(t->getPersona().getCI(), t);
        actual = actual->siguiente;
    } while (actual != cabeza);
}