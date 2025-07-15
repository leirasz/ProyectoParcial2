
#include "Fecha.h"
#include <ctime>

int Fecha::getDia(void) const
{
   return dia;
}

void Fecha::setDia(int newDia)
{
   dia = newDia;
}

int Fecha::getMes(void) const
{
   return mes;
}

void Fecha::setMes(int newMes)
{
   mes = newMes;
}

Anio Fecha::getAnio(void) const
{
   return anio;
}

void Fecha::setAnio(Anio newAnio)
{
   anio = newAnio;
}

Fecha::Fecha()
{
}


Fecha::~Fecha()
{
   // TODO : implement
}

bool Fecha::esValida() const
{
    // Validar año
    time_t t = time(nullptr);
    tm* now = localtime(&t);
    int anioActual = now->tm_year + 1900;
    if (anio.getAnio() < 1900 || anio.getAnio() > anioActual) {
        return false;
    }

    // Validar mes
    if (mes < 1 || mes > 12) {
        return false;
    }

    // Validar día según el mes y si el año es bisiesto
    bool esBisiesto = (anio.getAnio() % 4 == 0 && (anio.getAnio() % 100 != 0 || anio.getAnio() % 400 == 0));
    int diasMes[] = {31, esBisiesto ? 29 : 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (dia < 1 || dia > diasMes[mes - 1]) {
        return false;
    }

    return true;
}
