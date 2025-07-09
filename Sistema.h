#ifndef SISTEMA_H
#define SISTEMA_H

#include "Titular.h"
#include "Validaciones.h"
#include "ListaDobleCircular.h"
#include "ListaSucursales.h"
#include "BPlusTreeTitulares.h"
#include "TablaHash.h"
//#include <SFML/Graphics.hpp>

class Sistema {
private:
    ListaDobleCircular<Titular*> titulares;
    Validaciones val;
    BPlusTreeTitulares arbolTitulares;
    ListaSucursales listaSucursales;
    TablaHash hashes;
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
    void menuArbol();
    void guardarArchivoBinCifrado();
    void guardarArchivoBinSinCifrar();
    void decifrarArchivoCifrado();
    void actualizarContadoresSucursales();
    //void graficarArbol();
    void busquedasBinarias();
    void menuBB();
    void guardarTitularesEnTxt();
    std::string generarHashMD5(const std::string& nombreArchivo);
    bool compararHashArchivo(const std::string& nombreArchivo);
};

#endif