#ifndef VALIDACIONES_H
#define VALIDACIONES_H

#include <string>
using namespace std;

class Validaciones {
public:
    int ingresarEntero(const char* msj);
    float ingresarFlotante(const char* msj);
    string ingresarCadena(const char* msj);
    char ingresarCaracter(const char* msj);
    string ingresarOperacion(const char* msj);
    string ingresarCedula(const char* msj);

private:
    bool esCedulaValida(string cedula);
    bool validarLongitud(string cedula);
    bool validarNumeros(string cedula);
    bool validarRegion(string cedula);
    bool validarDigitoVerificador(string cedula);
};

#endif
