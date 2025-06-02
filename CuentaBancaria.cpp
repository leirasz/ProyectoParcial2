#include "CuentaBancaria.h"
#include "Pila.h"          // ← NECESARIO
#include "Movimiento.h" 
#include <cstdlib>

CuentaBancaria::CuentaBancaria() : saldo(0.0f) {
    generarID();
}

CuentaBancaria::~CuentaBancaria() {}

std::string CuentaBancaria::getID() const {
    return ID;
}

void CuentaBancaria::setID(const std::string& newID) {
    ID = newID;
}

Fecha CuentaBancaria::getFechaCre() const {
    return fechaCre;
}

void CuentaBancaria::setFechaCre(const Fecha& newFechaCre) {
    fechaCre = newFechaCre;
}

float CuentaBancaria::getSaldo() const {
    return saldo;
}

void CuentaBancaria::setSaldo(float newSaldo) {
    saldo = newSaldo;
}

std::string CuentaBancaria::getTipoCuenta() const {
    return tipoCuenta;
}

void CuentaBancaria::setTipoCuenta(const std::string& newTipoCuenta) {
    tipoCuenta = newTipoCuenta;
}

void CuentaBancaria::agregarMovimiento(Movimiento* mov) {
    if (mov == nullptr) return;

    if (mov->getTipo()) {
        // Depósito
        saldo += mov->getMonto();
    } else {
        // Retiro
        if (mov->getMonto() <= saldo) {
            saldo -= mov->getMonto();
        } else {
            std::cout << "Fondos insuficientes para realizar el retiro.\n";
            return;
        }
    }

    movimientos.insertar(mov);
}

ListaDobleCircular<Movimiento*>& CuentaBancaria::getMovimientos() {
    return movimientos;
}
const ListaDobleCircular<Movimiento*>& CuentaBancaria::getMovimientos() const {
    return movimientos;
}

void CuentaBancaria::generarID() {
    ID = "";
    for (int i = 0; i < 10; ++i) {
        ID += std::to_string(rand() % 10);
    }
}

void CuentaBancaria::imprimirMovimientos() {
    if (movimientos.vacia()) {
        std::cout << "  (Sin movimientos)\n";
        return;
    }
    NodoDoble<Movimiento*>* actual = movimientos.getCabeza();
    if (actual) {
        do {
            Movimiento* m = actual->dato;
            if (m) m->imprimir();
            actual = actual->siguiente;
        } while (actual != movimientos.getCabeza());
    }
}
void CuentaBancaria::imprimir() {
    std::cout << "Tipo de cuenta: " << tipoCuenta << std::endl;
    std::cout << "ID: " << ID << std::endl;
    std::cout << "Saldo: $" << saldo << std::endl;
    std::cout << "Movimientos:" << std::endl;
    imprimirMovimientos();
    std::cout << "---------------------------" << std::endl;
}