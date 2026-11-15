#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "screen.h"
#include "tipos.h"
#include "util.h"
#include "dados.h"
#include "tela.h"

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
        screenGotoxy(OFFX, OFFY + cy);      // antes: COLUNA_TERM, LINHA_TERM
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

// diz se o pixel (x, y) do sprite está ligado
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