#include "Anio.h"

int Anio::getAnio(void)
{
   return anio;
}

void Anio::setAnio(int newAnio)
{
   anio = newAnio;
}

bool Anio::getAnioBisiesto(void)
{
   return anioBisiesto;
}

void Anio::setAnioBisiesto(bool newAnioBisiesto)
{
   anioBisiesto = newAnioBisiesto;
}

Anio::Anio()
{
}

Anio::~Anio()
{
   // TODO : implement
}