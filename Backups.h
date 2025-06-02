#ifndef BACKUPS_H
#define BACKUPS_H
#include <iostream>
#include "ListaDobleCircular.h"
#include "Titular.h"


class Backups {
public:
    static void crearBackup(const ListaDobleCircular<Titular*>& titulares);
    static void restaurarBackup();
};
#endif
