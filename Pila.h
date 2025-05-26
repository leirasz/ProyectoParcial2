#ifndef PILA_H
#define PILA_H
#include "NodoS.h"

template <typename T>
class Pila{
    public  :
        Pila();
        ~ Pila();
        void push(T movimiento);
        void imprimir();
        bool vacia() const;
        T cima() const;
        void pop();
        
    private :
    NodoS<T>* tope;
};
#endif