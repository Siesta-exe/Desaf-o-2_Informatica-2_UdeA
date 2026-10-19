#ifndef USUARIO_H
#define USUARIO_H

#include "album.h"
#include "mensaje.h"
#include <cstddef>

class Plataforma;

class Usuario {

    friend class Plataforma;

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


    Album* albumInterno(int indice);
    Mensaje* mensajeInterno(int indice);
    bool setNombreUsuario(const char* valor);

public:
    Usuario();
    Usuario(const char* nom, const char* correo, const char* pass,
            const char* fNac, const char* fIns, const char* ciudad,
            const char* pais, const char* fPerfil);
    Usuario(const Usuario& otro);
    Usuario& operator=(const Usuario& otro);
    ~Usuario();

    // ---- Getters ----
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


    bool setCorreo(const char* valor);
    bool setContrasena(const char* valor);
    bool setFechaNacimiento(const char* valor);
    bool setFechaInscripcion(const char* valor);
    bool setCiudad(const char* valor);
    bool setPais(const char* valor);
    bool setFotoPerfil(const char* valor);


    int agregarAlbum(const char* nombre, const char* ruta, char privacidad);
    int buscarAlbum(const char* nombre) const;
    const Album* getAlbum(int indice) const;
    bool renombrarAlbum(int indice, const char* nuevoNombre);
    bool setPrivacidadAlbum(int indice, char privacidad);
    bool agregarFoto(int indiceAlbum, const char* ruta, long long fecha, int tamano, int likes);
    bool darLikeFoto(int indiceAlbum, int indiceFoto);
    bool quitarLikeFoto(int indiceAlbum, int indiceFoto);


    bool recibirMensaje(const char* contenido, long long fecha, int emisor, bool leido);
    const Mensaje* getMensaje(int indice) const;
    bool marcarMensajeLeido(int indice);
    bool reprogramarMensaje(int indice, long long nuevaFecha);

    int obtenerNoLeidos(int salida[], int maximo) const;
    int buscarMensajesPorFecha(long long inicio, long long fin, int salida[], int maximo) const;

    void intercambiar(Usuario& otro);

    size_t obtenerMemoria() const;
};

#endif
