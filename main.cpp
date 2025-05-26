#include <iostream>
#include <ctime>
#include "CuentaBancaria.h"
#include "Movimiento.h"

int main() {
    srand(time(0));  // Inicializar aleatoriedad para IDs

    // Crear cuenta bancaria
    CuentaBancaria cuenta;
    cuenta.setTipoCuenta("Ahorros");
    cuenta.setSaldo(0.0f);

    std::cout << "Cuenta creada con ID: " << cuenta.getID() << "\n";

    // Crear y agregar un depósito
    Movimiento deposito(150.0f, true, 1);
    cuenta.agregarMovimiento(deposito);
    cuenta.setSaldo(cuenta.getSaldo() + deposito.getMonto());

    // Crear y agregar un retiro
    Movimiento retiro(50.0f, false, 2);
    cuenta.agregarMovimiento(retiro);
    cuenta.setSaldo(cuenta.getSaldo() - retiro.getMonto());

    // Mostrar saldo actual
    std::cout << "\nSaldo actual: $" << cuenta.getSaldo() << "\n";

    // Mostrar movimientos
    std::cout << "\nHistorial de movimientos:\n";
    cuenta.imprimirMovimientos();

    return 0;
}
