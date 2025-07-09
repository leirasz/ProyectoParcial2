#ifndef BPLUSTREETITULARES_H
#define BPLUSTREETITULARES_H

#include "NodoBPlus.h"
#include "Titular.h"

class BPlusTreeTitulares {
    NodoBPlus* raiz;
    int grado; // Grado del árbol (máximo de claves por nodo)
public:
    BPlusTreeTitulares(int grado = 3);
    ~BPlusTreeTitulares();

    void insertar(const std::string& ci, Titular* titular);
    Titular* buscar(const std::string& ci) const;
    void imprimir() const;
    void construirDesdeLista(NodoDoble<Titular*>* cabeza);

private:
    void insertarEnNodo(NodoBPlus* nodo, const std::string& ci, Titular* titular, NodoBPlus*& nuevoHijo, std::string& nuevaClave);
    Titular* buscarEnNodo(NodoBPlus* nodo, const std::string& ci) const;
    void imprimirNodo(NodoBPlus* nodo, int nivel,bool esRaiz = false) const;
    void liberarNodo(NodoBPlus* nodo);
};

#endif