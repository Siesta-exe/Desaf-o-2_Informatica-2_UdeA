#ifndef FOTO_H
#define FOTO_H

#include <cstddef>
#include <ostream>

class Foto {
private:
    char* ruta;
    long long fechaCaptura;
    int tamano;
    int likes;

public:
    Foto();
    Foto(const char* ruta, long long fechaCaptura, int tamano, int likes = 0);
    Foto(const Foto& otra);
    Foto& operator=(const Foto& otra);
    ~Foto();

    const char* getRuta() const;
    long long getFechaCaptura() const;
    int getTamano() const;
    int getLikes() const;

    bool setRuta(const char* valor);
    bool setFechaCaptura(long long valor);
    bool setTamano(int valor);
    void setLikes(int valor);

    void darLike();
    void quitarLike();

    void intercambiar(Foto& otra);

    size_t obtenerMemoria() const;

    friend std::ostream& operator<<(std::ostream& os, const Foto& f);
};

#endif
