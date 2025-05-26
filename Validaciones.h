#ifndef VALIDACIONES_H
#define VALIDACIONES_H

#include <string>
using namespace std;

class Validaciones {
public:
    int ingresarEntero(char msj[50]);
    float ingresarFlotante(char msj[50]);
    string ingresarCadena(char msj[50]);
    char ingresarCaracter(char msj[50]);
    string ingresarOperacion(char msj[50]);
    string ingresarCedula(char msj[50]);

private:
    bool esCedulaValida(string cedula);
    bool validarLongitud(string cedula);
    bool validarNumeros(string cedula); // usa isdigit
    bool validarDigitoVerificador(string cedula);
};

#endif
