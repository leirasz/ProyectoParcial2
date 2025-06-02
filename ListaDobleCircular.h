#ifndef LISTADOBLECIRCULAR_H
#define LISTADOBLECIRCULAR_H

#include <iostream>
#include "NodoDoble.h"

template <typename T>
class ListaDobleCircular {
private:
    NodoDoble<T>* cabeza;

public:
    ListaDobleCircular();
    ~ListaDobleCircular();

    void insertar(const T& valor);
    void imprimirAdelante() const;
    void imprimirAtras() const;
    bool vacia() const;
    T eliminarPorCabeza();
    NodoDoble<T>* getCabeza() const;
    ListaDobleCircular<T> getCuentasAhorro() const;
};

template <typename T>
ListaDobleCircular<T>::ListaDobleCircular() {
    cabeza = nullptr;
}

template <typename T>
ListaDobleCircular<T>::~ListaDobleCircular() {
    if (!cabeza) return;
    NodoDoble<T>* actual = cabeza->siguiente;
    while (actual != cabeza) {
        NodoDoble<T>* temp = actual;
        actual = actual->siguiente;
        delete temp;
    }
    delete cabeza;
    cabeza = nullptr;
}

template <typename T>
void ListaDobleCircular<T>::insertar(const T& valor) {
    NodoDoble<T>* nuevo = new NodoDoble<T>(valor);
    if (!cabeza) {
        cabeza = nuevo;
        cabeza->siguiente = cabeza;
        cabeza->anterior = cabeza;
    } else {
        NodoDoble<T>* ultimo = cabeza->anterior;
        nuevo->siguiente = cabeza;
        nuevo->anterior = ultimo;
        cabeza->anterior = nuevo;
        ultimo->siguiente = nuevo;
    }
}

template <typename T>
void ListaDobleCircular<T>::imprimirAdelante() const {
    if (!cabeza) return;
    NodoDoble<T>* actual = cabeza;
    do {
        std::cout << actual->dato << " ";
        actual = actual->siguiente;
    } while (actual != cabeza);
    std::cout << std::endl;
}

template <typename T>
void ListaDobleCircular<T>::imprimirAtras() const {
    if (!cabeza) return;
    NodoDoble<T>* actual = cabeza->anterior;
    do {
        std::cout << actual->dato << " ";
        actual = actual->anterior;
    } while (actual != cabeza->anterior);
    std::cout << std::endl;
}

template <typename T>
bool ListaDobleCircular<T>::vacia() const {
    return cabeza == nullptr;
}

template <typename T>
NodoDoble<T>* ListaDobleCircular<T>::getCabeza() const {
    return cabeza;
}
template <typename T>
T ListaDobleCircular<T>::eliminarPorCabeza() {
    if (!cabeza) return nullptr;
    NodoDoble<T>* nodoEliminar = cabeza;
    T dato = nodoEliminar->dato;
    if (cabeza->siguiente == cabeza) {
        cabeza = nullptr;
    } else {
        NodoDoble<T>* anterior = cabeza->anterior;
        NodoDoble<T>* siguiente = cabeza->siguiente;
        anterior->siguiente = siguiente;
        siguiente->anterior = anterior;
        cabeza = siguiente;
    }
    delete nodoEliminar;
    return dato;
}

#endif