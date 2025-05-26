#include <iostream>
#include "Pila.h"

template <typename T>
Pila<T>::Pila() {
    tope = nullptr;
}

template <typename T>
Pila<T>::~Pila() {    
    Nwhile(tope != nullptr) {
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