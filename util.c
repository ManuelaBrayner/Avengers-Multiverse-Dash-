#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>

#include "screen.h"
#include "keyboard.h"
#include "tipos.h"
#include "util.h"

// pede memoria. se nao tiver, fecha o programa
void *xmalloc(size_t n){
    void *p = malloc(n);
    if (p == NULL) {
        keyboardDestroy();
        screenDestroy();
        fprintf(stderr, "Erro: sem memoria.\n");
        exit(1);
    }
    return p;
}

// numero aleatorio de 0 ate n-1
int rnd(int n){
    return rand() % n;
}

/* le uma tecla. devolve -1 se nao tem nenhuma */
int lerTeclaBruta(void){
    if (keyhit() == 0) {
        return -1;
    }

    int c = readch() & 0xFF;

    /* as setas chegam como 3 teclas: 27, '[' e a letra */
    if (c == 27) {
        if (keyhit()) {
            int c2 = readch() & 0xFF;
            if ((c2 == '[' || c2 == 'O') && keyhit()) {
                int c3 = readch() & 0xFF;
                if (c3 == 'A') return TECLA_CIMA;
                if (c3 == 'B') return TECLA_BAIXO;
            }
            return 0;
        }
        return 27;
    }
    return c;
}

/* igual a de cima, mas transforma maiuscula em minuscula */
int lerTecla(void){
    int c = lerTeclaBruta();
    if (c >= 'A' && c <= 'Z') {
        c = c + 32;
    }
    return c;
}

/* joga fora as teclas que estavam esperando */
void limparTeclado(void){
    while (keyhit()) {
        readch();
    }
}

/* conta quantas colunas o texto ocupa (letra com acento conta 1) */
static int larguraUtf8(const char *s){
    int n = 0;
    int i = 0;
    while (s[i] != '\0') {
        if (((unsigned char)s[i] & 0xC0) != 0x80) {
            n++;
        }
        i++;
    }
    return n;
}

/* escreve texto colorido numa posicao (funciona como printf) */
void escrever(int x, int y, int cor, const char *fmt, ...){
    char texto[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(texto, sizeof texto, fmt, args);
    va_end(args);

    screenGotoxy(x, y);
    screenSetColor(cor, FUNDO);
    printf("%s", texto);
}

/* escreve texto colorido no meio da linha y */
void centro(int y, int cor, const char *fmt, ...){
    char texto[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(texto, sizeof texto, fmt, args);
    va_end(args);

    int x = (80 - larguraUtf8(texto)) / 2 + 1;
    if (x < 1) {
        x = 1;
    }

    screenGotoxy(x, y);
    screenSetColor(cor, FUNDO);
    printf("%s", texto);
}

/* pinta o terminal inteiro com a cor de fundo */
void limparTela(void){
    screenSetColor(TXT, FUNDO);
    screenClear();
    for (int linha = 1; linha <= 24; linha++) {
        printf("\033[%d;1H", linha);
        for (int coluna = 0; coluna < 80; coluna++) {
            putchar(' ');
        }
    }
}

/* espera apertar Enter ou Espaco */
void esperarEnter(void){
    limparTeclado();
    while (1) {
        int t = lerTecla();
        if (t == 10 || t == 13 || t == ' ') {
            return;
        }
        usleep(10000);
    }
}