#include "Mensaje.h"
#include <cstring>

namespace {
// Hasta 512 caracteres más el terminador; nunca leer más allá de este límite.
int longitudValida(const char* origen) {
    if (!origen) return -1;
    int longitud = 0;
    while (longitud <= 512 && origen[longitud] != '\0') ++longitud;
    return (longitud >= 1 && longitud <= 512) ? longitud : -1;
}
char* copiarTexto(const char* origen, int longitud) {
    if (longitud < 0) return nullptr;
    char* copia = new char[longitud + 1];
    for (int i = 0; i < longitud; ++i) copia[i] = origen[i];
    copia[longitud] = '\0';
    return copia;
}
}

Mensaje::Mensaje() : emisor(-1), receptor(-1), fechaEnvio(0), texto(nullptr), leido(false) {}
Mensaje::Mensaje(int origen, int destino, long long fecha, const char* contenido, bool fueLeido)
    : emisor(origen), receptor(destino), fechaEnvio(fecha), texto(nullptr), leido(fueLeido) {
    const int n = longitudValida(contenido);
    if (n >= 0) texto = copiarTexto(contenido, n);
}
Mensaje::Mensaje(const Mensaje& otro)
    : emisor(otro.emisor), receptor(otro.receptor), fechaEnvio(otro.fechaEnvio),
    texto(copiarTexto(otro.texto, longitudValida(otro.texto))), leido(otro.leido) {}
Mensaje& Mensaje::operator=(const Mensaje& otro) {
    if (this != &otro) {
        char* nuevo = copiarTexto(otro.texto, longitudValida(otro.texto));
        delete[] texto;
        texto = nuevo;
        emisor = otro.emisor;
        receptor = otro.receptor;
        fechaEnvio = otro.fechaEnvio;
        leido = otro.leido;
    }
    return *this;
}
bool Mensaje::operator==(const Mensaje& otro) const {
    const bool mismoTexto = (!texto || !otro.texto)
    ? texto == otro.texto : std::strcmp(texto, otro.texto) == 0;
    return emisor == otro.emisor && receptor == otro.receptor &&
           fechaEnvio == otro.fechaEnvio && leido == otro.leido && mismoTexto;
}
Mensaje::~Mensaje() { delete[] texto; }
int Mensaje::getEmisor() const { return emisor; }
void Mensaje::setEmisor(int valor) { emisor = valor; }
int Mensaje::getReceptor() const { return receptor; }
void Mensaje::setReceptor(int valor) { receptor = valor; }
long long Mensaje::getFechaEnvio() const { return fechaEnvio; }
void Mensaje::setFechaEnvio(long long valor) { fechaEnvio = valor; }
const char* Mensaje::getTexto() const { return texto; }
bool Mensaje::setTexto(const char* valor) {
    const int n = longitudValida(valor);
    if (n < 0) return false;
    char* nuevo = copiarTexto(valor, n);
    delete[] texto;
    texto = nuevo;
    return true;
}
bool Mensaje::getLeido() const { return leido; }
void Mensaje::setLeido(bool valor) { leido = valor; }
void Mensaje::marcarLeido() { leido = true; }
bool Mensaje::estaEnRango(long long inicio, long long fin) const {
    return inicio <= fin && fechaEnvio >= inicio && fechaEnvio <= fin;
}
