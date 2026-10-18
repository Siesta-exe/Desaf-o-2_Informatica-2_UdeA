#include "foto.h"
#include "utilidades.h"

using namespace util;

Foto::Foto() : ruta(nullptr), fechaCaptura(0), tamano(0), likes(0) {}

Foto::Foto(const char* ruta, long long fechaCaptura, int tamano, int likes)
    : ruta(copiar(ruta)), fechaCaptura(fechaCaptura), tamano(tamano), likes(likes) {}

Foto::Foto(const Foto& otra)
    : ruta(copiar(otra.ruta)), fechaCaptura(otra.fechaCaptura),
      tamano(otra.tamano), likes(otra.likes) {}

Foto& Foto::operator=(const Foto& otra) {
    if (this != &otra) {
        char* nuevaRuta = copiar(otra.ruta);
        delete[] ruta;
        ruta = nuevaRuta;
        fechaCaptura = otra.fechaCaptura;
        tamano = otra.tamano;
        likes = otra.likes;
    }
    return *this;
}

Foto::~Foto() { delete[] ruta; }

const char* Foto::getRuta() const { return seguro(ruta); }
long long Foto::getFechaCaptura() const { return fechaCaptura; }
int Foto::getTamano() const { return tamano; }
int Foto::getLikes() const { return likes; }

void Foto::setRuta(const char* valor) {
    char* nueva = copiar(valor);
    delete[] ruta;
    ruta = nueva;
}
void Foto::setFechaCaptura(long long valor) { fechaCaptura = valor; }
void Foto::setTamano(int valor) { tamano = valor; }
void Foto::setLikes(int valor) { likes = valor < 0 ? 0 : valor; }

void Foto::darLike() { likes++; }
void Foto::quitarLike() { if (likes > 0) likes--; }

void Foto::intercambiar(Foto& otra) {
    char* r = ruta; ruta = otra.ruta; otra.ruta = r;
    long long f = fechaCaptura; fechaCaptura = otra.fechaCaptura; otra.fechaCaptura = f;
    int t = tamano; tamano = otra.tamano; otra.tamano = t;
    int l = likes; likes = otra.likes; otra.likes = l;
}

size_t Foto::obtenerMemoria() const {
    size_t mem = sizeof(*this);
    if (ruta) mem += (longitud(ruta) + 1) * sizeof(char);
    return mem;
}

std::ostream& operator<<(std::ostream& os, const Foto& f) {
    char fecha[20];
    formatearFecha(f.fechaCaptura, fecha, false, false);
    os << "Ruta: " << f.getRuta() << " | Likes: " << f.likes << " | Captura: " << fecha;
    return os;
}
