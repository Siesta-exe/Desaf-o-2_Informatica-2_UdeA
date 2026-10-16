#include "Foto.h"
#include <cstring>
#include <climits>

namespace {
char* copiarCadenaFoto(const char* origen) {
    if (!origen) return nullptr;
    const unsigned long long n = std::strlen(origen);
    char* copia = new char[n + 1];
    for (unsigned long long i = 0; i <= n; ++i) copia[i] = origen[i];
    return copia;
}
}

Foto::Foto() : fechaCaptura(0), tamano(0), ruta(nullptr), likes(0) {}
Foto::Foto(long long fecha, unsigned long long bytes, const char* rutaArchivo, unsigned int cantidadLikes)
    : fechaCaptura(fecha), tamano(bytes), ruta(copiarCadenaFoto(rutaArchivo)), likes(cantidadLikes) {}
Foto::Foto(const Foto& otra)
    : fechaCaptura(otra.fechaCaptura), tamano(otra.tamano), ruta(copiarCadenaFoto(otra.ruta)), likes(otra.likes) {}
Foto& Foto::operator=(const Foto& otra) {
    if (this != &otra) {
        char* nueva = copiarCadenaFoto(otra.ruta);
        delete[] ruta;
        ruta = nueva;
        fechaCaptura = otra.fechaCaptura;
        tamano = otra.tamano;
        likes = otra.likes;
    }
    return *this;
}
bool Foto::operator==(const Foto& otra) const {
    const bool mismasRutas = (ruta == nullptr || otra.ruta == nullptr)
    ? ruta == otra.ruta : std::strcmp(ruta, otra.ruta) == 0;
    return fechaCaptura == otra.fechaCaptura && tamano == otra.tamano &&
           likes == otra.likes && mismasRutas;
}
Foto::~Foto() { delete[] ruta; }
long long Foto::getFechaCaptura() const { return fechaCaptura; }
void Foto::setFechaCaptura(long long valor) { fechaCaptura = valor; }
unsigned long long Foto::getTamano() const { return tamano; }
void Foto::setTamano(unsigned long long valor) { tamano = valor; }
const char* Foto::getRuta() const { return ruta; }
void Foto::setRuta(const char* valor) {
    char* nueva = copiarCadenaFoto(valor); // copiar antes de liberar: admite alias interno
    delete[] ruta;
    ruta = nueva;
}
unsigned int Foto::getLikes() const { return likes; }
void Foto::setLikes(unsigned int valor) { likes = valor; }
bool Foto::incrementarLike() {
    if (likes == UINT_MAX) return false;
    ++likes;
    return true;
}
bool Foto::decrementarLike() {
    if (likes == 0) return false;
    --likes;
    return true;
}
