#ifndef PILA_H
#define PILA_H
#include "NodoS.h"

template <typename T>
class Pila{
    public  :
        void push(T movimiento);
        void imprimir();
        bool vacia() const;
        T cima() const;
        void pop();
        Pila();
        ~ Pila();
    private :
    NodoS<T>* tope;
};
#endif