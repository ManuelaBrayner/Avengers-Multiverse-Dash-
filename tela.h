#ifndef TELA_H
#define TELA_H

#include "tipos.h"

Tela *telaCriar(int larg, int alt);
void telaLiberar(Tela *t);
void telaLimpar(Tela *t);
void telaPut(Tela *t, int x, int y, int v);
void telaSprite(Tela *t, int x, int yTopo, const Sprite *s, int v);
void telaDesenhar(const Tela *t, int corJoia);
void spritePreview(int x, int y, const Sprite *s, int cor);

#endif