#ifndef PLATAFORMA_H
#define PLATAFORMA_H

#include "usuario.h"
#include <cstddef>
#include <random>


enum ResultadoSolicitud { SOL_OK = 0, SOL_INVALIDA, SOL_A_SI_MISMO, SOL_YA_AMIGOS,
                          SOL_YA_ENVIADA, SOL_YA_RECIBIDA };

enum ResultadoMensaje { MSG_OK = 0, MSG_INVALIDO, MSG_NO_AMIGO, MSG_VACIO, MSG_MUY_LARGO };

enum ComponenteExterno { EXT_NINGUNO = 0, EXT_ARCHIVOS = 1, EXT_ALEATORIOS = 2, EXT_RELOJ = 4 };

class Plataforma {
public:
    static constexpr int FOTOS_INICIO = 3;
    static constexpr int MAX_MENSAJE = Mensaje::MAX_CONTENIDO;
    static constexpr int MAX_SUGERENCIAS = 20;


    class MarcoLocal {
    private:
        const Plataforma& dueno;
        size_t bytes;
        MarcoLocal(const MarcoLocal&) = delete;
        MarcoLocal& operator=(const MarcoLocal&) = delete;
    public:
        MarcoLocal(const Plataforma& p, size_t b);
        ~MarcoLocal();
    };

private:
    Usuario* usuarios;
    int cantidadUsuarios;
    int capacidadUsuarios;

    unsigned char* matrizCompacta;
    size_t tamanoMatrizBytes;

    int* indiceUsuarios;
    int idxUsuarioAutenticado;
    int* amigosInicio;
    int* pesosInicio;
    int cantidadAmigosInicio;
    bool cambioFotosUsado;
    bool cambiosPendientes;
    bool perfilesModificados;
    char* rutaBD;

    mutable size_t memoriaLocal;
    mutable size_t memoriaLocalPico;
    mutable int componentesExternos;
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
    void prepararAmigosInicio();
    void liberarCacheInicio();
    int sortearFotosInicio(int salida[], int maximo);
    bool solicitudPendienteHaciaMi(int idxOtro) const;
    void liberar();
    int usuarioDeLinea(const char* nombre, int& ultimo) const;
    bool cargarArchivo(int archivo, const char* carpeta);
    bool guardarUsuarios(const char* carpeta) const;
    bool guardarAlbumes(const char* carpeta) const;
    bool guardarFotos(const char* carpeta) const;
    bool guardarRelaciones(const char* carpeta) const;
    bool guardarMensajes(const char* carpeta) const;

public:
    Plataforma();
    ~Plataforma();

    bool cargarDatos(const char* carpeta);
    bool guardarCambios(const char* carpeta = nullptr);


    int getCantidadUsuarios() const;
    int getCapacidadUsuarios() const;
    size_t getTamanoMatrizBytes() const;
    const char* getRutaBD() const;
    bool isCambioFotosUsado() const;
    bool hayCambiosPendientes() const;
    int getSesionActual() const;


    bool setRutaBD(const char* carpeta);
    void setSemilla(unsigned int semilla);
    bool renombrarUsuario(int idx, const char* nuevoNombre);

    int buscarUsuario(const char* nombreUsuario) const;
    const Usuario* consultarUsuario(int idx) const;

    bool autenticar(const char* nombreUsuario, const char* contrasena);
    void cerrarSesion();

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
    void liberarMemoriaLocal(size_t bytes) const;
    void registrarComponenteExterno(int componente) const;
    int getComponentesExternos() const;
    long long getContadorIteraciones() const;


    size_t medirMemoriaEstructuras() const;
    size_t getMemoriaActual() const;
    size_t getMemoriaPico() const;
};

#endif
