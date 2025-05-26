#ifndef CUENTABANCARIA_H
#define CUENTABANCARIA_H

#include <string>
#include "Fecha.h"
#include "Pila.h"
#include "Movimiento.h"  // Clase individual de movimiento

class CuentaBancaria {
private:
    std::string ID;
    Fecha fechaCre;
    float saldo;
    std::string tipoCuenta;  // Ahorros o Corriente
    Pila<Movimiento> movimientos;

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

    void agregarMovimiento(const Movimiento& mov);
    Pila<Movimiento>& getMovimientos();  

    void generarID();
    void imprimirMovimientos();
};

#endif
