
#include "Persona.h"
#include <string>
#include "Fecha.h"

std::string Persona::getCI() const
{
   return CI;
}

void Persona::setCI(std::string newCI)
{
   CI = newCI;
}

std::string Persona::getTelefono() const
{
   return telefono;
}

void Persona::setTelefono(std::string newTelefono)
{
   telefono = newTelefono;
}

std::string Persona::getNombre() const
{
   return nombre;
}

void Persona::setNombre(std::string newNombre)
{
   nombre = newNombre;
}

std::string Persona::getApellido() const
{
   return apellido;
}

void Persona::setApellido(std::string newApellido)
{
   apellido = newApellido;
}

std::string Persona::getCorreo() const
{
   return correo;
}

void Persona::setCorreo(std::string newCorreo)
{
   correo = newCorreo;
}

Fecha Persona::getFechaNa()
{
   return fechaNa;
}

void Persona::setFechaNa(Fecha newFechaNa)
{
   fechaNa = newFechaNa;
}

Persona::Persona()
{
}

Persona::~Persona()
{
   // TODO : implement
}