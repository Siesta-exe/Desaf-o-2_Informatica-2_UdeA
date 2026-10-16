#ifndef FOTO_H
#define FOTO_H

class Foto {
private:
    long long fechaCaptura;
    unsigned long long tamano;
    char* ruta;
    unsigned int likes;

public:
    Foto();
    Foto(long long fecha, unsigned long long bytes, const char* rutaArchivo, unsigned int cantidadLikes);
    Foto(const Foto& otra);
    Foto& operator=(const Foto& otra);
    bool operator==(const Foto& otra) const;
    ~Foto();

    long long getFechaCaptura() const;
    void setFechaCaptura(long long valor);
    unsigned long long getTamano() const;
    void setTamano(unsigned long long valor);
    const char* getRuta() const;
    void setRuta(const char* valor);
    unsigned int getLikes() const;
    void setLikes(unsigned int valor);

    bool incrementarLike();
    bool decrementarLike();
};
#endif
