#ifndef USUARIO_H
#define USUARIO_H

#include "album.h"
#include "mensaje.h"
#include <cstddef>

class Usuario {
private:
    char* nombreUsuario;
    char* correo;
    char* contrasena;
    char* fechaNacimiento;
    char* fechaInscripcion;
    char* ciudad;
    char* pais;
    char* fotoPerfil;

    Album* albumes;
    int cantidadAlbumes;
    int capacidadAlbumes;

    Mensaje* mensajesRecibidos;
    int cantidadMensajes;
    int capacidadMensajes;

    void redimensionarAlbumes(int nuevaCapacidad);
    void redimensionarMensajes(int nuevaCapacidad);

public:
    Usuario();
    Usuario(const char* nom, const char* correo, const char* pass,
            const char* fNac, const char* fIns, const char* ciudad,
            const char* pais, const char* fPerfil);
    Usuario(const Usuario& otro);
    Usuario& operator=(const Usuario& otro);
    ~Usuario();

    const char* getNombreUsuario() const;
    const char* getCorreo() const;
    const char* getContrasena() const;
    const char* getFechaNacimiento() const;
    const char* getFechaInscripcion() const;
    const char* getCiudad() const;
    const char* getPais() const;
    const char* getFotoPerfil() const;
    int getCantidadAlbumes() const;
    int getCantidadMensajes() const;

    void setFotoPerfil(const char* valor);

    Album* agregarAlbum(const char* nombre, const char* ruta, char privacidad);
    int buscarAlbum(const char* nombre, long long& iteraciones) const;
    Album* getAlbum(int indice);
    const Album* getAlbum(int indice) const;

    bool recibirMensaje(const char* contenido, long long fecha, int emisor, bool leido,
                        long long& iteraciones);
    Mensaje* getMensaje(int indice);
    const Mensaje* getMensaje(int indice) const;

    int obtenerNoLeidos(int salida[], int maximo, long long& iteraciones) const;

    int buscarMensajesPorFecha(long long inicio, long long fin, int salida[], int maximo,
                               long long& iteraciones) const;

    void intercambiar(Usuario& otro);

    size_t obtenerMemoria() const;
};

#endif
