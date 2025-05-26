#include <iostream>
#include "Titular.h"
#include "CuentaBancaria.h"
#include "Movimiento.h"

int main() {
    // Crear un titular con datos básicos
    Persona persona;
    persona.setNombre("Juan");
    persona.setApellido("Perez");
    persona.setCI("0102030405");
    persona.setTelefono("0991234567");

    Titular titular(persona);

    // Crear y asignar una cuenta corriente
    CuentaBancaria* cuentaCorr = new CuentaBancaria();
    cuentaCorr->setTipoCuenta("Corriente");
    cuentaCorr->setSaldo(1000.0f);

    // Agregar movimientos a la cuenta corriente
    Movimiento mov1(200.0f, true, 1);  // depósito
    Movimiento mov2(50.0f, false, 2);  // retiro
    cuentaCorr->agregarMovimiento(mov1);
    cuentaCorr->setSaldo(cuentaCorr->getSaldo() + mov1.getMonto());
    cuentaCorr->agregarMovimiento(mov2);
    cuentaCorr->setSaldo(cuentaCorr->getSaldo() - mov2.getMonto());

    titular.setCuentaCorriente(cuentaCorr);

    // Crear cuentas de ahorro y agregarlas
    for (int i = 0; i < 3; ++i) {
        CuentaBancaria* ahorro = new CuentaBancaria();
        ahorro->setTipoCuenta("Ahorro");
        ahorro->setSaldo(500.0f + i * 100);

        // Agregar un movimiento a cada cuenta de ahorro
        Movimiento movAhorro(100.0f + i * 50, true, 1);
        ahorro->agregarMovimiento(movAhorro);
        ahorro->setSaldo(ahorro->getSaldo() + movAhorro.getMonto());

        titular.agregarCuentaAhorro(ahorro);
    }

    // Mostrar información del titular
    std::cout << "Titular: " << persona.getNombre() << " " << persona.getApellido() << std::endl;
    std::cout << "\nCuenta corriente:" << std::endl;
    titular.getCuentaCorriente()->imprimir();

    std::cout << "\nCuentas de ahorro:" << std::endl;
    ListaDobleCircular<CuentaBancaria*> cuentas = titular.getCuentasAhorro();
    NodoDoble<CuentaBancaria*>* actual = cuentas.getCabeza();
    if (actual != nullptr) {
        do {
            actual->dato->imprimir();
            actual = actual->siguiente;
        } while (actual != cuentas.getCabeza());
    }

    return 0;
}
