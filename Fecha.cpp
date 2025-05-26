
#include "Fecha.h"

int Fecha::getDia(void)
{
   return dia;
}

void Fecha::setDia(int newDia)
{
   dia = newDia;
}

int Fecha::getMes(void)
{
   return mes;
}

void Fecha::setMes(int newMes)
{
   mes = newMes;
}

Anio Fecha::getAnio(void)
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
