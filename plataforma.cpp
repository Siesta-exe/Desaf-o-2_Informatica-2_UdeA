#include "plataforma.h"
#include "utilidades.h"
#include <fstream>
#include <chrono>

using namespace std;
using namespace util;

namespace {

enum Seccion { SEC_USUARIOS = 0, SEC_ALBUMES, SEC_FOTOS, SEC_RELACIONES, SEC_MENSAJES };

int dividir(char* linea, char* campos[], int maxCampos) {
    int n = 0;
    campos[n++] = linea;
    for (char* p = linea; *p != '\0'; ++p) {
        if (*p == '|' && n < maxCampos) {
            *p = '\0';
            campos[n++] = p + 1;
        }
    }
    return n;
}

void quitarFinDeLinea(char* linea) {
    size_t n = longitud(linea);
    while (n > 0 && (linea[n - 1] == '\r' || linea[n - 1] == '\n')) {
        linea[--n] = '\0';
    }
}

}

Plataforma::Plataforma()
    : usuarios(nullptr), cantidadUsuarios(0), capacidadUsuarios(0),
      matrizCompacta(nullptr), tamanoMatrizBytes(0), indiceUsuarios(nullptr),
      idxUsuarioAutenticado(-1), cantidadFotosInicio(0), cambioFotosUsado(false),
      cambiosPendientes(false), rutaBD(nullptr),
      contadorIteraciones(0), memoriaLocal(0) {
    for (int i = 0; i < FOTOS_INICIO; i++) fotosInicio[i] = -1;
    unsigned int semilla = static_cast<unsigned int>(
        chrono::steady_clock::now().time_since_epoch().count());
    try {
        random_device rd;
        semilla ^= rd();
    } catch (...) {

    }
    generador.seed(semilla);
}

Plataforma::~Plataforma() { liberar(); }

void Plataforma::liberar() {
    delete[] usuarios;
    delete[] matrizCompacta;
    delete[] indiceUsuarios;
    delete[] rutaBD;
    usuarios = nullptr;
    matrizCompacta = nullptr;
    indiceUsuarios = nullptr;
    rutaBD = nullptr;
    cantidadUsuarios = capacidadUsuarios = 0;
    tamanoMatrizBytes = 0;
    idxUsuarioAutenticado = -1;
}

size_t Plataforma::indiceTriangular(int i, int j) const {
    if (i > j) { int t = i; i = j; j = t; }
    size_t a = static_cast<size_t>(i);
    size_t b = static_cast<size_t>(j);
    size_t n = static_cast<size_t>(cantidadUsuarios);
    return a * n - (a * (a + 1)) / 2 + (b - a - 1);
}

int Plataforma::leerEstado(int a, int b) const {
    if (a == b || a < 0 || b < 0 || a >= cantidadUsuarios || b >= cantidadUsuarios) return 0;
    if (!matrizCompacta) return 0;
    size_t k = indiceTriangular(a, b);
    return (matrizCompacta[k / 4] >> ((k % 4) * 2)) & 0x03;
}

void Plataforma::escribirEstado(int a, int b, int estado) {
    if (a == b || a < 0 || b < 0 || a >= cantidadUsuarios || b >= cantidadUsuarios) return;
    if (!matrizCompacta) return;
    size_t k = indiceTriangular(a, b);
    int desplazamiento = static_cast<int>((k % 4) * 2);
    matrizCompacta[k / 4] = static_cast<unsigned char>(
        (matrizCompacta[k / 4] & ~(0x03 << desplazamiento)) | ((estado & 0x03) << desplazamiento));
}

int Plataforma::estadoRelacion(int idxA, int idxB) const { return leerEstado(idxA, idxB); }

bool Plataforma::sonAmigos(int idxA, int idxB) const { return leerEstado(idxA, idxB) == 3; }

void Plataforma::redimensionarUsuarios(int nuevaCapacidad) {
    Usuario* nuevo = new Usuario[nuevaCapacidad];
    for (int i = 0; i < cantidadUsuarios; i++) nuevo[i].intercambiar(usuarios[i]);
    delete[] usuarios;
    usuarios = nuevo;
    capacidadUsuarios = nuevaCapacidad;
}

void Plataforma::construirIndice() {
    delete[] indiceUsuarios;
    indiceUsuarios = nullptr;
    int n = cantidadUsuarios;
    if (n == 0) return;

    int* origen = new int[n];
    int* destino = new int[n];
    for (int i = 0; i < n; i++) origen[i] = i;

    for (int ancho = 1; ancho < n; ancho *= 2) {
        for (int lo = 0; lo < n; lo += 2 * ancho) {
            int mid = (lo + ancho < n) ? lo + ancho : n;
            int hi = (lo + 2 * ancho < n) ? lo + 2 * ancho : n;
            int i = lo, j = mid, k = lo;
            while (i < mid && j < hi) {
                contadorIteraciones++;
                if (comparar(usuarios[origen[i]].getNombreUsuario(),
                                   usuarios[origen[j]].getNombreUsuario()) <= 0) {
                    destino[k++] = origen[i++];
                } else {
                    destino[k++] = origen[j++];
                }
            }
            while (i < mid) { contadorIteraciones++; destino[k++] = origen[i++]; }
            while (j < hi)  { contadorIteraciones++; destino[k++] = origen[j++]; }
        }
        int* t = origen; origen = destino; destino = t;
    }
    indiceUsuarios = origen;
    delete[] destino;
}

void Plataforma::finalizarUsuarios() {
    construirIndice();
    size_t n = static_cast<size_t>(cantidadUsuarios);
    size_t pares = (n * (n - 1)) / 2;
    tamanoMatrizBytes = (pares + 3) / 4;
    delete[] matrizCompacta;
    matrizCompacta = new unsigned char[tamanoMatrizBytes]();
}

int Plataforma::getCantidadUsuarios() const { return cantidadUsuarios; }

int Plataforma::buscarUsuario(const char* nombreUsuario) const {
    if (cantidadUsuarios == 0 || !indiceUsuarios || !nombreUsuario) return -1;
    int izq = 0, der = cantidadUsuarios - 1;
    while (izq <= der) {
        contadorIteraciones++;
        int med = izq + (der - izq) / 2;
        int idxReal = indiceUsuarios[med];
        int comp = comparar(usuarios[idxReal].getNombreUsuario(), nombreUsuario);
        if (comp == 0) return idxReal;
        if (comp < 0) izq = med + 1;
        else der = med - 1;
    }
    return -1;
}

const Usuario* Plataforma::consultarUsuario(int idx) const {
    if (idx < 0 || idx >= cantidadUsuarios) return nullptr;
    return &usuarios[idx];
}

bool Plataforma::cargarDatos(const char* rutaArchivo) {
    ifstream archivo(rutaArchivo);
    if (!archivo.is_open()) return false;

    liberar();
    delete[] rutaBD;
    rutaBD = copiar(rutaArchivo);

    char linea[4096];
    char* campos[8];
    int seccion = SEC_USUARIOS;
    bool usuariosFinalizados = false;

    while (archivo.getline(linea, sizeof(linea))) {
        contadorIteraciones++;
        quitarFinDeLinea(linea);
        if (linea[0] == '\0') continue;

        if (!contiene(linea, '|')) {
            if (seccion == SEC_USUARIOS && !usuariosFinalizados) {
                finalizarUsuarios();
                usuariosFinalizados = true;
            }
            if (comienzaCon(linea, "ALBUMES")) seccion = SEC_ALBUMES;
            else if (comienzaCon(linea, "FOTOS")) seccion = SEC_FOTOS;
            else if (comienzaCon(linea, "RELACIONES")) seccion = SEC_RELACIONES;
            else if (comienzaCon(linea, "MENSAJES")) seccion = SEC_MENSAJES;
            continue;
        }

        if (seccion == SEC_USUARIOS) {
            if (dividir(linea, campos, 8) != 8) continue;
            if (cantidadUsuarios >= capacidadUsuarios) {
                redimensionarUsuarios(capacidadUsuarios == 0 ? 256 : capacidadUsuarios * 2);
            }
            if (!usuarios || cantidadUsuarios >= capacidadUsuarios) break;
            Usuario nuevo(campos[0], campos[1], campos[2], campos[3],
                          campos[4], campos[5], campos[6], campos[7]);
            usuarios[cantidadUsuarios].intercambiar(nuevo);
            cantidadUsuarios++;
        } else if (seccion == SEC_ALBUMES) {
            if (dividir(linea, campos, 4) != 4) continue;
            int idx = buscarUsuario(campos[0]);
            if (idx != -1) usuarios[idx].agregarAlbum(campos[1], campos[2], campos[3][0]);
        } else if (seccion == SEC_FOTOS) {
            if (dividir(linea, campos, 6) != 6) continue;
            int idx = buscarUsuario(campos[0]);
            if (idx == -1) continue;
            int ia = usuarios[idx].buscarAlbum(campos[1], contadorIteraciones);
            if (ia == -1) continue;
            usuarios[idx].getAlbum(ia)->agregarFoto(campos[4], aEntero(campos[2]),
                                                    static_cast<int>(aEntero(campos[3])),
                                                    static_cast<int>(aEntero(campos[5])));
        } else if (seccion == SEC_RELACIONES) {
            if (dividir(linea, campos, 3) != 3) continue;
            long long a = aEntero(campos[0]);
            long long b = aEntero(campos[1]);
            long long e = aEntero(campos[2]);
            if (a < 0 || b < 0 || a >= cantidadUsuarios || b >= cantidadUsuarios || a == b) continue;
            int ia = static_cast<int>(a), ib = static_cast<int>(b);
            if (e == 3) {
                escribirEstado(ia, ib, 3);
            } else if (e == 1 && leerEstado(ia, ib) != 3) {
                escribirEstado(ia, ib, ia < ib ? 1 : 2);
            }
        } else if (seccion == SEC_MENSAJES) {
            if (dividir(linea, campos, 4) != 4) continue;
            long long emisor = aEntero(campos[0]);
            long long dest = aEntero(campos[1]);
            if (emisor < 0 || dest < 0 || emisor >= cantidadUsuarios || dest >= cantidadUsuarios) continue;

            char* ultimo = nullptr;
            for (char* p = campos[3]; *p != '\0'; ++p) if (*p == '|') ultimo = p;
            bool leido = false;
            if (ultimo) { leido = (ultimo[1] == '1'); *ultimo = '\0'; }
            usuarios[dest].recibirMensaje(campos[3], aEntero(campos[2]),
                                          static_cast<int>(emisor), leido, contadorIteraciones);
        }
    }
    archivo.close();

    if (!usuariosFinalizados) finalizarUsuarios();
    cambiosPendientes = false;
    return cantidadUsuarios > 0;
}

bool Plataforma::guardarCambios(const char* rutaArchivo) {
    if (!cambiosPendientes) return true;
    const char* ruta = rutaArchivo ? rutaArchivo : rutaBD;
    if (!ruta) return false;

    ofstream archivo(ruta);
    if (!archivo.is_open()) return false;

    int totalAlbumes = 0, totalFotos = 0, totalMensajes = 0;
    for (int i = 0; i < cantidadUsuarios; i++) {
        totalAlbumes += usuarios[i].getCantidadAlbumes();
        totalMensajes += usuarios[i].getCantidadMensajes();
        for (int a = 0; a < usuarios[i].getCantidadAlbumes(); a++) {
            totalFotos += usuarios[i].getAlbum(a)->getCantidadFotos();
        }
    }

    for (int i = 0; i < cantidadUsuarios; i++) {
        const Usuario& u = usuarios[i];
        archivo << u.getNombreUsuario() << '|' << u.getCorreo() << '|' << u.getContrasena() << '|'
                << u.getFechaNacimiento() << '|' << u.getFechaInscripcion() << '|'
                << u.getCiudad() << '|' << u.getPais() << '|' << u.getFotoPerfil() << '\n';
    }

    archivo << "ALBUMES " << totalAlbumes << '\n';
    for (int i = 0; i < cantidadUsuarios; i++) {
        const Usuario& u = usuarios[i];
        for (int a = 0; a < u.getCantidadAlbumes(); a++) {
            const Album* alb = u.getAlbum(a);
            archivo << u.getNombreUsuario() << '|' << alb->getNombre() << '|'
                    << alb->getRuta() << '|' << alb->getPrivacidad() << '\n';
        }
    }

    archivo << "FOTOS " << totalFotos << '\n';
    for (int i = 0; i < cantidadUsuarios; i++) {
        const Usuario& u = usuarios[i];
        for (int a = 0; a < u.getCantidadAlbumes(); a++) {
            const Album* alb = u.getAlbum(a);
            for (int f = 0; f < alb->getCantidadFotos(); f++) {
                const Foto* foto = alb->getFoto(f);
                archivo << u.getNombreUsuario() << '|' << alb->getNombre() << '|'
                        << foto->getFechaCaptura() << '|' << foto->getTamano() << '|'
                        << foto->getRuta() << '|' << foto->getLikes() << '\n';
            }
        }
    }

    int totalRel = 0;
    size_t pares = (static_cast<size_t>(cantidadUsuarios) * (cantidadUsuarios - 1)) / 2;
    for (size_t k = 0; k < pares; k++) {
        if (((matrizCompacta[k / 4] >> ((k % 4) * 2)) & 0x03) != 0) totalRel++;
    }
    archivo << "RELACIONES " << totalRel << '\n';
    size_t k = 0;
    for (int i = 0; i < cantidadUsuarios; i++) {
        for (int j = i + 1; j < cantidadUsuarios; j++, k++) {
            int e = (matrizCompacta[k / 4] >> ((k % 4) * 2)) & 0x03;
            if (e == 3) archivo << i << '|' << j << "|3\n";
            else if (e == 1) archivo << i << '|' << j << "|1\n";
            else if (e == 2) archivo << j << '|' << i << "|1\n";
        }
    }

    archivo << "MENSAJES " << totalMensajes << '\n';
    for (int d = 0; d < cantidadUsuarios; d++) {
        const Usuario& u = usuarios[d];
        for (int m = 0; m < u.getCantidadMensajes(); m++) {
            const Mensaje* msg = u.getMensaje(m);
            archivo << msg->getEmisor() << '|' << d << '|' << msg->getFechaEnvio() << '|'
                    << msg->getContenido() << '|' << (msg->isLeido() ? 1 : 0) << '\n';
        }
    }

    archivo.close();
    if (archivo.fail()) return false;
    cambiosPendientes = false;
    return true;
}

bool Plataforma::autenticar(const char* nombreUsuario, const char* contrasena) {
    int idx = buscarUsuario(nombreUsuario);
    if (idx == -1) return false;
    if (!iguales(usuarios[idx].getContrasena(), contrasena)) return false;
    idxUsuarioAutenticado = idx;
    cantidadFotosInicio = 0;
    cambioFotosUsado = false;
    return true;
}

void Plataforma::cerrarSesion() {
    idxUsuarioAutenticado = -1;
    cantidadFotosInicio = 0;
    cambioFotosUsado = false;
}

int Plataforma::getSesionActual() const { return idxUsuarioAutenticado; }

int Plataforma::listarAmigos(int idx, int* destino) const {
    int cuenta = 0;
    for (int j = 0; j < cantidadUsuarios; j++) {
        contadorIteraciones++;
        if (j != idx && leerEstado(idx, j) == 3) destino[cuenta++] = j;
    }
    return cuenta;
}

int Plataforma::elegirFotosInicio(int salida[], int maximo, bool excluirPrevias) {
    if (idxUsuarioAutenticado < 0) return 0;
    int n = cantidadUsuarios;
    int* amigos = new int[n];
    int na = listarAmigos(idxUsuarioAutenticado, amigos);
    int* pesos = new int[na > 0 ? na : 1];
    registrarMemoriaLocal(static_cast<size_t>(n) * sizeof(int) + static_cast<size_t>(na) * sizeof(int));

    for (int a = 0; a < na; a++) {
        bool previa = false;
        if (excluirPrevias) {
            for (int p = 0; p < cantidadFotosInicio; p++) {
                contadorIteraciones++;
                if (fotosInicio[p] == amigos[a]) previa = true;
            }
        }
        if (previa) { pesos[a] = 0; continue; }
        int comun = 0;
        for (int g = 0; g < na; g++) {
            contadorIteraciones++;
            if (g != a && leerEstado(amigos[a], amigos[g]) == 3) comun++;
        }
        pesos[a] = (comun <= 10) ? 2 : (comun <= 20 ? 5 : 7);
    }

    int elegidos = 0;
    while (elegidos < maximo) {
        int total = 0;
        for (int a = 0; a < na; a++) { contadorIteraciones++; total += pesos[a]; }
        if (total == 0) break;
        uniform_int_distribution<int> dist(0, total - 1);
        int tiro = dist(generador);
        int acumulado = 0;
        for (int a = 0; a < na; a++) {
            contadorIteraciones++;
            if (pesos[a] == 0) continue;
            acumulado += pesos[a];
            if (tiro < acumulado) {
                salida[elegidos++] = amigos[a];
                pesos[a] = 0;
                break;
            }
        }
    }

    delete[] amigos;
    delete[] pesos;
    return elegidos;
}

int Plataforma::seleccionarFotosInicio(int salida[]) {
    int n = elegirFotosInicio(salida, FOTOS_INICIO, false);
    for (int i = 0; i < n; i++) fotosInicio[i] = salida[i];
    cantidadFotosInicio = n;
    return n;
}

int Plataforma::actualizarFotosInicio(int salida[]) {
    if (cambioFotosUsado) return -1;
    int n = elegirFotosInicio(salida, FOTOS_INICIO, true);
    if (n == 0) return 0;
    for (int i = 0; i < n; i++) fotosInicio[i] = salida[i];
    cantidadFotosInicio = n;
    cambioFotosUsado = true;
    return n;
}

bool Plataforma::solicitudPendienteHaciaMi(int idxOtro) const {
    int yo = idxUsuarioAutenticado;
    if (yo < 0 || idxOtro < 0 || idxOtro >= cantidadUsuarios || idxOtro == yo) return false;
    int e = leerEstado(yo, idxOtro);

    return (idxOtro < yo) ? (e == 1) : (e == 2);
}

Plataforma::ResultadoSolicitud Plataforma::enviarSolicitud(int idxDestino) {
    int yo = idxUsuarioAutenticado;
    if (yo < 0 || idxDestino < 0 || idxDestino >= cantidadUsuarios) return SOL_INVALIDA;
    if (idxDestino == yo) return SOL_A_SI_MISMO;
    int e = leerEstado(yo, idxDestino);
    if (e == 3) return SOL_YA_AMIGOS;
    if (e != 0) return solicitudPendienteHaciaMi(idxDestino) ? SOL_YA_RECIBIDA : SOL_YA_ENVIADA;
    escribirEstado(yo, idxDestino, yo < idxDestino ? 1 : 2);
    cambiosPendientes = true;
    return SOL_OK;
}

int Plataforma::obtenerSolicitudesRecibidas(int salida[], int maximo) const {
    if (idxUsuarioAutenticado < 0) return 0;
    int cuenta = 0;
    for (int i = 0; i < cantidadUsuarios; i++) {
        contadorIteraciones++;
        if (solicitudPendienteHaciaMi(i) && cuenta < maximo) salida[cuenta++] = i;
    }
    return cuenta;
}

bool Plataforma::aceptarSolicitud(int idxOrigen) {
    if (!solicitudPendienteHaciaMi(idxOrigen)) return false;
    escribirEstado(idxUsuarioAutenticado, idxOrigen, 3);
    cambiosPendientes = true;
    return true;
}

int Plataforma::sugerirAmigos(int K, int sugeridosOut[], int coincidenciasOut[]) const {
    int yo = idxUsuarioAutenticado;
    if (yo < 0 || K <= 0) return 0;
    int n = cantidadUsuarios;
    int* amigos = new int[n];
    int na = listarAmigos(yo, amigos);
    registrarMemoriaLocal(static_cast<size_t>(n) * sizeof(int));

    int cantidad = 0;
    for (int u = 0; u < n; u++) {
        contadorIteraciones++;
        if (u == yo || leerEstado(yo, u) != 0) continue;
        int comun = 0;
        for (int a = 0; a < na; a++) {
            contadorIteraciones++;
            if (leerEstado(u, amigos[a]) == 3) comun++;
        }
        if (comun == 0) continue;
        if (cantidad < K || comun > coincidenciasOut[cantidad - 1]) {
            int pos = (cantidad < K) ? cantidad : K - 1;
            while (pos > 0 && coincidenciasOut[pos - 1] < comun) {
                contadorIteraciones++;
                coincidenciasOut[pos] = coincidenciasOut[pos - 1];
                sugeridosOut[pos] = sugeridosOut[pos - 1];
                pos--;
            }
            coincidenciasOut[pos] = comun;
            sugeridosOut[pos] = u;
            if (cantidad < K) cantidad++;
        }
    }
    delete[] amigos;
    return cantidad;
}

Plataforma::ResultadoMensaje Plataforma::enviarMensaje(int idxDestino, const char* contenido) {
    int yo = idxUsuarioAutenticado;
    if (yo < 0 || idxDestino < 0 || idxDestino >= cantidadUsuarios || idxDestino == yo) return MSG_INVALIDO;
    if (!sonAmigos(yo, idxDestino)) return MSG_NO_AMIGO;
    if (!contenido || soloEspacios(contenido)) return MSG_VACIO;
    size_t largo = longitud(contenido);
    if (largo > static_cast<size_t>(MAX_MENSAJE)) return MSG_MUY_LARGO;

    char* limpio = new char[largo + 1];
    for (size_t i = 0; i < largo; i++) {
        contadorIteraciones++;
        char c = contenido[i];
        if (c == '|') c = '/';
        else if (c == '\n' || c == '\r') c = ' ';
        limpio[i] = c;
    }
    limpio[largo] = '\0';
    registrarMemoriaLocal(largo + 1);

    long long ahora = static_cast<long long>(chrono::duration_cast<chrono::seconds>(
        chrono::system_clock::now().time_since_epoch()).count());
    usuarios[idxDestino].recibirMensaje(limpio, ahora, yo, false, contadorIteraciones);
    delete[] limpio;
    cambiosPendientes = true;
    return MSG_OK;
}

int Plataforma::obtenerNoLeidos(int salida[], int maximo) const {
    if (idxUsuarioAutenticado < 0) return 0;
    return usuarios[idxUsuarioAutenticado].obtenerNoLeidos(salida, maximo, contadorIteraciones);
}

void Plataforma::marcarLeido(int idxMsg) {
    if (idxUsuarioAutenticado < 0) return;
    Mensaje* m = usuarios[idxUsuarioAutenticado].getMensaje(idxMsg);
    if (m && !m->isLeido()) {
        m->marcarLeido();
        cambiosPendientes = true;
    }
}

int Plataforma::buscarRecibidos(long long inicio, long long fin, int salida[], int maximo) const {
    if (idxUsuarioAutenticado < 0) return 0;
    return usuarios[idxUsuarioAutenticado].buscarMensajesPorFecha(inicio, fin, salida, maximo,
                                                                  contadorIteraciones);
}

void Plataforma::darLike(int idxUsuario, int idxAlbum, int idxFoto) {
    if (idxUsuario < 0 || idxUsuario >= cantidadUsuarios) return;
    Album* alb = usuarios[idxUsuario].getAlbum(idxAlbum);
    if (!alb) return;
    Foto* f = alb->getFoto(idxFoto);
    if (!f) return;
    f->darLike();
    cambiosPendientes = true;
}

void Plataforma::quitarLike(int idxUsuario, int idxAlbum, int idxFoto) {
    if (idxUsuario < 0 || idxUsuario >= cantidadUsuarios) return;
    Album* alb = usuarios[idxUsuario].getAlbum(idxAlbum);
    if (!alb) return;
    Foto* f = alb->getFoto(idxFoto);
    if (!f) return;
    int antes = f->getLikes();
    f->quitarLike();
    if (f->getLikes() != antes) cambiosPendientes = true;
}

void Plataforma::reiniciarMetricas() {
    contadorIteraciones = 0;
    memoriaLocal = 0;
}

void Plataforma::sumarIteraciones(long long n) const { contadorIteraciones += n; }

void Plataforma::registrarMemoriaLocal(size_t bytes) const { memoriaLocal += bytes; }

long long Plataforma::getContadorIteraciones() const { return contadorIteraciones; }

size_t Plataforma::getMemoriaEstimada() const {
    return medirConsumoMemoriaTotal() + memoriaLocal;
}

size_t Plataforma::medirConsumoMemoriaTotal() const {
    size_t total = sizeof(*this);
    total += tamanoMatrizBytes * sizeof(unsigned char);
    if (indiceUsuarios) total += static_cast<size_t>(cantidadUsuarios) * sizeof(int);
    if (rutaBD) total += (longitud(rutaBD) + 1) * sizeof(char);
    for (int i = 0; i < capacidadUsuarios; i++) total += usuarios[i].obtenerMemoria();
    return total;
}
