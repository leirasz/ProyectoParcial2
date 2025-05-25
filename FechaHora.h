#ifndef FECHAHORA_H
#define FECHAHORA_H

#include <string>

using namespace std;

class FechaHora {
public:
    FechaHora();

    string obtenerFecha();
    string obtenerHora();

private:
    int anio, mes, dia, hora, minuto, segundo;

    void actualizarFechaHora();
};

#endif
