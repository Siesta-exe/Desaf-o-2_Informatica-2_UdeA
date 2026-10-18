#ifndef PLATAFORMA_H
#define PLATAFORMA_H

#include "usuario.h"
#include <cstddef>
#include <random>

class Plataforma {
public:
    static constexpr int FOTOS_INICIO = 3;
    static constexpr int MAX_MENSAJE = 512;

    enum ResultadoSolicitud { SOL_OK = 0, SOL_INVALIDA, SOL_A_SI_MISMO, SOL_YA_AMIGOS,
                              SOL_YA_ENVIADA, SOL_YA_RECIBIDA };

    enum ResultadoMensaje { MSG_OK = 0, MSG_INVALIDO, MSG_NO_AMIGO, MSG_VACIO, MSG_MUY_LARGO };

private:
    Usuario* usuarios;
    int cantidadUsuarios;
    int capacidadUsuarios;

    unsigned char* matrizCompacta;
    size_t tamanoMatrizBytes;

    int* indiceUsuarios;
    int idxUsuarioAutenticado;
    int fotosInicio[FOTOS_INICIO];
    int cantidadFotosInicio;
    bool cambioFotosUsado;
    bool cambiosPendientes;
    char* rutaBD;

    mutable long long contadorIteraciones;
    mutable size_t memoriaLocal;
    std::mt19937 generador;

    Plataforma(const Plataforma&) = delete;
    Plataforma& operator=(const Plataforma&) = delete;

    size_t indiceTriangular(int i, int j) const;
    int leerEstado(int a, int b) const;
    void escribirEstado(int a, int b, int estado);
    void redimensionarUsuarios(int nuevaCapacidad);
    void construirIndice();
    void finalizarUsuarios();
    int listarAmigos(int idx, int* destino) const;
    int elegirFotosInicio(int salida[], int maximo, bool excluirPrevias);
    bool solicitudPendienteHaciaMi(int idxOtro) const;
    void liberar();

public:
    Plataforma();
    ~Plataforma();

    bool cargarDatos(const char* rutaArchivo);
    bool guardarCambios(const char* rutaArchivo = nullptr);

    int getCantidadUsuarios() const;
    int buscarUsuario(const char* nombreUsuario) const;
    const Usuario* consultarUsuario(int idx) const;

    bool autenticar(const char* nombreUsuario, const char* contrasena);
    void cerrarSesion();
    int getSesionActual() const;

    int estadoRelacion(int idxA, int idxB) const;
    bool sonAmigos(int idxA, int idxB) const;

    int seleccionarFotosInicio(int salida[]);

    int actualizarFotosInicio(int salida[]);

    ResultadoSolicitud enviarSolicitud(int idxDestino);
    int obtenerSolicitudesRecibidas(int salida[], int maximo) const;
    bool aceptarSolicitud(int idxOrigen);

    int sugerirAmigos(int K, int sugeridosOut[], int coincidenciasOut[]) const;

    ResultadoMensaje enviarMensaje(int idxDestino, const char* contenido);
    int obtenerNoLeidos(int salida[], int maximo) const;
    void marcarLeido(int idxMsg);
    int buscarRecibidos(long long inicio, long long fin, int salida[], int maximo) const;

    void darLike(int idxUsuario, int idxAlbum, int idxFoto);
    void quitarLike(int idxUsuario, int idxAlbum, int idxFoto);

    void reiniciarMetricas();
    void sumarIteraciones(long long n) const;
    void registrarMemoriaLocal(size_t bytes) const;
    long long getContadorIteraciones() const;
    size_t getMemoriaEstimada() const;
    size_t medirConsumoMemoriaTotal() const;
};

#endif
