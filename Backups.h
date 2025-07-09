#ifndef BACKUPS_H
#define BACKUPS_H
#include <iostream>
#include "ListaDobleCircular.h"
#include "Titular.h"
#include "BPlusTreeTitulares.h"


class Backups {
public:
    static void crearBackup(const ListaDobleCircular<Titular*>& titulares);
    bool restaurarBackup(ListaDobleCircular<Titular*>& titulares, BPlusTreeTitulares& arbolTitulares,const std::string& archivo) ;
};
#endif
