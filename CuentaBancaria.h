#ifndef CUENTABANCARIA_H
#define CUENTABANCARIA_H

#include <string>
#include "Fecha.h"
#include "Movimiento.h"
#include "ListaDobleCircular.h"
  // Clase individual de movimiento

class CuentaBancaria {
private:
    std::string ID;
    Fecha fechaCre;
    float saldo;
    std::string tipoCuenta;  // Ahorros o Corriente
    ListaDobleCircular<Movimiento*> movimientos;

public:
    CuentaBancaria();
    ~CuentaBancaria();

    // Getters y setters
    std::string getID() const;
    void setID(const std::string& newID);

    Fecha getFechaCre() const;
    void setFechaCre(const Fecha& newFechaCre);

    float getSaldo() const;
    void setSaldo(float newSaldo);

    std::string getTipoCuenta() const;
    void setTipoCuenta(const std::string& newTipoCuenta);

    void agregarMovimiento(Movimiento* mov);
    ListaDobleCircular<Movimiento*>& getMovimientos();
    const ListaDobleCircular<Movimiento*>& getMovimientos() const;

    void generarID();
    void imprimirMovimientos();
    void imprimir();
};

#endif
