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
#include "Cita.h"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <fstream>
#include <iostream>
//#include <SFML/Graphics.hpp>
using namespace std;

Sistema::Sistema(): arbolTitulares(3) {
    listaSucursales.agregarSucursal(Sucursal("Sucursal Central", -34.6037, -58.3816, "123"));
    listaSucursales.agregarSucursal(Sucursal("Sucursal Norte", -34.7000, -58.3000, "456"));
    listaSucursales.agregarSucursal(Sucursal("Sucursal Sur", -34.8000, -58.4000, "789"));
    actualizarContadoresSucursales();
}
/**
 * @brief Destroy the Sistema:: Sistema object
 * 
 */
Sistema::~Sistema() {
    NodoDoble<Titular*>* actual = titulares.getCabeza();
    if (actual != nullptr) {
        do {
            delete actual->dato;
            actual = actual->siguiente;
        } while (actual != titulares.getCabeza());
    }
}
/**
 * @brief   Genera un archivo PDF con los titulares registrados.
 * 
 */
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
/**
 * @brief Muestra el menú principal del sistema bancario y permite al usuario seleccionar opciones.
 * 
 */
void Sistema::menuPrincipal() {
    const char* opciones[] = {
        "Registrar titular",
        "Crear cuenta",
        "Realizar deposito",
        "Realizar retiro",
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
        opcion = menu.ingresarMenu("SISTEMA BANCARIO", opciones, 16); // Update to 17 options
        switch(opcion) {
            case 1: registrarTitular(); break;
            case 2: crearCuenta(); break;
            case 3: realizarDeposito(); break;
            case 4: realizarRetiro(); break;
            //case 5: buscarMovimientosPorFecha(); break;
            case 5: buscarPorTitular(); break;
            case 6: buscarPersonalizada(); break;
            case 7: menuBB(); break;
            case 8: menuSecundario(); break;
            case 9: menuArbol(); break;
            case 10: guardarTitularesEnTxt(); break;
            case 11: {
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
            case 12: {
                system("cls");
                cout << "\n--- MOSTRAR TABLA HASH ---" << endl;
                hashes.mostrarContenido();
                system("pause");
                break;
            }
            case 13: mostrarAyuda(); break;
            case 14: generarQRPDF(); break;
            case 15: generarPDFTitulares(); break; // Nueva opción
            case 16: {Backups backup;
                backup.crearBackup(titulares);
                cout << "\nSaliendo...\n" << endl; break;}
            default: cout << "\nOpcion invalida." << endl; system("pause"); break;
        }
    } while(opcion != 16);
}
/**
 * @brief Genera un código QR básico (versión 1, 21x21) a partir de una cadena de datos.
 * 
 * @param data 
 * @param qrMatrix 
 */
// Generar código QR básico (versión 1, 21x21)
void imprimirQR(const bool qrMatrix[25][25]) {
    cout << "\nMatriz QR generada (■ = negro, espacio = blanco):\n";
    for (int i = 0; i < 25; ++i) {
        for (int j = 0; j < 25; ++j) {
            cout << (qrMatrix[i][j] ? "■" : " ");
        }
        cout << endl;
    }
}

// Funciones para Reed-Solomon en GF(2^8)
vector<int> gf_log(256, 0), gf_exp(511, 0);

void init_gf() {
    int x = 1;
    for (int i = 0; i < 255; ++i) {
        gf_exp[i] = x;
        gf_log[x] = i;
        x = (x << 1) ^ (x & 0x80 ? 0x11D : 0); // Polinomio x^8 + x^4 + x^3 + x^2 + 1
    }
    for (int i = 255; i < 511; ++i) {
        gf_exp[i] = gf_exp[i - 255];
    }
}

int gf_mult(int a, int b) {
    if (a == 0 || b == 0) return 0;
    return gf_exp[(gf_log[a] + gf_log[b]) % 255];
}

int gf_div(int a, int b) {
    if (a == 0) return 0;
    if (b == 0) throw runtime_error("División por cero en GF(2^8)");
    return gf_exp[(gf_log[a] + 255 - gf_log[b]) % 255];
}

vector<int> generate_polynomial(int degree) {
    vector<int> poly(degree + 1);
    poly[0] = 1;
    for (int i = 0; i < degree; ++i) {
        vector<int> temp = poly;
        for (int j = 0; j < poly.size(); ++j) {
            poly[j] = gf_mult(temp[j], gf_exp[i]);
            if (j > 0) poly[j] ^= temp[j - 1];
        }
    }
    return poly;
}

vector<int> reed_solomon(const vector<int>& data, int ec_bytes) {
    vector<int> gen = generate_polynomial(ec_bytes);
    vector<int> msg = data;
    msg.resize(data.size() + ec_bytes, 0);
    for (size_t i = 0; i < data.size(); ++i) {
        if (msg[i] == 0) continue;
        int coef = msg[i];
        for (size_t j = 0; j < gen.size(); ++j) {
            msg[i + j] ^= gf_mult(gen[j], coef);
        }
    }
    vector<int> ec_codes(ec_bytes);
    for (int i = 0; i < ec_bytes; ++i) {
        ec_codes[i] = msg[data.size() + i];
    }
    return ec_codes;
}

void Sistema::generarQR(const string& data, bool qrMatrix[25][25]) {
    // Inicializar la matriz QR
    for (int i = 0; i < 25; ++i) {
        for (int j = 0; j < 25; ++j) {
            qrMatrix[i][j] = false;
        }
    }

    // Patrones fijos (finder patterns)
    auto dibujarFinderPattern = [&](int startX, int startY) {
        for (int i = 0; i < 7; ++i) {
            qrMatrix[startX][startY + i] = true;
            qrMatrix[startX + 6][startY + i] = true;
            qrMatrix[startX + i][startY] = true;
            qrMatrix[startX + i][startY + 6] = true;
        }
        for (int i = 1; i < 6; ++i) {
            for (int j = 1; j < 6; ++j) {
                qrMatrix[startX + i][startY + j] = false;
            }
        }
        for (int i = 2; i < 5; ++i) {
            for (int j = 2; j < 5; ++j) {
                qrMatrix[startX + i][startY + j] = true;
            }
        }
    };

    dibujarFinderPattern(0, 0);   // Superior izquierda
    dibujarFinderPattern(0, 18);  // Superior derecha
    dibujarFinderPattern(18, 0);  // Inferior izquierda

    // Líneas de temporización
    for (int i = 8; i < 17; ++i) {
        qrMatrix[6][i] = (i % 2 == 0);
        qrMatrix[i][6] = (i % 2 == 0);
    }

    // Patrón de alineación
    for (int i = 16; i < 21; ++i) {
        for (int j = 16; j < 21; ++j) {
            if (i == 16 || i == 20 || j == 16 || j == 20) {
                qrMatrix[i][j] = true;
            } else if (i == 17 || i == 19 || j == 17 || j == 19) {
                qrMatrix[i][j] = false;
            } else {
                qrMatrix[i][j] = true;
            }
        }
    }

    // Formato de información (nivel L, máscara 0: 01000 0010111110)
    bool formatInfo[15] = {0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0};
    int formatPositions[15][2] = {
        {0, 8}, {1, 8}, {2, 8}, {3, 8}, {4, 8}, {5, 8}, {7, 8}, {8, 8},
        {8, 7}, {8, 5}, {8, 4}, {8, 3}, {8, 2}, {8, 1}, {8, 0}
    };
    int formatPositionsRight[15][2] = {
        {0, 24}, {1, 24}, {2, 24}, {3, 24}, {4, 24}, {5, 24}, {7, 24}, {8, 24},
        {24, 7}, {24, 5}, {24, 4}, {24, 3}, {24, 2}, {24, 1}, {24, 0}
    };
    for (int i = 0; i < 15; ++i) {
        qrMatrix[formatPositions[i][0]][formatPositions[i][1]] = formatInfo[i];
        qrMatrix[formatPositionsRight[i][0]][formatPositionsRight[i][1]] = formatInfo[i];
    }

    // Codificación de datos (modo alfanumérico)
    const string alfanumerico = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ $%*+-./:";
    auto getCharValue = [&](char c) -> int {
        for (size_t i = 0; i < alfanumerico.size(); ++i) {
            if (toupper(c) == alfanumerico[i]) return i;
        }
        return -1;
    };

    // Validar y preparar datos
    vector<int> valores;
    string cleanData;
    for (char c : data) {
        if (getCharValue(c) != -1) {
            cleanData += toupper(c);
        } else {
            cout << "\nAdvertencia: Carácter inválido '" << c << "' ignorado.\n";
        }
    }
    string truncatedData = cleanData.substr(0, 47); // Máximo 47 caracteres
    cout << "\nDatos para el QR: " << truncatedData << "\n";

    for (char c : truncatedData) {
        int val = getCharValue(c);
        if (val != -1) {
            valores.push_back(val);
        }
    }
    if (valores.empty()) {
        cout << "\nError: No se encontraron caracteres válidos para el QR.\n";
        return;
    }

    // Codificar datos en bits
    vector<bool> bits;
    bits.insert(bits.end(), {0, 0, 1, 0}); // Modo alfanumérico: 0010
    int len = valores.size();
    for (int i = 8; i >= 0; --i) {
        bits.push_back((len >> i) & 1); // Longitud (9 bits)
    }
    for (size_t i = 0; i < valores.size(); i += 2) {
        int val1 = valores[i];
        int val2 = (i + 1 < valores.size()) ? valores[i + 1] : 0;
        int combined = val1 * 45 + val2;
        for (int j = 10; j >= 0; --j) {
            bits.push_back((combined >> j) & 1); // 11 bits por grupo
        }
    }

    // Convertir bits a bytes para Reed-Solomon
    vector<int> data_bytes;
    for (size_t i = 0; i < bits.size(); i += 8) {
        int byte = 0;
        for (int j = 0; j < 8 && (i + j) < bits.size(); ++j) {
            byte |= (bits[i + j] ? 1 : 0) << (7 - j);
        }
        data_bytes.push_back(byte);
    }
    while (data_bytes.size() < 26) { // Rellenar hasta 26 bytes
        data_bytes.push_back(0);
    }

    // Generar códigos de corrección Reed-Solomon
    init_gf();
    vector<int> ec_codes = reed_solomon(data_bytes, 7); // 7 bytes de corrección

    // Convertir bytes de datos y corrección a bits
    bits.clear();
    for (int byte : data_bytes) {
        for (int i = 7; i >= 0; --i) {
            bits.push_back((byte >> i) & 1);
        }
    }
    for (int ec : ec_codes) {
        for (int i = 7; i >= 0; --i) {
            bits.push_back((ec >> i) & 1);
        }
    }

    // Añadir padding hasta 272 bits
    vector<bool> padding = {1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1}; // ECBB
    while (bits.size() < 272) {
        bits.insert(bits.end(), padding.begin(), padding.end());
    }
    if (bits.size() > 272) {
        bits.resize(272);
    }

    // Colocar datos en la matriz (zigzag)
    int bitIndex = 0;
    for (int col = 24; col >= 0 && bitIndex < bits.size(); col -= 2) {
        if (col == 6) continue; // Saltar líneas de temporización
        for (int row = 24; row >= 0 && bitIndex < bits.size(); --row) {
            if (row <= 8 && (col <= 8 || col >= 16)) continue; // Finder patterns
            if (row >= 16 && col <= 8) continue; // Finder inferior izquierda
            if (row >= 16 && col >= 16 && row <= 20 && col <= 20) continue; // Patrón alineación
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
        for (int row = 0; row < 25 && bitIndex < bits.size(); ++row) {
            if (row <= 8 && (col <= 8 || col >= 16)) continue;
            if (row >= 16 && col <= 8) continue;
            if (row >= 16 && col >= 16 && row <= 20 && col <= 20) continue;
            if (qrMatrix[row][col] == false) {
                qrMatrix[row][col] = bits[bitIndex++];
            }
            if (col - 1 >= 0 && qrMatrix[row][col - 1] == false) {
                qrMatrix[row][col - 1] = bitIndex < bits.size() ? bits[bitIndex++] : false;
            }
        }
    }

    // Aplicar máscara 0: (i + j) % 2 == 0
    for (int i = 0; i < 25; ++i) {
        for (int j = 0; j < 25; ++j) {
            if (i <= 8 && (j <= 8 || j >= 16)) continue; // Finder patterns
            if (i >= 16 && j <= 8) continue;
            if (i >= 16 && j >= 16 && i <= 20 && j <= 20) continue; // Patrón alineación
            if (i == 6 || j == 6) continue; // Líneas de temporización
            if ((i + j) % 2 == 0) {
                qrMatrix[i][j] = !qrMatrix[i][j];
            }
        }
    }

    // Imprimir matriz para depuración
    imprimirQR(qrMatrix);
}

void Sistema::generarPDF(const string& nombre, const string& cedula, const string& numeroCuenta, bool qrMatrix[25][25], const string& outputFile) {
    ofstream archivo(outputFile, ios::binary);
    if (!archivo) {
        cout << "\nNo se pudo crear el archivo PDF.\n" << endl;
        return;
    }

    archivo << "%PDF-1.4\n";
    archivo << "1 0 obj\n<< /Type /Catalog /Pages 2 0 R >>\nendobj\n";
    archivo << "2 0 obj\n<< /Type /Pages /Kids [3 0 R] /Count 1 >>\nendobj\n";
    archivo << "3 0 obj\n<< /Type /Page /Parent 2 0 R /Resources << /Font << /F1 4 0 R >> >> /MediaBox [0 0 595 842] /Contents 5 0 R >>\nendobj\n";
    archivo << "4 0 obj\n<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica >>\nendobj\n";

    stringstream contenido;
    contenido << "5 0 obj\n<< /Length " << /* longitud se calculará después */ " >>\nstream\nBT\n/F1 12 Tf\n100 750 Td\n(Código QR - Información de Cuenta Bancaria) Tj\n100 730 Td\n(Titular: " << nombre << ") Tj\n100 710 Td\n(Cédula: " << cedula << ") Tj\n100 690 Td\n(Nº Cuenta: " << numeroCuenta << ") Tj\nET\n";

    float xBase = 100, yBase = 600;
    float moduleSize = 12; // Aumentado para mejor legibilidad
    float quietZone = 48;  // 4 módulos (4 * 12)
    contenido << "q\n1 1 1 rg\n" << (xBase - quietZone) << " " << (yBase - 25 * moduleSize - quietZone) << " " << (25 * moduleSize + 2 * quietZone) << " " << (25 * moduleSize + 2 * quietZone) << " re\nf\nQ\n";
    for (int i = 0; i < 25; ++i) {
        for (int j = 0; j < 25; ++j) {
            if (qrMatrix[i][j]) {
                float x = xBase + j * moduleSize;
                float y = yBase - i * moduleSize;
                contenido << "q\n0 0 0 rg\n" << x << " " << (y - moduleSize) << " " << moduleSize << " " << moduleSize << " re\nf\nQ\n";
            }
        }
    }

    contenido << "endstream\nendobj\n";
    string contenidoStr = contenido.str();
    string longitud = to_string(contenidoStr.size() - string("5 0 obj\n<< /Length  >> stream\n").size() - string("endstream\nendobj\n").size());
    contenidoStr.replace(contenidoStr.find("/Length ") + 8, 0, longitud);
    archivo << contenidoStr;

    archivo << "xref\n0 6\n0000000000 65535 f \n";
    stringstream xref;
    xref << setfill('0') << setw(10);
    size_t offset = 9;
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

    archivo << "trailer\n<< /Size 6 /Root 1 0 R >>\nstartxref\n" << offset << "\n%%EOF\n";
    archivo.close();
    cout << "\nPDF generado exitosamente: " << outputFile << "\n" << endl;
}

void Sistema::generarQRPDF() {
    system("cls");
    cout << "\n--- GENERAR CÓDIGO QR EN PDF ---\n" << endl;

    if (titulares.vacia()) {
        cout << "\nNo hay titulares registrados.\n" << endl;
        system("pause");
        return;
    }

    string cedula = val.ingresarCedula("\nIngrese cédula del titular:");
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

    CuentaBancaria* cuenta = nullptr;
    string tipoCuenta;
    if (titular->getCuentaCorriente()) {
        cuenta = titular->getCuentaCorriente();
        tipoCuenta = "CORRIENTE";
    } else {
        NodoDoble<CuentaBancaria*>* actual = titular->getCuentasAhorro().getCabeza();
        if (actual) {
            cuenta = actual->dato;
            tipoCuenta = "AHORRO";
        }
    }

    if (!cuenta) {
        cout << "\nNo se encontró una cuenta válida para el titular.\n" << endl;
        system("pause");
        return;
    }

    string nombre = titular->getPersona().getNombre() + " " + titular->getPersona().getApellido();
    string numeroCuenta = cuenta->getID();
    string qrData = "CI:" + cedula + ";NOMBRE:" + nombre + ";CUENTA:" + numeroCuenta;
    if (qrData.size() > 47) {
        qrData = qrData.substr(0, 47);
        cout << "\nAdvertencia: Los datos fueron truncados a 47 caracteres para el código QR.\n";
    }

    cout << "\nTitular encontrado:\n";
    cout << "Nombre: " << nombre << "\n";
    cout << "Cédula: " << cedula << "\n";
    cout << "Cuenta seleccionada: " << tipoCuenta << " (ID: " << numeroCuenta << ")\n";

    bool qrMatrix[25][25] = {false};
    generarQR(qrData, qrMatrix);

    string outputFile = "qr_" + cedula + ".pdf";
    generarPDF(nombre, cedula, numeroCuenta, qrMatrix, outputFile);

    system("pause");
}
/**
 * @brief Muestra el menú del árbol B+ y permite al usuario realizar operaciones sobre él.
 * 
 */
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
            case 3: {
                system("cls");
                std::cout << "\n--- GRAFICAR ARBOL B+ (REPRESENTACION TEXTUAL) ---" << std::endl;
                std::cout << "\nMostrando la estructura del Arbol B+ con niveles:\n";
                arbolTitulares.imprimirArbolBPlus();
                
                system("pause");
                break;
            }
            case 4: cout << "\nRegresando al menu principal...\n" << endl; break;
            default: cout << "\nOpcion invalida." << endl; system("pause"); break;
        }
    } while(opcion != 4);
}
/**
 * @brief Muestra el menú secundario del sistema bancario y permite al usuario seleccionar opciones relacionadas con archivos binarios.
 * 
 */
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
/**
 * @brief Registra un nuevo titular en el sistema bancario.
 * 
 */
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
/**
 * @brief Busca un titular por su cédula de identidad (CI).
 * 
 * @param ci 
 * @return Titular* 
 */
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
/**
 * @brief Crea una nueva cuenta bancaria para un titular existente.
 * 
 */
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
/**
 * @brief Realiza un depósito en una cuenta bancaria de un titular.
 * 
 */
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

    float monto;
    do {
    monto = val.ingresarMonto((char*)"\nIngrese monto a depositar:\n");
    if (monto < 10.0) {
        printf("Error: El monto debe ser de 10 dolares o mas.\n");
    }
    } while (monto < 10.0);
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

/**
 * @brief Realiza un retiro de una cuenta bancaria de un titular.
 * 
 */
void Sistema::realizarRetiro() {
    if (titulares.vacia()) {
        cout << "\nNo hay titulares registrados.\n" << endl;
        system("pause");
        return;
    }
    system("cls");
    cout << "\n--- REALIZAR RETIRO ---\n" << endl;

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
    if (monto > cuenta->getSaldo()|| monto<10) {
        cout << "\nSaldo insuficiente para realizar el retiro o monto menor a 10 dolares\n" << endl;
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
   // cuenta->setSaldo(cuenta->getSaldo() - monto);

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

/*void Sistema::buscarMovimientosPorFecha() {
    if (titulares.vacia()) {
        std::cout << "\nNo hay titulares registrados.\n" << std::endl;
        system("pause");
        return;
    }
    system("cls");
    std::cout << "\n--- BUSCAR MOVIMIENTOS POR RANGO DE FECHAS ---\n";

    try {
        // Ingreso y validación de fechas de inicio y fin
        ValidacionFecha valFecha;
        std::string fechaStr1, fechaStr2;
        int d1, m1, a1, d2, m2, a2;
        std::cout << "\nFecha de INICIO:\n";
        if (!valFecha.ingresarFecha(fechaStr1, d1, m1, a1)) {
            std::cout << "\nError al ingresar la fecha de inicio.\n";
            system("pause");
            return;
        }
        std::cout << "\nFecha de FIN:\n";
        if (!valFecha.ingresarFecha(fechaStr2, d2, m2, a2)) {
            std::cout << "\nError al ingresar la fecha de fin.\n";
            system("pause");
            return;
        }

        // Asegurar que la fecha menor sea la inicial
        int iniDia, iniMes, iniAnio, finDia, finMes, finAnio;
        if (valFecha.compararFechas(d1, m1, a1, d2, m2, a2) <= 0) {
            iniDia = d1; iniMes = m1; iniAnio = a1;
            finDia = d2; finMes = m2; finAnio = a2;
        } else {
            iniDia = d2; iniMes = m2; iniAnio = a2;
            finDia = d1; finMes = m1; finAnio = a1;
        }

        auto fechaEnRango = [&](const Fecha& f) {
            if (!f.esValida()) return false; // Suponiendo que Fecha tenga un método esValida()
            int cmpIni = valFecha.compararFechas(iniDia, iniMes, iniAnio, f.getDia(), f.getMes(), f.getAnio().getAnio());
            int cmpFin = valFecha.compararFechas(f.getDia(), f.getMes(), f.getAnio().getAnio(), finDia, finMes, finAnio);
            return cmpIni <= 0 && cmpFin <= 0;
        };

        NodoDoble<Titular*>* actual = titulares.getCabeza();
        bool encontrado = false;
        if (!actual) {
            std::cout << "\nNo hay titulares disponibles.\n" << std::endl;
            system("pause");
            return;
        }

        do {
            if (!actual->dato) {
                actual = actual->siguiente;
                continue;
            }
            Titular* titular = actual->dato;
            Persona p = titular->getPersona();
            std::string nombreTitular = p.getNombre() + " " + p.getApellido();

            // Cuenta corriente
            CuentaBancaria* cc = titular->getCuentaCorriente();
            if (cc) {
                ListaDobleCircular<Movimiento*> movs = cc->getMovimientos();
                NodoDoble<Movimiento*>* nodoMov = movs.getCabeza();
                if (nodoMov) {
                    do {
                        if (nodoMov->dato) {
                            Movimiento* m = nodoMov->dato;
                            Fecha f = m->getFechaMov();
                            if (fechaEnRango(f)) {
                                std::cout << "Cuenta ID: " << cc->getID()
                                          << " | Fecha: " << f.getDia() << "/" << f.getMes() << "/" << f.getAnio().getAnio()
                                          << " | Titular: " << nombreTitular
                                          << " | Monto: " << m->getMonto()
                                          << " | Tipo: " << (m->getTipo() ? "Deposito" : "Retiro")
                                          << std::endl;
                                encontrado = true;
                            }
                        }
                        nodoMov = nodoMov->siguiente;
                    } while (nodoMov && nodoMov != movs.getCabeza());
                }
            }

            // Cuentas de ahorro
            NodoDoble<CuentaBancaria*>* nodoA = titular->getCuentasAhorro().getCabeza();
            if (nodoA) {
                NodoDoble<CuentaBancaria*>* temp = nodoA;
                do {
                    if (temp->dato) {
                        CuentaBancaria* ca = temp->dato;
                        ListaDobleCircular<Movimiento*> movs = ca->getMovimientos();
                        NodoDoble<Movimiento*>* nodoMov = movs.getCabeza();
                        if (nodoMov) {
                            do {
                                if (nodoMov->dato) {
                                    Movimiento* m = nodoMov->dato;
                                    Fecha f = m->getFechaMov();
                                    if (fechaEnRango(f)) {
                                        std::cout << "Cuenta ID: " << ca->getID()
                                                  << " | Fecha: " << f.getDia() << "/" << f.getMes() << "/" << f.getAnio().getAnio()
                                                  << " | Titular: " << nombreTitular
                                                  << " | Monto: " << m->getMonto()
                                                  << " | Tipo: " << (m->getTipo() ? "Deposito" : "Retiro")
                                                  << std::endl;
                                        encontrado = true;
                                    }
                                }
                                nodoMov = nodoMov->siguiente;
                            } while (nodoMov && nodoMov != movs.getCabeza());
                        }
                    }
                    temp = temp->siguiente;
                } while (temp && temp != nodoA);
            }

            actual = actual->siguiente;
        } while (actual && actual != titulares.getCabeza());

        if (!encontrado) {
            std::cout << "\nNo se encontraron movimientos en el rango de fechas.\n" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "\nError inesperado: " << e.what() << "\n";
    }
    system("pause");
}*/
/**
 * @brief Busca titulares por nombre o apellido.
 * 
 */
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
/**
 * @brief Busca titulares y cuentas bancarias utilizando un criterio de búsqueda personalizado.
 * 
 */
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
/**
 * @brief Muestra la ayuda del sistema abriendo un archivo CHM.
 * 
 */
void Sistema::mostrarAyuda() {
    system("cls");
    cout << "\n--- AYUDA DEL SISTEMA ---\n" << endl;
    cout << "Abriendo el archivo de ayuda...\n" << endl;
    // Abre el archivo CHM con la aplicación predeterminada en Windows
    system("start Ayuda-CuentasBancarias.chm");
    system("pause");
}
/**
 * @brief Actualiza los contadores de cuentas en cada sucursal.
 * 
 * Recorre la lista de titulares y sus cuentas, actualizando el contador de cuentas en cada sucursal.
 */
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
/**
 * @brief Muestra el menú de busquedas binarias.
 * 
 */
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
            }case 5: {cout << "\n--- Buscar sucursal mas cercana ---\n";
    cout << " Esta funcion encuentra la sucursal mas cercana a las coordenadas geograficas ingresadas.\n";
    float latUsuario = val.ingresarCoordenada((char*)"\nIngrese latitud:", true);
    float lonUsuario = val.ingresarCoordenada((char*)"\nIngrese longitud:", false);
    Sucursal* sucursal = buscador.sucursalMasCercana(listaSucursales.getCabeza(), latUsuario, lonUsuario);
    if (sucursal) {
        cout << "\nSucursal mas cercana:\n";
        sucursal->imprimir();
        
        // Crear cita para el siguiente día laborable
        FechaHora fechaActual;
        fechaActual.actualizarFechaHora(); // Obtener fecha y hora actual
        Cita cita(sucursal, fechaActual);
        cita.mostrar();
        
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
    /**
         * @brief Guarda los titulares y sus cuentas bancarias en un archivo de texto.
         * 
         */
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
/**
 * @brief Genera un hash MD5 simple del contenido de un archivo.
 * 
 * @param nombreArchivo 
 * @return std::string 
 */
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

/*void Sistema::graficarArbol() {
    // Crear la ventana SFML
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Arbol B+");
    window.setFramerateLimit(60);

    // Cargar la fuente para el texto
    sf::Font font;
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        std::cout << "Error al cargar la fuente Arial." << std::endl;
        return;
    }

    // Obtener la raíz del árbol B+
    NodoBPlus* raiz = arbolTitulares.getRaiz();

    // Función recursiva para dibujar nodos
    auto dibujarNodo = [&](auto& self, NodoBPlus* nodo, float x, float y, float dx, int nivel) -> void {
        if (!nodo) return;

        // Calcular el ancho del nodo basado en el número de claves
        float nodoAncho = 50.0f + (nodo->numClaves * 50.0f); // Aumentar espacio por clave
        float nodoAlto = 40.0f;

        // Crear el rectángulo del nodo
        sf::RectangleShape rect(sf::Vector2f(nodoAncho, nodoAlto));
        rect.setPosition(x - nodoAncho / 2, y);
        rect.setFillColor(nodo->esHoja ? sf::Color(144, 238, 144) : sf::Color(173, 216, 230)); // Verde para hojas, azul para internos
        rect.setOutlineColor(sf::Color::Black);
        rect.setOutlineThickness(1);

        // Crear el texto con las claves
        std::string texto;
        for (int i = 0; i < nodo->numClaves; ++i) {
            texto += nodo->claves[i] + (i < nodo->numClaves - 1 ? ", " : "");
        }
        sf::Text textoNodo(texto, font, 12);
        textoNodo.setFillColor(sf::Color::Black);
        textoNodo.setPosition(x - nodoAncho / 2 + 5, y + 10); // Ajustar posición del texto

        // Dibujar el nodo
        window.draw(rect);
        window.draw(textoNodo);

        // Dibujar conexiones a hijos si no es hoja
        if (!nodo->esHoja) {
            float hijoX = x - ((nodo->numClaves) * dx / 2); // Posición inicial de los hijos
            float nuevoY = y + 80; // Separación vertical
            for (int i = 0; i <= nodo->numClaves; ++i) {
                if (nodo->hijos[i]) {
                    // Dibujar línea de conexión
                    sf::Vertex linea[] = {
                        sf::Vertex(sf::Vector2f(x - nodoAncho / 2 + (i * nodoAncho / (nodo->numClaves + 1)), y + nodoAlto), sf::Color::Black),
                        sf::Vertex(sf::Vector2f(hijoX + i * dx, nuevoY), sf::Color::Black)
                    };
                    window.draw(linea, 2, sf::Lines);
                    // Llamar recursivamente para dibujar el hijo
                    self(self, nodo->hijos[i], hijoX + i * dx, nuevoY, dx / std::max(2.0f, float(nodo->numClaves + 1)), nivel + 1);
                }
            }
        }
    };

    // Bucle principal de la ventana
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                window.close();
            }
        }

        // Limpiar la ventana
        window.clear(sf::Color::White);

        // Dibujar el árbol o un mensaje si está vacío
        if (raiz) {
            dibujarNodo(dibujarNodo, raiz, 600, 50, 300, 0); // Centro en x=600, y=50, separación inicial=300
        } else {
            sf::Text texto("Arbol B+ vacio", font, 20);
            texto.setFillColor(sf::Color::Black);
            texto.setPosition(550, 350);
            window.draw(texto);
        }

        // Mostrar en pantalla
        window.display();
    }
}*/