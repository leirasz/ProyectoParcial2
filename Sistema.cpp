#include "Sistema.h"
#include "Menu.h"
#include "ValidacionFecha.h"
#include "FechaHora.h"
#include "ArchivoBinario.h"
#include "CifradoCesar.h"
#include <ctime>

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
        "Buscar movimientos por fecha",
        "Buscar titular por datos",
        "Busqueda personalizada",
        "Guardar Archivo Binario",
        "Mostrar ayuda",
        "Salir"
    };
    Menu menu;
    int opcion;
    do {
        opcion = menu.ingresarMenu("SISTEMA BANCARIO", opciones, 10);
        switch(opcion) {
            case 1: registrarTitular(); break;
            case 2: crearCuenta(); break;
            case 3: realizarDeposito(); break;
            case 4: realizarRetiro(); break;
            case 5: buscarMovimientosPorFecha(); break;
            case 6: buscarPorTitular(); break;
            case 7: buscarPersonalizada(); break;
            case 8: menuSecundario(); break;
            case 9: mostrarAyuda(); break;
            case 10: cout << "\nSaliendo...\n" << endl; break;
            default: cout << "\nOpcion invalida." << endl; system("pause"); break;
        }
    } while(opcion != 10);
}
void Sistema::menuSecundario(){
    const char* opciones[] = {
        "Guardar archivo sin cifrar",
        "Guardar archivo cifrado",
        "Decifrar archivo cifrado",
        "Regresar al menu principal"
    };
    Menu menu;
    int opcion;
    do {
        opcion = menu.ingresarMenu("Archivos Binarios", opciones, 4);
        switch(opcion) {
            case 1: guardarArchivoBinSinCifrar(); break;
            case 2: guardarArchivoBinCifrado(); break;
            case 3: decifrarArchivoCifrado(); break;
            case 4: cout << "\nRegresando al menu principal...\n" << endl; break;
            default: cout << "\nOpcion invalida." << endl; system("pause"); break;
        }
    } while(opcion != 4);

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
    crearBackup();
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
    crearBackup();
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
        if (actual) {
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

    float monto = val.ingresarMonto((char*)"\nIngrese monto a depositar:\n");

    int numMov = 1;
    if (!cuenta->getMovimientos().vacia()) {
        numMov = cuenta->getMovimientos().getCabeza()->anterior->dato->getNumeroMovimiento() + 1;
    }

    Movimiento* mov = new Movimiento(monto, true, numMov);
    cuenta->agregarMovimiento(mov);  // Ya actualiza el saldo

    cout << "\nDeposito realizado exitosamente.\n" << endl;
    system("pause");
    crearBackup();
}


void Sistema::realizarRetiro() {
    if (titulares.vacia()) {
        cout << "\nNo hay titulares registrados.\n" << endl;
        system("pause");
        return;
    }
    system("cls");
    cout << "--- REALIZAR RETIRO ---" << endl;

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
        if (actual) {
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

    float monto = val.ingresarMonto((char*)"\nIngrese monto a retirar:\n");

    if (monto > cuenta->getSaldo()) {
        cout << "\nFondos insuficientes para realizar el retiro.\n" << endl;
        system("pause");
        return;
    }

    int numMov = 1;
    if (!cuenta->getMovimientos().vacia()) {
        numMov = cuenta->getMovimientos().getCabeza()->anterior->dato->getNumeroMovimiento() + 1;
    }

    Movimiento* mov = new Movimiento(monto, false, numMov);
    cuenta->agregarMovimiento(mov);  // Ya valida el saldo y actualiza

    cout << "\nRetiro realizado exitosamente.\n" << endl;
    system("pause");
    crearBackup();
}

void Sistema::guardarArchivoBinCifrado() {
    system("cls");
    cout << "--- GUARDAR CUENTAS EN ARCHIVO BINARIO ---" << endl;
    if (titulares.vacia()) {
        cout << "\nNo hay titulares registrados para guardar.\n" << endl;
        system("pause");
        return;
    }
    ArchivoBinario::guardar(titulares, "cuentas_temp.bin");

    int desplazamiento = 3; // Puedes elegir el desplazamiento que desees
    
    cifrarCesarArchivoBinario(std::string("cuentas_temp.bin"), std::string("cuentasCifrado.bin"), desplazamiento);

    remove("cuentas_temp.bin");

    cout << "\nCuentas guardadas y cifradas en 'cuentasCifrado.bin'.\n" << endl;
    system("pause");
}
void Sistema::guardarArchivoBinSinCifrar() {
    system("cls");
    cout << "--- GUARDAR CUENTAS EN ARCHIVO BINARIO ---" << endl;
    if (titulares.vacia()) {
        cout << "\nNo hay titulares registrados para guardar.\n" << endl;
        system("pause");
        return;
    }
    ArchivoBinario::guardar(titulares, "cuentasSinCifrar.bin");

    cout << "\nCuentas guardadas 'cuentasSinCifrar.bin'.\n" << endl;
    system("pause");
}

void Sistema::decifrarArchivoCifrado() {
    system("cls");
    cout << "--- DECIFRAR ARCHIVO CIFRADO ---" << endl;
    ifstream archivo("cuentasCifrado.bin", ios::binary);
    if (!archivo) {
        cout << "\nNo se pudo abrir el archivo cifrado.\n" << endl;
        system("pause");
        return;
    }

    int desplazamiento = 3; 
    cifrarCesarArchivoBinario(std::string("cuentasCifrado.bin"), std::string("cuentasDecifradas.bin"), -desplazamiento);
    ArchivoBinario::guardar(titulares, "cuentasDecifradas.bin");
    cout << "\nArchivo decifrado y cargado exitosamente.\n" << endl;
    system("pause");
}

void Sistema::buscarMovimientosPorFecha() {
    if (titulares.vacia()) {
        cout << "\nNo hay titulares registrados.\n" << endl;
        system("pause");
        return;
    }
    system("cls");
    cout << "--- BUSCAR MOVIMIENTOS POR RANGO DE FECHAS ---" << endl;

    // Ingreso y validación de fechas de inicio y fin
    ValidacionFecha valFecha;
    string fechaStr1, fechaStr2;
    int d1, m1, a1, d2, m2, a2;
    cout << "\nFecha de INICIO:" << endl;
    valFecha.ingresarFecha(fechaStr1, d1, m1, a1);
    cout << "\nFecha de FIN:" << endl;
    valFecha.ingresarFecha(fechaStr2, d2, m2, a2);

    // Proceso para asegurar que la fecha menor sea la inicial
    int iniDia, iniMes, iniAnio, finDia, finMes, finAnio;
    if (valFecha.compararFechas(d1, m1, a1, d2, m2, a2) <= 0) {
        iniDia = d1; iniMes = m1; iniAnio = a1;
        finDia = d2; finMes = m2; finAnio = a2;
    } else {
        iniDia = d2; iniMes = m2; iniAnio = a2;
        finDia = d1; finMes = m1; finAnio = a1;
    }

    // Función para saber si una fecha está en el rango
    auto fechaEnRango = [&](const Fecha& f) {
        int cmpIni = valFecha.compararFechas(iniDia, iniMes, iniAnio, f.getDia(), f.getMes(), f.getAnio().getAnio());
        int cmpFin = valFecha.compararFechas(f.getDia(), f.getMes(), f.getAnio().getAnio(), finDia, finMes, finAnio);
        return cmpIni <= 0 && cmpFin <= 0;
    };

    NodoDoble<Titular*>* actual = titulares.getCabeza();
    bool encontrado = false;
    if (actual) {
        do {
            Titular* titular = actual->dato;
            Persona p = titular->getPersona();
            string nombreTitular = p.getNombre() + " " + p.getApellido();

            // Cuenta corriente
            CuentaBancaria* cc = titular->getCuentaCorriente();
            if (cc) {
                ListaDobleCircular<Movimiento*> movs = cc->getMovimientos();
                NodoDoble<Movimiento*>* nodoMov = movs.getCabeza();
                if (nodoMov) {
                    do {
                        Movimiento* m = nodoMov->dato;
                        if (m) {
                            Fecha f = m->getFechaMov();
                            if (fechaEnRango(f)) {
                                cout << "Cuenta ID: " << cc->getID()
                                     << " | Fecha: " << f.getDia() << "/" << f.getMes() << "/" << f.getAnio().getAnio()
                                     << " | Titular: " << nombreTitular
                                     << " | Monto: " << m->getMonto()
                                     << " | Tipo: " << (m->getTipo() ? "Deposito" : "Retiro")
                                     << endl;
                                encontrado = true;
                            }
                        }
                        nodoMov = nodoMov->siguiente;
                    } while (nodoMov != movs.getCabeza());
                }
            }

            // Cuentas de ahorro
            NodoDoble<CuentaBancaria*>* nodoA = titular->getCuentasAhorro().getCabeza();
            if (nodoA) {
                NodoDoble<CuentaBancaria*>* temp = nodoA;
                do {
                    CuentaBancaria* ca = temp->dato;
                    ListaDobleCircular<Movimiento*> movs = ca->getMovimientos();
                    NodoDoble<Movimiento*>* nodoMov = movs.getCabeza();
                    if (nodoMov) {
                        do {
                            Movimiento* m = nodoMov->dato;
                            if (m) {
                                Fecha f = m->getFechaMov();
                                if (fechaEnRango(f)) {
                                    cout << "Cuenta ID: " << ca->getID()
                                         << " | Fecha: " << f.getDia() << "/" << f.getMes() << "/" << f.getAnio().getAnio()
                                         << " | Titular: " << nombreTitular
                                         << " | Monto: " << m->getMonto()
                                         << " | Tipo: " << (m->getTipo() ? "Deposito" : "Retiro")
                                         << endl;
                                    encontrado = true;
                                }
                            }
                            nodoMov = nodoMov->siguiente;
                        } while (nodoMov != movs.getCabeza());
                    }
                    temp = temp->siguiente;
                } while (temp != nodoA);
            }

            actual = actual->siguiente;
        } while (actual != titulares.getCabeza());
    }

    if (!encontrado) {
        cout << "\nNo se encontraron movimientos en el rango de fechas.\n" << endl;
    }
    system("pause");
}

void Sistema::buscarPorTitular() {
    system("cls");
    cout << "--- BUSQUEDA POR TITULAR ---" << endl;
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

void Sistema::buscarPersonalizada() {
    system("cls");
    cout << "--- BUSQUEDA PERSONALIZADA GLOBAL ---" << endl;
    string criterioOriginal = val.ingresarTextoLibre((char*)"Ingrese cualquier dato a buscar (nombre, apellido, cedula, telefono, correo, ID cuenta, tipo cuenta, saldo, movimiento, etc):");

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

    NodoDoble<Titular*>* actual = titulares.getCabeza();
    bool encontrado = false;
    if (actual) {
        do {
            Persona p = actual->dato->getPersona();
            // Buscar en Persona
            string ci = toLower(p.getCI());
            string nombre = toLower(p.getNombre());
            string apellido = toLower(p.getApellido());
            string telefono = toLower(p.getTelefono());
            string correo = toLower(p.getCorreo());

            bool personaCoincide = ci.find(criterio) != string::npos ||
                                   nombre.find(criterio) != string::npos ||
                                   apellido.find(criterio) != string::npos ||
                                   telefono.find(criterio) != string::npos ||
                                   correo.find(criterio) != string::npos;

            // Buscar en cuentas corriente y ahorro
            auto buscarEnCuenta = [&](CuentaBancaria* cuenta) {
                if (!cuenta) return false;
                string idCuenta = toLower(cuenta->getID());
                string tipoCuenta = toLower(cuenta->getTipoCuenta());
                string saldo = toLower(to_string(cuenta->getSaldo()));

                bool cuentaCoincide = idCuenta.find(criterio) != string::npos ||
                                      tipoCuenta.find(criterio) != string::npos ||
                                      saldo.find(criterio) != string::npos;

                // Buscar en movimientos (lista doble circular)
                bool movimientoCoincide = false;
                ListaDobleCircular<Movimiento*>& movs = cuenta->getMovimientos();
                NodoDoble<Movimiento*>* nodoMov = movs.getCabeza();
                if (nodoMov) {
                    do {
                        Movimiento* m = nodoMov->dato;
                        string monto = toLower(to_string(m->getMonto()));
                        string numMov = toLower(to_string(m->getNumeroMovimiento()));
                        string tipoMov = m->getTipo() ? "deposito" : "retiro";
                        tipoMov = toLower(tipoMov);

                        Fecha f = m->getFechaMov();
                        string fecha = to_string(f.getDia()) + "/" + to_string(f.getMes()) + "/" + to_string(f.getAnio().getAnio());

                        if (monto.find(criterio) != string::npos ||
                            numMov.find(criterio) != string::npos ||
                            tipoMov.find(criterio) != string::npos ||
                            fecha.find(criterio) != string::npos) {
                            movimientoCoincide = true;
                            break;
                        }
                        nodoMov = nodoMov->siguiente;
                    } while (nodoMov != movs.getCabeza());
                }

                if (cuentaCoincide || movimientoCoincide) {
                    cout << "\n--- DATOS DEL TITULAR ---" << endl;
                    p.imprimir();
                    cout << "--- DATOS DE LA CUENTA ---" << endl;
                    cuenta->imprimir();
                    // Mostrar movimientos que coincidan
                    NodoDoble<Movimiento*>* nodoMov = movs.getCabeza();
                    bool movMostrado = false;
                    if (nodoMov) {
                        do {
                            Movimiento* m = nodoMov->dato;
                            string monto = toLower(to_string(m->getMonto()));
                            string numMov = toLower(to_string(m->getNumeroMovimiento()));
                            string tipoMov = m->getTipo() ? "deposito" : "retiro";
                            tipoMov = toLower(tipoMov);
                            Fecha f = m->getFechaMov();
                            string fecha = to_string(f.getDia()) + "/" + to_string(f.getMes()) + "/" + to_string(f.getAnio().getAnio());

                            if (monto.find(criterio) != string::npos ||
                                numMov.find(criterio) != string::npos ||
                                tipoMov.find(criterio) != string::npos ||
                                fecha.find(criterio) != string::npos) {
                                if (!movMostrado) {
                                    cout << "--- MOVIMIENTOS COINCIDENTES ---" << endl;
                                    movMostrado = true;
                                }
                                m->imprimir();
                            }
                            nodoMov = nodoMov->siguiente;
                        } while (nodoMov != movs.getCabeza());
                    }
                    encontrado = true;
                }
                return cuentaCoincide || movimientoCoincide;
            };

            // Si coincide persona, mostrar todo
            if (personaCoincide) {
                cout << "\n--- DATOS DEL TITULAR ---" << endl;
                p.imprimir();
                if (actual->dato->getCuentaCorriente()) {
                    cout << "--- CUENTA CORRIENTE ---" << endl;
                    actual->dato->getCuentaCorriente()->imprimir();
                }
                NodoDoble<CuentaBancaria*>* nodoA = actual->dato->getCuentasAhorro().getCabeza();
                if (nodoA) {
                    int idx = 1;
                    NodoDoble<CuentaBancaria*>* temp = nodoA;
                    do {
                        cout << "--- CUENTA DE AHORRO #" << idx++ << " ---" << endl;
                        temp->dato->imprimir();
                        temp = temp->siguiente;
                    } while (temp != nodoA);
                }
                encontrado = true;
            } else {
                // Buscar en cuentas
                if (buscarEnCuenta(actual->dato->getCuentaCorriente())) {
                    // Ya mostrado dentro de la lambda
                }
                NodoDoble<CuentaBancaria*>* nodoA = actual->dato->getCuentasAhorro().getCabeza();
                if (nodoA) {
                    NodoDoble<CuentaBancaria*>* temp = nodoA;
                    do {
                        buscarEnCuenta(temp->dato);
                        temp = temp->siguiente;
                    } while (temp != nodoA);
                }
            }
            actual = actual->siguiente;
        } while (actual != titulares.getCabeza());
    }

    if (!encontrado) {
        cout << "\nNo se encontraron coincidencias.\n" << endl;
    }
    system("pause");
}
void Sistema::crearBackup() {
    if (titulares.vacia()) {
        cout << "\nNo hay titulares registrados para respaldar.\n" << endl;
        system("pause");
        return;
    }

    time_t now = time(0);
    tm* ltm = localtime(&now);
    char nombreArchivo[100];
    sprintf(nombreArchivo, "%04d%02d%02d_%02d%02d%02d.bak",
        1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday,
        ltm->tm_hour, ltm->tm_min, ltm->tm_sec);

    ofstream archivo(nombreArchivo, ios::binary);
    if (!archivo) {
        cout << "\nNo se pudo crear el archivo de respaldo.\n" << endl;
        system("pause");
        return;
    }

    // Funciones auxiliares para strings
    auto escribirString = [&](const std::string& s) {
        size_t len = s.size();
        archivo.write(reinterpret_cast<const char*>(&len), sizeof(size_t));
        archivo.write(s.c_str(), len);
    };

    NodoDoble<Titular*>* actual = titulares.getCabeza();
    if (actual) {
        do {
            Titular* t = actual->dato;
            Persona p = t->getPersona();
            // Guardar Persona campo por campo
            escribirString(p.getCI());
            escribirString(p.getNombre());
            escribirString(p.getApellido());
            escribirString(p.getTelefono());
            escribirString(p.getCorreo());

            // Cuenta corriente
            CuentaBancaria* c = t->getCuentaCorriente();
            bool tieneCorriente = c != nullptr;
            archivo.write(reinterpret_cast<char*>(&tieneCorriente), sizeof(bool));
            if (tieneCorriente) {
                escribirString(c->getID());
                Fecha fechaCre = c->getFechaCre();
                archivo.write(reinterpret_cast<char*>(&fechaCre), sizeof(Fecha));
                float saldo = c->getSaldo();
                archivo.write(reinterpret_cast<char*>(&saldo), sizeof(float));
                escribirString(c->getTipoCuenta());

                // Movimientos
                ListaDobleCircular<Movimiento*>& movs = c->getMovimientos();
                int count = 0;
                NodoDoble<Movimiento*>* nodoMov = movs.getCabeza();
                if (nodoMov) {
                    NodoDoble<Movimiento*>* temp = nodoMov;
                    do {
                        count++;
                        temp = temp->siguiente;
                    } while (temp != nodoMov);
                }
                archivo.write(reinterpret_cast<char*>(&count), sizeof(int));
                if (nodoMov) {
                    NodoDoble<Movimiento*>* temp = nodoMov;
                    do {
                        Movimiento* m = temp->dato;
                        escribirString(m->getIDMovimiento());
                        Fecha fechaMov = m->getFechaMov();
                        archivo.write(reinterpret_cast<char*>(&fechaMov), sizeof(Fecha));
                        Hora hora = m->getHora();
                        archivo.write(reinterpret_cast<char*>(&hora), sizeof(Hora));
                        float monto = m->getMonto();
                        archivo.write(reinterpret_cast<char*>(&monto), sizeof(float));
                        bool tipo = m->getTipo();
                        archivo.write(reinterpret_cast<char*>(&tipo), sizeof(bool));
                        int numMov = m->getNumeroMovimiento();
                        archivo.write(reinterpret_cast<char*>(&numMov), sizeof(int));
                        temp = temp->siguiente;
                    } while (temp != nodoMov);
                }
            }

            // Cuentas de ahorro
            int totalAhorros = 0;
            NodoDoble<CuentaBancaria*>* nodoA = t->getCuentasAhorro().getCabeza();
            if (nodoA) {
                NodoDoble<CuentaBancaria*>* temp = nodoA;
                do { totalAhorros++; temp = temp->siguiente; } while (temp != nodoA);
            }
            archivo.write(reinterpret_cast<char*>(&totalAhorros), sizeof(int));
            if (nodoA) {
                NodoDoble<CuentaBancaria*>* temp = nodoA;
                do {
                    CuentaBancaria* ahorro = temp->dato;
                    escribirString(ahorro->getID());
                    Fecha fechaCreAhorro = ahorro->getFechaCre();
                    archivo.write(reinterpret_cast<char*>(&fechaCreAhorro), sizeof(Fecha));
                    float saldoAhorro = ahorro->getSaldo();
                    archivo.write(reinterpret_cast<char*>(&saldoAhorro), sizeof(float));
                    escribirString(ahorro->getTipoCuenta());

                    // Movimientos
                    ListaDobleCircular<Movimiento*>& movs = ahorro->getMovimientos();
                    int count = 0;
                    NodoDoble<Movimiento*>* nodoMov = movs.getCabeza();
                    if (nodoMov) {
                        NodoDoble<Movimiento*>* tmp = nodoMov;
                        do {
                            count++;
                            tmp = tmp->siguiente;
                        } while (tmp != nodoMov);
                    }
                    archivo.write(reinterpret_cast<char*>(&count), sizeof(int));
                    if (nodoMov) {
                        NodoDoble<Movimiento*>* tmp = nodoMov;
                        do {
                            Movimiento* m = tmp->dato;
                            escribirString(m->getIDMovimiento());
                            Fecha fechaMov = m->getFechaMov();
                            archivo.write(reinterpret_cast<char*>(&fechaMov), sizeof(Fecha));
                            Hora hora = m->getHora();
                            archivo.write(reinterpret_cast<char*>(&hora), sizeof(Hora));
                            float monto = m->getMonto();
                            archivo.write(reinterpret_cast<char*>(&monto), sizeof(float));
                            bool tipo = m->getTipo();
                            archivo.write(reinterpret_cast<char*>(&tipo), sizeof(bool));
                            int numMov = m->getNumeroMovimiento();
                            archivo.write(reinterpret_cast<char*>(&numMov), sizeof(int));
                            tmp = tmp->siguiente;
                        } while (tmp != nodoMov);
                    }
                    temp = temp->siguiente;
                } while (temp != nodoA);
            }
            actual = actual->siguiente;
        } while (actual != titulares.getCabeza());
    }
    archivo.close();
    cout << "\nBackup binario creado exitosamente en '" << nombreArchivo << "'.\n" << endl;
    system("pause");
}

void Sistema::mostrarAyuda() {
    system("cls");
    cout << "--- AYUDA DEL SISTEMA ---\n" << endl;
    cout << "Abriendo el archivo de ayuda...\n" << endl;
    // Abre el archivo CHM con la aplicación predeterminada en Windows
    system("start Ayuda-CuentasBancarias.chm");
    system("pause");
}