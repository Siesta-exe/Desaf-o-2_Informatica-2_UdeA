#ifndef ALBUM_H
#define ALBUM_H

#include "foto.h"
#include <cstddef>

class Album {
private:
    char* nombre;
    char* ruta;
    char privacidad;
    Foto* fotos;
    int cantidadFotos;
    int capacidadFotos;

    void redimensionarFotos(int nuevaCapacidad);

public:
    Album();
    Album(const char* nombre, const char* ruta, char privacidad);
    Album(const Album& otro);
    Album& operator=(const Album& otro);
    ~Album();

    const char* getNombre() const;
    const char* getRuta() const;
    char getPrivacidad() const;
    const char* getPrivacidadTexto() const;
    int getCantidadFotos() const;

    Foto* getFoto(int indice);
    const Foto* getFoto(int indice) const;

    void setNombre(const char* valor);
    void setRuta(const char* valor);
    void setPrivacidad(char valor);

    Foto* agregarFoto(const char* ruta, long long fecha, int tamano, int likes);

    bool puedeVisualizar(bool esAmigo, bool esPropietario) const;

    void intercambiar(Album& otro);

    size_t obtenerMemoria() const;
};

#endif
