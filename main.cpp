#include "Sistema.h"
#include "Marquesina.h"

int main() {
    Marquesina marquesina;
    marquesina.iniciar("TU SISTEMA BANCARIO PREFERIDO");

    Sistema sistema;
    sistema.menuPrincipal();

    marquesina.detener();
    return 0;
}

