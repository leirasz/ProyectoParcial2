
#include "Persona.h"
#include <string>
#include "Fecha.h"

std::string Persona::getCI(void)
{
   return CI;
}

void Persona::setCI(std::string newCI)
{
   CI = newCI;
}

std::string Persona::getTelefono(void)
{
   return telefono;
}

void Persona::setTelefono(std::string newTelefono)
{
   telefono = newTelefono;
}

std::string Persona::getNombre(void)
{
   return nombre;
}

void Persona::setNombre(std::string newNombre)
{
   nombre = newNombre;
}

std::string Persona::getApellido(void)
{
   return apellido;
}

void Persona::setApellido(std::string newApellido)
{
   apellido = newApellido;
}

std::string Persona::getCorreo(void)
{
   return correo;
}

void Persona::setCorreo(std::string newCorreo)
{
   correo = newCorreo;
}

Fecha Persona::getFechaNa(void)
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