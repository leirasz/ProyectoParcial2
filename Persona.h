#include <string>
#include "Fecha.h"


#ifndef PERSONA_H
#define PERSONA_H

class Persona
{
public:
   std::string getCI() const;
   void setCI(std::string newCI);
   std::string getTelefono() const;
   void setTelefono(std::string newTelefono);
   std::string getNombre() const;
   void setNombre(std::string newNombre);
   std::string getApellido() const;
   void setApellido(std::string newApellido);
   std::string getCorreo() const;
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