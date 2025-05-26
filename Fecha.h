#ifndef FECHA_H
#define FECHA_H
#include "Anio.h"
class Fecha
{
public:
   int getDia(void);
   void setDia(int newDia);
   int getMes(void);
   void setMes(int newMes);
   Anio getAnio(void);
   void setAnio(Anio newAnio);
   Fecha();
   ~Fecha();

protected:
private:
   int dia;
   int mes;
   Anio anio;


};
#endif 