#include "Validaciones.h"
#include <iostream>
#include <conio.h>
#include <cstdlib>
#include <cctype> // para isdigit

using namespace std;

int Validaciones::ingresarEntero(char msj[50]) {
    char c;
    int i = 0;
    char dato[10];
    cout << msj << endl;

    while ((c = getch()) != 13) {
        if (c >= '0' && c <= '9') {
            if (i < 9) {
                dato[i++] = c;
                cout << c;
            }
        } else if (c == 8 && i > 0) {
            i--;
            cout << "\b \b";
        }
    }

    dato[i] = '\0';
    return atoi(dato);
}

float Validaciones::ingresarFlotante(char msj[50]) {
    char c;
    int i = 0;
    char dato[15];
    bool hayPunto = false;

    cout << msj << endl;

    while ((c = getch()) != 13) {
        if (c >= '0' && c <= '9') {
            if (i < 14) {
                dato[i++] = c;
                cout << c;
            }
        } else if (c == '.' && !hayPunto && i > 0) {
            dato[i++] = c;
            cout << c;
            hayPunto = true;
        } else if (c == 8 && i > 0) {
            if (dato[i - 1] == '.') hayPunto = false;
            i--;
            cout << "\b \b";
        }
    }

    dato[i] = '\0';
    return atof(dato);
}

string Validaciones::ingresarCadena(char msj[50]) {
    char c;
    int i = 0;
    char dato[50];

    cout << msj << endl;

    while ((c = getch()) != 13) {
        if (isalpha(c) || c == ' ') {
            if (i < 49) {
                dato[i++] = c;
                cout << c;
            }
        } else if (c == 8 && i > 0) {
            i--;
            cout << "\b \b";
        }
    }

    dato[i] = '\0';
    return string(dato);
}

char Validaciones::ingresarCaracter(char msj[50]) {
    char c;
    cout << msj << endl;

    while (true) {
        c = getch();
        if (isalpha(c)) {
            cout << c;
            return c;
        }
    }
}

string Validaciones::ingresarOperacion(char msj[50]) {
    char c;
    int i = 0;
    char dato[50];

    cout << msj << endl;

    while ((c = getch()) != 13) {
        if (isdigit(c) || c == '+' || c == '-' || c == '*' || c == '/' || c == '(' || c == ')') {
            if (i < 49) {
                dato[i++] = c;
                cout << c;
            }
        } else if (c == 8 && i > 0) {
            i--;
            cout << "\b \b";
        }
    }

    dato[i] = '\0';
    return string(dato);
}

string Validaciones::ingresarCedula(char msj[50]) {
    char c;
    int i = 0;
    char cedula[11];  // 10 digitos + '\0'

    do {
        system("cls");
        i = 0;
        cout << msj << endl;

        while ((c = getch()) != 13) {
            if (c >= '0' && c <= '9') {
                if (i < 10) {
                    cedula[i++] = c;
                    cout << c;
                }
            } else if (c == 8 && i > 0) {
                i--;
                cout << "\b \b";
            }
        }

        cedula[i] = '\0';

        if (!esCedulaValida(string(cedula))) {
            cout << "\nCedula no valida Intente de nuevo\n";
            system("pause");
        }

    } while (!esCedulaValida(string(cedula)));

    return string(cedula);
}

bool Validaciones::esCedulaValida(string cedula) {
    return validarLongitud(cedula) &&
           validarNumeros(cedula) &&
           validarDigitoVerificador(cedula);
}

bool Validaciones::validarLongitud(string cedula) {
    return cedula.length() == 10;
}

bool Validaciones::validarNumeros(string cedula) {
    for (char c : cedula) {
        if (!isdigit(c)) return false;
    }
    return true;
}

bool Validaciones::validarDigitoVerificador(string cedula) {
    int coeficientes[9] = {2, 1, 2, 1, 2, 1, 2, 1, 2};
    int suma = 0;

    for (int i = 0; i < 9; ++i) {
        int val = (cedula[i] - '0') * coeficientes[i];
        if (val > 9) val -= 9;
        suma += val;
    }

    int verificador = (10 - (suma % 10)) % 10;
    return verificador == (cedula[9] - '0');
}
