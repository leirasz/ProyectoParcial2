#ifndef NODOS_H
#define NODOS_H

template <typename T>
class NodoS{
    public:
        T dato;
        NodoS<T>* siguiente;

        Nodo(T valor){
            dato = valor;
            siguiente = nullptr;
        }
};
# endif
