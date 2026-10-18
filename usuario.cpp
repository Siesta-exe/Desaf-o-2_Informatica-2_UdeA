#include "usuario.h"
#include "utilidades.h"

using namespace util;

void Usuario::redimensionarAlbumes(int nuevaCapacidad) {
    Album* nuevoArr = new Album[nuevaCapacidad];
    for (int i = 0; i < cantidadAlbumes; i++) nuevoArr[i].intercambiar(albumes[i]);
    delete[] albumes;
    albumes = nuevoArr;
    capacidadAlbumes = nuevaCapacidad;
}

void Usuario::redimensionarMensajes(int nuevaCapacidad) {
    Mensaje* nuevoArr = new Mensaje[nuevaCapacidad];
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
        for (int i = 0; i < cantidadAlbumes; i++) albumes[i] = otro.albumes[i];
    }
    if (capacidadMensajes > 0) {
        mensajesRecibidos = new Mensaje[capacidadMensajes];
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

void Usuario::setFotoPerfil(const char* valor) {
    char* nuevo = copiar(valor);
    delete[] fotoPerfil;
    fotoPerfil = nuevo;
}

Album* Usuario::agregarAlbum(const char* nombre, const char* ruta, char privacidad) {
    if (cantidadAlbumes >= capacidadAlbumes) {
        redimensionarAlbumes(capacidadAlbumes == 0 ? 2 : capacidadAlbumes * 2);
    }
    if (!albumes || cantidadAlbumes >= capacidadAlbumes) return nullptr;
    Album temporal(nombre, ruta, privacidad);
    albumes[cantidadAlbumes].intercambiar(temporal);
    cantidadAlbumes++;
    return &albumes[cantidadAlbumes - 1];
}

int Usuario::buscarAlbum(const char* nombre, long long& iteraciones) const {
    for (int i = 0; i < cantidadAlbumes; i++) {
        iteraciones++;
        if (iguales(albumes[i].getNombre(), nombre)) return i;
    }
    return -1;
}

Album* Usuario::getAlbum(int indice) {
    if (indice < 0 || indice >= cantidadAlbumes) return nullptr;
    return &albumes[indice];
}

const Album* Usuario::getAlbum(int indice) const {
    if (indice < 0 || indice >= cantidadAlbumes) return nullptr;
    return &albumes[indice];
}

bool Usuario::recibirMensaje(const char* contenido, long long fecha, int emisor, bool leido,
                             long long& iteraciones) {
    if (cantidadMensajes >= capacidadMensajes) {
        redimensionarMensajes(capacidadMensajes == 0 ? 4 : capacidadMensajes * 2);
    }
    if (!mensajesRecibidos || cantidadMensajes >= capacidadMensajes) return false;
    Mensaje temporal(contenido, fecha, emisor, leido);
    mensajesRecibidos[cantidadMensajes].intercambiar(temporal);
    int pos = cantidadMensajes;
    cantidadMensajes++;

    while (pos > 0 && mensajesRecibidos[pos - 1].getFechaEnvio() > mensajesRecibidos[pos].getFechaEnvio()) {
        iteraciones++;
        mensajesRecibidos[pos - 1].intercambiar(mensajesRecibidos[pos]);
        pos--;
    }
    return true;
}

Mensaje* Usuario::getMensaje(int indice) {
    if (indice < 0 || indice >= cantidadMensajes) return nullptr;
    return &mensajesRecibidos[indice];
}

const Mensaje* Usuario::getMensaje(int indice) const {
    if (indice < 0 || indice >= cantidadMensajes) return nullptr;
    return &mensajesRecibidos[indice];
}

int Usuario::obtenerNoLeidos(int salida[], int maximo, long long& iteraciones) const {
    int cuenta = 0;
    for (int i = 0; i < cantidadMensajes; i++) {
        iteraciones++;
        if (!mensajesRecibidos[i].isLeido() && cuenta < maximo) salida[cuenta++] = i;
    }
    return cuenta;
}

int Usuario::buscarMensajesPorFecha(long long inicio, long long fin, int salida[], int maximo,
                                    long long& iteraciones) const {

    int izq = 0, der = cantidadMensajes;
    while (izq < der) {
        iteraciones++;
        int med = izq + (der - izq) / 2;
        if (mensajesRecibidos[med].getFechaEnvio() < inicio) izq = med + 1;
        else der = med;
    }
    int cuenta = 0;
    for (int i = izq; i < cantidadMensajes && mensajesRecibidos[i].getFechaEnvio() <= fin; i++) {
        iteraciones++;
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
