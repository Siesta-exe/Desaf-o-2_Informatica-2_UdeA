#ifndef UTILIDADES_H
#define UTILIDADES_H

#include <cstddef>

namespace util {

size_t longitud(const char* s);
char* copiar(const char* origen);
int comparar(const char* a, const char* b);
bool iguales(const char* a, const char* b);
const char* seguro(const char* s);
bool comienzaCon(const char* s, const char* prefijo);
bool contiene(const char* s, char c);
bool soloEspacios(const char* s);
bool esEnteroNoNegativo(const char* s);
long long aEntero(const char* s);
bool parsearFecha(const char* s, long long& timestamp);
void formatearFecha(long long ts, char* destino, bool conHora, bool aplicarDesfase = true);


bool campoValido(const char* s, bool permitirPipe = false);


void reiniciarIteraciones();
long long iteraciones();
void sumarIteraciones(long long n);
void fijarIteraciones(long long valor);

}

#endif
