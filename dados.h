#ifndef DADOS_H
#define DADOS_H

#include "tipos.h"

/* espaço, meio bloco de cima, meio bloco de baixo, bloco cheio */
static const char *const BLK[4] = { " ", "\xe2\x96\x80", "\xe2\x96\x84", "\xe2\x96\x88" };

/* A pessoa C acrescenta aqui embaixo: sprites, SPR_JOG, PAR_FRAG, PAR_JOIA,
   NOME_JOGADOR e FASES (tudo static const) */


/* ------------------------------------------------------------------ */
/* Sprites                                                            */
/* ------------------------------------------------------------------ */
#define HOMEM_CORPO  "..###....",".#####...",".#####...","..###....",".#####...","#.###.#..","#.###.#..","..###....","..###...."
#define MULHER_CORPO "..###....","#.#####..","#..###...","..###....",".#####...","#.###.#..","#.###.#..",".#####...","#######.."
#define PERNAS_A     "..#.#....","..#.#....",".##.##..." //Pernas retas e juntas
#define PERNAS_B     "..#.#....",".#...#...",".##..##.." //Pernas abertas, em passada
#define PERNAS_PULO  ".#...#...",".#...#...","##...##.." //Pernas abertas desde o início

/* matriz de sprites: [personagem][pose]  pose: 0 corre A, 1 corre B, 2 pulo, 3 agachado */
/*static = só existe dentro do arquivo e cons = dados fixos, não podem ser alterados pelo programa*/

static const Sprite SPR_JOG[2][4] = { //0 = homem e 1 = mulher
    //sprite0, sprite1, sprite2, sprite3 (linha 0 da matriz: homem)
  { {9 /*largura*/, 12 /*altura*/, {HOMEM_CORPO, PERNAS_A}}, {9,12,{HOMEM_CORPO, PERNAS_B}}, {9,12,{HOMEM_CORPO, PERNAS_PULO}},
    {10,8,{"...###....","..#####...",".#######..","########..","#########.",".#######..","..#...#...",".##...##.."}} }, //Agachado = Não usa #define
    //sprite0, sprite1, sprite2, sprite3 (linha 0 da matriz: mulher)
  { {9,12,{MULHER_CORPO, PERNAS_A}}, {9,12,{MULHER_CORPO, PERNAS_B}}, {9,12,{MULHER_CORPO, PERNAS_PULO}},
    {10,8,{"...###....","#.#####...",".#######..","########..","#########.",".#######..","..#...#...",".##...##.."}} } //Agachado = Não usa #define
};
static const char *NOME_JOGADOR[2] = { "HEROI", "HEROINA" };

#define THANOS_CORPO "....######....","...########...","...########...","...#.####.#...","...########...","....######....","..##########..",".############.","##.########.##","##.########.##","##.########.##","##.########.##","##..######..##","....######...."
static const Sprite SPR_THANOS[2] = {
    //Duas versões de pernas
  {14,18,{THANOS_CORPO,"....##..##....","....##..##....","...###..###...","...###..###..."}},
  {14,18,{THANOS_CORPO,"...##....##...","...##....##...","..###....###..","..###....###.."}}
};

static const Sprite SP_PEDRA = {9,7,{"...#.....","...##.#..","..#####..",".#######.","########.","#########","#########"}}; //Monte de pedras
static const Sprite SP_CRISTAL = {7,8,{"...#...","..###..","..###.#",".#####.",".#####.",".######","#######","#######"}}; //Cristal
static const Sprite SP_ROBO = {7,8,{"..###..","..#.#..","#.###.#","#.###.#","..###..","..#.#..","..#.#..",".##.##."}}; //Robô
static const Sprite SP_PORTAL = {8,10,{"..####..",".#....#.","#......#","#......#","#......#","#......#","#......#","#......#",".#....#.","..####.."}}; //Portal
static const Sprite SP_NAVE = {10,4,{"...####...","##########",".########.","..#.#.#..."}}; //Nave
static const Sprite SP_CORVO_A = {12,6,{"......#.....","....###.....","...#####....","###########.",".########...","............"}}; //Asas pra cima
static const Sprite SP_CORVO_B = {12,6,{"............","............","###########.",".#########..","...#####....","....###....."}}; //Asas pra baixo
static const Sprite SP_FRAG = {3,3, {".#.","###",".#."}}; //Fragmento, pequena cruz
static const Sprite SP_JOIA = {7,7,{"...#...","..###..",".#####.","#######",".#####.","..###..","...#..."}};

//Quando o segundo é NULL, o objeto não tem animação, não tem nenhum sprite.
//Cada posição guarda um endereço
static const Sprite *const PAR_FRAG[2] = { &SP_FRAG, NULL };
static const Sprite *const PAR_JOIA[2] = { &SP_JOIA, NULL };



#endif