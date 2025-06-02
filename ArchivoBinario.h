#ifndef ARCHIVOBINARIO_H
#define ARCHIVOBINARIO_H

#include "Titular.h"
#include "NodoDoble.h"
#include <string>

class ArchivoBinario {
public:
    static void guardar(const ListaDobleCircular<Titular*>& titulares, const std::string& nombreArchivo);
    void cargar(ListaDobleCircular<Titular*>& titulares, const std::string& nombreArchivo);
    static bool cargarBackup(ListaDobleCircular<Titular*>& titulares, const std::string& nombreArchivo);
};

#endif