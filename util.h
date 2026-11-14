#ifndef UTIL_H // *
#define UTIL_H // *

#include <stddef.h>

void *xmalloc(size_t n);
int rnd(int n);
int lerTeclaBruta(void);
int lerTecla(void);
void limparTeclado(void);
void escrever(int x, int y, int cor, const char *fmt, ...);
void centro(int y, int cor, const char *fmt, ...);
void limparTela(void);
void esperarEnter(void);

#endif // * eles evitam que o arquivo seja lido duas vezes