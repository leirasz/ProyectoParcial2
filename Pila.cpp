#include <iostream>
#include "Pila.h"

template <typename T>
Pila<T>::Pila() {
    tope = nullptr;
}

template <typename T>
Pila<T>::~Pila() {    
    while(tope != nullptr) {
        NodoS<T>* temp = tope;
        tope = tope->siguiente;
        delete temp;
    }   
}

template <typename T>
void Pila<T>::push(T movimiento) {
    NodoS<T>* nuevo = new NodoS<T>(movimiento);
    nuevo->siguiente = tope;
    tope = nuevo;
}

template <typename T>
void Pila<T>::imprimir() {
    NodoS<T>* actual = tope;
    while (actual != nullptr) {
        std::cout << actual->dato << " ";
        actual = actual->siguiente;
    }
    std::cout << std::endl;
}

template <typename T>
bool Pila<T>::vacia() const {
    return tope == nullptr;
}

template <typename T>
T Pila<T>::cima() const {
    if (tope != nullptr) {
        return tope->dato;
    }
    throw std::runtime_error("La pila está vacía.");
}

template <typename T>
void Pila<T>::pop() {
    if (tope != nullptr) {
        NodoS<T>* temp = tope;
        tope = tope->siguiente;
        delete temp;
    }
}