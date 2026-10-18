#include "album.h"
#include "utilidades.h"

using namespace util;

namespace {
char normalizarPrivacidad(char c) {
    if (c == 'P' || c == 'p') return 'P';
    if (c == 'A' || c == 'a') return 'A';
    return 'R';
}
}

void Album::redimensionarFotos(int nuevaCapacidad) {
    Foto* nuevoArr = new Foto[nuevaCapacidad];
    for (int i = 0; i < cantidadFotos; i++) {
        nuevoArr[i].intercambiar(fotos[i]);
    }
    delete[] fotos;
    fotos = nuevoArr;
    capacidadFotos = nuevaCapacidad;
}

Album::Album()
    : nombre(nullptr), ruta(nullptr), privacidad('R'),
      fotos(nullptr), cantidadFotos(0), capacidadFotos(0) {}

Album::Album(const char* nombre, const char* ruta, char privacidad)
    : nombre(copiar(nombre)), ruta(copiar(ruta)),
      privacidad(normalizarPrivacidad(privacidad)),
      fotos(nullptr), cantidadFotos(0), capacidadFotos(0) {}

Album::Album(const Album& otro)
    : nombre(copiar(otro.nombre)), ruta(copiar(otro.ruta)),
      privacidad(otro.privacidad), fotos(nullptr),
      cantidadFotos(otro.cantidadFotos), capacidadFotos(otro.cantidadFotos) {
    if (capacidadFotos > 0) {
        fotos = new Foto[capacidadFotos];
        for (int i = 0; i < cantidadFotos; i++) fotos[i] = otro.fotos[i];
    }
}

Album& Album::operator=(const Album& otro) {
    if (this != &otro) {
        char* nuevoNombre = copiar(otro.nombre);
        char* nuevaRuta = copiar(otro.ruta);
        Foto* nuevasFotos = nullptr;
        if (otro.cantidadFotos > 0) {
            nuevasFotos = new Foto[otro.cantidadFotos];
            for (int i = 0; i < otro.cantidadFotos; i++) nuevasFotos[i] = otro.fotos[i];
        }
        delete[] nombre;
        delete[] ruta;
        delete[] fotos;
        nombre = nuevoNombre;
        ruta = nuevaRuta;
        fotos = nuevasFotos;
        privacidad = otro.privacidad;
        cantidadFotos = otro.cantidadFotos;
        capacidadFotos = otro.cantidadFotos;
    }
    return *this;
}

Album::~Album() {
    delete[] nombre;
    delete[] ruta;
    delete[] fotos;
}

const char* Album::getNombre() const { return seguro(nombre); }
const char* Album::getRuta() const { return seguro(ruta); }
char Album::getPrivacidad() const { return privacidad; }
int Album::getCantidadFotos() const { return cantidadFotos; }

const char* Album::getPrivacidadTexto() const {
    if (privacidad == 'P') return "publico";
    if (privacidad == 'A') return "amigos";
    return "privado";
}

Foto* Album::getFoto(int indice) {
    if (indice < 0 || indice >= cantidadFotos) return nullptr;
    return &fotos[indice];
}

const Foto* Album::getFoto(int indice) const {
    if (indice < 0 || indice >= cantidadFotos) return nullptr;
    return &fotos[indice];
}

void Album::setNombre(const char* valor) {
    char* nuevo = copiar(valor);
    delete[] nombre;
    nombre = nuevo;
}

void Album::setRuta(const char* valor) {
    char* nuevo = copiar(valor);
    delete[] ruta;
    ruta = nuevo;
}

void Album::setPrivacidad(char valor) { privacidad = normalizarPrivacidad(valor); }

Foto* Album::agregarFoto(const char* rutaFoto, long long fecha, int tamano, int likes) {
    if (cantidadFotos >= capacidadFotos) {
        redimensionarFotos(capacidadFotos == 0 ? 4 : capacidadFotos * 2);
    }
    if (!fotos || cantidadFotos >= capacidadFotos) return nullptr;
    Foto temporal(rutaFoto, fecha, tamano, likes);
    fotos[cantidadFotos].intercambiar(temporal);
    cantidadFotos++;
    return &fotos[cantidadFotos - 1];
}

bool Album::puedeVisualizar(bool esAmigo, bool esPropietario) const {
    if (esPropietario) return true;
    if (privacidad == 'P') return true;
    if (privacidad == 'A' && esAmigo) return true;
    return false;
}

void Album::intercambiar(Album& otro) {
    char* n = nombre; nombre = otro.nombre; otro.nombre = n;
    char* r = ruta; ruta = otro.ruta; otro.ruta = r;
    char p = privacidad; privacidad = otro.privacidad; otro.privacidad = p;
    Foto* f = fotos; fotos = otro.fotos; otro.fotos = f;
    int c = cantidadFotos; cantidadFotos = otro.cantidadFotos; otro.cantidadFotos = c;
    int k = capacidadFotos; capacidadFotos = otro.capacidadFotos; otro.capacidadFotos = k;
}

size_t Album::obtenerMemoria() const {
    size_t mem = sizeof(*this);
    if (nombre) mem += (longitud(nombre) + 1) * sizeof(char);
    if (ruta) mem += (longitud(ruta) + 1) * sizeof(char);

    for (int i = 0; i < capacidadFotos; i++) mem += fotos[i].obtenerMemoria();
    return mem;
}
