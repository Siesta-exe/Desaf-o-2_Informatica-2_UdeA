#include "plataforma.h"
#include "utilidades.h"
#include <iostream>

using namespace std;
using namespace util;

static int leerLinea(char* destino, int tam) {
    cin.getline(destino, tam);
    if (cin.eof() && cin.gcount() == 0) return -1;
    if (cin.fail() && !cin.eof()) {
        cin.clear();
        cin.ignore(100000, '\n');
        return 0;
    }
    size_t n = longitud(destino);
    while (n > 0 && destino[n - 1] == '\r') destino[--n] = '\0';
    return 1;
}

static bool leerEntero(const char* mensaje, int& valor) {
    char buffer[64];
    while (true) {
        cout << mensaje;
        int r = leerLinea(buffer, sizeof(buffer));
        if (r == -1) return false;
        if (r == 1 && esEnteroNoNegativo(buffer)) {
            valor = static_cast<int>(aEntero(buffer));
            return true;
        }
        cout << "Entrada invalida: ingrese un numero entero." << endl;
    }
}

static void mostrarMetricas(const Plataforma& red) {
    size_t memoria = red.getMemoriaEstimada();
    cout << "\n--------------------------------------------------" << endl;
    cout << " [METRICAS DE EFICIENCIA]" << endl;
    cout << "  -> Iteraciones ejecutadas : " << red.getContadorIteraciones() << endl;
    cout << "  -> Memoria consumida      : " << memoria << " bytes ("
         << (memoria / 1024.0) << " KiB)" << endl;
    cout << "--------------------------------------------------" << endl;
}

static const char* nombreDe(const Plataforma& red, int idx) {
    const Usuario* u = red.consultarUsuario(idx);
    return u ? u->getNombreUsuario() : "(desconocido)";
}

static void mostrarFotosInicio(const Plataforma& red, const int fotos[], int cantidad) {
    cout << "\nFotos de perfil de algunos de tus amigos:" << endl;
    for (int i = 0; i < cantidad; i++) {
        const Usuario* amigo = red.consultarUsuario(fotos[i]);
        if (amigo) cout << "  [" << (i + 1) << "] " << amigo->getNombreUsuario()
                        << " -> " << amigo->getFotoPerfil() << endl;
    }
}

static void menuAdministrarAmistades(Plataforma& red) {
    red.reiniciarMetricas();

    cout << "\n=== ADMINISTRAR AMISTADES ===" << endl;
    cout << "1. Enviar solicitud de amistad" << endl;
    cout << "2. Aceptar solicitudes recibidas" << endl;
    int op;
    if (!leerEntero("Seleccione una opcion: ", op)) return;

    if (op == 1) {
        char nombre[128];
        red.registrarMemoriaLocal(sizeof(nombre));
        cout << "Ingrese el nombre de usuario de destino: ";
        if (leerLinea(nombre, sizeof(nombre)) != 1) { cout << "Nombre invalido." << endl; return; }

        int idx = red.buscarUsuario(nombre);
        if (idx == -1) {
            cout << "El usuario especificado no existe." << endl;
            return;
        }
        switch (red.enviarSolicitud(idx)) {
        case Plataforma::SOL_OK:
            cout << "Solicitud enviada correctamente a " << nombre << "." << endl; break;
        case Plataforma::SOL_A_SI_MISMO:
            cout << "No puedes enviarte una solicitud a ti mismo." << endl; break;
        case Plataforma::SOL_YA_AMIGOS:
            cout << "Ya son amigos." << endl; break;
        case Plataforma::SOL_YA_ENVIADA:
            cout << "Ya enviaste una solicitud pendiente a ese usuario." << endl; break;
        case Plataforma::SOL_YA_RECIBIDA:
            cout << "Ese usuario ya te envio una solicitud: aceptala en la opcion 2." << endl; break;
        default:
            cout << "No se pudo enviar la solicitud." << endl; break;
        }
    } else if (op == 2) {
        int tam = red.getCantidadUsuarios();
        if (tam < 1) tam = 1;
        int* solicitudes = new int[tam];
        red.registrarMemoriaLocal(static_cast<size_t>(tam) * sizeof(int));
        int cantidad = red.obtenerSolicitudesRecibidas(solicitudes, tam);
        if (cantidad > tam) cantidad = tam;

        if (cantidad == 0) {
            cout << "No tienes solicitudes pendientes." << endl;
        } else {
            cout << "\nSolicitudes de amistad recibidas:" << endl;
            for (int i = 0; i < cantidad; i++) {
                cout << (i + 1) << ". " << nombreDe(red, solicitudes[i]) << endl;
            }
            int sel;
            if (leerEntero("Numero de la solicitud a aceptar (0 para cancelar): ", sel)
                && sel > 0 && sel <= cantidad) {
                if (red.aceptarSolicitud(solicitudes[sel - 1])) {
                    cout << "Ahora eres amigo de " << nombreDe(red, solicitudes[sel - 1]) << "." << endl;
                }
            }
        }
        delete[] solicitudes;
    } else {
        cout << "Opcion no valida." << endl;
    }
}

static void menuVisitarPerfil(Plataforma& red) {
    red.reiniciarMetricas();

    char nombre[128];
    red.registrarMemoriaLocal(sizeof(nombre));
    cout << "\nIngrese el nombre de usuario del amigo a visitar: ";
    if (leerLinea(nombre, sizeof(nombre)) != 1) { cout << "Nombre invalido." << endl; return; }

    int yo = red.getSesionActual();
    int idxAmigo = red.buscarUsuario(nombre);
    if (idxAmigo == -1) {
        cout << "El usuario no existe." << endl;
        return;
    }
    if (!red.sonAmigos(yo, idxAmigo)) {
        cout << "Solo puedes visitar el perfil de tus amigos." << endl;
        return;
    }

    const Usuario* amigo = red.consultarUsuario(idxAmigo);
    cout << "\n==========================================" << endl;
    cout << "Perfil de: " << amigo->getNombreUsuario() << endl;
    cout << "Foto de perfil: " << amigo->getFotoPerfil() << endl;
    cout << "==========================================" << endl;

    int totalAlbumes = amigo->getCantidadAlbumes();
    int* visibles = new int[totalAlbumes > 0 ? totalAlbumes : 1];
    red.registrarMemoriaLocal(static_cast<size_t>(totalAlbumes) * sizeof(int));
    int cantVisibles = 0;
    for (int i = 0; i < totalAlbumes; i++) {
        red.sumarIteraciones(1);

        if (amigo->getAlbum(i)->puedeVisualizar(true, false)) visibles[cantVisibles++] = i;
    }

    if (cantVisibles == 0) {
        cout << "Este usuario no tiene albumes accesibles para ti." << endl;
        delete[] visibles;
        return;
    }

    cout << "\nAlbumes accesibles:" << endl;
    for (int i = 0; i < cantVisibles; i++) {
        const Album* alb = amigo->getAlbum(visibles[i]);
        cout << (i + 1) << ". " << alb->getNombre() << " [" << alb->getPrivacidadTexto() << "] ("
             << alb->getCantidadFotos() << " fotos)" << endl;
    }

    int sel;
    if (leerEntero("Seleccione el numero de album para ver (0 para salir): ", sel)
        && sel > 0 && sel <= cantVisibles) {
        int idxAlbum = visibles[sel - 1];
        const Album* alb = amigo->getAlbum(idxAlbum);
        int total = alb->getCantidadFotos();

        if (total == 0) {
            cout << "El album esta vacio." << endl;
        } else {
            int actual = 0;
            bool enCarrusel = true;
            red.registrarMemoriaLocal(sizeof(actual) + sizeof(enCarrusel));
            while (enCarrusel) {
                const Foto* f = alb->getFoto(actual);
                cout << "\n------------------------------------------" << endl;
                cout << "Album: " << alb->getNombre() << " [Foto " << (actual + 1) << "/" << total << "]" << endl;
                cout << *f << endl;
                cout << "------------------------------------------" << endl;
                cout << "1. Siguiente\n2. Previa\n3. Dar / Quitar like\n4. Salir del album" << endl;
                int opC;
                if (!leerEntero("Opcion: ", opC)) break;
                red.sumarIteraciones(1);

                if (opC == 1) {
                    actual = (actual + 1) % total;
                } else if (opC == 2) {
                    actual = (actual - 1 + total) % total;
                } else if (opC == 3) {
                    int sub;
                    if (leerEntero("1. Dar like | 2. Quitar like: ", sub)) {
                        if (sub == 1) red.darLike(idxAmigo, idxAlbum, actual);
                        else if (sub == 2) red.quitarLike(idxAmigo, idxAlbum, actual);
                        else cout << "Opcion no valida." << endl;
                    }
                } else if (opC == 4) {
                    enCarrusel = false;
                } else {
                    cout << "Opcion no valida." << endl;
                }
            }
        }
    }
    delete[] visibles;
}

static void menuSugerenciasAmistad(Plataforma& red) {
    red.reiniciarMetricas();

    int k = 0;
    while (true) {
        if (!leerEntero("\nIngrese la cantidad K de sugerencias (1 - 20): ", k)) return;
        if (k >= 1 && k <= 20) break;
        cout << "K debe estar entre 1 y 20." << endl;
    }

    int sugeridos[20];
    int coincidencias[20];
    red.registrarMemoriaLocal(sizeof(sugeridos) + sizeof(coincidencias) + sizeof(k));
    int cantidad = red.sugerirAmigos(k, sugeridos, coincidencias);

    if (cantidad == 0) {
        cout << "No hay sugerencias disponibles en este momento." << endl;
        return;
    }

    cout << "\n=== SUGERENCIAS DE AMISTAD ===" << endl;
    for (int i = 0; i < cantidad; i++) {
        cout << (i + 1) << ". " << nombreDe(red, sugeridos[i]) << " -> "
             << coincidencias[i] << " amigos en comun" << endl;
    }

    int sel;
    if (leerEntero("\nEnviar solicitud a una sugerencia (numero, 0 para omitir): ", sel)
        && sel > 0 && sel <= cantidad) {
        if (red.enviarSolicitud(sugeridos[sel - 1]) == Plataforma::SOL_OK) {
            cout << "Solicitud enviada exitosamente a " << nombreDe(red, sugeridos[sel - 1]) << "." << endl;
        } else {
            cout << "No se pudo enviar la solicitud." << endl;
        }
    }
}

static void mostrarMensaje(const Plataforma& red, const Mensaje* m) {
    cout << "De: " << nombreDe(red, m->getEmisor()) << "  " << *m << endl;
}

static void menuAdministrarMensajes(Plataforma& red) {
    red.reiniciarMetricas();

    cout << "\n=== ADMINISTRAR MENSAJES ===" << endl;
    cout << "1. Enviar mensaje a un amigo" << endl;
    cout << "2. Leer mensajes nuevos (no leidos)" << endl;
    cout << "3. Buscar mensajes por rango de fecha" << endl;
    int op;
    if (!leerEntero("Opcion: ", op)) return;

    int yo = red.getSesionActual();
    const Usuario* actual = red.consultarUsuario(yo);

    if (op == 1) {
        char nombre[128];
        char texto[1024];
        red.registrarMemoriaLocal(sizeof(nombre) + sizeof(texto));
        cout << "Ingrese el nombre del amigo destinatario: ";
        if (leerLinea(nombre, sizeof(nombre)) != 1) { cout << "Nombre invalido." << endl; return; }

        int idxDest = red.buscarUsuario(nombre);
        if (idxDest == -1 || !red.sonAmigos(yo, idxDest)) {
            cout << "El usuario no existe o no es tu amigo." << endl;
            return;
        }

        cout << "Escriba el mensaje (max " << Plataforma::MAX_MENSAJE << " caracteres): ";
        int r = leerLinea(texto, sizeof(texto));
        if (r == -1) return;
        if (r == 0) { cout << "El mensaje excede los " << Plataforma::MAX_MENSAJE << " caracteres." << endl; return; }

        switch (red.enviarMensaje(idxDest, texto)) {
        case Plataforma::MSG_OK: cout << "Mensaje enviado exitosamente." << endl; break;
        case Plataforma::MSG_VACIO: cout << "No se pueden enviar mensajes vacios." << endl; break;
        case Plataforma::MSG_MUY_LARGO: cout << "El mensaje excede los " << Plataforma::MAX_MENSAJE << " caracteres." << endl; break;
        case Plataforma::MSG_NO_AMIGO: cout << "Solo puedes escribirle a tus amigos." << endl; break;
        default: cout << "Error al enviar el mensaje." << endl; break;
        }
    } else if (op == 2) {
        int tam = actual->getCantidadMensajes();
        if (tam < 1) tam = 1;
        int* noLeidos = new int[tam];
        red.registrarMemoriaLocal(static_cast<size_t>(tam) * sizeof(int));
        int cantidad = red.obtenerNoLeidos(noLeidos, tam);
        if (cantidad > tam) cantidad = tam;

        if (cantidad == 0) {
            cout << "No tienes mensajes nuevos sin leer." << endl;
        } else {
            cout << "\n--- Mensajes nuevos (" << cantidad << ") ---" << endl;
            for (int i = 0; i < cantidad; i++) mostrarMensaje(red, actual->getMensaje(noLeidos[i]));
            for (int i = 0; i < cantidad; i++) red.marcarLeido(noLeidos[i]);
            red.sumarIteraciones(cantidad);
            cout << "Los mensajes mostrados fueron marcados como leidos." << endl;
        }
        delete[] noLeidos;
    } else if (op == 3) {
        char fInicio[64], fFin[64];
        long long inicio, fin;
        red.registrarMemoriaLocal(sizeof(fInicio) + sizeof(fFin) + sizeof(inicio) + sizeof(fin));
        cout << "Fecha inicio (AAAA-MM-DD): ";
        if (leerLinea(fInicio, sizeof(fInicio)) != 1 || !parsearFecha(fInicio, inicio)) {
            cout << "Fecha de inicio invalida." << endl; return;
        }
        cout << "Fecha fin    (AAAA-MM-DD): ";
        if (leerLinea(fFin, sizeof(fFin)) != 1 || !parsearFecha(fFin, fin)) {
            cout << "Fecha de fin invalida." << endl; return;
        }
        if (fin < inicio) { cout << "La fecha fin es anterior a la fecha inicio." << endl; return; }
        fin += 86399;

        int tam = actual->getCantidadMensajes();
        if (tam < 1) tam = 1;
        int* resultado = new int[tam];
        red.registrarMemoriaLocal(static_cast<size_t>(tam) * sizeof(int));
        int cantidad = red.buscarRecibidos(inicio, fin, resultado, tam);
        if (cantidad > tam) cantidad = tam;

        if (cantidad == 0) {
            cout << "No se encontraron mensajes en ese rango de fechas." << endl;
        } else {
            cout << "\n--- Mensajes recibidos del " << fInicio << " al " << fFin
                 << " (" << cantidad << ") ---" << endl;
            for (int i = 0; i < cantidad; i++) mostrarMensaje(red, actual->getMensaje(resultado[i]));
        }
        delete[] resultado;
    } else {
        cout << "Opcion no valida." << endl;
    }
}

int main(int argc, char** argv) {
    Plataforma redSocial;

    cout << "==========================================" << endl;
    cout << "    INICIALIZANDO PLATAFORMA UDE@BOOK     " << endl;
    cout << "==========================================" << endl;

    const char* candidatas[8];
    int nc = 0;
    if (argc > 1) candidatas[nc++] = argv[1];
#ifdef RUTA_BD
    candidatas[nc++] = RUTA_BD;
#endif
    candidatas[nc++] = "base_datos.txt";
    candidatas[nc++] = "../base_datos.txt";
    candidatas[nc++] = "../../base_datos.txt";

    redSocial.reiniciarMetricas();
    bool cargada = false;
    for (int i = 0; i < nc && !cargada; i++) {
        cargada = redSocial.cargarDatos(candidatas[i]);
        if (cargada) cout << "Base de datos cargada desde: " << candidatas[i] << endl;
    }
    if (!cargada) {
        cout << "Error: no se pudo cargar 'base_datos.txt'. Ubiquelo junto al ejecutable\n"
                "o pase su ruta como argumento." << endl;
        return 1;
    }
    cout << "Usuarios cargados: " << redSocial.getCantidadUsuarios() << endl;
    mostrarMetricas(redSocial);

    bool ejecutando = true;
    while (ejecutando) {
        char usuarioInput[128], claveInput[128];
        cout << "\n--- INICIO DE SESION (escriba 'salir' como usuario para terminar) ---" << endl;
        cout << "Usuario : ";
        if (leerLinea(usuarioInput, sizeof(usuarioInput)) == -1) break;
        if (iguales(usuarioInput, "salir")) break;
        cout << "Clave   : ";
        if (leerLinea(claveInput, sizeof(claveInput)) == -1) break;

        redSocial.reiniciarMetricas();
        if (!redSocial.autenticar(usuarioInput, claveInput)) {
            cout << "Error: Credenciales invalidas. Intente de nuevo." << endl;
            continue;
        }

        cout << "\n¡ Bienvenido a Ude@Book, " << usuarioInput << "!" << endl;

        int fotosInicio[Plataforma::FOTOS_INICIO];
        redSocial.registrarMemoriaLocal(sizeof(usuarioInput) + sizeof(claveInput) + sizeof(fotosInicio));
        int cantFotos = redSocial.seleccionarFotosInicio(fotosInicio);
        if (cantFotos == 0) {
            cout << "\nAun no tienes amigos para mostrar sus fotos de perfil." << endl;
        } else {
            mostrarFotosInicio(redSocial, fotosInicio, cantFotos);
            char respuesta[16];
            cout << "\nPuede cambiar este grupo de fotos UNA sola vez ¿Desea cambiarlo? (S/N): ";
            if (leerLinea(respuesta, sizeof(respuesta)) == 1 && (respuesta[0] == 'S' || respuesta[0] == 's')) {
                int nuevas = redSocial.actualizarFotosInicio(fotosInicio);
                if (nuevas > 0) mostrarFotosInicio(redSocial, fotosInicio, nuevas);
                else cout << "No hay otros amigos distintos para mostrar." << endl;
            }
        }
        mostrarMetricas(redSocial);

        bool sesionActiva = true;
        while (sesionActiva) {
            cout << "\n==========================================" << endl;
            cout << "   Menu de opciones - " << nombreDe(redSocial, redSocial.getSesionActual()) << endl;
            cout << "==========================================" << endl;
            cout << "1. Administrar amistades" << endl;
            cout << "2. Visitar perfil de un amigo" << endl;
            cout << "3. Ver sugerencias de amistad" << endl;
            cout << "4. Administrar mensajes" << endl;
            cout << "5. Cerrar sesion" << endl;
            cout << "6. Salir del programa" << endl;
            int opcion;
            if (!leerEntero("Opcion: ", opcion)) { ejecutando = false; break; }

            switch (opcion) {
            case 1:
                menuAdministrarAmistades(redSocial);
                mostrarMetricas(redSocial);
                break;
            case 2:
                menuVisitarPerfil(redSocial);
                mostrarMetricas(redSocial);
                break;
            case 3:
                menuSugerenciasAmistad(redSocial);
                mostrarMetricas(redSocial);
                break;
            case 4:
                menuAdministrarMensajes(redSocial);
                mostrarMetricas(redSocial);
                break;
            case 5:
                redSocial.cerrarSesion();
                redSocial.guardarCambios();
                sesionActiva = false;
                cout << "Sesion cerrada correctamente." << endl;
                break;
            case 6:
                redSocial.cerrarSesion();
                sesionActiva = false;
                ejecutando = false;
                break;
            default:
                cout << "Opcion no valida." << endl;
                break;
            }
        }
    }

    cout << "\nGuardando cambios en la base de datos..." << endl;
    if (!redSocial.guardarCambios()) {
        cout << "Advertencia: no se pudieron guardar los cambios." << endl;
    }
    cout << "¡Proceso finalizado con exito!" << endl;
    return 0;
}
