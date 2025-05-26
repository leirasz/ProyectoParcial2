#include "Movimiento.h"

Movimiento::Movimiento() : monto(0.0), numeroMovimiento(0) {}

Movimiento::Movimiento(float monto, bool esDeposito, int numeroMovimiento)
    : monto(monto), numeroMovimiento(numeroMovimiento) {
    FechaHora fh;

    fechaMov.setDia(fh.getDia());
    fechaMov.setMes(fh.getMes());
    fechaMov.setAnio(fh.getAnio());

    time.setHoras(fh.getHora());
    time.setMinutos(fh.getMinuto());
    time.setSegundos(fh.getSegundo());

    generarID(esDeposito);
}

Movimiento::~Movimiento() {}

float Movimiento::getMonto() const {
    return monto;
}

void Movimientos::setMonto(float newMonto) {
    monto = newMonto;
}

std::string Movimientos::getIDMovimiento() const {
    return IDMovimiento;
}

void Movimientos::setIDMovimiento(const std::string& newIDMovimiento) {
    IDMovimiento = newIDMovimiento;
}

Fecha Movimientos::getFechaMov() const {
    return fechaMov;
}

void Movimientos::setFechaMov(const Fecha& newFechaMov) {
    fechaMov = newFechaMov;
}

Hora Movimientos::getHora() const {
    return time;
}

void Movimientos::setHora(const Hora& newHora) {
    time = newHora;
}

int Movimientos::getNumeroMovimiento() const {
    return numeroMovimiento;
}

void Movimientos::setNumeroMovimiento(int num) {
    numeroMovimiento = num;
}

void Movimientos::generarID(bool esDeposito) {
    std::string tipo = esDeposito ? "D" : "R";

    std::string diaStr = (fechaMov.getDia() < 10 ? "0" : "") + std::to_string(fechaMov.getDia());
    std::string mesStr = (fechaMov.getMes() < 10 ? "0" : "") + std::to_string(fechaMov.getMes());
    std::string anioStr = std::to_string(fechaMov.getAnio());

    std::string horaStr = (time.getHora() < 10 ? "0" : "") + std::to_string(time.getHora());
    std::string minutoStr = (time.getMinuto() < 10 ? "0" : "") + std::to_string(time.getMinuto());
    std::string segundoStr = (time.getSegundo() < 10 ? "0" : "") + std::to_string(time.getSegundo());

    std::string numMovStr = (numeroMovimiento < 10 ? "00" :
                            numeroMovimiento < 100 ? "0" : "") + std::to_string(numeroMovimiento);

    IDMovimiento = tipo + "-" + diaStr + mesStr + anioStr + "-" +
                   horaStr + minutoStr + segundoStr + "-" + numMovStr;
}
// ID formato es: T-DDMMYYYY-HHMMSS-NNN