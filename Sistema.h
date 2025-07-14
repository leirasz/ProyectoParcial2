/**
 * @file Sistema.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-07-10
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef SISTEMA_H
#define SISTEMA_H

#include "Titular.h"
#include "Validaciones.h"
#include "ListaDobleCircular.h"
#include "ListaSucursales.h"
#include "BPlusTreeTitulares.h"
#include "TablaHash.h"
#include <vector>
#include <string>

class Sistema {
private:
    ListaDobleCircular<Titular*> titulares;
    Validaciones val;
    BPlusTreeTitulares arbolTitulares;
    ListaSucursales listaSucursales;
    TablaHash hashes;

    Titular* buscarTitularPorCI(const std::string& cedula);
    void actualizarContadoresSucursales();
    std::string generarHashMD5(const std::string& nombreArchivo);
    bool compararHashArchivo(const std::string& nombreArchivo);
    // Nuevas funciones para generar el código QR y el PDF
    void generarQR(const std::string& data, bool qrMatrix[21][21]);
    void generarPDF(const std::string& nombre, const std::string& numeroCuenta, bool qrMatrix[21][21], const std::string& outputFile);

public:
    Sistema();
    ~Sistema();

    void menuPrincipal();
    void registrarTitular();
    void crearCuenta();
    void realizarDeposito();
    void realizarRetiro();
    void buscarMovimientosPorFecha();
    void generarPDFTitulares();
    void buscarPorTitular();
    void buscarPersonalizada();
    void crearBackup();
    void mostrarAyuda();
    void menuSecundario();
    void menuArbol();
    void guardarArchivoBinCifrado();
    void guardarArchivoBinSinCifrar();
    void decifrarArchivoCifrado();
    void busquedasBinarias();
    void menuBB();
    void guardarTitularesEnTxt();
    // Nueva función para generar el QR en PDF
    void generarQRPDF();
};

#endif