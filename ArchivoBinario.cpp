#include "ArchivoBinario.h"
#include <fstream>
#include <iostream>
using namespace std;

void ArchivoBinario::guardar(const ListaDobleCircular<Titular*>& titulares, const std::string& nombreArchivo) {
    ofstream archivo(nombreArchivo, ios::binary);
    if (!archivo) {
        cout << "\nNo se pudo abrir el archivo.\n" << endl;
        return;
    }

    NodoDoble<Titular*>* actual = titulares.getCabeza();
    if (actual) {
        do {
            Titular* t = actual->dato;
            Persona p = t->getPersona();
            archivo.write(reinterpret_cast<char*>(&p), sizeof(Persona));
            CuentaBancaria* c = t->getCuentaCorriente();
            bool tieneCorriente = c != nullptr;
            archivo.write(reinterpret_cast<char*>(&tieneCorriente), sizeof(bool));
            if (tieneCorriente) {
                archivo.write(reinterpret_cast<char*>(c), sizeof(CuentaBancaria));

                Pila<Movimiento> movs = c->getMovimientos();
                Pila<Movimiento> copia = movs;
                int count = 0;
                Pila<Movimiento> temp = movs;
                while (!temp.vacia()) {
                    temp.pop();
                    count++;
                }
                archivo.write(reinterpret_cast<char*>(&count), sizeof(int));
                while (!copia.vacia()) {
                    Movimiento m = copia.cima();
                    archivo.write(reinterpret_cast<char*>(&m), sizeof(Movimiento));
                    copia.pop();
                }
            }

            // Guardar cuentas de ahorro
            int totalAhorros = 0;
            NodoDoble<CuentaBancaria*>* nodoA = t->getCuentasAhorro().getCabeza();
            if (nodoA) {
                NodoDoble<CuentaBancaria*>* temp = nodoA;
                do {
                    totalAhorros++;
                    temp = temp->siguiente;
                } while (temp != nodoA);
            }
            archivo.write(reinterpret_cast<char*>(&totalAhorros), sizeof(int));

            if (nodoA) {
                NodoDoble<CuentaBancaria*>* temp = nodoA;
                do {
                    CuentaBancaria* ahorro = temp->dato;
                    archivo.write(reinterpret_cast<char*>(ahorro), sizeof(CuentaBancaria));

                    Pila<Movimiento> movs = ahorro->getMovimientos();
                    Pila<Movimiento> copia = movs;
                    int count = 0;
                    Pila<Movimiento> tmp = movs;
                    while (!tmp.vacia()) {
                        tmp.pop();
                        count++;
                    }
                    archivo.write(reinterpret_cast<char*>(&count), sizeof(int));
                    while (!copia.vacia()) {
                        Movimiento m = copia.cima();
                        archivo.write(reinterpret_cast<char*>(&m), sizeof(Movimiento));
                        copia.pop();
                    }
                    temp = temp->siguiente;
                } while (temp != nodoA);
            }

            actual = actual->siguiente;
        } while (actual != titulares.getCabeza());
    }
    archivo.close();
}

void ArchivoBinario::cargar(ListaDobleCircular<Titular*>& titulares, const std::string& nombreArchivo) {
    ifstream archivo(nombreArchivo, ios::binary);
    if (!archivo) {
        cout << "\nNo se pudo abrir el archivo para lectura.\n" << endl;
        return;
    }

    while (archivo.peek() != EOF) {
        // Leer Persona
        Persona p;
        archivo.read(reinterpret_cast<char*>(&p), sizeof(Persona));
        Titular* t = new Titular(p);

        // Leer cuenta corriente
        bool tieneCorriente = false;
        archivo.read(reinterpret_cast<char*>(&tieneCorriente), sizeof(bool));
        if (tieneCorriente) {
            CuentaBancaria* cc = new CuentaBancaria();
            archivo.read(reinterpret_cast<char*>(cc), sizeof(CuentaBancaria));
            // Leer movimientos de cuenta corriente
            int count = 0;
            archivo.read(reinterpret_cast<char*>(&count), sizeof(int));
            for (int i = 0; i < count; ++i) {
                Movimiento m;
                archivo.read(reinterpret_cast<char*>(&m), sizeof(Movimiento));
                cc->agregarMovimiento(m);
            }
            t->setCuentaCorriente(cc);
        }

        // Leer cuentas de ahorro
        int totalAhorros = 0;
        archivo.read(reinterpret_cast<char*>(&totalAhorros), sizeof(int));
        for (int i = 0; i < totalAhorros; ++i) {
            CuentaBancaria* ahorro = new CuentaBancaria();
            archivo.read(reinterpret_cast<char*>(ahorro), sizeof(CuentaBancaria));
            // Leer movimientos de cuenta de ahorro
            int count = 0;
            archivo.read(reinterpret_cast<char*>(&count), sizeof(int));
            for (int j = 0; j < count; ++j) {
                Movimiento m;
                archivo.read(reinterpret_cast<char*>(&m), sizeof(Movimiento));
                ahorro->agregarMovimiento(m);
            }
            t->agregarCuentaAhorro(ahorro);
        }

        titulares.insertar(t);
    }

    archivo.close();
}