#ifndef TIPOS_H
#define TIPOS_H

#include "screen.h"

#define LARG        72     
#define ALTP        34     
#define OFFX        5      
#define OFFY        4      
#define GL          28    
#define PES         27     
#define PX          20    
#define VOO         9      
#define NUM_FASES   6
#define ARQ_SCORES  "scores.txt"
#define MAX_RANK    8
#define GRAV        0.30
#define PULO        2.65
#define QUEDA       (-2.0)
#define PENALIDADE  5.0   

#define TECLA_CIMA 1001
#define TECLA_BAIXO 1002

#define FUNDO LIGHTGRAY
#define TXT BLACK
#define SUAVE DARKGRAY

typedef struct {
    int w, h;
    const char *r[18];
} Sprite;

typedef struct {
    int larg, alt;
    unsigned char **m;
} Tela;

typedef enum {
    OBSTACULO, FRAGMENTO, JOIA
} TipoEnt;

typedef struct Entidade {
    TipoEnt tipo;
    double x;
    int alt;
    int w, h;
    int valor;
    const Sprite *spr[2];
    struct Entidade *prox;
} Entidade;

typedef struct {
    int sexo;
    double altura, vy;
    int noChao, agachado, invul;
} Jogador;

typedef struct{
    double dist;
} Perseguidor;

typedef struct {
    const char *nome;
    const Sprite *spr[2];
    int alt;
} Modelo;

typedef struct {
    const char *cenario, *joia, *intro1, *intro2;
    int cor;
    double vel;
    int meta;
    int gapMin, gapVar;
    int pctVoador, pctDupla;
    double distInt;
    Modelo mod[3];
} Fase;

typedef struct {
    int sexo;
    Jogador jog;
    Perseguidor th;
    Entidade *lista;
    Tela *tela;
    double offChao, metros, proxSpawn;
    int fragFase, fragTotal, joiaViva;
    int joias[NUM_FASES], nJoias, bonus, recorde;
    unsigned long frame;
} Jogo;

typedef struct Score {      
    char nome[16];
    int pontos, joias;
    struct Score *prox;
} Score;

#endif


