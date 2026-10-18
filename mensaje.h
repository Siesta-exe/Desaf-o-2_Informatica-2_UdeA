#ifndef MENSAJE_H
#define MENSAJE_H

#include <cstddef>
#include <ostream>

class Mensaje {
private:
    char* contenido;
    long long fechaEnvio;
    int emisor;
    bool leido;

public:
    Mensaje();
    Mensaje(const char* contenido, long long fechaEnvio, int emisor, bool leido = false);
    Mensaje(const Mensaje& otro);
    Mensaje& operator=(const Mensaje& otro);
    ~Mensaje();

    const char* getContenido() const;
    long long getFechaEnvio() const;
    int getEmisor() const;
    bool isLeido() const;

    void setContenido(const char* valor);
    void setFechaEnvio(long long valor);
    void setEmisor(int valor);
    void setLeido(bool valor);
    void marcarLeido();

    void intercambiar(Mensaje& otro);

    size_t obtenerMemoria() const;

    friend std::ostream& operator<<(std::ostream& os, const Mensaje& m);
};

#endif
