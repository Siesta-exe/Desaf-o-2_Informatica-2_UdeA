#ifndef MENSAJE_H
#define MENSAJE_H

class Mensaje {
private:
    int emisor;
    int receptor;
    long long fechaEnvio; // clave cronologica normalizada
    char* texto; // longitud valida: 1 a 512 caracteres
    bool leido;

public:
    Mensaje();
    Mensaje(int origen, int destino, long long fecha, const char* contenido, bool fueLeido);
    Mensaje(const Mensaje& otro);
    Mensaje& operator=(const Mensaje& otro);
    bool operator==(const Mensaje& otro) const;
    ~Mensaje();

    int getEmisor() const;
    void setEmisor(int valor);
    int getReceptor() const;
    void setReceptor(int valor);
    long long getFechaEnvio() const;
    void setFechaEnvio(long long valor);
    const char* getTexto() const;
    bool setTexto(const char* valor);
    bool getLeido() const;
    void setLeido(bool valor);
    void marcarLeido();
    bool estaEnRango(long long inicio, long long fin) const;
};
#endif
