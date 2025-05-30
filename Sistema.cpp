#include "Sistema.h"
#include "Menu.h"
#include "ValidacionFecha.h"
#include <fstream>
#include <iostream>
using namespace std;

Sistema::Sistema() {}

Sistema::~Sistema() {
    NodoDoble<Titular*>* actual = titulares.getCabeza();
    if (actual != nullptr) {
        do {
            delete actual->dato;
            actual = actual->siguiente;
        } while (actual != titulares.getCabeza());
    }
}

void Sistema::menuPrincipal() {
    const char* opciones[] = {
        "Registrar titular",
        "Crear cuenta",
        "Realizar deposito",
        "Realizar retiro",
        "Guardar Archivo Binario",
        "Buscar movimientos por fecha",
        "Buscar titular por datos",
        "Busqueda personalizada",
        "Salir"
    };
    Menu menu;
    int opcion;
    do {
        opcion = menu.ingresarMenu("SISTEMA BANCARIO", opciones, 9);
        switch(opcion) {
            case 1: registrarTitular(); break;
            case 2: crearCuenta(); break;
            case 3: realizarDeposito(); break;
            case 4: realizarRetiro(); break;
            case 5: guardarArchivoBin(); break;
            case 6: buscarMovimientosPorFecha(); break;
            case 7: buscarPorTitular(); break;
            case 8: /*buscarPersonalizada();*/ break;
            case 9: cout << "\nSaliendo...\n" << endl; break;
            default: cout << "\nOpcion invalida." << endl; system("pause"); break;
        }
    } while(opcion != 9);
}

void Sistema::registrarTitular() {
    system("cls");
    cout << "--- REGISTRAR TITULAR --- \n" << endl;
    string nombre = val.ingresarCadena((char*)"Ingrese nombre:");
    string apellido = val.ingresarCadena((char*)"\nIngrese apellido:");
    for (char& c : nombre) c = toupper(c);
    for (char& c : apellido) c = toupper(c);
    string cedula = val.ingresarCedula((char*)"\nIngrese cedula:");
    if (buscarTitularPorCI(cedula) != nullptr) {
        cout << "\nYa existe un titular registrado con esa cedula.\n" << endl;
        system("pause");
        return;
    }
    string telefono = val.ingresarNumeroTelefonico((char*)"\nIngrese telefono:");
    string correo = val.ingresarCorreo((char*)"\nIngrese correo electronico:");

    // Ingreso y validación de fecha de nacimiento usando ValidacionFecha
    ValidacionFecha valFecha;
    string fechaStr;
    int dia, mes, anio;
    bool fechaValida = false;
    do {
        valFecha.ingresarFecha(fechaStr, dia, mes, anio);
        if (!valFecha.valoresValidos(dia, mes, anio)) {
            cout << "\nFecha invalida. Intente de nuevo." << endl;
            system("pause");
            continue;
        }
        if (!valFecha.esMayorDeEdad(dia, mes, anio)) {
            cout << "\nNo puede registrarse. Debe ser mayor de edad (18+)." << endl;
            system("pause");
            return;
        }
        fechaValida = true;
    } while (!fechaValida);

    Anio anioObj;
    anioObj.setAnio(anio);
    anioObj.setAnioBisiesto(valFecha.esBisiesto(anio));

    Fecha fechaNacimiento;
    fechaNacimiento.setDia(dia);
    fechaNacimiento.setMes(mes);
    fechaNacimiento.setAnio(anioObj);

    Persona persona;
    persona.setNombre(nombre);
    persona.setApellido(apellido);
    persona.setCI(cedula);
    persona.setTelefono(telefono);
    persona.setCorreo(correo);
    persona.setFechaNa(fechaNacimiento);

    Titular* nuevo = new Titular(persona);
    titulares.insertar(nuevo);

    cout << "\nTitular registrado exitosamente." << endl;
    system("pause");
}

Titular* Sistema::buscarTitularPorCI(const std::string& ci) {
    NodoDoble<Titular*>* actual = titulares.getCabeza();
    if (actual != nullptr) {
        do {
            if (actual->dato->getPersona().getCI() == ci)
                return actual->dato;
            actual = actual->siguiente;
        } while (actual != titulares.getCabeza());
    }
    return nullptr;
}

void Sistema::crearCuenta() {
    system("cls");
    cout << "--- CREAR CUENTA ---\n" << endl;
    string cedula = val.ingresarCedula((char*)"\nIngrese cedula del titular: ");
    Titular* titular = buscarTitularPorCI(cedula);
    if (!titular) {
        cout << "\nTitular no encontrado." << endl;
        system("pause");
        return;
    }

    string tipo = val.ingresarCadena((char*)"\nIngrese tipo de cuenta (Corriente/Ahorro): ");
    CuentaBancaria* nuevaCuenta = new CuentaBancaria();
    for (char& c : tipo) c = toupper(c);
    nuevaCuenta->setTipoCuenta(tipo);
    nuevaCuenta->generarID();

    //... (asignar fecha, saldo, etc)

    if (tipo == "CORRIENTE") {
        if (titular->getCuentaCorriente() != nullptr) {
            cout << "\nEste titular ya tiene una cuenta corriente.\n" << endl;
            delete nuevaCuenta;
        } else {
            titular->setCuentaCorriente(nuevaCuenta);
            cout << "\nCuenta corriente creada exitosamente.\n" << endl;
            cout << "--- DATOS DEL TITULAR ---" << endl;
            titular->getPersona().imprimir();
            cout << "--- DATOS DE LA CUENTA ---" << endl;
            nuevaCuenta->imprimir(); // <-- Aquí muestras la información
        }
    } else if (tipo == "AHORRO") {
        titular->agregarCuentaAhorro(nuevaCuenta);
        cout << "\nCuenta de ahorro creada exitosamente.\n" << endl;
        cout << "--- DATOS DEL TITULAR ---" << endl;
        titular->getPersona().imprimir();
        cout << "--- DATOS DE LA CUENTA ---" << endl;
        nuevaCuenta->imprimir();  // <-- Aquí muestras la información
    } else {
        cout << "\nTipo de cuenta no valido.\n" << endl;
        delete nuevaCuenta;
    }

    system("pause");
}

void Sistema::realizarDeposito() {
    if (titulares.vacia()) {
        cout << "\nNo hay titulares registrados.\n" << endl;
        system("pause");
        return;
    }
    system("cls");
    cout << "--- REALIZAR DEPOSITO ---" << endl;
    string cedula = val.ingresarCedula((char*)"\nIngrese cedula del titular:");
    Titular* titular = buscarTitularPorCI(cedula);

    if (!titular) {
        cout << "\nTitular no encontrado." << endl;
        system("pause");
        return;
    }
    if (!titular->getCuentaCorriente() && titular->getCuentasAhorro().vacia()) {
        cout << "\nEl titular no tiene cuentas registradas.\n" << endl;
        system("pause");
        return;
    }
    string tipo = val.ingresarCadena((char*)"\nTipo de cuenta (Corriente/Ahorro):");
    for (char& c : tipo) c = toupper(c);
    CuentaBancaria* cuenta = nullptr;

    string idCuenta = val.ingresarNumeros((char*)"\nIngrese ID de la cuenta:");
    
    if (tipo == "CORRIENTE") {
        cuenta = titular->getCuentaCorriente();
        if (!cuenta || cuenta->getID() != idCuenta) {
            cout << "\nCuenta corriente no encontrada o ID incorrecto.\n" << endl;
            system("pause");
            return;
        }
    } else if (tipo == "AHORRO") {
        NodoDoble<CuentaBancaria*>* actual = titular->getCuentasAhorro().getCabeza();
        if (actual != nullptr) {
            do {
                if (actual->dato->getID() == idCuenta) {
                    cuenta = actual->dato;
                    break;
                }
                actual = actual->siguiente;
            } while (actual != titular->getCuentasAhorro().getCabeza());
        }
        if (!cuenta) {
            cout << "\nCuenta de ahorro no encontrada.\n" << endl;
            system("pause");
            return;
        }
    } else {
        cout << "\nTipo de cuenta no valido.\n" << endl;
        system("pause");
        return;
    }

    float monto = val.ingresarFlotante((char*)"\nIngrese monto a depositar:\n");
    Movimiento mov(monto, true, cuenta->getMovimientos().vacia() ? 1 : cuenta->getMovimientos().cima().getNumeroMovimiento() + 1);
    cuenta->agregarMovimiento(mov);
    cuenta->setSaldo(cuenta->getSaldo() + monto);

    cout << "\nDeposito realizado exitosamente.\n" << endl;
    system("pause");
}

void Sistema::realizarRetiro() {
    if (titulares.vacia()) {
        cout << "\nNo hay titulares registrados.\n" << endl;
        system("pause");
        return;
    }
    system("cls");
    cout << "--- REALIZAR RETIRO ---" << endl;
    string cedula = val.ingresarCedula((char*)"\nIngrese cedula del titular:\n");
    Titular* titular = buscarTitularPorCI(cedula);

    if (!titular) {
        cout << "\nTitular no encontrado.\n" << endl;
        system("pause");
        return;
    }
    if (!titular->getCuentaCorriente() && titular->getCuentasAhorro().vacia()) {
        cout << "\nEl titular no tiene cuentas registradas.\n" << endl;
        system("pause");
        return;
    }
    string tipo = val.ingresarCadena((char*)"\nTipo de cuenta (Corriente/Ahorro):\n");
    for (char& c : tipo) c = toupper(c);
    CuentaBancaria* cuenta = nullptr;

    string idCuenta = val.ingresarNumeros((char*)"\nIngrese ID de la cuenta:");
    

    if (tipo == "CORRIENTE") {
        cuenta = titular->getCuentaCorriente();
        if (!cuenta || cuenta->getID() != idCuenta) {
            cout << "\nCuenta corriente no encontrada o ID incorrecto.\n" << endl;
            system("pause");
            return;
        }
    } else if (tipo == "AHORRO") {
        NodoDoble<CuentaBancaria*>* actual = titular->getCuentasAhorro().getCabeza();
        if (actual != nullptr) {
            do {
                if (actual->dato->getID() == idCuenta) {
                    cuenta = actual->dato;
                    break;
                }
                actual = actual->siguiente;
            } while (actual != titular->getCuentasAhorro().getCabeza());
        }
        if (!cuenta) {
            cout << "\nCuenta de ahorro no encontrada.\n" << endl;
            system("pause");
            return;
        }
    } else {
        cout << "\nTipo de cuenta no valido.\n" << endl;
        system("pause");
        return;
    }

    float monto = val.ingresarFlotante((char*)"\nIngrese monto a retirar:");
    if (monto > cuenta->getSaldo()) {
        cout << "\nSaldo insuficiente." << endl;
        system("pause");
        return;
    }

    Movimiento mov(monto, false, cuenta->getMovimientos().vacia() ? 1 : cuenta->getMovimientos().cima().getNumeroMovimiento() + 1);
    cuenta->agregarMovimiento(mov);
    cuenta->setSaldo(cuenta->getSaldo() - monto);

    cout << "\nRetiro realizado exitosamente." << endl;
    system("pause");
}

void Sistema::guardarArchivoBin() {
    system("cls");
    cout << "--- GUARDAR CUENTAS EN ARCHIVO BINARIO ---" << endl;
    if (titulares.vacia()) {
        cout << "\nNo hay titulares registrados para guardar.\n" << endl;
        system("pause");
        return;
    }
    ofstream archivo("cuentas.bin", ios::binary);
    if (!archivo) {
        cout << "\nNo se pudo abrir el archivo.\n" << endl;
        system("pause");
        return;
    }

    NodoDoble<Titular*>* actual = titulares.getCabeza();
    if (actual) {
        do {
            Titular* t = actual->dato;
            Persona p = t->getPersona();
            archivo.write(reinterpret_cast<char*>(&p), sizeof(Persona));

            CuentaBancaria* c = t->getCuentaCorriente();
            bool tieneCorriente = c != nullptr;
            archivo.write(reinterpret_cast<char*>(&tieneCorriente), sizeof(bool));
            if (tieneCorriente) {
                archivo.write(reinterpret_cast<char*>(c), sizeof(CuentaBancaria));

                Pila<Movimiento> movs = c->getMovimientos();
                Pila<Movimiento> copia = movs;
                int count = 0;
                Pila<Movimiento> temp = movs;
                while (!temp.vacia()) {
                    temp.pop();
                    count++;
                }
                archivo.write(reinterpret_cast<char*>(&count), sizeof(int));
                while (!copia.vacia()) {
                    Movimiento m = copia.cima();
                    archivo.write(reinterpret_cast<char*>(&m), sizeof(Movimiento));
                    copia.pop();
                }
            }

            // Guardar cuentas de ahorro
            int totalAhorros = 0;
            NodoDoble<CuentaBancaria*>* nodoA = t->getCuentasAhorro().getCabeza();
            if (nodoA) {
                NodoDoble<CuentaBancaria*>* temp = nodoA;
                do {
                    totalAhorros++;
                    temp = temp->siguiente;
                } while (temp != nodoA);
            }
            archivo.write(reinterpret_cast<char*>(&totalAhorros), sizeof(int));

            if (nodoA) {
                NodoDoble<CuentaBancaria*>* temp = nodoA;
                do {
                    CuentaBancaria* ahorro = temp->dato;
                    archivo.write(reinterpret_cast<char*>(ahorro), sizeof(CuentaBancaria));

                    Pila<Movimiento> movs = ahorro->getMovimientos();
                    Pila<Movimiento> copia = movs;
                    int count = 0;
                    Pila<Movimiento> tmp = movs;
                    while (!tmp.vacia()) {
                        tmp.pop();
                        count++;
                    }
                    archivo.write(reinterpret_cast<char*>(&count), sizeof(int));
                    while (!copia.vacia()) {
                        Movimiento m = copia.cima();
                        archivo.write(reinterpret_cast<char*>(&m), sizeof(Movimiento));
                        copia.pop();
                    }
                    temp = temp->siguiente;
                } while (temp != nodoA);
            }

            actual = actual->siguiente;
        } while (actual != titulares.getCabeza());
    }
    archivo.close();
    cout << "\nCuentas guardadas en 'cuentas.bin'.\n" << endl;
    system("pause");
}

void Sistema::buscarMovimientosPorFecha() {
    if (titulares.vacia()) {
        cout << "\nNo hay titulares registrados.\n" << endl;
        system("pause");
        return;
    }
    system("cls");
    cout << "--- BUSCAR MOVIMIENTOS POR FECHA ---" << endl;
    string cedula = val.ingresarCedula((char*)"\nIngrese cedula del titular:");
    Titular* titular = buscarTitularPorCI(cedula);
    if (!titular) {
        cout << "\nTitular no encontrado.\n" << endl;
        system("pause");
        return;
    }

    // Ingreso y validación de fecha usando ValidacionFecha
    ValidacionFecha valFecha;
    string fechaStr;
    int dia, mes, anio;
    valFecha.ingresarFecha(fechaStr, dia, mes, anio);

    auto filtrarMovimientos = [=](CuentaBancaria* cuenta) {
        Pila<Movimiento> movimientos = cuenta->getMovimientos();
        Pila<Movimiento> copia = movimientos;
        while (!copia.vacia()) {
            Movimiento m = copia.cima();
            Fecha f = m.getFechaMov();
            if (f.getDia() == dia && f.getMes() == mes && f.getAnio().getAnio() == anio) {
                cout << "Cuenta ID: " << cuenta->getID() << endl;
                m.imprimir();
            }
            copia.pop();
        }
    };

    if (titular->getCuentaCorriente()) {
        filtrarMovimientos(titular->getCuentaCorriente());
    }

    NodoDoble<CuentaBancaria*>* actual = titular->getCuentasAhorro().getCabeza();
    if (actual) {
        do {
            filtrarMovimientos(actual->dato);
            actual = actual->siguiente;
        } while (actual != titular->getCuentasAhorro().getCabeza());
    }
    system("pause");
}

void Sistema::buscarPorTitular() {
    system("cls");
    cout << "--- BUSQUEDA PERSONALIZADA ---" << endl;
    string criterioOriginal = val.ingresarCadena((char*)"Ingrese cualquier dato a buscar (nombre o apellido):");

    // Función lambda para convertir una cadena a minúsculas sin usar <algorithm>
    auto toLower = [](std::string s) -> std::string {
        for (size_t i = 0; i < s.size(); ++i) {
            if (s[i] >= 'A' && s[i] <= 'Z') {
                s[i] += 32;
            }
        }
        return s;
    };

    string criterio = toLower(criterioOriginal);

    // Lambda para verificar si un campo contiene el criterio
    auto coincide = [&](const Persona& p) {
        string ci = toLower(p.getCI());
        string nombre = toLower(p.getNombre());
        string apellido = toLower(p.getApellido());
        return ci.find(criterio) != string::npos ||
               nombre.find(criterio) != string::npos ||
               apellido.find(criterio) != string::npos;
    };

    NodoDoble<Titular*>* actual = titulares.getCabeza();
    bool encontrado = false;
    if (actual) {
        do {
            Persona p = actual->dato->getPersona();
            if (coincide(p)) {
                cout << "CI: " << p.getCI() << " - Nombre: " << p.getNombre() << " " << p.getApellido() << " - Tel: " << p.getTelefono() << endl;
                encontrado = true;
            }
            actual = actual->siguiente;
        } while (actual != titulares.getCabeza());
    }

    if (!encontrado) {
        cout << "\nNo se encontraron coincidencias.\n" << endl;
    }
    system("pause");
}

