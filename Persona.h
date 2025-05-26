#include <string>
#include "Fecha.h"


#ifndef PERSONA_H
#define PERSONA_H

class Persona
{
public:
   std::string getCI();
   void setCI(std::string newCI);
   std::string getTelefono();
   void setTelefono(std::string newTelefono);
   std::string getNombre();
   void setNombre(std::string newNombre);
   std::string getApellido();
   void setApellido(std::string newApellido);
   std::string getCorreo();
   void setCorreo(std::string newCorreo);
   Fecha getFechaNa();
   void setFechaNa(Fecha newFechaNa);
   Persona();
   ~Persona();

protected:
private:
   std::string CI;
   std::string telefono;
   std::string nombre;
   std::string apellido;
   std::string correo;
   Fecha fechaNa;


};

#endif