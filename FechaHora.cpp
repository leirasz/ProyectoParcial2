#include "FechaHora.h"
#include <ctime>

using namespace std;

FechaHora::FechaHora() {
    actualizarFechaHora();
}

void FechaHora::actualizarFechaHora() {
    time_t ahora = time(nullptr);
    tm* tiempoLocal = localtime(&ahora);

    anio = 1900 + tiempoLocal->tm_year;
    mes = 1 + tiempoLocal->tm_mon;
    dia = tiempoLocal->tm_mday;
    hora = tiempoLocal->tm_hour;
    minuto = tiempoLocal->tm_min;
    segundo = tiempoLocal->tm_sec;
}

string FechaHora::obtenerFecha() {
    actualizarFechaHora();
    return to_string(dia) + "/" + to_string(mes) + "/" + to_string(anio);
}

string FechaHora::obtenerHora() {
    actualizarFechaHora();
    return to_string(hora) + ":" + to_string(minuto) + ":" + to_string(segundo);
}
