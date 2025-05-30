#include "Validaciones.h"
#include <iostream>
#include <conio.h>
#include <cstdlib>
#include <cstring>
#include <cctype> // para isdigit

using namespace std;

int Validaciones::ingresarEntero(char msj[50]) {
    char c;
    int i = 0;
    char dato[11];
    cout << msj << endl;

    while ((c = getch()) != 13) {
        if (c >= '0' && c <= '9') {
            if (i < 10) {
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
    
    do {
        i = 0;
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
        
    } while (i == 0);
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
string Validaciones::ingresarNumeroTelefonico(char msj[50]) {
    char c;
    int i = 0;
    char numero[11]; // 10 digitos + '\0'

    do {
        system("cls");
        i = 0;
        cout << msj << endl;

        while ((c = getch()) != 13) {
            if (c >= '0' && c <= '9') {
                if (i < 10) {
                    numero[i++] = c;
                    cout << c;
                }
            } else if (c == 8 && i > 0) {
                i--;
                cout << "\b \b";
            }
        }

        numero[i] = '\0';

        if (i != 10 || numero[0] != '0'|| numero[1] != '9') {
            cout << "\nNumero invalido Debe tener 10 digitos y comenzar con 09\n";
            system("pause");
        }

    } while (i != 10 || numero[0] != '0'|| numero[1] != '9');

    return string(numero);
}
float ingresarMonto(const char* mensaje) {
    char cad[20];
    char c;
    int i = 0;
    bool tienePunto = false;
    int decimales = 0;

    printf("%s", mensaje);

    while ((c = _getch()) != 13) { // Enter
        if (c >= '0' && c <= '9') {
            if (tienePunto) {
                if (decimales < 2) {
                    printf("%c", c);
                    cad[i++] = c;
                    decimales++;
                }
            } else {
                printf("%c", c);
                cad[i++] = c;
            }
        }
        else if (c == '.' && !tienePunto && i > 0) {
            printf("%c", c);
            cad[i++] = c;
            tienePunto = true;
            decimales = 0;
        }
        else if (c == 8 && i > 0) { // Backspace
            if (cad[i - 1] == '.') {
                tienePunto = false;
            } else if (tienePunto && decimales > 0) {
                decimales--;
            }
            printf("\b \b");
            i--;
        }
    }

    cad[i] = '\0';

    if (strlen(cad) == 0 || cad[0] == '.') {
        printf("\nEntrada invalida.\n");
        return ingresarMonto(mensaje); // Reintenta
    }

    return atof(cad);
}
string Validaciones::ingresarNumeros(char msj[50]) {
    char c;
    int i = 0;
    char dato[11]; // Ajusta el tamaño según la longitud máxima de tu ID

    cout << msj << endl;

    while ((c = getch()) != 13) { // Enter
        if (c >= '0' && c <= '9') {
            if (i < 19) {
                dato[i++] = c;
                cout << c;
            }
        } else if (c == 8 && i > 0) { // Backspace
            i--;
            cout << "\b \b";
        }
    }

    dato[i] = '\0';
    return string(dato);
}

string Validaciones::ingresarCorreo(char msj[50]) {
    char c;
    int i = 0;
    char correo[51]; // Máximo 50 caracteres + '\0'

    do {
        system("cls");
        i = 0;
        cout << msj << endl;

        while ((c = getch()) != 13) {
            if (isalnum(c) || c == '@' || c == '.' || c == '-' || c == '_') {
                if (i < 50) {
                    correo[i++] = c;
                    cout << c;
                }
            } else if (c == 8 && i > 0) {
                i--;
                cout << "\b \b";
            }
        }
        correo[i] = '\0';

        // Validación básica de formato
        string correoStr = string(correo);
        size_t arroba = correoStr.find('@');
        size_t punto = correoStr.find('.', arroba);
        if (arroba == string::npos || punto == string::npos || arroba == 0 || punto == correoStr.length() - 1 || punto < arroba + 2) {
            cout << "\nCorreo invalido. Debe tener formato usuario@dominio.ext\n";
            system("pause");
        } else {
            return correoStr;
        }
    } while (true);
}

string Validaciones::ingresarTextoLibre(char msj[50]) {
    char c;
    int i = 0;
    char dato[50];
    cout << msj << endl;
    do {
        i = 0;
        while ((c = getch()) != 13) { // Enter
            if (isalnum(c) || c == ' ') { // Permite letras y números
                if (i < 49) {
                    dato[i++] = c;
                    cout << c;
                }
            } else if (c == 8 && i > 0) { // Backspace
                i--;
                cout << "\b \b";
            }
        }
        dato[i] = '\0';
    } while (i == 0);
    return string(dato);
}
