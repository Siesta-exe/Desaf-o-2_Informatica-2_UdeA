#ifndef USUARIO_H
#define USUARIO_H

#include "Album.h"

class Usuario {
private:
    char* nombre;
    long long fechaNacimiento;
    char* correo;
    char* rutaPerfil;
    char* clave;
    char* ciudad;
    char* pais;
    long long fechaInscripcion;

    Album* albumes;
    int cantidadAlbumes;
    int capacidadAlbumes;
    int* indicesRecibidos; // indices estables del almacen global, no mensajes copiados
    int cantidadRecibidos;
    int capacidadRecibidos;

    bool reservarAlbumes(int capacidadNueva);
    bool reservarIndicesRecibidos(int capacidadNueva);

public:
    Usuario();
    Usuario(const char* nombreUsuario, long long nacimiento,
            const char* email, const char* fotoPerfil, const char* contrasena,
            const char* ciudadUsuario, const char* paisUsuario, long long inscripcion);
    Usuario(const Usuario& otro);
    Usuario& operator=(const Usuario& otro);
    bool operator==(const Usuario& otro) const;
    ~Usuario();

    const char* getNombre() const;
    void setNombre(const char* valor);
    long long getFechaNacimiento() const;
    void setFechaNacimiento(long long valor);
    const char* getCorreo() const;
    void setCorreo(const char* valor);
    const char* getRutaPerfil() const;
    void setRutaPerfil(const char* valor);
    const char* getClave() const;
    void setClave(const char* valor);
    const char* getCiudad() const;
    void setCiudad(const char* valor);
    const char* getPais() const;
    void setPais(const char* valor);
    long long getFechaInscripcion() const;
    void setFechaInscripcion(long long valor);

    const Album* getAlbumes() const; // vista no propietaria
    bool setAlbumes(const Album* valores, int cantidad); // copia profunda
    int getCantidadAlbumes() const;
    bool setCantidadAlbumes(int cantidad);
    int getCapacidadAlbumes() const;
    bool setCapacidadAlbumes(int nuevaCapacidad);

    const int* getIndicesRecibidos() const; // vista no propietaria
    bool setIndicesRecibidos(const int* valores, int cantidad); // copia
    int getCantidadRecibidos() const;
    bool setCantidadRecibidos(int cantidad);
    int getCapacidadRecibidos() const;
    bool setCapacidadRecibidos(int nuevaCapacidad);

    const Album* getAlbum(int indice) const;
    Album* getAlbum(int indice);
    bool agregarAlbum(const Album& valor);
    int buscarAlbumPorNombre(const char* nombreAlbum) const;
    bool agregarIndiceRecibido(int indiceMensaje);
    int getIndiceRecibido(int posicion) const;
};
#endif
