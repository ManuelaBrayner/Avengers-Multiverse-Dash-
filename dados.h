#ifndef DADOS_H
#define DADOS_H

#include "tipos.h"

/* espaço, meio bloco de cima, meio bloco de baixo, bloco cheio */
static const char *const BLK[4] = { " ", "\xe2\x96\x80", "\xe2\x96\x84", "\xe2\x96\x88" };



// SPRITES
#define HOMEM_CORPO  "..###....",".#####...",".#####...","..###....",".#####...","#.###.#..","#.###.#..","..###....","..###...."
#define MULHER_CORPO "..###....","#.#####..","#..###...","..###....",".#####...","#.###.#..","#.###.#..",".#####...","#######.."
#define PERNAS_A     "..#.#....","..#.#....",".##.##..." //Pernas retas e juntas
#define PERNAS_B     "..#.#....",".#...#...",".##..##.." //Pernas abertas, em passada
#define PERNAS_PULO  ".#...#...",".#...#...","##...##.." //Pernas abertas desde o início

/* matriz de sprites: [personagem][pose]  pose: 0 corre A, 1 corre B, 2 pulo, 3 agachado */
/*static = só existe dentro do arquivo e cons = dados fixos, não podem ser alterados pelo programa*/

static const Sprite SPR_JOG[2][4] = { 
// Linha 0 da matriz: Homem (sprite0: passo A, sprite1: passo B, sprite2: pulo, sprite3: agachado)
    {
        { 9, 12, { HOMEM_CORPO, PERNAS_A } },
        { 9, 12, { HOMEM_CORPO, PERNAS_B } },
        { 9, 12, { HOMEM_CORPO, PERNAS_PULO } },
        { 10, 8, {
            "...###....",
            "..#####...",
            ".#######..",
            "########..",
            "#########.",
            ".#######..",
            "..#...#...",
            ".##...##.."
        }} // Agachado = Não usa #define
    },

    // Linha 1 da matriz: Mulher (sprite0: passo A, sprite1: passo B, sprite2: pulo, sprite3: agachado)
    {
        { 9, 12, { MULHER_CORPO, PERNAS_A } },
        { 9, 12, { MULHER_CORPO, PERNAS_B } },
        { 9, 12, { MULHER_CORPO, PERNAS_PULO } },
        { 10, 8, {
            "...###....",
            "#.#####...", // <- Mantido o '#' na 1ª coluna (cabelo); troque por '.' se foi sem querer
            ".#######..",
            "########..",
            "#########.",
            ".#######..",
            "..#...#...",
            ".##...##.."
        }} // Agachado = Não usa #define
    } 
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



/* ================= FASES ================= */
static const Fase FASES[NUM_FASES] = {
    /*Posições 0 e 1 são obstáculos de chão e posção 2 é voador*/
    { "Nova York", "Joia do Espaço",
    "Os Chitauri invadiram a cidade e o Tesseract brilha.",
    "Fuja de Thanos, pule os destroços e agache sob as naves.",
    BLUE, 1.40, 8, 22, 16, 10, 0, 12.0,
    { {"Destroço", {&SP_PEDRA, NULL}, 0}, {"Chitauri", {&SP_ROBO, NULL}, 0}, {"Nave", {&SP_NAVE, NULL}, VOO} } }, /*0 é obstáculo de chão e VOO é obstáculo voador*/

    { "Sokovia", "Joia da Mente",
    "Os drones de Ultron guardam o cetro na cidade que flutua.",
    "Desvie dos sentinelas e agache sob os drones.",
    BROWN, 1.56, 12, 21, 15, 18, 0, 11.0,
    { {"Escombros", {&SP_PEDRA, NULL}, 0}, {"Sentinela", {&SP_ROBO, NULL}, 0}, {"Drone", {&SP_NAVE, NULL}, VOO} } }, /*0 é obstáculo de chão e VOO é obstáculo voador*/

    { "Asgard", "Joia da Realidade",
    "O Éter se espalha pelos reinos e os Elfos Negros atacam.",
    "Cuidado: as naves voam baixo, agache na hora certa.",
    RED, 1.72, 16, 20, 14, 22, 10, 10.0,
    { {"Cristais", {&SP_CRISTAL, NULL}, 0}, {"Elfo Negro", {&SP_ROBO, NULL}, 0}, {"Nave Negra", {&SP_NAVE, NULL}, VOO} } }, 


    { "Xandar", "Joia do Poder",
    "A Orbe repousa em Xandar, cercada de soldados Kree.",
    "Os obstáculos vêm em dupla. Pule alto e mantenha o ritmo.",
    MAGENTA, 1.88, 20, 19, 13, 26, 18, 9.0,
    { {"Cristais", {&SP_CRISTAL, NULL}, 0}, {"Soldado Kree", {&SP_ROBO, NULL}, 0}, {"Nave Kree", {&SP_NAVE, NULL}, VOO} } }, 

    { "Kamar-Taj", "Joia do Tempo",
    "O Olho de Agamotto dobra o tempo e abre portais.",
    "Fragmentos de tempo cortam o ar. Não deixe Thanos chegar.",
    GREEN, 2.04, 24, 18, 12, 30, 25, 8.0,
    { {"Portal", {&SP_PORTAL, NULL}, 0}, {"Mago Sombrio", {&SP_ROBO, NULL}, 0}, {"Fragmento", {&SP_FRAG, NULL}, VOO} } }, 

    { "Vormir", "Joia da Alma",
    "No penhasco de Vormir, a última joia cobra um preço alto.",
    "Tudo é mais rápido. Esta é a última corrida.",
    LIGHTRED, 2.20, 28, 17, 12, 34, 32, 8.0,
    { {"Rochas", {&SP_PEDRA, NULL}, 0}, {"Espectro", {&SP_ROBO, NULL}, 0}, {"Corvo", {&SP_CORVO_A, &SP_CORVO_B}, VOO} } } 
};


#endif