#ifndef SISTEMA_H
#define SISTEMA_H

#include "Titular.h"
#include "Validaciones.h"
#include "ListaDobleCircular.h"

class Sistema {
private:
    ListaDobleCircular<Titular*> titulares;
    Validaciones val;

public:
    Sistema();
    ~Sistema();

    void menuPrincipal();

    void registrarTitular();
    void crearCuenta();
    void realizarDeposito();
    void realizarRetiro();

    void buscarMovimientosPorFecha();
    void buscarPorTitular();
    void buscarPersonalizada();
    Titular* buscarTitularPorCI(const std::string& cedula);
    void crearBackup();
    void mostrarAyuda();
    void menuSecundario();
    void guardarArchivoBinCifrado();
    void guardarArchivoBinSinCifrar();
    void decifrarArchivoCifrado();
};

#endif