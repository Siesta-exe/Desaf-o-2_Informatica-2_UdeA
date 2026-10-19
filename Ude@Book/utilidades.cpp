#include "utilidades.h"

namespace {

long long g_iteraciones = 0;

const long long DESFASE_COLOMBIA_SEG = 5LL * 3600LL;

long long diasDesdeCivil(long long a, int m, int d) {
    a -= (m <= 2) ? 1 : 0;
    long long era = (a >= 0 ? a : a - 399) / 400;
    long long yoe = a - era * 400;
    long long doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
    long long doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + doe - 719468;
}

void civilDesdeDias(long long z, long long& a, int& m, int& d) {
    z += 719468;
    long long era = (z >= 0 ? z : z - 146096) / 146097;
    long long doe = z - era * 146097;
    long long yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    long long y = yoe + era * 400;
    long long doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    long long mp = (5 * doy + 2) / 153;
    d = static_cast<int>(doy - (153 * mp + 2) / 5 + 1);
    m = static_cast<int>(mp < 10 ? mp + 3 : mp - 9);
    a = y + (m <= 2 ? 1 : 0);
}

bool esBisiesto(long long a) {
    return (a % 4 == 0 && a % 100 != 0) || (a % 400 == 0);
}

int diasDelMes(long long a, int m) {
    static const int dias[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (m == 2 && esBisiesto(a)) return 29;
    return dias[m - 1];
}

}

namespace util {

void reiniciarIteraciones() { g_iteraciones = 0; }
long long iteraciones() { return g_iteraciones; }
void sumarIteraciones(long long n) { g_iteraciones += n; }
void fijarIteraciones(long long valor) { g_iteraciones = valor; }

size_t longitud(const char* s) {
    size_t n = 0;
    if (!s) return 0;
    while (s[n] != '\0') { g_iteraciones++; n++; }
    return n;
}

char* copiar(const char* origen) {
    if (!origen) return nullptr;
    size_t n = longitud(origen);
    char* destino = new char[n + 1];
    for (size_t i = 0; i <= n; i++) { g_iteraciones++; destino[i] = origen[i]; }
    return destino;
}

int comparar(const char* a, const char* b) {
    a = a ? a : "";
    b = b ? b : "";
    while (*a != '\0' && *a == *b) { g_iteraciones++; a++; b++; }
    return static_cast<int>(static_cast<unsigned char>(*a)) - static_cast<int>(static_cast<unsigned char>(*b));
}

bool iguales(const char* a, const char* b) {
    return comparar(a, b) == 0;
}

const char* seguro(const char* s) {
    return s ? s : "";
}

bool comienzaCon(const char* s, const char* prefijo) {
    while (*prefijo != '\0') {
        g_iteraciones++;
        if (*s != *prefijo) return false;
        s++; prefijo++;
    }
    return true;
}

bool contiene(const char* s, char c) {
    for (; *s != '\0'; s++) {
        g_iteraciones++;
        if (*s == c) return true;
    }
    return false;
}

bool soloEspacios(const char* s) {
    for (; *s != '\0'; s++) {
        g_iteraciones++;
        if (*s != ' ' && *s != '\t' && *s != '\r' && *s != '\n') return false;
    }
    return true;
}

bool campoValido(const char* s, bool permitirPipe) {
    if (!s) return false;
    bool hayContenido = false;
    for (; *s != '\0'; s++) {
        g_iteraciones++;
        if (*s == '\n' || *s == '\r') return false;
        if (*s == '|' && !permitirPipe) return false;
        if (*s != ' ' && *s != '\t') hayContenido = true;
    }
    return hayContenido;
}

bool esEnteroNoNegativo(const char* s) {
    while (*s == ' ' || *s == '\t') { g_iteraciones++; s++; }
    if (*s == '\0') return false;
    int digitos = 0;
    while (*s >= '0' && *s <= '9') { g_iteraciones++; s++; digitos++; }
    while (*s == ' ' || *s == '\t' || *s == '\r') { g_iteraciones++; s++; }
    return digitos > 0 && digitos < 10 && *s == '\0';
}

long long aEntero(const char* s) {
    while (*s == ' ' || *s == '\t') { g_iteraciones++; s++; }
    bool negativo = false;
    if (*s == '-') { negativo = true; s++; }
    else if (*s == '+') { s++; }
    long long v = 0;
    while (*s >= '0' && *s <= '9') { g_iteraciones++; v = v * 10 + (*s - '0'); s++; }
    return negativo ? -v : v;
}

bool parsearFecha(const char* s, long long& timestamp) {
    while (*s == ' ' || *s == '\t') { g_iteraciones++; s++; }
    if (longitud(s) < 10) return false;
    for (int i = 0; i < 10; i++) {
        g_iteraciones++;
        if (i == 4 || i == 7) { if (s[i] != '-') return false; }
        else if (s[i] < '0' || s[i] > '9') return false;
    }
    for (int i = 10; s[i] != '\0'; i++) {
        g_iteraciones++;
        if (s[i] != ' ' && s[i] != '\t' && s[i] != '\r') return false;
    }
    long long a = (s[0] - '0') * 1000 + (s[1] - '0') * 100 + (s[2] - '0') * 10 + (s[3] - '0');
    int m = (s[5] - '0') * 10 + (s[6] - '0');
    int d = (s[8] - '0') * 10 + (s[9] - '0');
    if (a < 1970 || m < 1 || m > 12 || d < 1 || d > diasDelMes(a, m)) return false;
    timestamp = diasDesdeCivil(a, m, d) * 86400LL + DESFASE_COLOMBIA_SEG;
    return true;
}

void formatearFecha(long long ts, char* destino, bool conHora, bool aplicarDesfase) {
    if (aplicarDesfase) ts -= DESFASE_COLOMBIA_SEG;
    if (ts < 0) ts = 0;
    long long dias = ts / 86400;
    long long resto = ts % 86400;
    long long a; int m, d;
    civilDesdeDias(dias, a, m, d);
    int p = 0;
    destino[p++] = static_cast<char>('0' + (a / 1000) % 10);
    destino[p++] = static_cast<char>('0' + (a / 100) % 10);
    destino[p++] = static_cast<char>('0' + (a / 10) % 10);
    destino[p++] = static_cast<char>('0' + a % 10);
    destino[p++] = '-';
    destino[p++] = static_cast<char>('0' + m / 10);
    destino[p++] = static_cast<char>('0' + m % 10);
    destino[p++] = '-';
    destino[p++] = static_cast<char>('0' + d / 10);
    destino[p++] = static_cast<char>('0' + d % 10);
    if (conHora) {
        int hh = static_cast<int>(resto / 3600);
        int mm = static_cast<int>((resto % 3600) / 60);
        destino[p++] = ' ';
        destino[p++] = static_cast<char>('0' + hh / 10);
        destino[p++] = static_cast<char>('0' + hh % 10);
        destino[p++] = ':';
        destino[p++] = static_cast<char>('0' + mm / 10);
        destino[p++] = static_cast<char>('0' + mm % 10);
    }
    destino[p] = '\0';
}

}
