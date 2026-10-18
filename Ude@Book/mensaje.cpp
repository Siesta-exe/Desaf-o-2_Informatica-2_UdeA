#include "mensaje.h"
#include "utilidades.h"

using namespace util;

Mensaje::Mensaje() : contenido(nullptr), fechaEnvio(0), emisor(-1), leido(false) {}

Mensaje::Mensaje(const char* contenido, long long fechaEnvio, int emisor, bool leido)
    : contenido(copiar(contenido)), fechaEnvio(fechaEnvio), emisor(emisor), leido(leido) {}

Mensaje::Mensaje(const Mensaje& otro)
    : contenido(copiar(otro.contenido)), fechaEnvio(otro.fechaEnvio),
      emisor(otro.emisor), leido(otro.leido) {}

Mensaje& Mensaje::operator=(const Mensaje& otro) {
    if (this != &otro) {
        char* nuevo = copiar(otro.contenido);
        delete[] contenido;
        contenido = nuevo;
        fechaEnvio = otro.fechaEnvio;
        emisor = otro.emisor;
        leido = otro.leido;
    }
    return *this;
}

Mensaje::~Mensaje() { delete[] contenido; }

const char* Mensaje::getContenido() const { return seguro(contenido); }
long long Mensaje::getFechaEnvio() const { return fechaEnvio; }
int Mensaje::getEmisor() const { return emisor; }
bool Mensaje::isLeido() const { return leido; }

void Mensaje::setContenido(const char* valor) {
    char* nuevo = copiar(valor);
    delete[] contenido;
    contenido = nuevo;
}
void Mensaje::setFechaEnvio(long long valor) { fechaEnvio = valor; }
void Mensaje::setEmisor(int valor) { emisor = valor; }
void Mensaje::setLeido(bool valor) { leido = valor; }
void Mensaje::marcarLeido() { leido = true; }

void Mensaje::intercambiar(Mensaje& otro) {
    char* c = contenido; contenido = otro.contenido; otro.contenido = c;
    long long f = fechaEnvio; fechaEnvio = otro.fechaEnvio; otro.fechaEnvio = f;
    int e = emisor; emisor = otro.emisor; otro.emisor = e;
    bool l = leido; leido = otro.leido; otro.leido = l;
}

size_t Mensaje::obtenerMemoria() const {
    size_t mem = sizeof(*this);
    if (contenido) mem += (longitud(contenido) + 1) * sizeof(char);
    return mem;
}

std::ostream& operator<<(std::ostream& os, const Mensaje& m) {
    char fecha[20];
    formatearFecha(m.fechaEnvio, fecha, true);
    os << "[" << fecha << "] " << m.getContenido();
    return os;
}
