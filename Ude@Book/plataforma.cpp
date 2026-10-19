#include "plataforma.h"
#include "utilidades.h"
#include <fstream>
#include <chrono>

using namespace std;
using namespace chrono;
using namespace util;

namespace {

enum Archivo { ARCH_USUARIOS = 0, ARCH_ALBUMES, ARCH_FOTOS, ARCH_RELACIONES, ARCH_MENSAJES };

const char* const NOMBRES_ARCHIVOS[5] = {
    "usuarios.txt", "albumes.txt", "fotos.txt", "relaciones.txt", "mensajes.txt"
};

char* unirRuta(const char* carpeta, const char* nombre, size_t& bytes) {
    size_t a = longitud(carpeta);
    size_t b = longitud(nombre);
    bool separador = a > 0 && carpeta[a - 1] != '/' && carpeta[a - 1] != '\\';
    bytes = a + b + 2;
    char* ruta = new char[bytes];
    size_t p = 0;
    sumarIteraciones(static_cast<long long>(a + b));
    for (size_t i = 0; i < a; i++) ruta[p++] = carpeta[i];
    if (separador) ruta[p++] = '/';
    for (size_t i = 0; i < b; i++) ruta[p++] = nombre[i];
    ruta[p] = '\0';
    return ruta;
}

int dividir(char* linea, char* campos[], int maxCampos) {
    int n = 0;
    campos[n++] = linea;
    for (char* p = linea; *p != '\0'; ++p) {
        sumarIteraciones(1);
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
        sumarIteraciones(1);
        linea[--n] = '\0';
    }
}

}

Plataforma::MarcoLocal::MarcoLocal(const Plataforma& p, size_t b) : dueno(p), bytes(b) {
    dueno.registrarMemoriaLocal(bytes);
}

Plataforma::MarcoLocal::~MarcoLocal() { dueno.liberarMemoriaLocal(bytes); }

Plataforma::Plataforma()
    : usuarios(nullptr), cantidadUsuarios(0), capacidadUsuarios(0),
      matrizCompacta(nullptr), tamanoMatrizBytes(0), indiceUsuarios(nullptr),
      idxUsuarioAutenticado(-1), amigosInicio(nullptr), pesosInicio(nullptr),
      cantidadAmigosInicio(0), cambioFotosUsado(false),
      cambiosPendientes(false), perfilesModificados(false), rutaBD(nullptr),
      memoriaLocal(0), memoriaLocalPico(0), componentesExternos(0) {
    unsigned int semilla = static_cast<unsigned int>(
        steady_clock::now().time_since_epoch().count());
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
    liberarCacheInicio();
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
    sumarIteraciones(cantidadUsuarios);
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


    MarcoLocal marco(*this, 8 * sizeof(int) + 3 * sizeof(int*));
    MarcoLocal temporales(*this, 2 * static_cast<size_t>(n) * sizeof(int));
    int* origen = new int[n];
    int* destino = new int[n];
    sumarIteraciones(n);
    for (int i = 0; i < n; i++) origen[i] = i;

    for (int ancho = 1; ancho < n; ancho *= 2) {
        sumarIteraciones(1);
        for (int lo = 0; lo < n; lo += 2 * ancho) {
            sumarIteraciones(1);
            int mid = (lo + ancho < n) ? lo + ancho : n;
            int hi = (lo + 2 * ancho < n) ? lo + 2 * ancho : n;
            int i = lo, j = mid, k = lo;
            while (i < mid && j < hi) {
                sumarIteraciones(1);
                if (comparar(usuarios[origen[i]].getNombreUsuario(),
                                   usuarios[origen[j]].getNombreUsuario()) <= 0) {
                    destino[k++] = origen[i++];
                } else {
                    destino[k++] = origen[j++];
                }
            }
            while (i < mid) { sumarIteraciones(1); destino[k++] = origen[i++]; }
            while (j < hi)  { sumarIteraciones(1); destino[k++] = origen[j++]; }
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
int Plataforma::getCapacidadUsuarios() const { return capacidadUsuarios; }
size_t Plataforma::getTamanoMatrizBytes() const { return tamanoMatrizBytes; }
const char* Plataforma::getRutaBD() const { return seguro(rutaBD); }
bool Plataforma::isCambioFotosUsado() const { return cambioFotosUsado; }
bool Plataforma::hayCambiosPendientes() const { return cambiosPendientes || perfilesModificados; }
int Plataforma::getSesionActual() const { return idxUsuarioAutenticado; }

bool Plataforma::setRutaBD(const char* carpeta) {
    if (!campoValido(carpeta, true)) return false;
    char* nueva = copiar(carpeta);
    delete[] rutaBD;
    rutaBD = nueva;
    return true;
}

void Plataforma::setSemilla(unsigned int semilla) { generador.seed(semilla); }

bool Plataforma::renombrarUsuario(int idx, const char* nuevoNombre) {
    MarcoLocal marco(*this, sizeof(idx) + sizeof(nuevoNombre) + sizeof(int));
    if (idx < 0 || idx >= cantidadUsuarios || !campoValido(nuevoNombre)) return false;
    if (iguales(usuarios[idx].getNombreUsuario(), nuevoNombre)) return true;
    if (buscarUsuario(nuevoNombre) != -1) return false;
    if (!usuarios[idx].setNombreUsuario(nuevoNombre)) return false;
    construirIndice();
    perfilesModificados = true;
    cambiosPendientes = true;
    return true;
}

int Plataforma::buscarUsuario(const char* nombreUsuario) const {
    if (cantidadUsuarios == 0 || !indiceUsuarios || !nombreUsuario) return -1;
    MarcoLocal marco(*this, sizeof(nombreUsuario) + 5 * sizeof(int));   // izq, der, med, idxReal, comp
    int izq = 0, der = cantidadUsuarios - 1;
    while (izq <= der) {
        sumarIteraciones(1);
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

int Plataforma::usuarioDeLinea(const char* nombre, int& ultimo) const {
    if (ultimo != -1) {
        if (iguales(usuarios[ultimo].getNombreUsuario(), nombre)) return ultimo;
    }
    ultimo = buscarUsuario(nombre);
    return ultimo;
}

// ---------------- Carga ----------------

bool Plataforma::cargarArchivo(int archivo, const char* carpeta) {
    size_t bytesRuta = 0;
    char* ruta = unirRuta(carpeta, NOMBRES_ARCHIVOS[archivo], bytesRuta);
    registrarMemoriaLocal(bytesRuta);
    registrarComponenteExterno(EXT_ARCHIVOS);
    ifstream entrada(ruta);
    delete[] ruta;
    liberarMemoriaLocal(bytesRuta);
    if (!entrada.is_open()) return false;

    char linea[4096];
    char* campos[8];
    int ultimoUsuario = -1;
    int ultimoAlbum = -1;


    MarcoLocal marco(*this, sizeof(archivo) + sizeof(carpeta) + sizeof(ruta) + sizeof(bytesRuta) +
                            sizeof(entrada) + sizeof(linea) + sizeof(campos) +
                            sizeof(ultimoUsuario) + sizeof(ultimoAlbum) +
                            sizeof(Usuario) + 6 * sizeof(int) + 5 * sizeof(long long) +
                            2 * sizeof(char*) + sizeof(bool));

    while (entrada.getline(linea, sizeof(linea))) {
        sumarIteraciones(1);
        quitarFinDeLinea(linea);
        if (!contiene(linea, '|')) continue;

        if (archivo == ARCH_USUARIOS) {
            if (dividir(linea, campos, 8) != 8) continue;
            if (cantidadUsuarios >= capacidadUsuarios) {
                redimensionarUsuarios(capacidadUsuarios == 0 ? 256 : capacidadUsuarios * 2);
            }
            if (!usuarios || cantidadUsuarios >= capacidadUsuarios) break;
            Usuario nuevo(campos[0], campos[1], campos[2], campos[3],
                          campos[4], campos[5], campos[6], campos[7]);
            usuarios[cantidadUsuarios].intercambiar(nuevo);
            cantidadUsuarios++;
        } else if (archivo == ARCH_ALBUMES) {
            if (dividir(linea, campos, 4) != 4) continue;
            int idx = usuarioDeLinea(campos[0], ultimoUsuario);
            if (idx != -1) usuarios[idx].agregarAlbum(campos[1], campos[2], campos[3][0]);
        } else if (archivo == ARCH_FOTOS) {
            if (dividir(linea, campos, 6) != 6) continue;
            int previo = ultimoUsuario;
            int idx = usuarioDeLinea(campos[0], ultimoUsuario);
            if (idx == -1) continue;
            if (idx != previo) ultimoAlbum = -1;
            int ia = -1;
            if (idx == ultimoUsuario && ultimoAlbum != -1 && iguales(
                    usuarios[idx].getAlbum(ultimoAlbum)->getNombre(), campos[1])) {
                ia = ultimoAlbum;
            } else {
                ia = usuarios[idx].buscarAlbum(campos[1]);
            }
            if (ia == -1) continue;
            ultimoUsuario = idx;
            ultimoAlbum = ia;
            usuarios[idx].agregarFoto(ia, campos[4], aEntero(campos[2]),
                                      static_cast<int>(aEntero(campos[3])),
                                      static_cast<int>(aEntero(campos[5])));
        } else if (archivo == ARCH_RELACIONES) {
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
        } else {
            if (dividir(linea, campos, 4) != 4) continue;
            long long emisor = aEntero(campos[0]);
            long long dest = aEntero(campos[1]);
            if (emisor < 0 || dest < 0 || emisor >= cantidadUsuarios || dest >= cantidadUsuarios) continue;
            char* ultimo = nullptr;
            for (char* p = campos[3]; *p != '\0'; ++p) {
                sumarIteraciones(1);
                if (*p == '|') ultimo = p;
            }
            bool leido = false;
            if (ultimo) { leido = (ultimo[1] == '1'); *ultimo = '\0'; }
            usuarios[dest].recibirMensaje(campos[3], aEntero(campos[2]),
                                          static_cast<int>(emisor), leido);
        }
    }
    return true;
}

bool Plataforma::cargarDatos(const char* carpeta) {
    int a = 0;
    MarcoLocal marco(*this, sizeof(carpeta) + sizeof(a));
    liberar();
    rutaBD = copiar(carpeta);

    if (!cargarArchivo(ARCH_USUARIOS, carpeta)) return false;
    finalizarUsuarios();
    if (cantidadUsuarios == 0) return false;

    for (a = ARCH_ALBUMES; a <= ARCH_MENSAJES; a++) {
        sumarIteraciones(1);
        if (!cargarArchivo(a, carpeta)) return false;
    }
    cambiosPendientes = false;
    perfilesModificados = false;
    return true;
}



bool Plataforma::guardarUsuarios(const char* carpeta) const {
    size_t bytesRuta = 0;
    char* ruta = unirRuta(carpeta, NOMBRES_ARCHIVOS[ARCH_USUARIOS], bytesRuta);
    registrarMemoriaLocal(bytesRuta);
    registrarComponenteExterno(EXT_ARCHIVOS);
    ofstream salida(ruta);
    delete[] ruta;
    liberarMemoriaLocal(bytesRuta);
    if (!salida.is_open()) return false;

    MarcoLocal marco(*this, sizeof(carpeta) + sizeof(bytesRuta) + sizeof(ruta) + sizeof(int*) + sizeof(int));
    for (int i = 0; i < cantidadUsuarios; i++) {
        sumarIteraciones(1);
        const Usuario& u = usuarios[i];
        salida << u.getNombreUsuario() << '|' << u.getCorreo() << '|' << u.getContrasena() << '|'
               << u.getFechaNacimiento() << '|' << u.getFechaInscripcion() << '|' << u.getCiudad() << '|'
               << u.getPais() << '|' << u.getFotoPerfil() << '\n';
    }
    salida.close();
    return !salida.fail();
}

bool Plataforma::guardarAlbumes(const char* carpeta) const {
    size_t bytesRuta = 0;
    char* ruta = unirRuta(carpeta, NOMBRES_ARCHIVOS[ARCH_ALBUMES], bytesRuta);
    registrarMemoriaLocal(bytesRuta);
    registrarComponenteExterno(EXT_ARCHIVOS);
    ofstream salida(ruta);
    delete[] ruta;
    liberarMemoriaLocal(bytesRuta);
    if (!salida.is_open()) return false;

    MarcoLocal marco(*this, sizeof(carpeta) + sizeof(bytesRuta) + sizeof(ruta) + sizeof(int*) + 2 * sizeof(int));
    for (int i = 0; i < cantidadUsuarios; i++) {
        sumarIteraciones(1);
        const Usuario& u = usuarios[i];
        for (int a = 0; a < u.getCantidadAlbumes(); a++) {
            sumarIteraciones(1);
            const Album* alb = u.getAlbum(a);
            salida << u.getNombreUsuario() << '|' << alb->getNombre() << '|' << alb->getRuta() << '|'
                   << alb->getPrivacidad() << '\n';
        }
    }
    salida.close();
    return !salida.fail();
}

bool Plataforma::guardarFotos(const char* carpeta) const {
    size_t bytesRuta = 0;
    char* ruta = unirRuta(carpeta, NOMBRES_ARCHIVOS[ARCH_FOTOS], bytesRuta);
    registrarMemoriaLocal(bytesRuta);
    registrarComponenteExterno(EXT_ARCHIVOS);
    ofstream salida(ruta);
    delete[] ruta;
    liberarMemoriaLocal(bytesRuta);
    if (!salida.is_open()) return false;

    MarcoLocal marco(*this, sizeof(carpeta) + sizeof(bytesRuta) + sizeof(ruta) + sizeof(int*) + 3 * sizeof(int));
    for (int i = 0; i < cantidadUsuarios; i++) {
        sumarIteraciones(1);
        const Usuario& u = usuarios[i];
        for (int a = 0; a < u.getCantidadAlbumes(); a++) {
            sumarIteraciones(1);
            const Album* alb = u.getAlbum(a);
            for (int f = 0; f < alb->getCantidadFotos(); f++) {
                sumarIteraciones(1);
                const Foto* foto = alb->getFoto(f);
                salida << u.getNombreUsuario() << '|' << alb->getNombre() << '|'
                       << foto->getFechaCaptura() << '|' << foto->getTamano() << '|'
                       << foto->getRuta() << '|' << foto->getLikes() << '\n';
            }
        }
    }
    salida.close();
    return !salida.fail();
}

bool Plataforma::guardarRelaciones(const char* carpeta) const {
    size_t bytesRuta = 0;
    char* ruta = unirRuta(carpeta, NOMBRES_ARCHIVOS[ARCH_RELACIONES], bytesRuta);
    registrarMemoriaLocal(bytesRuta);
    registrarComponenteExterno(EXT_ARCHIVOS);
    ofstream salida(ruta);
    delete[] ruta;
    liberarMemoriaLocal(bytesRuta);
    if (!salida.is_open()) return false;

    MarcoLocal marco(*this, sizeof(carpeta) + sizeof(bytesRuta) + sizeof(ruta) + sizeof(int*) +
                            sizeof(size_t) + 3 * sizeof(int));
    size_t k = 0;
    for (int i = 0; i < cantidadUsuarios; i++) {
        sumarIteraciones(1);
        for (int j = i + 1; j < cantidadUsuarios; j++, k++) {
            sumarIteraciones(1);
            int e = (matrizCompacta[k / 4] >> ((k % 4) * 2)) & 0x03;
            if (e == 3) salida << i << '|' << j << "|3\n";
            else if (e == 1) salida << i << '|' << j << "|1\n";
            else if (e == 2) salida << j << '|' << i << "|1\n";
        }
    }
    salida.close();
    return !salida.fail();
}

bool Plataforma::guardarMensajes(const char* carpeta) const {
    size_t bytesRuta = 0;
    char* ruta = unirRuta(carpeta, NOMBRES_ARCHIVOS[ARCH_MENSAJES], bytesRuta);
    registrarMemoriaLocal(bytesRuta);
    registrarComponenteExterno(EXT_ARCHIVOS);
    ofstream salida(ruta);
    delete[] ruta;
    liberarMemoriaLocal(bytesRuta);
    if (!salida.is_open()) return false;

    MarcoLocal marco(*this, sizeof(carpeta) + sizeof(bytesRuta) + sizeof(ruta) + sizeof(int*) + 4 * sizeof(int));
    int total = 0;
    for (int i = 0; i < cantidadUsuarios; i++) {
        sumarIteraciones(1);
        total += usuarios[i].getCantidadMensajes();
    }
    salida << "MENSAJES " << total << '\n';
    for (int d = 0; d < cantidadUsuarios; d++) {
        sumarIteraciones(1);
        const Usuario& u = usuarios[d];
        for (int m = 0; m < u.getCantidadMensajes(); m++) {
            sumarIteraciones(1);
            const Mensaje* msg = u.getMensaje(m);
            salida << msg->getEmisor() << '|' << d << '|' << msg->getFechaEnvio() << '|'
                   << msg->getContenido() << '|' << (msg->isLeido() ? 1 : 0) << '\n';
        }
    }
    salida.close();
    return !salida.fail();
}

bool Plataforma::guardarCambios(const char* carpeta) {
    MarcoLocal marco(*this, sizeof(carpeta) + sizeof(const char*));
    if (!cambiosPendientes && !perfilesModificados) return true;
    const char* destino = carpeta ? carpeta : rutaBD;
    if (!destino) return false;

    if (perfilesModificados) {
        if (!guardarUsuarios(destino)) return false;
        if (!guardarAlbumes(destino)) return false;
    }
    if (!guardarFotos(destino)) return false;
    if (!guardarRelaciones(destino)) return false;
    if (!guardarMensajes(destino)) return false;
    cambiosPendientes = false;
    perfilesModificados = false;
    return true;
}

// ---------------- Sesion y fotos de inicio ----------------

bool Plataforma::autenticar(const char* nombreUsuario, const char* contrasena) {
    MarcoLocal marco(*this, sizeof(nombreUsuario) + sizeof(contrasena) + sizeof(int));
    int idx = buscarUsuario(nombreUsuario);
    if (idx == -1) return false;
    if (!iguales(usuarios[idx].getContrasena(), contrasena)) return false;
    idxUsuarioAutenticado = idx;
    liberarCacheInicio();
    cambioFotosUsado = false;
    return true;
}

void Plataforma::cerrarSesion() {
    idxUsuarioAutenticado = -1;
    liberarCacheInicio();
    cambioFotosUsado = false;
}

int Plataforma::listarAmigos(int idx, int* destino) const {
    MarcoLocal marco(*this, sizeof(idx) + sizeof(destino) + 2 * sizeof(int));   // cuenta, j
    int cuenta = 0;
    for (int j = 0; j < cantidadUsuarios; j++) {
        sumarIteraciones(1);
        if (j != idx && leerEstado(idx, j) == 3) destino[cuenta++] = j;
    }
    return cuenta;
}

void Plataforma::liberarCacheInicio() {
    delete[] amigosInicio;
    delete[] pesosInicio;
    amigosInicio = nullptr;
    pesosInicio = nullptr;
    cantidadAmigosInicio = 0;
}

void Plataforma::prepararAmigosInicio() {
    liberarCacheInicio();
    if (idxUsuarioAutenticado < 0) return;

    int* temporal = new int[cantidadUsuarios];
    const size_t bytesTemporal = static_cast<size_t>(cantidadUsuarios) * sizeof(int);
    registrarMemoriaLocal(bytesTemporal);
    MarcoLocal marco(*this, sizeof(temporal) + sizeof(bytesTemporal) + 5 * sizeof(int));
    int na = listarAmigos(idxUsuarioAutenticado, temporal);


    amigosInicio = (na > 0) ? new int[na] : nullptr;
    pesosInicio = (na > 0) ? new int[na] : nullptr;
    cantidadAmigosInicio = na;
    sumarIteraciones(na);
    for (int a = 0; a < na; a++) {
        amigosInicio[a] = temporal[a];
        pesosInicio[a] = 0;
    }
    delete[] temporal;
    liberarMemoriaLocal(bytesTemporal);


    for (int a = 0; a < na; a++) {
        sumarIteraciones(1);
        for (int g = a + 1; g < na; g++) {
            sumarIteraciones(1);
            if (leerEstado(amigosInicio[a], amigosInicio[g]) == 3) {
                pesosInicio[a]++;
                pesosInicio[g]++;
            }
        }
    }
    sumarIteraciones(na);
    for (int a = 0; a < na; a++) {
        int comun = pesosInicio[a];
        pesosInicio[a] = (comun <= 10) ? 2 : (comun <= 20 ? 5 : 7);
    }
}

int Plataforma::sortearFotosInicio(int salida[], int maximo) {

    MarcoLocal marco(*this, sizeof(int*) + sizeof(maximo) + 5 * sizeof(int) +
                            sizeof(uniform_int_distribution<int>));
    int total = 0;
    sumarIteraciones(cantidadAmigosInicio);
    for (int a = 0; a < cantidadAmigosInicio; a++) total += pesosInicio[a];

    int elegidos = 0;
    while (elegidos < maximo && total > 0) {
        sumarIteraciones(1);
        registrarComponenteExterno(EXT_ALEATORIOS);
        uniform_int_distribution<int> dist(0, total - 1);
        int tiro = dist(generador);
        int acumulado = 0;
        for (int a = 0; a < cantidadAmigosInicio; a++) {
            sumarIteraciones(1);
            if (pesosInicio[a] == 0) continue;
            acumulado += pesosInicio[a];
            if (tiro < acumulado) {
                salida[elegidos++] = amigosInicio[a];
                total -= pesosInicio[a];
                pesosInicio[a] = 0;
                break;
            }
        }
    }
    return elegidos;
}

int Plataforma::seleccionarFotosInicio(int salida[]) {
    MarcoLocal marco(*this, sizeof(int*));
    if (idxUsuarioAutenticado < 0) return 0;
    prepararAmigosInicio();
    return sortearFotosInicio(salida, FOTOS_INICIO);
}

int Plataforma::actualizarFotosInicio(int salida[]) {
    MarcoLocal marco(*this, sizeof(int*) + sizeof(int));
    if (cambioFotosUsado) return -1;
    int n = sortearFotosInicio(salida, FOTOS_INICIO);
    if (n == 0) return 0;
    cambioFotosUsado = true;
    return n;
}



bool Plataforma::solicitudPendienteHaciaMi(int idxOtro) const {
    int yo = idxUsuarioAutenticado;
    if (yo < 0 || idxOtro < 0 || idxOtro >= cantidadUsuarios || idxOtro == yo) return false;
    int e = leerEstado(yo, idxOtro);
    return (idxOtro < yo) ? (e == 1) : (e == 2);
}

ResultadoSolicitud Plataforma::enviarSolicitud(int idxDestino) {
    MarcoLocal marco(*this, sizeof(idxDestino) + 2 * sizeof(int));
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
    MarcoLocal marco(*this, sizeof(int*) + sizeof(maximo) + 2 * sizeof(int));
    if (idxUsuarioAutenticado < 0) return 0;
    int cuenta = 0;
    for (int i = 0; i < cantidadUsuarios; i++) {
        sumarIteraciones(1);
        if (solicitudPendienteHaciaMi(i) && cuenta < maximo) salida[cuenta++] = i;
    }
    return cuenta;
}

bool Plataforma::aceptarSolicitud(int idxOrigen) {
    MarcoLocal marco(*this, sizeof(idxOrigen));
    if (!solicitudPendienteHaciaMi(idxOrigen)) return false;
    escribirEstado(idxUsuarioAutenticado, idxOrigen, 3);
    cambiosPendientes = true;
    return true;
}


int Plataforma::sugerirAmigos(int K, int sugeridosOut[], int coincidenciasOut[]) const {
    int yo = idxUsuarioAutenticado;
    if (yo < 0 || K <= 0) return 0;
    if (K > MAX_SUGERENCIAS) K = MAX_SUGERENCIAS;
    int n = cantidadUsuarios;
    int* amigos = new int[n];
    const size_t bytesAmigos = static_cast<size_t>(n) * sizeof(int);
    registrarMemoriaLocal(bytesAmigos);
    MarcoLocal marco(*this, sizeof(K) + 2 * sizeof(int*) + 8 * sizeof(int) + sizeof(amigos) + sizeof(bytesAmigos));
    int na = listarAmigos(yo, amigos);

    int cantidad = 0;
    for (int u = 0; u < n; u++) {
        sumarIteraciones(1);
        if (u == yo || leerEstado(yo, u) != 0) continue;
        int comun = 0;
        for (int a = 0; a < na; a++) {
            sumarIteraciones(1);
            if (leerEstado(u, amigos[a]) == 3) comun++;
        }
        if (comun == 0) continue;
        if (cantidad < K || comun > coincidenciasOut[cantidad - 1]) {
            int pos = (cantidad < K) ? cantidad : K - 1;
            while (pos > 0 && coincidenciasOut[pos - 1] < comun) {
                sumarIteraciones(1);
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
    liberarMemoriaLocal(bytesAmigos);
    return cantidad;
}


ResultadoMensaje Plataforma::enviarMensaje(int idxDestino, const char* contenido) {
    int yo = idxUsuarioAutenticado;
    if (yo < 0 || idxDestino < 0 || idxDestino >= cantidadUsuarios || idxDestino == yo) return MSG_INVALIDO;
    if (!sonAmigos(yo, idxDestino)) return MSG_NO_AMIGO;
    if (!contenido || soloEspacios(contenido)) return MSG_VACIO;
    size_t largo = longitud(contenido);
    if (largo > static_cast<size_t>(MAX_MENSAJE)) return MSG_MUY_LARGO;

    char* limpio = new char[largo + 1];
    const size_t bytesLimpio = largo + 1;
    registrarMemoriaLocal(bytesLimpio);
    MarcoLocal marco(*this, sizeof(idxDestino) + sizeof(contenido) + sizeof(yo) + sizeof(largo) +
                            sizeof(limpio) + sizeof(bytesLimpio) + sizeof(size_t) + sizeof(char) +
                            sizeof(long long));
    sumarIteraciones(static_cast<long long>(largo));
    for (size_t i = 0; i < largo; i++) {
        char c = contenido[i];
        if (c == '|') c = '/';
        else if (c == '\n' || c == '\r') c = ' ';
        limpio[i] = c;
    }
    limpio[largo] = '\0';

    registrarComponenteExterno(EXT_RELOJ);
    long long ahora = static_cast<long long>(duration_cast<seconds>(
        system_clock::now().time_since_epoch()).count());
    usuarios[idxDestino].recibirMensaje(limpio, ahora, yo, false);
    delete[] limpio;
    liberarMemoriaLocal(bytesLimpio);
    cambiosPendientes = true;
    return MSG_OK;
}

int Plataforma::obtenerNoLeidos(int salida[], int maximo) const {
    MarcoLocal marco(*this, sizeof(int*) + sizeof(maximo));
    if (idxUsuarioAutenticado < 0) return 0;
    return usuarios[idxUsuarioAutenticado].obtenerNoLeidos(salida, maximo);
}

void Plataforma::marcarLeido(int idxMsg) {
    MarcoLocal marco(*this, sizeof(idxMsg));
    if (idxUsuarioAutenticado < 0) return;
    if (usuarios[idxUsuarioAutenticado].marcarMensajeLeido(idxMsg)) cambiosPendientes = true;
}

int Plataforma::buscarRecibidos(long long inicio, long long fin, int salida[], int maximo) const {
    MarcoLocal marco(*this, sizeof(inicio) + sizeof(fin) + sizeof(int*) + sizeof(maximo));
    if (idxUsuarioAutenticado < 0) return 0;
    return usuarios[idxUsuarioAutenticado].buscarMensajesPorFecha(inicio, fin, salida, maximo);
}


void Plataforma::darLike(int idxUsuario, int idxAlbum, int idxFoto) {
    MarcoLocal marco(*this, 3 * sizeof(int));
    if (idxUsuario < 0 || idxUsuario >= cantidadUsuarios) return;
    if (usuarios[idxUsuario].darLikeFoto(idxAlbum, idxFoto)) cambiosPendientes = true;
}

void Plataforma::quitarLike(int idxUsuario, int idxAlbum, int idxFoto) {
    MarcoLocal marco(*this, 3 * sizeof(int));
    if (idxUsuario < 0 || idxUsuario >= cantidadUsuarios) return;
    if (usuarios[idxUsuario].quitarLikeFoto(idxAlbum, idxFoto)) cambiosPendientes = true;
}


void Plataforma::reiniciarMetricas() {
    reiniciarIteraciones();
    memoriaLocal = 0;
    memoriaLocalPico = 0;
    componentesExternos = 0;
}

void Plataforma::sumarIteraciones(long long n) const { util::sumarIteraciones(n); }

void Plataforma::registrarMemoriaLocal(size_t bytes) const {
    memoriaLocal += bytes;
    if (memoriaLocal > memoriaLocalPico) memoriaLocalPico = memoriaLocal;
}

void Plataforma::liberarMemoriaLocal(size_t bytes) const {
    memoriaLocal = (bytes > memoriaLocal) ? 0 : memoriaLocal - bytes;
}

void Plataforma::registrarComponenteExterno(int componente) const { componentesExternos |= componente; }

int Plataforma::getComponentesExternos() const { return componentesExternos; }

long long Plataforma::getContadorIteraciones() const { return iteraciones(); }

size_t Plataforma::medirMemoriaEstructuras() const {

    const long long iteracionesPrevias = iteraciones();

    size_t total = sizeof(*this);
    total += tamanoMatrizBytes * sizeof(unsigned char);
    if (indiceUsuarios) total += static_cast<size_t>(cantidadUsuarios) * sizeof(int);
    total += static_cast<size_t>(cantidadAmigosInicio) * 2 * sizeof(int);
    if (rutaBD) total += (longitud(rutaBD) + 1) * sizeof(char);
    for (int i = 0; i < capacidadUsuarios; i++) total += usuarios[i].obtenerMemoria();

    fijarIteraciones(iteracionesPrevias);
    return total;
}

size_t Plataforma::getMemoriaActual() const { return medirMemoriaEstructuras() + memoriaLocal; }

size_t Plataforma::getMemoriaPico() const { return medirMemoriaEstructuras() + memoriaLocalPico; }
