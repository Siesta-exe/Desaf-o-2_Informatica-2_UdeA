#ifndef ALBUM_H
#define ALBUM_H
#include "Foto.h"

class Album {
private:
    char* nombre;
    char* ruta;
    char privacidad; // 'P' publico, 'A' amigos, 'R' privado (convencion interna)
    Foto* fotos;
    int cantidadFotos;
    int capacidadFotos;

    bool reservarFotos(int capacidadNueva);

public:
    Album();
    Album(const char* nombreAlbum, const char* rutaAlbum, char nivelPrivacidad);
    Album(const Album& otro);
    Album& operator=(const Album& otro);
    bool operator==(const Album& otro) const;
    ~Album();

    const char* getNombre() const;
    void setNombre(const char* valor);
    const char* getRuta() const;
    void setRuta(const char* valor);
    char getPrivacidad() const;
    void setPrivacidad(char valor);
    const Foto* getFotos() const; // vista no propietaria
    bool setFotos(const Foto* valores, int cantidad); // copia profunda
    int getCantidadFotos() const;
    bool setCantidadFotos(int cantidad); // solo admite cantidad existente (no inventa fotos)
    int getCapacidadFotos() const;
    bool setCapacidadFotos(int nuevaCapacidad);

    const Foto* getFoto(int indice) const;
    Foto* getFoto(int indice);
    bool agregarFoto(const Foto& valor);
    int buscarFotoPorRuta(const char* rutaArchivo) const;
    bool permiteAcceso(bool esPropietario, bool esAmigo) const;
};
#endif