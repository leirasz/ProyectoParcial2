#ifndef FECHAHORA_H
#define FECHAHORA_H

#include <string>

using namespace std;

class FechaHora {
public:
    FechaHora();

    string obtenerFecha();
    string obtenerHora();

    // Getters
    int getAnio() const;
    int getMes() const;
    int getDia() const;
    int getHora() const;
    int getMinuto() const;
    int getSegundo() const;

    // Setters
    void setAnio(int a);
    void setMes(int m);
    void setDia(int d);
    void setHora(int h);
    void setMinuto(int m);
    void setSegundo(int s);

private:
    int anio, mes, dia, hora, minuto, segundo;

    void actualizarFechaHora();
};

#endif
