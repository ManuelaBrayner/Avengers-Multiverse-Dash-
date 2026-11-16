#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "screen.h"
#include "keyboard.h"
#include "timer.h"
#include "tipos.h"
#include "dados.h"


/* ================= SECAO 1 - UTILITARIOS (Pessoa A) ================= */

// pede memoria. se nao tiver, fecha o programa
void *xmalloc(size_t n)
{
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
int rnd(int n)
{
    return rand() % n;
}

// le uma tecla. devolve -1 se nao tem nenhuma
int lerTeclaBruta(void)
{
    if (keyhit() == 0) {
        return -1;
    }

    int c = readch() & 0xFF;

    // as setas chegam como 3 teclas: 27, '[' e a letra
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

// igual a de cima, mas transforma maiuscula em minuscula
int lerTecla(void)
{
    int c = lerTeclaBruta();
    if (c >= 'A' && c <= 'Z') {
        c = c + 32;
    }
    return c;
}

// joga fora as teclas que estavam esperando
void limparTeclado(void)
{
    while (keyhit()) {
        readch();
    }
}

// conta quantas colunas o texto ocupa (letra com acento conta 1)
static int larguraUtf8(const char *s)
{
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

// escreve texto colorido numa posicao (funciona como printf)
void escrever(int x, int y, int cor, const char *fmt, ...)
{
    char texto[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(texto, sizeof texto, fmt, args);
    va_end(args);

    screenGotoxy(x, y);
    screenSetColor(cor, FUNDO);
    printf("%s", texto);
}

// escreve texto colorido no meio da linha y
void centro(int y, int cor, const char *fmt, ...)
{
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

// pinta o terminal inteiro com a cor de fundo
void limparTela(void)
{
    screenSetColor(TXT, FUNDO);
    screenClear();
    for (int linha = 1; linha <= 24; linha++) {
        printf("\033[%d;1H", linha);
        for (int coluna = 0; coluna < 80; coluna++) {
            putchar(' ');
        }
    }
}

// espera apertar Enter ou Espaco
void esperarEnter(void)
{
    limparTeclado();
    while (1) {
        int t = lerTecla();
        if (t == 10 || t == 13 || t == ' ') {
            return;
        }
        usleep(10000);
    }
}

/* ================= FIM DA SECAO 1 ================= */



/* ================= SECAO 2 - TELA DE PIXELS (Pessoa A) ================= */

// cria a matriz de pixels
Tela *telaCriar(int larg, int alt)
{
    Tela *t = xmalloc(sizeof(Tela));
    t->larg = larg;
    t->alt = alt;

    t->m = xmalloc(alt * sizeof(unsigned char *));
    for (int i = 0; i < alt; i++) {
        t->m[i] = xmalloc(larg);
    }
    return t;
}

// libera na ordem contraria -> linhas, vetor, struct
void telaLiberar(Tela *t)
{
    if (t == NULL) {
        return;
    }
    for (int i = 0; i < t->alt; i++) {
        free(t->m[i]);
    }
    free(t->m);
    free(t);
}

// zera todos os pixels
void telaLimpar(Tela *t)
{
    for (int y = 0; y < t->alt; y++) {
        memset(t->m[y], 0, t->larg);
    }
}

// liga um pixel (ignora se estiver fora da tela)
void telaPut(Tela *t, int x, int y, int v)
{
    if (x < 0 || y < 0 || x >= t->larg || y >= t->alt) {
        return;
    }
    t->m[y][x] = (unsigned char)v;
}

// desenha um sprite -> cada # vira um pixel
void telaSprite(Tela *t, int x, int yTopo, const Sprite *s, int v)
{
    for (int linha = 0; linha < s->h; linha++) {
        for (int coluna = 0; coluna < s->w; coluna++) {
            if (s->r[linha][coluna] == '#') {
                telaPut(t, x + coluna, yTopo + linha, v);
            }
        }
    }
}

// transforma a matriz em caracteres e mostra no terminal
void telaDesenhar(const Tela *t, int corJoia)
{
    screenSetColor(TXT, FUNDO);

    for (int cy = 0; cy < t->alt / 2; cy++) {
        screenGotoxy(OFFX, OFFY + cy);
        int corAtual = -1;

        for (int x = 0; x < t->larg; x++) {
            int cima = t->m[2 * cy][x];
            int baixo = t->m[2 * cy + 1][x];

            int idx = 0;
            if (cima != 0) idx = idx | 1;
            if (baixo != 0) idx = idx | 2;

            if (idx != 0) {
                int forte = cima;
                if (baixo > cima) forte = baixo;

                if (forte != corAtual) {
                    if (forte == 3) screenSetColor(corJoia, FUNDO);
                    else screenSetColor(TXT, FUNDO);
                    corAtual = forte;
                }
            }
            fputs(BLK[idx], stdout);
        }
    }
}

// diz se o pixel (x, y) do sprite esta ligado
static int pix(const Sprite *s, int x, int y)
{
    if (y >= s->h || x >= s->w) {
        return 0;
    }
    return s->r[y][x] == '#';
}

// mostra um sprite direto no terminal (usado nos menus)
void spritePreview(int x, int y, const Sprite *s, int cor)
{
    screenSetColor(cor, FUNDO);
    for (int cy = 0; cy < (s->h + 1) / 2; cy++) {
        screenGotoxy(x, y + cy);
        for (int cx = 0; cx < s->w; cx++) {
            int idx = pix(s, cx, 2 * cy) | (pix(s, cx, 2 * cy + 1) << 1);
            fputs(BLK[idx], stdout);
        }
    }
}

/* ================= FIM DA SECAO 2 ================= */



/* ================= SECAO 3 - MUNDO (Pessoa B) ================= */
/* ================= FIM DA SECAO 3 ================= */



/* ================= SECAO 4 - RANKING (Pessoa D) ================= */
/* ================= FIM DA SECAO 4 ================= */



/* ================= SECAO 5 - MENUS E TELAS (Pessoa D) ================= */
/* ================= FIM DA SECAO 5 ================= */



/* ================= SECAO 6 - DESENHO E FASE (Pessoa C) ================= */
/* ================= FIM DA SECAO 6 ================= */



/* ================= SECAO 7 - JOGARPARTIDA E MAIN (todos) ================= */
#ifndef SEM_MAIN
int main(void)
{
    return 0;
}
#endif
/* ================= FIM DA SECAO 7 ================= */