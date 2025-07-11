#include "Sistema.h"
#include "Menu.h"
#include "ValidacionFecha.h"
#include "FechaHora.h"
#include "ArchivoBinario.h"
#include "CifradoCesar.h"
#include "Backups.h"
#include "BPlusTreeTitulares.h"
#include "BusquedasBinarias.h"
#include "TablaHash.h"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <fstream>
#include <iostream>
using namespace std;

Sistema::Sistema(): arbolTitulares(3) {
    listaSucursales.agregarSucursal(Sucursal("Sucursal Central", -34.6037, -58.3816, "123"));
    listaSucursales.agregarSucursal(Sucursal("Sucursal Norte", -34.7000, -58.3000, "456"));
    listaSucursales.agregarSucursal(Sucursal("Sucursal Sur", -34.8000, -58.4000, "789"));
    actualizarContadoresSucursales();
}

Sistema::~Sistema() {
    NodoDoble<Titular*>* actual = titulares.getCabeza();
    if (actual != nullptr) {
        do {
            delete actual->dato;
            actual = actual->siguiente;
        } while (actual != titulares.getCabeza());
    }
}
void Sistema::generarPDFTitulares() {
    system("cls");
    cout << "\n--- GENERAR PDF DE TITULARES ---\n" << endl;

    // Open the input text file
    ifstream archivo("titulares.txt");
    if (!archivo) {
        cout << "\nNo se pudo abrir el archivo 'titulares.txt'.\n" << endl;
        system("pause");
        return;
    }

    // Output PDF file
    string outputFile = "titulares.pdf";
    ofstream pdf(outputFile, ios::binary);
    if (!pdf) {
        cout << "\nNo se pudo crear el archivo PDF.\n" << endl;
        archivo.close();
        system("pause");
        return;
    }

    // Write PDF header
    pdf << "%PDF-1.4\n";

    // Object 1: Catalog
    pdf << "1 0 obj\n"
        << "<< /Type /Catalog /Pages 2 0 R >>\n"
        << "endobj\n";

    // Object 2: Pages
    pdf << "2 0 obj\n"
        << "<< /Type /Pages /Kids [3 0 R] /Count 1 >>\n"
        << "endobj\n";

    // Object 3: Page
    pdf << "3 0 obj\n"
        << "<< /Type /Page /Parent 2 0 R /Resources << /Font << /F1 4 0 R >> >> /MediaBox [0 0 595 842] /Contents 5 0 R >>\n"
        << "endobj\n";

    // Object 4: Font
    pdf << "4 0 obj\n"
        << "<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica >>\n"
        << "endobj\n";

    // Object 5: Content stream
    stringstream contenido;
    contenido << "5 0 obj\n"
              << "<< /Length " << /* Length placeholder */ " >>\n"
              << "stream\n"
              << "BT\n"
              << "/F1 12 Tf\n"
              << "1 0 0 1 50 792 Tm\n" // Set text matrix (start at top-left, 50,792)
              << "(Listado de Titulares) Tj\n";

    // Adjust text position
    float yPos = 772; // Start below the title
    const float lineSpacing = 14; // Space between lines
    const float pageBottom = 50; // Bottom margin
    const float pageHeight = 842; // A4 height in points

    // Read the text file line by line
    string linea;
    while (getline(archivo, linea)) {
        // Check if we need a new page (simple pagination)
        if (yPos < pageBottom) {
            contenido << "ET\n"
                      << "endstream\n"
                      << "endobj\n";
            // Update length and write current content
            string contenidoStr = contenido.str();
            string longitud = to_string(contenidoStr.size() - string("5 0 obj\n<< /Length  >> stream\n").size() - string("endstream\nendobj\n").size());
            contenidoStr.replace(contenidoStr.find("/Length ") + 8, 0, longitud);
            pdf << contenidoStr;

            // Start a new page (simplified: new content stream)
            contenido.str(""); // Clear stream
            yPos = 792; // Reset Y position
            contenido << "5 0 obj\n"
                      << "<< /Length " << /* Length placeholder */ " >>\n"
                      << "stream\n"
                      << "BT\n"
                      << "/F1 12 Tf\n"
                      << "1 0 0 1 50 792 Tm\n";
        }

        // Escape special characters for PDF
        string escapedLine = linea;
        for (size_t i = 0; i < escapedLine.size(); ++i) {
            if (escapedLine[i] == '(' || escapedLine[i] == ')' || escapedLine[i] == '\\') {
                escapedLine.insert(i, "\\");
                ++i;
            }
        }

        // Write line to PDF
        contenido << "0 -" << lineSpacing << " Td\n"
                  << "(" << escapedLine << ") Tj\n";
        yPos -= lineSpacing;
    }

    contenido << "ET\n"
              << "endstream\n"
              << "endobj\n";

    // Update content length
    string contenidoStr = contenido.str();
    string longitud = to_string(contenidoStr.size() - string("5 0 obj\n<< /Length  >> stream\n").size() - string("endstream\nendobj\n").size());
    contenidoStr.replace(contenidoStr.find("/Length ") + 8, 0, longitud);
    pdf << contenidoStr;

    // Cross-reference table
    pdf << "xref\n"
        << "0 6\n"
        << "0000000000 65535 f \n";
    stringstream xref;
    xref << setfill('0') << setw(10);
    size_t offset = 9; // Length of "%PDF-1.4\n"
    xref << offset << " 00000 n \n";
    offset += string("1 0 obj\n<< /Type /Catalog /Pages 2 0 R >>\nendobj\n").size();
    xref << offset << " 00000 n \n";
    offset += string("2 0 obj\n<< /Type /Pages /Kids [3 0 R] /Count 1 >>\nendobj\n").size();
    xref << offset << " 00000 n \n";
    offset += string("3 0 obj\n<< /Type /Page /Parent 2 0 R /Resources << /Font << /F1 4 0 R >> >> /MediaBox [0 0 595 842] /Contents 5 0 R >>\nendobj\n").size();
    xref << offset << " 00000 n \n";
    offset += string("4 0 obj\n<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica >>\nendobj\n").size();
    xref << offset << " 00000 n \n";
    pdf << xref.str();

    // Trailer
    pdf << "trailer\n"
        << "<< /Size 6 /Root 1 0 R >>\n"
        << "startxref\n"
        << offset << "\n"
        << "%%EOF\n";

    pdf.close();
    archivo.close();
    cout << "\nPDF generado exitosamente: " << outputFile << "\n" << endl;
    system("pause");
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
        "Busquedas Binarias",
        "Guardar Archivo Binario",
        "Arbol B+",
        "Guardar titulares en archivo TXT",
        "Verificar integridad de archivo TXT",
        "Mostrar tabla hash",
        "Mostrar ayuda",
        "Generar codigo QR en PDF",
        "Generar PDF de titulares", // Nueva opción
        "Salir"
    };
    Menu menu;
    int opcion;
    do {
        opcion = menu.ingresarMenu("SISTEMA BANCARIO", opciones, 17); // Update to 17 options
        switch(opcion) {
            case 1: registrarTitular(); break;
            case 2: crearCuenta(); break;
            case 3: realizarDeposito(); break;
            case 4: realizarRetiro(); break;
            case 5: buscarMovimientosPorFecha(); break;
            case 6: buscarPorTitular(); break;
            case 7: buscarPersonalizada(); break;
            case 8: menuBB(); break;
            case 9: menuSecundario(); break;
            case 10: menuArbol(); break;
            case 11: guardarTitularesEnTxt(); break;
            case 12: {
                system("cls");
                cout << "\n--- VERIFICAR INTEGRIDAD DE ARCHIVO TXT ---" << endl;
                cout << "Esto compara el hash MD5 actual del archivo con el hash almacenado en la tabla hash.\n";
                string nombreArchivo = val.ingresarNombreArchivo((char*)"Ingrese el nombre del archivo TXT (ejemplo: titulares.txt): ");
                if (compararHashArchivo(nombreArchivo)) {
                    cout << "\nEl archivo no ha sido modificado (los hashes coinciden).\n" << endl;
                } else {
                    cout << "\nEl archivo ha sido modificado o no se encontro el hash en la tabla hash.\n" << endl;
                }
                system("pause");
                break;
            }
            case 13: {
                system("cls");
                cout << "\n--- MOSTRAR TABLA HASH ---" << endl;
                hashes.mostrarContenido();
                system("pause");
                break;
            }
            case 14: mostrarAyuda(); break;
            case 15: generarQRPDF(); break;
            case 16: generarPDFTitulares(); break; // Nueva opción
            case 17: cout << "\nSaliendo...\n" << endl; break;
            default: cout << "\nOpcion invalida." << endl; system("pause"); break;
        }
    } while(opcion != 17);
}
// Generar código QR básico (versión 1, 21x21)
void Sistema::generarQR(const std::string& data, bool qrMatrix[21][21]) {
    // Inicializar la matriz QR
    for (int i = 0; i < 21; ++i) {
        for (int j = 0; j < 21; ++j) {
            qrMatrix[i][j] = false;
        }
    }

    // Patrones fijos (finder patterns) en las esquinas
    auto dibujarFinderPattern = [&](int startX, int startY) {
        // Cuadrado exterior 7x7
        for (int i = 0; i < 7; ++i) {
            qrMatrix[startX][startY + i] = true;
            qrMatrix[startX + 6][startY + i] = true;
            qrMatrix[startX + i][startY] = true;
            qrMatrix[startX + i][startY + 6] = true;
        }
        // Cuadrado interior 5x5 (blanco)
        for (int i = 1; i < 6; ++i) {
            for (int j = 1; j < 6; ++j) {
                qrMatrix[startX + i][startY + j] = false;
            }
        }
        // Cuadrado central 3x3
        for (int i = 2; i < 5; ++i) {
            for (int j = 2; j < 5; ++j) {
                qrMatrix[startX + i][startY + j] = true;
            }
        }
    };

    dibujarFinderPattern(0, 0);   // Superior izquierda
    dibujarFinderPattern(0, 14);  // Superior derecha
    dibujarFinderPattern(14, 0);  // Inferior izquierda

    // Líneas de temporización
    for (int i = 8; i < 13; ++i) {
        qrMatrix[6][i] = (i % 2 == 0);
        qrMatrix[i][6] = (i % 2 == 0);
    }

    // Codificación de datos (modo alfanumérico simplificado)
    const string alfanumerico = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ $%*+-./:";
    auto getCharValue = [&](char c) -> int {
        for (size_t i = 0; i < alfanumerico.size(); ++i) {
            if (toupper(c) == alfanumerico[i]) return i;
        }
        return -1;
    };

    vector<int> valores;
    string truncatedData = data.substr(0, 25); // Limitar a 25 caracteres
    for (char c : truncatedData) {
        int val = getCharValue(c);
        if (val != -1) valores.push_back(val);
    }

    // Codificar en modo alfanumérico
    vector<bool> bits;
    bits.insert(bits.end(), {0, 0, 1, 0}); // Modo alfanumérico: 0010
    int len = valores.size();
    for (int i = 7; i >= 0; --i) {
        bits.push_back((len >> i) & 1);
    }
    for (size_t i = 0; i < valores.size(); i += 2) {
        int val1 = valores[i];
        int val2 = (i + 1 < valores.size()) ? valores[i + 1] : 0;
        int combined = val1 * 45 + val2;
        for (int j = 10; j >= 0; --j) {
            bits.push_back((combined >> j) & 1);
        }
    }
    while (bits.size() < 208) {
        bits.push_back(0);
    }

    // Colocar datos en la matriz (patrón en zigzag simplificado)
    int bitIndex = 0;
    for (int col = 20; col >= 0 && bitIndex < bits.size(); col -= 2) {
        if (col == 6) continue;
        for (int row = 20; row >= 0 && bitIndex < bits.size(); --row) {
            if (qrMatrix[row][col] == false) {
                qrMatrix[row][col] = bits[bitIndex++];
            }
            if (col - 1 >= 0 && qrMatrix[row][col - 1] == false) {
                qrMatrix[row][col - 1] = bitIndex < bits.size() ? bits[bitIndex++] : false;
            }
        }
        col--;
        if (col < 0) break;
        if (col == 6) col--;
        for (int row = 0; row < 21 && bitIndex < bits.size(); ++row) {
            if (qrMatrix[row][col] == false) {
                qrMatrix[row][col] = bits[bitIndex++];
            }
            if (col - 1 >= 0 && qrMatrix[row][col - 1] == false) {
                qrMatrix[row][col - 1] = bitIndex < bits.size() ? bits[bitIndex++] : false;
            }
        }
    }
}

// Generar archivo PDF
void Sistema::generarPDF(const std::string& nombre, const std::string& numeroCuenta, bool qrMatrix[21][21], const std::string& outputFile) {
    ofstream archivo(outputFile, ios::binary);
    if (!archivo) {
        cout << "\nNo se pudo crear el archivo PDF.\n" << endl;
        return;
    }

    // Escribir el encabezado del PDF
    archivo << "%PDF-1.4\n";

    // Objeto 1: Catálogo
    archivo << "1 0 obj\n"
            << "<< /Type /Catalog /Pages 2 0 R >>\n"
            << "endobj\n";

    // Objeto 2: Página
    archivo << "2 0 obj\n"
            << "<< /Type /Pages /Kids [3 0 R] /Count 1 >>\n"
            << "endobj\n";

    // Objeto 3: Contenido de la página
    archivo << "3 0 obj\n"
            << "<< /Type /Page /Parent 2 0 R /Resources << /Font << /F1 4 0 R >> >> /MediaBox [0 0 595 842] /Contents 5 0 R >>\n"
            << "endobj\n";

    // Objeto 4: Fuente
    archivo << "4 0 obj\n"
            << "<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica >>\n"
            << "endobj\n";

    // Objeto 5: Contenido de la página
    stringstream contenido;
    contenido << "5 0 obj\n"
              << "<< /Length " << /* longitud se calculará después */ " >>\n"
              << "stream\n"
              << "BT\n"
              << "/F1 12 Tf\n"
              << "100 750 Td\n"
              << "(Código QR - Información de Cuenta Bancaria) Tj\n"
              << "100 730 Td\n"
              << "(Titular: " << nombre << ") Tj\n"
              << "100 710 Td\n"
              << "(Nº Cuenta: " << numeroCuenta << ") Tj\n"
              << "ET\n";

    // Dibujar el código QR (10x10 puntos por módulo)
    float xBase = 100, yBase = 600;
    float moduleSize = 10;
    for (int i = 0; i < 21; ++i) {
        for (int j = 0; j < 21; ++j) {
            if (qrMatrix[i][j]) {
                float x = xBase + j * moduleSize;
                float y = yBase - i * moduleSize; // Invertir Y para PDF
                contenido << "q\n"
                          << "0 0 0 rg\n"
                          << x << " " << (y - moduleSize) << " " << moduleSize << " " << moduleSize << " re\n"
                          << "f\n"
                          << "Q\n";
            }
        }
    }

    string contenidoStr = contenido.str();
    contenidoStr += "endstream\nendobj\n";

    // Actualizar la longitud del contenido
    string longitud = to_string(contenidoStr.size() - string("5 0 obj\n<< /Length  >> stream\n").size() - string("endstream\nendobj\n").size());
    contenidoStr.replace(contenidoStr.find("/Length ") + 8, 0, longitud);

    archivo << contenidoStr;

    // Cross-reference table
    archivo << "xref\n"
            << "0 6\n"
            << "0000000000 65535 f \n";
    stringstream xref;
    xref << setfill('0') << setw(10);
    size_t offset = 9; // Longitud de "%PDF-1.4\n"
    xref << offset << " 00000 n \n";
    offset += string("1 0 obj\n<< /Type /Catalog /Pages 2 0 R >>\nendobj\n").size();
    xref << offset << " 00000 n \n";
    offset += string("2 0 obj\n<< /Type /Pages /Kids [3 0 R] /Count 1 >>\nendobj\n").size();
    xref << offset << " 00000 n \n";
    offset += string("3 0 obj\n<< /Type /Page /Parent 2 0 R /Resources << /Font << /F1 4 0 R >> >> /MediaBox [0 0 595 842] /Contents 5 0 R >>\nendobj\n").size();
    xref << offset << " 00000 n \n";
    offset += string("4 0 obj\n<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica >>\nendobj\n").size();
    xref << offset << " 00000 n \n";
    archivo << xref.str();

    // Trailer
    archivo << "trailer\n"
            << "<< /Size 6 /Root 1 0 R >>\n"
            << "startxref\n"
            << offset << "\n"
            << "%%EOF\n";

    archivo.close();
    cout << "\nPDF generado exitosamente: " << outputFile << "\n" << endl;
}

// Generar código QR en PDF
void Sistema::generarQRPDF() {
    system("cls");
    cout << "\n--- GENERAR CÓDIGO QR EN PDF ---\n" << endl;

    if (titulares.vacia()) {
        cout << "\nNo hay titulares registrados.\n" << endl;
        system("pause");
        return;
    }

    string cedula = val.ingresarCedula("\nIngrese cedula del titular:");
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

    string tipo = val.ingresarCadena("\nTipo de cuenta (Corriente/Ahorro):");
    for (char& c : tipo) c = toupper(c);
    CuentaBancaria* cuenta = nullptr;
    string idCuenta = val.ingresarNumeros("\nIngrese ID de la cuenta:");

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

    // Generar el texto para el QR
    string nombre = titular->getPersona().getNombre() + " " + titular->getPersona().getApellido();
    string numeroCuenta = cuenta->getID();
    string qrData = "Titular: " + nombre + ", Cuenta: " + numeroCuenta;
    if (qrData.size() > 25) {
        qrData = qrData.substr(0, 25); // Limitar a 25 caracteres
        cout << "\nAdvertencia: Los datos fueron truncados a 25 caracteres para el código QR.\n";
    }

    // Generar la matriz QR
    bool qrMatrix[21][21];
    generarQR(qrData, qrMatrix);

    // Generar el PDF
    string outputFile = "qr_" + cedula + ".pdf";
    generarPDF(nombre, numeroCuenta, qrMatrix, outputFile);

    system("pause");
}
void Sistema::menuArbol(){
    const char* opciones[] = {
        "Buscar en Arbol B+",
        "Eliminar en Arbol B+",
        "Graficar Arbol B+",
        "Regresar al menu principal"
    };
    Menu menu;
    int opcion;
    do {
        opcion = menu.ingresarMenu("Arbol B+", opciones, 4);
        switch(opcion) {
            case 1:{
                string ciBuscar = val.ingresarCedula((char*)"\nIngrese el CI del titular para buscar: ");
                std::cout << "\nBuscando en el Arbol B+...\n";
                Titular* titularEncontrado = arbolTitulares.buscar(ciBuscar);
                std::cout << "\nResultado de la busqueda:\n";
                if (titularEncontrado) {
                    std::cout << "\nTitular encontrado:\n";
                    std::cout << "Nombre: " << titularEncontrado->getPersona().getNombre() << " " 
                              << titularEncontrado->getPersona().getApellido() << std::endl;
                    std::cout << "Numero de cuenta corriente: "
                              << (titularEncontrado->getCuentaCorriente() ? titularEncontrado->getCuentaCorriente()->getID() : "No tiene cuenta corriente") << std::endl;
                    std::cout << "Cuentas de ahorro: ";
                    if (titularEncontrado->getCuentasAhorro().vacia()) {
                        std::cout << "No tiene cuentas de ahorro" << std::endl;
                    } else {
                        std::cout << std::endl;
                        titularEncontrado->mostrarCuentasAhorro();
                    }
                } else {
                    std::cout << "\nTitular no encontrado." << std::endl;
                }
                std::cout.flush(); // Forzar la salida
                system("pause");
                break;
            }  
            case 2: {
                string ciEliminar = val.ingresarCedula((char*)"\nIngrese el CI del titular para eliminar: ");
                std::cout << "\nBuscando titular para eliminar...\n";
                Titular* titularEncontrado = arbolTitulares.buscar(ciEliminar);
                if (titularEncontrado) {
                    // Eliminar del árbol B+
                    arbolTitulares.eliminar(ciEliminar);
                    // Eliminar de la lista de titulares
                    NodoDoble<Titular*>* actual = titulares.getCabeza();
                    if (actual) {
                        do {
                            if (actual->dato->getPersona().getCI() == ciEliminar) {
                                titulares.eliminar(actual);
                                delete actual->dato; // Liberar memoria del titular
                                break;
                            }
                            actual = actual->siguiente;
                        } while (actual != titulares.getCabeza());
                    }
                    std::cout << "\nTitular eliminado exitosamente.\n";
                    // Actualizar contadores de sucursales
                    actualizarContadoresSucursales();
                    // Crear backup
                    Backups backup;
                    backup.crearBackup(titulares);
                    // Imprimir árbol para verificar
                    std::cout << "\nArbol despues de la eliminacion:\n";
                    arbolTitulares.imprimir();
                } else {
                    std::cout << "\nTitular no encontrado." << std::endl;
                }
                std::cout.flush();
                system("pause");
                break;
            }
            case 3:  break;
            case 4: cout << "\nRegresando al menu principal...\n" << endl; break;
            default: cout << "\nOpcion invalida." << endl; system("pause"); break;
        }
    } while(opcion != 4);
}
void Sistema::menuSecundario(){
    const char* opciones[] = {
        "Guardar archivo sin cifrar",
        "Guardar archivo cifrado",
        "Decifrar archivo cifrado",
        "Restaurar backup",
        "Regresar al menu principal"
    };
    Menu menu;
    int opcion;
    do {
        opcion = menu.ingresarMenu("Archivos Binarios", opciones, 5);
        switch(opcion) {
            case 1: guardarArchivoBinSinCifrar(); break;
            case 2: guardarArchivoBinCifrado(); break;
            case 3: decifrarArchivoCifrado(); break;
            case 4: { // Restaurar backup
                system("cls");
                cout << "/n--- RESTAURAR BACKUP ---/n" << endl;
                string archivo = val.ingresarCodigoBak((char*)"Ingrese el nombre del archivo de backup (ejemplo: 20240601_153000.bak):");
                Backups backup;
                
                if (backup.restaurarBackup(titulares, arbolTitulares,archivo)) {
                    actualizarContadoresSucursales();
                }
                break;
            }
            case 5: cout << "\nRegresando al menu principal...\n" << endl; break;
            default: cout << "\nOpcion invalida." << endl; system("pause"); break;
        }
    } while(opcion != 5);
}
void Sistema::registrarTitular() {
    system("cls");
    cout << "\n--- REGISTRAR TITULAR --- \n" << endl;
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

    Persona persona(nombre, apellido, cedula, telefono, correo, fechaNacimiento);

    // Crear Titular con la persona
    Titular* nuevo = new Titular(persona);

    // Insertar el nuevo titular en la lista y en el árbol B+
    titulares.insertar(nuevo);
    arbolTitulares.insertar(cedula, nuevo);
    
    arbolTitulares.imprimir();
    std::cout.flush();
    cout << "\nTitular registrado exitosamente." << endl;
    Backups backup;
    backup.crearBackup(titulares);
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
    cout << "\n--- CREAR CUENTA ---\n" << endl;
    string cedula = val.ingresarCedula((char*)"\nIngrese cedula del titular: ");
    Titular* titular = buscarTitularPorCI(cedula);
    if (!titular) {
        cout << "\nTitular no encontrado." << endl;
        system("pause");
        return;
    }
    cout << "\nSeleccione la sucursal para la cuenta:\n";
    listaSucursales.mostrarSucursales();
    string idSucursal = val.ingresarCodigoSucursal((char*)"\nIngrese el ID de la sucursal: ");
    if (!listaSucursales.existeSucursal(idSucursal)) {
        cout << "\nSucursal no encontrada." << endl;
        system("pause");
        return;
    }
    NodoSucursal* nodo = listaSucursales.getCabeza();
    while (nodo != nullptr) {
        if (nodo->sucursal.getIdSucursal() == idSucursal) {
            nodo->sucursal.incrementarContadorCuentas();
            break;
        }
        nodo = nodo->siguiente;
    }
    string tipo = val.ingresarCadena((char*)"\nIngrese tipo de cuenta (Corriente/Ahorro): ");
    CuentaBancaria* nuevaCuenta = new CuentaBancaria(idSucursal);
    for (char& c : tipo) c = toupper(c);
    nuevaCuenta->setTipoCuenta(tipo);

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

    Backups backup;
    backup.crearBackup(titulares);
    system("pause");

}

void Sistema::realizarDeposito() {
    if (titulares.vacia()) {
        cout << "\nNo hay titulares registrados.\n" << endl;
        system("pause");
        return;
    }
    system("cls");
    cout << "\n--- REALIZAR DEPOSITO ---" << endl;

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
    // Obtener el número de movimiento
    ListaDobleCircular<Movimiento*>& movs = cuenta->getMovimientos();
    int numMov = 1;
    if (!cuenta->getMovimientos().vacia()) {
        numMov = cuenta->getMovimientos().getCabeza()->anterior->dato->getNumeroMovimiento() + 1;
    }

    Movimiento* mov = new Movimiento(monto, true, numMov);
    cuenta->agregarMovimiento(mov);  // Ya actualiza el saldo

    cout << "\nDeposito realizado exitosamente.\n" << endl;
    Backups backup;
    backup.crearBackup(titulares);
    system("pause");
    
}


void Sistema::realizarRetiro() {
    if (titulares.vacia()) {
        cout << "\nNo hay titulares registrados.\n" << endl;
        system("pause");
        return;
    }
    system("cls");
    cout << "\n--- REALIZAR RETIRO ---" << endl;

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
        cout << "\nSaldo insuficiente para realizar el retiro.\n" << endl;
        system("pause");
        return;
    }
    // Obtener el número de movimiento
    ListaDobleCircular<Movimiento*>& movs = cuenta->getMovimientos();
    int numMov = 1;
    if (!cuenta->getMovimientos().vacia()) {
        numMov = cuenta->getMovimientos().getCabeza()->anterior->dato->getNumeroMovimiento() + 1;
    }
    Movimiento* mov = new Movimiento(monto, false, numMov);
    cuenta->agregarMovimiento(mov);
    cuenta->setSaldo(cuenta->getSaldo() - monto);

    // *** Obtener la lista actualizada después de agregar el movimiento ***
    movs = cuenta->getMovimientos();
    cout << "\n[DEBUG] Movimientos en la cuenta " << cuenta->getID() << " tras la operación:\n";
    NodoDoble<Movimiento*>* actualMov = movs.getCabeza();
    if (actualMov) {
        do {
            Movimiento* m = actualMov->dato;
            if (m) {
                std::cout << "  Movimiento #" << m->getNumeroMovimiento()
                          << " | Monto: " << m->getMonto()
                          << " | Tipo: " << (m->getTipo() ? "Deposito" : "Retiro")
                          << std::endl;
            }
            actualMov = actualMov->siguiente;
        } while (actualMov != movs.getCabeza());
    }

    cout << "\nRetiro realizado exitosamente.\n" << endl;
    Backups backup;
    backup.crearBackup(titulares);
    system("pause");

}

void Sistema::guardarArchivoBinCifrado() {
    system("cls");
    cout << "\n--- GUARDAR CUENTAS EN ARCHIVO BINARIO ---" << endl;
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
    cout << "\n--- GUARDAR CUENTAS EN ARCHIVO BINARIO ---" << endl;
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
    cout << "\n--- DECIFRAR ARCHIVO CIFRADO ---" << endl;
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
    cout << "\n--- BUSCAR MOVIMIENTOS POR RANGO DE FECHAS ---" << endl;

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
    cout << "\n--- BUSQUEDA POR TITULAR ---" << endl;
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
    cout << "\n--- BUSQUEDA PERSONALIZADA GLOBAL ---" << endl;
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

void Sistema::mostrarAyuda() {
    system("cls");
    cout << "\n--- AYUDA DEL SISTEMA ---\n" << endl;
    cout << "Abriendo el archivo de ayuda...\n" << endl;
    // Abre el archivo CHM con la aplicación predeterminada en Windows
    system("start Ayuda-CuentasBancarias.chm");
    system("pause");
}

// Supón que tienes: titulares (ListaDobleCircular<Titular*>&) y listaSucursales (ListaSucursales&)
void Sistema::actualizarContadoresSucursales() {
    // Reinicia todos los contadores
    NodoSucursal* nodoSuc = listaSucursales.getCabeza();
    while (nodoSuc != nullptr) {
        nodoSuc->sucursal.setContadorCuentas(0); // Debes tener este setter
        nodoSuc = nodoSuc->siguiente;
    }

    // Recorre titulares y cuentas
    NodoDoble<Titular*>* actual = titulares.getCabeza();
    if (actual) {
        do {
            Titular* t = actual->dato;
            // Cuenta corriente
            CuentaBancaria* c = t->getCuentaCorriente();
            if (c) {
                std::string idSucursal = c->getID().substr(0, 3); // Ajusta si tu ID es diferente
                NodoSucursal* nodo = listaSucursales.getCabeza();
                while (nodo != nullptr) {
                    if (nodo->sucursal.getIdSucursal() == idSucursal) {
                        nodo->sucursal.incrementarContadorCuentas();
                        break;
                    }
                    nodo = nodo->siguiente;
                }
            }
            // Cuentas de ahorro
            NodoDoble<CuentaBancaria*>* nodoA = t->getCuentasAhorro().getCabeza();
            if (nodoA) {
                NodoDoble<CuentaBancaria*>* temp = nodoA;
                do {
                    CuentaBancaria* ahorro = temp->dato;
                    std::string idSucursal = ahorro->getID().substr(0, 3);
                    NodoSucursal* nodo = listaSucursales.getCabeza();
                    while (nodo != nullptr) {
                        if (nodo->sucursal.getIdSucursal() == idSucursal) {
                            nodo->sucursal.incrementarContadorCuentas();
                            break;
                        }
                        nodo = nodo->siguiente;
                    }
                    temp = temp->siguiente;
                } while (temp != nodoA);
            }
            actual = actual->siguiente;
        } while (actual != titulares.getCabeza());
    }
}
/*void Sistema::graficarArbol() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Arbol B+");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        std::cout << "Error al cargar la fuente." << std::endl;
        return;
    }

    NodoBPlus* raiz = arbolTitulares.getRaiz();

    auto dibujarNodo = [&](auto& self, NodoBPlus* nodo, float x, float y, float dx, int nivel) -> void {
        if (!nodo) return;

        // Calcular ancho del nodo basado en el número de claves
        float nodoAncho = 50.0f + (nodo->numClaves * 30.0f); // 30.0f por clave
        float nodoAlto = 40.0f;

        // Dibujar rectángulo del nodo
        sf::RectangleShape rect(sf::Vector2f(nodoAncho, nodoAlto));
        rect.setPosition(x - nodoAncho / 2, y);
        rect.setFillColor(nodo->esHoja ? sf::Color(144, 238, 144) : sf::Color(173, 216, 230)); // Verde claro para hojas, azul claro para internos
        rect.setOutlineColor(sf::Color::Black);
        rect.setOutlineThickness(1);

        // Construir texto con las claves
        std::string texto;
        for (int i = 0; i < nodo->numClaves; ++i) {
            texto += nodo->claves[i] + " ";
        }
        sf::Text textoNodo(texto, font, 14);
        textoNodo.setFillColor(sf::Color::Black);
        textoNodo.setPosition(x - nodoAncho / 2 + 5, y + 5);

        // Dibujar nodo
        window.draw(rect);
        window.draw(textoNodo);

        // Dibujar conexiones a hijos si no es hoja
        if (!nodo->esHoja && nodo->hijos) {
            float hijoX = x - ((nodo->numClaves) * dx / 2); // Ajustar posición inicial de hijos
            float nuevoY = y + 80;
            for (int i = 0; i <= nodo->numClaves; ++i) {
                if (nodo->hijos[i]) {
                    sf::Vertex linea[] = {
                        sf::Vertex(sf::Vector2f(x, y + nodoAlto), sf::Color::Black),
                        sf::Vertex(sf::Vector2f(hijoX + i * dx + nodoAncho / 2, nuevoY), sf::Color::Black)
                    };
                    window.draw(linea, 2, sf::Lines);
                    self(self, nodo->hijos[i], hijoX + i * dx, nuevoY, dx / std::max(2.0f, float(nodo->numClaves + 1)), nivel + 1);
                }
            }
        }
    };

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                window.close();
            }
        }
        window.clear(sf::Color::White);
        if (raiz) {
            dibujarNodo(dibujarNodo, raiz, 600, 50, 200, 0); // Centro en x=600, y=50
        } else {
            sf::Text texto("Arbol B+ vacio", font, 20);
            texto.setFillColor(sf::Color::Black);
            texto.setPosition(550, 350);
            window.draw(texto);
        }
        window.display();
    }
}*/

// BUSQUEDAS BINARIAS
void Sistema::busquedasBinarias() {
    menuBB();
}

void Sistema::menuBB() {
    system("cls");
    BusquedasBinarias buscador;
    Menu menu;
    ListaSucursales sucursales;
    const char* opciones[] = {
        "Buscar deposito mayor o igual a un monto",
        "Deposito minimo mensual para meta de ahorro",
        "Buscar titular por CI",
        "Buscar titular por anio de nacimiento ",
        "Buscar sucursal mas cercana",
        "Regresar al menu principal"
    };
    int opcion;
    do {
        opcion = menu.ingresarMenu("BUSQUEDAS BINARIAS", opciones, 6);
        switch(opcion) {
            case 1: {
                // Buscar primer deposito mayor o igual a un monto
                cout << "\n--- Buscar primer deposito mayor o igual a un monto ---\n";
                cout << " Esta funcion permite buscar el primer deposito en una cuenta bancaria que sea mayor o igual a un monto especificado.\n";
                string cedula = val.ingresarCedula((char*)"\nIngrese cedula del titular:");
                Titular* titular = buscarTitularPorCI(cedula);
                if (!titular) {
                    cout << "Titular no encontrado.\n"; system("pause"); break;
                }
                string tipo = val.ingresarCadena((char*)"\nTipo de cuenta (Corriente/Ahorro):");
                for (char& c : tipo) c = toupper(c);
                CuentaBancaria* cuenta = nullptr;
                string idCuenta = val.ingresarNumeros((char*)"\nIngrese ID de la cuenta:");
                if (tipo == "CORRIENTE") {
                    cuenta = titular->getCuentaCorriente();
                    if (!cuenta || cuenta->getID() != idCuenta) {
                        cout << "Cuenta corriente no encontrada o ID incorrecto.\n"; system("pause"); break;
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
                        cout << "Cuenta de ahorro no encontrada.\n"; system("pause"); break;
                    }
                } else {
                    cout << "Tipo de cuenta no valido.\n"; system("pause"); break;
                }
                float monto = val.ingresarMonto((char*)"\nIngrese monto minimo a buscar:");
                Movimiento* mov = buscador.primerDepositoMayorIgual(cuenta->getMovimientos(), monto);
                if (mov) {
                    cout << "\n El primer deposito mayor o igual al monto " << monto << " es :\n";
                    mov->imprimir();
                } else {
                    cout << "\nNo se encontro ningun deposito mayor o igual a ese monto.";
                }
                system("pause");
                break;
            }
            case 2: {
                cout << "\n--- Deposito minimo mensual para meta de ahorro ---\n";
                cout << " Esta funcion permite calcular el deposito mensual minimo necesario para alcanzar una meta de ahorro en un plazo determinado inciando con el saldo actual de la cuenta .\n";
                string cedula = val.ingresarCedula((char*)"\nIngrese cedula del titular:");
                Titular* titular = buscarTitularPorCI(cedula);
                if (!titular) {
                    cout << "Titular no encontrado.\n"; system("pause"); break;
                }
                if (titular->getCuentasAhorro().vacia()) {
                    cout << "El titular no tiene cuentas de ahorro.\n"; system("pause"); break;
                }
                string idCuenta = val.ingresarNumeros((char*)"\nIngrese ID de la cuenta de ahorro:");
                CuentaBancaria* cuentaAhorro = nullptr;
                NodoDoble<CuentaBancaria*>* actual = titular->getCuentasAhorro().getCabeza();
                if (actual) {
                    do {
                        if (actual->dato->getID() == idCuenta) {
                            cuentaAhorro = actual->dato;
                            break;
                        }
                        actual = actual->siguiente;
                    } while (actual != titular->getCuentasAhorro().getCabeza());
                }
                if (!cuentaAhorro) {
                    cout << "Cuenta de ahorro no encontrada.\n"; system("pause"); break;
                }

                float saldoInicial = cuentaAhorro->getSaldo();
                cout << "Saldo inicial de la cuenta: $" << saldoInicial << endl;
                float saldoMeta;
                int meses;
                do {
                    cout << "Saldo meta: "; cin >> saldoMeta;
                    if (saldoMeta <= saldoInicial) {
                        cout << "El saldo meta debe ser mayor al saldo actual de la cuenta. Intente de nuevo.\n";
                    }
                } while (saldoMeta <= saldoInicial);
                cout << "Meses para alcanzar la meta: "; 
                cin >> meses;
                int deposito = buscador.depositoMinimoParaMeta(saldoInicial, saldoMeta, meses);
                cout << "Deposito mensual minimo necesario: $" << deposito << endl;
                system("pause");
                break;
            }
            case 3: {
                cout << "\n--- Buscar primer titular por CI ---\n";
                cout << " Esta funcion permite buscar el primer titular cuyo CI sea mayor o igual al ingresado.\n";
                string ci = val.ingresarCedula((char*)"Ingrese CI a buscar:");
                Titular* t = buscador.primerTitularCIMayorIgual(titulares, ci);
                if (t) {
                    cout << "Primer titular con CI >= " << ci << ":\n";
                    t->getPersona().imprimir();
                } else {
                    cout << "No se encontro ningun titular con ese CI o mayor.\n";
                }
                system("pause");
                break;
            }
            case 4: {
                // Buscar primer titular por año de nacimiento
                cout << "\n--- Buscar primer titular por anio de nacimiento (mayor o igual) ---\n";
                int anio;
                cout << "Ingrese anio de nacimiento a buscar: "; cin >> anio;
                Titular* t = buscador.primerTitularAnioNacimientoMayorIgual(titulares, anio);
                if (t) {
                    cout << "Primer titular con anio de nacimiento >= " << anio << ":\n";
                    t->getPersona().imprimir();
                } else {
                    cout << "No se encontro ningun titular con ese anio o mayor.\n";
                }
                system("pause");
                break;
            }case 5: {
                cout << "\n--- Buscar sucursal mas cercana ---\n";
                cout << " Esta funcion encuentra la sucursal mas cercana a las coordenadas geograficas ingresadas.\n";
                float latUsuario = val.ingresarCoordenada((char*)"Ingrese latitud:",true);
                float lonUsuario = val.ingresarCoordenada((char*)"Ingrese longitud:",false);
                Sucursal* sucursal = buscador.sucursalMasCercana(listaSucursales.getCabeza(), latUsuario, lonUsuario);
                if (sucursal) {

                    cout << "\nSucursal mas cercana:\n";
                    sucursal->imprimir();
                    srand(time(0));
                    int diasFuturos = rand() % 5 + 1;      // Entre 1 y 5 días después
                    int hora = rand() % 9 + 8;             // Entre 08 y 16
                    int minuto = (rand() % 4) * 15;        // 0, 15, 30, 45

                    FechaHora cita;
                    cita.actualizarFechaHora();            // obtener fecha actual
                    cita.setDia(cita.getDia() + diasFuturos);
                    cita.setHora(hora);
                    cita.setMinuto(minuto);
                    cita.setSegundo(0);

                    // Mostrar cita generada
                    cout << "\nCita generada exitosamente:\n";
                    cout << "Sucursal: " << sucursal->getNombre() << endl;
                    cout << "FechaDeGeneracionDeCita: " << cita.obtenerFecha() << endl;
                    cout << "HoraDeGeneracionDeCita: " << cita.obtenerHora() << endl;
                 } else {
                    cout << "No se encontraron sucursales.\n";
                }
                system("pause");
                break;
                }  
            /*case 6: {
                cout << "\n--- Calcular intervalo maximo entre citas ---\n";
                            cout << " Esta funcion calcula el maximo intervalo de tiempo (en minutos) entre citas consecutivas para programar un numero dado de clientes.\n";
                            int nClientes;
                            cout << "Ingrese numero de clientes a programar: ";
                            cin >> nClientes;
                            if (cin.fail() || nClientes < 2 || nClientes > 100000) {
                                cin.clear();
                                cin.ignore(10000, '\n');
                                cout << "Numero de clientes invalido. Debe estar entre 2 y 100000.\n";
                                system("pause");
                                break;
                            }
                            int duracionCita;
                            cout << "Ingrese duracion de cada cita (en minutos, minimo 1): ";
                            cin >> duracionCita;
                            if (cin.fail() || duracionCita < 1) {
                                cin.clear();
                                cin.ignore(10000, '\n');
                                cout << "Duracion de cita invalida. Debe ser al menos 1 minuto.\n";
                                system("pause");
                                break;
                            }
                            if (!listaSucursales.getCabeza()) {
                                cout << "No hay sucursales disponibles.\n";
                                system("pause");
                                break;
                            }
                            int maxD = buscador.maximoIntervaloCitas(listaSucursales.getCabeza(), nClientes, duracionCita);
                            if (maxD == 0) {
                                cout << "No es posible programar las citas con los datos proporcionados.\n";
                            } else {
                                cout << "El maximo intervalo entre citas es: " << maxD << " minutos.\n";
                            }
                            system("pause");
                            break;
}
           */ case 6:
                cout << "\nRegresando al menu principal...\n";
                break;
            default:
                cout << "\nOpcion invalida.\n"; system("pause"); break;
        }
        }while(opcion != 6);
    }
void Sistema::guardarTitularesEnTxt() {
    system("cls");
    cout << "\n--- GUARDAR TITULARES EN ARCHIVO TXT ---" << endl;

    if (titulares.vacia()) {
        cout << "\nNo hay titulares registrados para guardar.\n" << endl;
        system("pause");
        return;
    }

    ofstream archivo("titulares.txt");
    if (!archivo) {
        cout << "\nNo se pudo abrir el archivo para escribir.\n" << endl;
        system("pause");
        return;
    }

    NodoDoble<Titular*>* actual = titulares.getCabeza();
    if (actual) {
        do {
            Titular* titular = actual->dato;
            Persona persona = titular->getPersona();

            cout << "Guardando titular: " << persona.getNombre() << " " << persona.getApellido() << endl;

            archivo << "CI: " << persona.getCI() << " - Nombre: " << persona.getNombre() << " " 
                    << persona.getApellido() << " - Tel: " << persona.getTelefono() << " - Correo: " 
                    << persona.getCorreo() << " - Fecha Nacimiento: " 
                    << persona.getFechaNa().getDia() << "/" << persona.getFechaNa().getMes() << "/" 
                    << persona.getFechaNa().getAnio().getAnio() << "\n";

            CuentaBancaria* cuentaCorriente = titular->getCuentaCorriente();
            if (cuentaCorriente) {
                cout << "Guardando cuenta corriente de " << persona.getNombre() << endl;
                archivo << "--- CUENTA CORRIENTE ---" << endl;
                archivo << "ID Cuenta: " << cuentaCorriente->getID() << " - Tipo: " 
                        << cuentaCorriente->getTipoCuenta() << " - Saldo: " 
                        << cuentaCorriente->getSaldo() << "\n";
            } else {
                cout << "No tiene cuenta corriente." << endl;
            }

            NodoDoble<CuentaBancaria*>* nodoAhorro = titular->getCuentasAhorro().getCabeza();
            if (nodoAhorro) {
                cout << "Guardando cuentas de ahorro de " << persona.getNombre() << endl;
                do {
                    archivo << "--- CUENTA DE AHORRO ---" << endl;
                    archivo << "ID Cuenta: " << nodoAhorro->dato->getID() << " - Tipo: " 
                            << nodoAhorro->dato->getTipoCuenta() << " - Saldo: " 
                            << nodoAhorro->dato->getSaldo() << "\n";
                    nodoAhorro = nodoAhorro->siguiente;
                } while (nodoAhorro != titular->getCuentasAhorro().getCabeza());
            } else {
                cout << "No tiene cuentas de ahorro." << endl;
            }

            archivo << "\n";
            actual = actual->siguiente;
        } while (actual != titulares.getCabeza());
    }

    archivo.close();
    cout << "\nTitulares guardados exitosamente en 'titulares.txt'.\n" << endl;

    // Generar y guardar el hash MD5
    string hash = generarHashMD5("titulares.txt");
    if (!hash.empty()) {
        // Guardar en la tabla hash
        hashes.insertar("titulares.txt", hash);
        cout << "\nHash MD5 almacenado en la tabla hash para 'titulares.txt'.\n" << endl;

        // Guardar en el archivo de hash como respaldo
        ofstream hashFile("titulares.txt.md5");
        if (hashFile) {
            hashFile << hash;
            hashFile.close();
            cout << "\nHash MD5 guardado en 'titulares.txt.md5'.\n" << endl;
        } else {
            cout << "\nNo se pudo guardar el archivo de hash.\n" << endl;
        }
    } else {
        cout << "\nNo se pudo generar el hash MD5.\n" << endl;
    }

    system("pause");
}

std::string Sistema::generarHashMD5(const std::string& nombreArchivo) {
    ifstream archivo(nombreArchivo, ios::binary);
    if (!archivo) {
        cout << "\nNo se pudo abrir el archivo para calcular el hash.\n" << endl;
        return "";
    }

    // Leer todo el contenido del archivo
    string contenido;
    char c;
    while (archivo.get(c)) {
        contenido += c;
    }
    archivo.close();

    // Cálculo de un hash simple (simulación de MD5)
    unsigned long hash = 0;
    for (char ch : contenido) {
        hash += (unsigned char)ch;
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    // Convertir a hexadecimal (32 caracteres)
    stringstream ss;
    ss << hex << setw(32) << setfill('0') << hash;
    string resultado = ss.str().substr(0, 32);
    return resultado;
}

bool Sistema::compararHashArchivo(const std::string& nombreArchivo) {
    // Generar el hash MD5 actual del archivo de texto
    string hashActual = generarHashMD5(nombreArchivo);
    if (hashActual.empty()) {
        cout << "\nNo se pudo calcular el hash del archivo.\n" << endl;
        return false;
    }

    // Buscar el hash en la tabla hash
    string hashAlmacenado;
    if (!hashes.buscar(nombreArchivo, hashAlmacenado)) {
        cout << "\nNo se encontro un hash registrado para '" << nombreArchivo << "' en la tabla hash.\n" << endl;
        return false;
    }

    // Comparar los hashes
    bool coincide = hashActual == hashAlmacenado;
    if (coincide) {
        cout << "\nHash actual: " << hashActual << "\nHash almacenado: " << hashAlmacenado << endl;
    } else {
        cout << "\nHash actual: " << hashActual << "\nHash almacenado: " << hashAlmacenado << "\nLos hashes no coinciden.\n";
    }
    return coincide;
}