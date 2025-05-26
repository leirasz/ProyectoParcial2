#ifndef ANIO_H
#define ANIO_H

class Anio
{
public:
   int getAnio(void);
   void setAnio(int newAnio);
   bool getAnioBisiesto(void);
   void setAnioBisiesto(bool newAnioBisiesto);
   Anio();
   ~Anio();

protected:
private:
   int anio;
   bool anioBisiesto;


};

#endif