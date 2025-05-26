#ifndef TITULAR_H
#define TITULAR_H

#include "Persona.h"
#include "CuentaBancaria.h"
#include "ListaDobleCircular.h"

class Titular {
private:
    Persona persona;
    CuentaBancaria* cuentaCorriente;
    ListaDobleCircular<CuentaBancaria*> cuentasAhorro;

public:
    Titular();
    Titular(const Persona& p);
    ~Titular();

    Persona getPersona() const;
    void setPersona(const Persona& p);

    CuentaBancaria* getCuentaCorriente() const;
    void setCuentaCorriente(CuentaBancaria* cuenta);

    void agregarCuentaAhorro(CuentaBancaria* cuenta);
    void mostrarCuentasAhorro() const;
    ListaDobleCircular<CuentaBancaria*>& getCuentasAhorro();
};
#endif