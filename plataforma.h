#ifndef PLATAFORMA_H
#define PLATAFORMA_H

#include "Usuario.h"
#include "Mensaje.h"

class Plataforma {
private:
    Usuario* usuarios;
    int cantidadUsuarios;
    int capacidadUsuarios;
    int* indiceUsuarios; // permutacion ordenada por nombre, posiciones originales estables
    unsigned char* relaciones; // 2 bits por par, matriz triangular sin diagonal
    Mensaje** mensajes; // cada objeto Mensaje se reserva individualmente
    int cantidadMensajes;
    int capacidadMensajes;
    int sesionActual; // -1 si no existe sesion
    bool fotosActualizadas; // solo para la sesion actual
    unsigned long long contadorIteraciones;
    bool cambiosPendientes;

    bool reservarUsuarios(int nuevaCapacidad);
    bool reservarMensajes(int nuevaCapacidad);
    bool reconstruirIndiceUsuarios();
    unsigned long long posicionRelacion(int a, int b) const;
    bool establecerRelacion(int a, int b, unsigned char estado);
    bool esAmigo(int a, int b) const;
    int contarAmigosEnComun(int a, int b);
    bool indiceUsuarioValido(int indice) const;
    bool existeSesion() const;

public:
    Plataforma();
    Plataforma(const Plataforma& otra);
    Plataforma& operator=(const Plataforma& otra);
    bool operator==(const Plataforma& otra) const;
    ~Plataforma();

    const Usuario* getUsuarios() const;
    bool setUsuarios(const Usuario* valores, int cantidad);
    int getCantidadUsuarios() const;
    bool setCantidadUsuarios(int cantidad);
    int getCapacidadUsuarios() const;
    bool setCapacidadUsuarios(int nuevaCapacidad);
    const int* getIndiceUsuarios() const;
    bool setIndiceUsuarios(const int* valores, int cantidad);
    const unsigned char* getRelaciones() const;
    bool setRelaciones(const unsigned char* datos, unsigned long long bytes);
    Mensaje* const* getMensajes() const;
    bool setMensajes(Mensaje* const* valores, int cantidad); // copia profunda
    int getCantidadMensajes() const;
    bool setCantidadMensajes(int cantidad);
    int getCapacidadMensajes() const;
    bool setCapacidadMensajes(int nuevaCapacidad);
    int getSesionActual() const;
    bool setSesionActual(int indice);
    bool getFotosActualizadas() const;
    void setFotosActualizadas(bool valor);
    unsigned long long getContadorIteraciones() const;
    void setContadorIteraciones(unsigned long long valor);
    bool getCambiosPendientes() const;
    void setCambiosPendientes(bool valor);

    bool cargarDatos(); // implementacion: archivos y formato documentados
    bool guardarCambios();
    bool autenticar(const char* nombre, const char* clave);
    void cerrarSesion();
    int buscarUsuario(const char* nombre); // busqueda binaria propia
    const Usuario* consultarUsuario(int indice) const;
    unsigned char estadoRelacion(int a, int b) const;
    bool enviarSolicitud(int destino);
    bool aceptarSolicitud(int origen);
    int obtenerSolicitudesRecibidas(int* salida, int capacidad);
    int sugerirAmigos(int k, int* salida, int* coincidencias);
    int seleccionarFotosInicio(int* salida, int capacidad);
    bool actualizarFotosInicio(int* salida, int capacidad);
    bool enviarMensaje(int destino, const char* texto, long long fecha);
    int obtenerNoLeidos(int* salida, int capacidad);
    bool marcarLeido(int indiceMensaje);
    int buscarRecibidos(long long inicio, long long fin, int* salida, int capacidad);
    bool darLike(int amigo, int album, int foto);
    bool quitarLike(int amigo, int album, int foto);
    unsigned long long getMemoriaEstimada() const;
    void reiniciarMetricas();
};
#endif
