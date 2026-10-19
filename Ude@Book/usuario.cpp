#include "usuario.h"
#include "utilidades.h"
using namespace std;
using namespace util;

void Usuario::redimensionarAlbumes(int nuevaCapacidad) {
    Album* nuevoArr = new Album[nuevaCapacidad];
    sumarIteraciones(cantidadAlbumes);
    for (int i = 0; i < cantidadAlbumes; i++) nuevoArr[i].intercambiar(albumes[i]);
    delete[] albumes;
    albumes = nuevoArr;
    capacidadAlbumes = nuevaCapacidad;
}

void Usuario::redimensionarMensajes(int nuevaCapacidad) {
    Mensaje* nuevoArr = new Mensaje[nuevaCapacidad];
    sumarIteraciones(cantidadMensajes);
    for (int i = 0; i < cantidadMensajes; i++) nuevoArr[i].intercambiar(mensajesRecibidos[i]);
    delete[] mensajesRecibidos;
    mensajesRecibidos = nuevoArr;
    capacidadMensajes = nuevaCapacidad;
}

Usuario::Usuario()
    : nombreUsuario(nullptr), correo(nullptr), contrasena(nullptr),
      fechaNacimiento(nullptr), fechaInscripcion(nullptr), ciudad(nullptr),
      pais(nullptr), fotoPerfil(nullptr),
      albumes(nullptr), cantidadAlbumes(0), capacidadAlbumes(0),
      mensajesRecibidos(nullptr), cantidadMensajes(0), capacidadMensajes(0) {}

Usuario::Usuario(const char* nom, const char* correo, const char* pass,
                 const char* fNac, const char* fIns, const char* ciudad,
                 const char* pais, const char* fPerfil)
    : nombreUsuario(copiar(nom)), correo(copiar(correo)),
      contrasena(copiar(pass)), fechaNacimiento(copiar(fNac)),
      fechaInscripcion(copiar(fIns)), ciudad(copiar(ciudad)),
      pais(copiar(pais)), fotoPerfil(copiar(fPerfil)),
      albumes(nullptr), cantidadAlbumes(0), capacidadAlbumes(0),
      mensajesRecibidos(nullptr), cantidadMensajes(0), capacidadMensajes(0) {}

Usuario::Usuario(const Usuario& otro)
    : nombreUsuario(copiar(otro.nombreUsuario)), correo(copiar(otro.correo)),
      contrasena(copiar(otro.contrasena)),
      fechaNacimiento(copiar(otro.fechaNacimiento)),
      fechaInscripcion(copiar(otro.fechaInscripcion)),
      ciudad(copiar(otro.ciudad)), pais(copiar(otro.pais)),
      fotoPerfil(copiar(otro.fotoPerfil)),
      albumes(nullptr), cantidadAlbumes(otro.cantidadAlbumes), capacidadAlbumes(otro.cantidadAlbumes),
      mensajesRecibidos(nullptr), cantidadMensajes(otro.cantidadMensajes),
      capacidadMensajes(otro.cantidadMensajes) {
    if (capacidadAlbumes > 0) {
        albumes = new Album[capacidadAlbumes];
        sumarIteraciones(cantidadAlbumes);
        for (int i = 0; i < cantidadAlbumes; i++) albumes[i] = otro.albumes[i];
    }
    if (capacidadMensajes > 0) {
        mensajesRecibidos = new Mensaje[capacidadMensajes];
        sumarIteraciones(cantidadMensajes);
        for (int i = 0; i < cantidadMensajes; i++) mensajesRecibidos[i] = otro.mensajesRecibidos[i];
    }
}

Usuario& Usuario::operator=(const Usuario& otro) {
    if (this != &otro) {
        Usuario copia(otro);
        intercambiar(copia);
    }
    return *this;
}

Usuario::~Usuario() {
    delete[] nombreUsuario; delete[] correo; delete[] contrasena;
    delete[] fechaNacimiento; delete[] fechaInscripcion;
    delete[] ciudad; delete[] pais; delete[] fotoPerfil;
    delete[] albumes; delete[] mensajesRecibidos;
}

const char* Usuario::getNombreUsuario() const { return seguro(nombreUsuario); }
const char* Usuario::getCorreo() const { return seguro(correo); }
const char* Usuario::getContrasena() const { return seguro(contrasena); }
const char* Usuario::getFechaNacimiento() const { return seguro(fechaNacimiento); }
const char* Usuario::getFechaInscripcion() const { return seguro(fechaInscripcion); }
const char* Usuario::getCiudad() const { return seguro(ciudad); }
const char* Usuario::getPais() const { return seguro(pais); }
const char* Usuario::getFotoPerfil() const { return seguro(fotoPerfil); }
int Usuario::getCantidadAlbumes() const { return cantidadAlbumes; }
int Usuario::getCantidadMensajes() const { return cantidadMensajes; }

namespace {
void reemplazar(char*& destino, const char* valor) {
    char* nuevo = copiar(valor);
    delete[] destino;
    destino = nuevo;
}
}

bool Usuario::setNombreUsuario(const char* valor) {
    if (!campoValido(valor)) return false;
    reemplazar(nombreUsuario, valor);
    return true;
}

bool Usuario::setCorreo(const char* valor) {
    if (!campoValido(valor) || !contiene(valor, '@')) return false;
    reemplazar(correo, valor);
    return true;
}

bool Usuario::setContrasena(const char* valor) {
    if (!campoValido(valor)) return false;
    reemplazar(contrasena, valor);
    return true;
}

bool Usuario::setFechaNacimiento(const char* valor) {
    long long ts;
    if (!campoValido(valor) || !parsearFecha(valor, ts)) return false;
    reemplazar(fechaNacimiento, valor);
    return true;
}

bool Usuario::setFechaInscripcion(const char* valor) {
    long long ts;
    if (!campoValido(valor) || !parsearFecha(valor, ts)) return false;
    reemplazar(fechaInscripcion, valor);
    return true;
}

bool Usuario::setCiudad(const char* valor) {
    if (!campoValido(valor)) return false;
    reemplazar(ciudad, valor);
    return true;
}

bool Usuario::setPais(const char* valor) {
    if (!campoValido(valor)) return false;
    reemplazar(pais, valor);
    return true;
}

bool Usuario::setFotoPerfil(const char* valor) {
    if (!campoValido(valor)) return false;
    reemplazar(fotoPerfil, valor);
    return true;
}


int Usuario::agregarAlbum(const char* nombre, const char* ruta, char privacidad) {
    if (cantidadAlbumes >= capacidadAlbumes) {
        redimensionarAlbumes(capacidadAlbumes == 0 ? 2 : capacidadAlbumes * 2);
    }
    if (!albumes || cantidadAlbumes >= capacidadAlbumes) return -1;
    Album temporal(nombre, ruta, privacidad);
    albumes[cantidadAlbumes].intercambiar(temporal);
    cantidadAlbumes++;
    return cantidadAlbumes - 1;
}

int Usuario::buscarAlbum(const char* nombre) const {
    for (int i = 0; i < cantidadAlbumes; i++) {
        sumarIteraciones(1);
        if (iguales(albumes[i].getNombre(), nombre)) return i;
    }
    return -1;
}

Album* Usuario::albumInterno(int indice) {
    if (indice < 0 || indice >= cantidadAlbumes) return nullptr;
    return &albumes[indice];
}

const Album* Usuario::getAlbum(int indice) const {
    if (indice < 0 || indice >= cantidadAlbumes) return nullptr;
    return &albumes[indice];
}

bool Usuario::renombrarAlbum(int indice, const char* nuevoNombre) {
    Album* a = albumInterno(indice);
    if (!a || !campoValido(nuevoNombre)) return false;
    int existente = buscarAlbum(nuevoNombre);
    if (existente != -1 && existente != indice) return false;
    return a->setNombre(nuevoNombre);
}

bool Usuario::setPrivacidadAlbum(int indice, char privacidad) {
    Album* a = albumInterno(indice);
    if (!a) return false;
    a->setPrivacidad(privacidad);
    return true;
}

bool Usuario::agregarFoto(int indiceAlbum, const char* ruta, long long fecha, int tamano, int likes) {
    Album* a = albumInterno(indiceAlbum);
    if (!a) return false;
    return a->agregarFoto(ruta, fecha, tamano, likes) != nullptr;
}

bool Usuario::darLikeFoto(int indiceAlbum, int indiceFoto) {
    Album* a = albumInterno(indiceAlbum);
    Foto* f = a ? a->getFoto(indiceFoto) : nullptr;
    if (!f) return false;
    f->darLike();
    return true;
}

bool Usuario::quitarLikeFoto(int indiceAlbum, int indiceFoto) {
    Album* a = albumInterno(indiceAlbum);
    Foto* f = a ? a->getFoto(indiceFoto) : nullptr;
    if (!f) return false;
    int antes = f->getLikes();
    f->quitarLike();
    return f->getLikes() != antes;
}



bool Usuario::recibirMensaje(const char* contenido, long long fecha, int emisor, bool leido) {
    if (cantidadMensajes >= capacidadMensajes) {
        redimensionarMensajes(capacidadMensajes == 0 ? 4 : capacidadMensajes * 2);
    }
    if (!mensajesRecibidos || cantidadMensajes >= capacidadMensajes) return false;
    Mensaje temporal(contenido, fecha, emisor, leido);
    mensajesRecibidos[cantidadMensajes].intercambiar(temporal);
    int pos = cantidadMensajes;
    cantidadMensajes++;

    while (pos > 0 && mensajesRecibidos[pos - 1].getFechaEnvio() > mensajesRecibidos[pos].getFechaEnvio()) {
        sumarIteraciones(1);
        mensajesRecibidos[pos - 1].intercambiar(mensajesRecibidos[pos]);
        pos--;
    }
    return true;
}

Mensaje* Usuario::mensajeInterno(int indice) {
    if (indice < 0 || indice >= cantidadMensajes) return nullptr;
    return &mensajesRecibidos[indice];
}

const Mensaje* Usuario::getMensaje(int indice) const {
    if (indice < 0 || indice >= cantidadMensajes) return nullptr;
    return &mensajesRecibidos[indice];
}

bool Usuario::marcarMensajeLeido(int indice) {
    Mensaje* m = mensajeInterno(indice);
    if (!m || m->isLeido()) return false;
    m->marcarLeido();
    return true;
}

bool Usuario::reprogramarMensaje(int indice, long long nuevaFecha) {
    Mensaje* m = mensajeInterno(indice);
    if (!m || !m->setFechaEnvio(nuevaFecha)) return false;
    int pos = indice;
    while (pos > 0 && mensajesRecibidos[pos - 1].getFechaEnvio() > mensajesRecibidos[pos].getFechaEnvio()) {
        sumarIteraciones(1);
        mensajesRecibidos[pos - 1].intercambiar(mensajesRecibidos[pos]);
        pos--;
    }
    while (pos < cantidadMensajes - 1 &&
           mensajesRecibidos[pos + 1].getFechaEnvio() < mensajesRecibidos[pos].getFechaEnvio()) {
        sumarIteraciones(1);
        mensajesRecibidos[pos + 1].intercambiar(mensajesRecibidos[pos]);
        pos++;
    }
    return true;
}

int Usuario::obtenerNoLeidos(int salida[], int maximo) const {
    int cuenta = 0;
    for (int i = 0; i < cantidadMensajes; i++) {
        sumarIteraciones(1);
        if (!mensajesRecibidos[i].isLeido() && cuenta < maximo) salida[cuenta++] = i;
    }
    return cuenta;
}

int Usuario::buscarMensajesPorFecha(long long inicio, long long fin, int salida[], int maximo) const {
    int izq = 0, der = cantidadMensajes;
    while (izq < der) {
        sumarIteraciones(1);
        int med = izq + (der - izq) / 2;
        if (mensajesRecibidos[med].getFechaEnvio() < inicio) izq = med + 1;
        else der = med;
    }
    int cuenta = 0;
    for (int i = izq; i < cantidadMensajes && mensajesRecibidos[i].getFechaEnvio() <= fin; i++) {
        sumarIteraciones(1);
        if (cuenta < maximo) salida[cuenta++] = i;
    }
    return cuenta;
}

void Usuario::intercambiar(Usuario& otro) {
    char* t;
    t = nombreUsuario; nombreUsuario = otro.nombreUsuario; otro.nombreUsuario = t;
    t = correo; correo = otro.correo; otro.correo = t;
    t = contrasena; contrasena = otro.contrasena; otro.contrasena = t;
    t = fechaNacimiento; fechaNacimiento = otro.fechaNacimiento; otro.fechaNacimiento = t;
    t = fechaInscripcion; fechaInscripcion = otro.fechaInscripcion; otro.fechaInscripcion = t;
    t = ciudad; ciudad = otro.ciudad; otro.ciudad = t;
    t = pais; pais = otro.pais; otro.pais = t;
    t = fotoPerfil; fotoPerfil = otro.fotoPerfil; otro.fotoPerfil = t;

    Album* a = albumes; albumes = otro.albumes; otro.albumes = a;
    int n = cantidadAlbumes; cantidadAlbumes = otro.cantidadAlbumes; otro.cantidadAlbumes = n;
    n = capacidadAlbumes; capacidadAlbumes = otro.capacidadAlbumes; otro.capacidadAlbumes = n;

    Mensaje* m = mensajesRecibidos; mensajesRecibidos = otro.mensajesRecibidos; otro.mensajesRecibidos = m;
    n = cantidadMensajes; cantidadMensajes = otro.cantidadMensajes; otro.cantidadMensajes = n;
    n = capacidadMensajes; capacidadMensajes = otro.capacidadMensajes; otro.capacidadMensajes = n;
}

size_t Usuario::obtenerMemoria() const {
    size_t mem = sizeof(*this);
    const char* cadenas[8] = {nombreUsuario, correo, contrasena, fechaNacimiento,
                              fechaInscripcion, ciudad, pais, fotoPerfil};
    for (int i = 0; i < 8; i++) {
        if (cadenas[i]) mem += (longitud(cadenas[i]) + 1) * sizeof(char);
    }
    for (int i = 0; i < capacidadAlbumes; i++) mem += albumes[i].obtenerMemoria();
    for (int i = 0; i < capacidadMensajes; i++) mem += mensajesRecibidos[i].obtenerMemoria();
    return mem;
}
