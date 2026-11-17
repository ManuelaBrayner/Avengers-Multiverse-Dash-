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

// le uma tecla. devolve -1 se nao tem nenhuma
int lerTeclaBruta(void){
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
int lerTecla(void){
    int c = lerTeclaBruta();
    if (c >= 'A' && c <= 'Z') {
        c = c + 32;
    }
    return c;
}

// joga fora as teclas que estavam esperando
void limparTeclado(void){
    while (keyhit()) {
        readch();
    }
}

// conta quantas colunas o texto ocupa (letra com acento conta 1)
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

// escreve texto colorido numa posicao (funciona como printf)
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

// escreve texto colorido no meio da linha y
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

// pinta o terminal inteiro com a cor de fundo
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

// espera apertar Enter ou Espaco
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

/* ================= FIM DA SECAO 1 ================= */



/* ================= SECAO 2 - TELA DE PIXELS (Pessoa A) ================= */

// cria a matriz de pixels
Tela *telaCriar(int larg, int alt){
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
void telaLiberar(Tela *t){
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
void telaLimpar(Tela *t){
    for (int y = 0; y < t->alt; y++) {
        memset(t->m[y], 0, t->larg);
    }
}

// liga um pixel (ignora se estiver fora da tela)
void telaPut(Tela *t, int x, int y, int v){
    if (x < 0 || y < 0 || x >= t->larg || y >= t->alt) {
        return;
    }
    t->m[y][x] = (unsigned char)v;
}

// desenha um sprite -> cada # vira um pixel
void telaSprite(Tela *t, int x, int yTopo, const Sprite *s, int v){
    for (int linha = 0; linha < s->h; linha++) {
        for (int coluna = 0; coluna < s->w; coluna++) {
            if (s->r[linha][coluna] == '#') {
                telaPut(t, x + coluna, yTopo + linha, v);
            }
        }
    }
}

// transforma a matriz em caracteres e mostra no terminal
void telaDesenhar(const Tela *t, int corJoia){
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
static int pix(const Sprite *s, int x, int y){
    if (y >= s->h || x >= s->w) {
        return 0;
    }
    return s->r[y][x] == '#';
}

// mostra um sprite direto no terminal (usado nos menus)
void spritePreview(int x, int y, const Sprite *s, int cor){
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
Entidade *entidadeNova(TipoEnt tipo, double x, int alt, int w, int h, int valor, const Sprite *const *sp){
    Entidade *e = xmalloc(sizeof *e);
    e->tipo = tipo; e->x = x; e->alt = alt; e->w = w; e->h = h; e->valor = valor;
    e->spr[0] = sp[0];
    e->spr[1] = sp[1];
    e->prox = NULL;
    return e;
}

void listaInserir(Entidade **lista, Entidade *novo){
    novo->prox = *lista;
    *lista = novo;
}

void listaLiberar(Entidade **lista){
    Entidade *e = *lista;
    while (e) { Entidade *p = e->prox; free(e); e = p; }
    *lista = NULL;
}

int pontosTotais(const Jogo *j){
    return (int)j->metros + j->fragTotal * 10 + j->nJoias * 500 + j->bonus;
}

int colide(const Jogador *p, const Entidade *e){
    int ini = (int)floor(e->x), fim = ini + e->w - 1;
    if (e->tipo == OBSTACULO) { ini++; fim--; }          /* hitbox um pouco menor */
    if (ini > PX + 6 || fim < PX + 2) return 0;          /* horizontal */
    int ra = (int)(p->altura + 0.5);
    int ph = (p->agachado > 0 && p->noChao) ? 7 : 11;    /* altura do jogador */
    return ra < e->alt + e->h && e->alt < ra + ph;       /* vertical */
}

void criarObstaculo(Jogo *j, const Modelo *m, double x){
    listaInserir(&j->lista,
        entidadeNova(OBSTACULO, x, m->alt, m->spr[0]->w, m->spr[0]->h, 2, m->spr));
}

void criarTrilha(Jogo *j, double x, int alt, int qtd){
    for (int i = 0; i < qtd; i++)
        listaInserir(&j->lista, entidadeNova(FRAGMENTO, x + i * 5, alt, 3, 3, 3, PAR_FRAG));
}

void spawnEvento(Jogo *j, const Fase *f){
    double x = LARG;
    int meta = j->fragFase >= f->meta;

    if (meta && !j->joiaViva) {                           /* hora da joia */
        listaInserir(&j->lista, entidadeNova(JOIA, x, rnd(2) ? 16 : 2, 7, 7, 3, PAR_JOIA));
        j->joiaViva = 1;
        return;
    }

    int r = rnd(100);
    if (r < f->pctVoador) {                               /* obstaculo voador */
        criarObstaculo(j, &f->mod[2], x);
        if (!meta) criarTrilha(j, x + 22, 3, 3);
    } else if (r < f->pctVoador + 50 || meta) {           /* obstaculo no chao */
        const Modelo *m = &f->mod[rnd(2)];
        criarObstaculo(j, m, x);
        int lw = m->spr[0]->w;
        if (rnd(100) < f->pctDupla) {                     /* obstaculo duplo */
            const Modelo *m2 = &f->mod[rnd(2)];
            criarObstaculo(j, m2, x + lw + 4);
            lw += m2->spr[0]->w + 4;
        }
        if (!meta && rnd(100) < 75) criarTrilha(j, x + lw / 2 - 5, m->spr[0]->h + 3, 3);
    } else {                                              /* so fragmentos */
        criarTrilha(j, x, rnd(2) ? 16 : 3, 4);
    }
}

/* move o mundo e trata colisoes; retorna 1 se pegou a joia */
int atualizarMundo(Jogo *j, const Fase *f, double v){
    int pegouJoia = 0;
    Entidade **pp = &j->lista;          /* ponteiro para o ponteiro: remove qualquer no */
    while (*pp) {
        Entidade *e = *pp;
        int remover = 0;
        e->x -= v;
        if (e->x + e->w < 0) {
            remover = 1;
            if (e->tipo == JOIA) j->joiaViva = 0;
        } else if (colide(&j->jog, e)) {
            if (e->tipo == FRAGMENTO) {
                j->fragFase++; j->fragTotal++;
                j->th.dist += 0.5;
                if (j->th.dist > f->distIni) j->th.dist = f->distIni;
                remover = 1;
            } else if (e->tipo == JOIA) {
                pegouJoia = 1;
                remover = 1;
            } else if (j->jog.invul == 0) {
                j->th.dist -= PENALIDADE;
                j->jog.invul = 24;
                remover = 1;
            }
        }
        if (remover) { *pp = e->prox; free(e); }
        else pp = &e->prox;
    }
    return pegouJoia;
}

void atualizarJogador(Jogador *p){
    if (!p->noChao) {
        p->altura += p->vy;
        p->vy -= GRAV;
        if (p->altura <= 0) { p->altura = 0; p->vy = 0; p->noChao = 1; }
    }
    if (p->agachado > 0) p->agachado--;
    if (p->invul > 0) p->invul--;
}
/* ================= FIM DA SECAO 3 ================= */



/* ================= SECAO 4 - RANKING (Pessoa D) ================= */
// cria um no novo e coloca ele ja na posicao certa (maior pontuacao primeiro)
// devolve a cabeca da lista (ela muda se o novo for o maior)
// novoOut devolve o no criado (para destacar na tela). pode ser NULL
Score *rankingInserir(Score *ini, const char *nome, int pontos, int joias, Score **novoOut)
{
    Score *novo = xmalloc(sizeof(Score));
    snprintf(novo->nome, sizeof novo->nome, "%s", nome);
    novo->pontos = pontos;
    novo->joias = joias;
    novo->prox = NULL;

    if (novoOut != NULL) {
        *novoOut = novo;
    }

    // lista vazia ou novo e o maior de todos: vira a cabeca
    if (ini == NULL || pontos > ini->pontos) {
        novo->prox = ini;
        return novo;
    }

    // senao anda ate o ultimo que tem pontos >= (no empate, quem ja estava fica na frente)
    Score *atual = ini;
    while (atual->prox != NULL && atual->prox->pontos >= pontos) {
        atual = atual->prox;
    }
    novo->prox = atual->prox;
    atual->prox = novo;
    return ini;
}

// le o arquivo e monta a lista ordenada. se o arquivo nao existe, devolve lista vazia
Score *rankingCarregar(const char *arq)
{
    FILE *f = fopen(arq, "r");
    if (f == NULL) {
        return NULL;
    }

    Score *lista = NULL;
    char nome[16];
    int pontos, joias;

    // %15s -> no maximo 15 letras, porque nome[16] precisa do '\0'
    while (fscanf(f, "%15s %d %d", nome, &pontos, &joias) == 3) {
        lista = rankingInserir(lista, nome, pontos, joias, NULL);
    }
    fclose(f);
    return lista;
}
 
// grava os MAX_RANK melhores no arquivo (o "w" apaga o conteudo antigo)
void rankingSalvar(const Score *lista, const char *arq)
{
    FILE *f = fopen(arq, "w");
    if (f == NULL) {
        return;
    }

    int n = 0;
    for (const Score *s = lista; s != NULL && n < MAX_RANK; s = s->prox) {
        fprintf(f, "%s %d %d\n", s->nome, s->pontos, s->joias);
        n++;
    }
    fclose(f);
}

// libera todos os nos
void rankingLiberar(Score *lista)
{
    while (lista != NULL) {
        Score *prox = lista->prox;   // guarda o proximo ANTES do free
        free(lista);
        lista = prox;
    }
}

// desenha a tabela. o no 'destaque' (pode ser NULL) aparece em vermelho
void rankingMostrar(const Score *lista, const Score *destaque)
{
    centro(4, TXT, "RANKING");
    escrever(22, 6, SUAVE, "POS  NOME           PONTOS  JOIAS");

    int n = 0;
    for (const Score *s = lista; s != NULL && n < MAX_RANK; s = s->prox) {
        int cor = TXT;
        if (s == destaque) {
            cor = RED;
        }
        escrever(22, 7 + n, cor, "%2d   %-12s  %6d  %d/%d",
                 n + 1, s->nome, s->pontos, s->joias, NUM_FASES);
        n++;
    }

    if (n == 0) {
        centro(9, SUAVE, "Nenhuma partida ainda.");
    }
}
/* ================= FIM DA SECAO 4 ================= */


/* ================= SECAO 5 - MENUS E TELAS (Pessoa D) ================= */
// carrega o ranking, mostra, libera e espera Enter
// carrega o ranking, mostra, libera e espera Enter
void telaRanking(void)
{
    Score *r = rankingCarregar(ARQ_SCORES);

    limparTela();
    rankingMostrar(r, NULL);
    centro(20, SUAVE, "ENTER para voltar");
    screenUpdate();

    rankingLiberar(r);
    esperarEnter();
}

// le o nome tecla por tecla (letras, numeros e hifen; Backspace apaga)
// dest precisa ter espaco para max + 1 caracteres
void lerNome(char *dest, int max, int x, int y)
{
    int n = 0;
    dest[0] = '\0';
    limparTeclado();

    while (1) {
        // mostra o nome com '_' de cursor (o espaco no fim apaga a letra apagada)
        escrever(x, y, TXT, "%s_ ", dest);
        screenUpdate();

        int t;
        while ((t = lerTeclaBruta()) == -1) {
            usleep(10000);
        }

        if (t == 10 || t == 13) {                       // Enter termina
            break;
        }
        if ((t == 127 || t == 8) && n > 0) {            // Backspace apaga
            n--;
            dest[n] = '\0';
        } else if (t > 0 && t < 128 && n < max && (isalnum(t) || t == '-')) {
            dest[n] = (char)t;
            n++;
            dest[n] = '\0';
        }
    }

    if (n == 0) {
        snprintf(dest, max + 1, "ANONIMO");
    }
}
 
// tela que aparece antes de cada fase
void telaIntroFase(const Fase *f, int idx){
    limparTela();
    centro(5, f->cor, "FASE %d  -  %s", idx + 1, f->cenario);
    centro(7, TXT, "%s", f->joia);
    centro(10, SUAVE, "%s", f->intro1);
    centro(11, SUAVE, "%s", f->intro2);
    centro(14, TXT, "Junte %d fragmentos e pegue a joia.", f->meta);
    centro(20, SUAVE, "ENTER para correr");
    screenUpdate();
    esperarEnter();
}

// tela que aparece quando pega a joia da fase
void telaJoiaConquistada(const Jogo *j, int idx){
    const Fase *f = &FASES[idx];
 
    limparTela();
    centro(6, f->cor, "%s conquistada!", f->joia);
    spritePreview(38, 9, &SP_JOIA, f->cor);
    centro(14, TXT, "Pontos: %d    Joias: %d/%d", pontosTotais(j), j->nJoias, NUM_FASES);
    centro(20, SUAVE, "ENTER para continuar");
    screenUpdate();
    esperarEnter();
}
 
// fim da partida: mostra resultado, pede o nome, salva e mostra o ranking
void fimDeJogo(Jogo *j, int venceu, int ultimaFase){
    limparTela();
    if (venceu) {
        centro(5, GREEN, "Voce reuniu as 6 Joias do Infinito!");
        j->bonus = 2000;
    } else {
        centro(5, RED, "Thanos te alcancou!");
        centro(6, SUAVE, "Fase %d: %s", ultimaFase + 1, FASES[ultimaFase].cenario);
    }
    centro(9, TXT, "Pontos: %d    Joias: %d/%d", pontosTotais(j), j->nJoias, NUM_FASES);
    centro(12, TXT, "Seu nome (ate 12 letras) e ENTER:");
    screenUpdate();
 
    char nome[16];
    lerNome(nome, 12, 34, 14);
 
    // carrega, insere o novo, salva (so os 8 melhores vao pro arquivo)
    Score *r = rankingCarregar(ARQ_SCORES);
    Score *novo = NULL;
    r = rankingInserir(r, nome, pontosTotais(j), j->nJoias, &novo);
    rankingSalvar(r, ARQ_SCORES);
 
    // descobre em que posicao o novo ficou
    int pos = 1;
    for (const Score *s = r; s != NULL && s != novo; s = s->prox) {
        pos++;
    }
 
    limparTela();
    rankingMostrar(r, novo);
    if (pos > MAX_RANK) {
        centro(17, SUAVE, "Voce ficou na posicao %d (so os %d melhores aparecem).", pos, MAX_RANK);
    }
    centro(20, SUAVE, "ENTER para voltar ao menu");
    screenUpdate();
 
    rankingLiberar(r);
    esperarEnter();
}

// menu principal. devolve 1 (jogar), 2 (ranking), 3 (como jogar) ou 4 (sair)
int menuPrincipal(void){
    limparTela();
    centro(5, TXT, "A V E N G E R S    R U N");
    centro(7, SUAVE, "Fuja de Thanos e reuna as 6 Joias do Infinito");
    escrever(34, 11, TXT, "[1] Jogar");
    escrever(34, 13, TXT, "[2] Ranking");
    escrever(34, 15, TXT, "[3] Como jogar");
    escrever(34, 17, TXT, "[4] Sair");
    screenUpdate();
 
    limparTeclado();
    while (1) {
        int t = lerTecla();
        if (t >= '1' && t <= '4') {
            return t - '0';
        }
        usleep(10000);
    }
}
 
// escolha do personagem. devolve 0 (homem), 1 (mulher) ou -1 (voltar)
int escolherPersonagem(void){
    limparTela();
    centro(4, TXT, "Escolha seu personagem");
    for (int p = 0; p < 2; p++) {
        int x = 22;
        if (p == 1) {
            x = 50;
        }
        escrever(x - 1, 8, TXT, "[%d] %s", p + 1, NOME_JOGADOR[p]);
        spritePreview(x + 2, 11, &SPR_JOG[p][0], TXT);
    }
    centro(20, SUAVE, "[0] voltar");
    screenUpdate();
 
    limparTeclado();
    while (1) {
        int t = lerTecla();
        if (t == '1') return 0;
        if (t == '2') return 1;
        if (t == '0' || t == 27) return -1;
        usleep(10000);
    }
}
 
// regras e controles
void telaComoJogar(void)
{
    limparTela();
    centro(3, TXT, "Como jogar");
    escrever(10, 6, TXT, "Fuja de Thanos! Em cada fase (uma Joia do Infinito):");
    escrever(10, 8, SUAVE, "1. Junte os fragmentos para a joia aparecer.");
    escrever(10, 9, SUAVE, "2. Pegue a joia para passar de fase.");
    escrever(10, 10, SUAVE, "3. Bater num obstaculo aproxima Thanos.");
    escrever(10, 11, SUAVE, "4. Se ele te alcancar, acabou.");
    escrever(10, 14, TXT, "Controles:");
    escrever(12, 15, SUAVE, "W / ESPACO ... pular");
    escrever(12, 16, SUAVE, "S ............ agachar (no ar: cair rapido)");
    escrever(12, 17, SUAVE, "P ............ pausa      Q ... sair da fase");
    centro(20, SUAVE, "ENTER para voltar");
    screenUpdate();
    esperarEnter();
}

/* ================= FIM DA SECAO 5 ================= */



/* ================= SECAO 6 - DESENHO E FASE (Pessoa C) ================= */

static void desenharJogo(Jogo *j, const Fase *f){ /*Cenário e personagens*/
    /*Ponteiro para a tela virtual e apaga o quadro anterior, para começar do zero*/
    Tela *t = j->tela;
    telaLimpar(t);

    /* chao */
    int valor_oh = (int)j->offChao; /*offChao é o deslocamento do chão e valor_oh é esse valor convertido pra inteiro*/
    for (int i = 0; i < LARG; i++) { /*Percorre todas as colunas*/
        if (((unsigned int)(valor_oh + i) * 2654435761u >> 16) % 23 == 0){ /*constante de hash de Knuth*/
            telaPut(t, i, GL + 2, 2);
        /*O if decide se aquela coluna ganha um detalhe extra (tipo uma pedrinha) duas linhas abaixo, em GL + 2*/
        }
    }

    /* obstaculos, fragmentos e joia (percorre a lista) */
    int quadro = (int)((j->frame / 4) % 2); /*Animação alterna a cada 4 quadros*/
    for (const Entidade *e = j->lista; e; e = e->prox) {
        const Sprite *s; 
        if (e->spr[1] && quadro){
            s = e->spr[1]; /*Se a entidade tem um segundo sprite e estamos no quadro 1, usa o segundo*/
        } else {
            s = e->spr[0]; /* Senão usa o primeiro.*/
        }
        telaSprite(t, (int)floor(e->x), PES - e->alt - (s->h - 1), s, e->valor);
        /*PES é o nível do chão*/
    }

    /* Thanos */
    int dir = PX - 1 - (int)j->th.dist; /*dir é a coluna da borda direita do Thanos e PX é a coluna do jogador*/
    telaSprite(t, dir - 13, PES - 17, &SPR_THANOS[(j->frame / 3) % 2], 2); /*SPR_THANOS[(frame / 3) % 2] alterna entre dois desenhos a cada 3 quadros, dando a animação*/

    /* jogador (pisca quando leva dano) */
    if (!(j->jog.invul > 0 && (j->frame % 4) < 2)) { /*Invunerável e fase "apagada"*/
        int pose;
        if (!j->jog.noChao){ /*no ar: pose de pulo*/
            pose = 2;
        } else if (j->jog.agachado > 0){ /*agachado*/
            pose = 3;
        } else { /*correndo: alterna 0 e 1*/
            pose = (int)((j->frame / 3) % 2);
        }
        const Sprite *s = &SPR_JOG[j->sexo][pose];
        telaSprite(t, PX, PES - (s->h - 1) - (int)(j->jog.altura + 0.5), s, 2);
    }
    telaDesenhar(t, f->cor);
}

/*Escrito direto no terminal diferentemente de desenharJogo*/
static void desenharHud(const Jogo *j, const Fase *f, int idx){ /*Painel de informações (fase, pontos, barra do Thanos, joias)*/
    int pts = pontosTotais(j);
    int hi; /*Guarda resultado; Maior entre o recordo salvo e os pontos atuais*/
    if (j->recorde > pts) {
        hi = j->recorde;
    } else {
        hi = pts;
    }

    int frag;
    if(j->fragFase > f->meta) {
        frag = f->meta;
    } else {
        frag = j->fragFase;
    }

    screenGotoxy(5, 2);
    screenSetColor(TXT, FUNDO);
    printf("FASE %d/%d  ", idx + 1, NUM_FASES); /*Mostra a fase atual*/
    screenSetColor(f->cor, FUNDO);
    printf("%-10s", f->cenario); /*Mostra o nome do cenário na cor da fase*/
    screenSetColor(TXT, FUNDO);
    printf("   FRAG %02d/%02d", frag, f->meta); /*Mostra os fragmentos coletados*/
    screenSetColor(SUAVE, FUNDO);
    printf("      HI %06d", hi); /*Recorde com 6 dígitos*/
    screenSetColor(TXT, FUNDO);
    printf("  %06d  ", pts); /*Mostra os pontos atuais*/

    /* Barra de distancia do Thanos */
    double prop = j->th.dist / f->distIni;  /*Fração que resta entre eu e o Thanos (distância atual / distância inicial)*/
    if (prop < 0) {
        prop = 0;
    }
    if (prop > 1) {
        prop = 1;
    }
    int cheio = (int)(prop * 20); /*Quantos dos 20 blocos da barra ficam preenchidos*/

    screenGotoxy(5, 22);
    printf("THANOS ");
    if (prop > 0.4){
        screenSetColor(TXT, FUNDO); /*Barra fica na cor normal*/
    } else {
        screenSetColor(RED, FUNDO); /*Barra fica vermelha quando Thanos está perto*/
    }

    for (int i = 0; i < 20; i++) {
        if (i < cheio) {
            fputs("█", stdout); /*Bloco cheio*/
        } else {
            fputs("░", stdout); /*Bloco vazio, claro*/
        } /*stdout assumindo a saída padrão na tela e fputs porque não há formatação a fazer*/
    }

    screenSetColor(TXT, FUNDO);
    printf("    JOIAS ");
    for (int i = 0; i < NUM_FASES; i++) { /*Mostra uma joia por fase*/
        if (j->joias[i]) {
            screenSetColor(FASES[i].cor, FUNDO);
            printf("◆ "); /*Joia coletada*/
        } else {
            screenSetColor(SUAVE, FUNDO);
            printf("◇ "); /*Joia ainda não coletada*/
        }
    }
}

/* retorna 1 = venceu a fase, 0 = Thanos alcancou, -1 = saiu */
static int jogarFase(Jogo *j, int idx){ /*idx = número da fase*/
    const Fase *f = &FASES[idx];
    int resultado = -2, pausado = 0; /*0 = rodando, 1 = pausado*/

    telaIntroFase(f, idx);

    listaLiberar(&j->lista);
    j->jog = (Jogador){ .sexo = j->sexo, .noChao = 1 }; /*jogador é reiniciado*/
    j->th.dist = f->distIni; /*Thanos começa à distância inicial da fase*/
    j->fragFase = 0;  /*Nenhum fragmento coletado nesta fase*/
    j->joiaViva = 0; /*Não há joia em cena (provavelmente uma flag)*/
    j->proxSpawn = 40; /*Contador até o próximo obstáculo/item aparecer*/
    j->offChao = 0; /* O chão começa sem deslocamento */
    j->frame = 0; /* Contador de quadros zerado */

    limparTela(); /*Apaga o terminal*/
    limparTeclado(); /*Descarta teclas que tenham sido pressionadas*/
    timerInit(50);  /* 20 quadros por segundo */

    while (resultado == -2) {
        int t;
        /*Lendo o teclado*/
        while ((t = lerTecla()) != -1) { 
            if (t == 'q' || t == 27 || t == 3) { 
                resultado = -1;
                break;
            } 
            if (t == 'p') {
                pausado = !pausado;
            }
            if (pausado) {
                continue;
            }

            if (t == 'w' || t == ' ' || t == TECLA_CIMA) { /*Pular*/
                if (j->jog.noChao) { 
                    j->jog.vy = PULO; j->jog.noChao = 0; j->jog.agachado = 0; 
                }
            } else if (t == 's' || t == TECLA_BAIXO) { /*Agachar*/
                if (j->jog.noChao) {
                    j->jog.agachado = 10;
                } else {
                    j->jog.vy = QUEDA;
                }
            }
        }

        /* Se o jogador apertou para sair, pula o restante da iteração */
        if (resultado != -2) {
            break;
        }

        /*Cálculo da velocidade*/
        if (timerTimeOver() && !pausado) { 
            int frags;
            if (j->fragFase > f->meta) {
                frags = f->meta;
            } else {
                frags = j->fragFase;
            }
            double prog = (double)frags / f->meta;
            double v = f->vel * (1.0 + 0.25 * prog); /* Acelera dentro da fase conforme coleta fragmentos */

            /*Atualizando o jogo e mundo*/
            atualizarJogador(&j->jog);
            if (atualizarMundo(j, f, v)) { 
                resultado = 1; 
            } else {
                /*Gerar novos obstáculos*/
                j->proxSpawn = j->proxSpawn - v; /*Diminui o contador de spawn pela velocidade atual do jogo*/
                if (j->proxSpawn <= 0) {
                    spawnEvento(j, f);
                    j->proxSpawn = f->gapMin + v * 14 + rnd(f->gapVar);
                }

                /*Thanos*/
                j->th.dist += 0.03;  /* Thanos se afasta devagar */
                if (j->th.dist > f->distIni) { 
                    j->th.dist = f->distIni; 
                } /* Thanos não pode se afastar mais que a distância inicial */
                if (j->th.dist <= 0) { 
                    resultado = 0; 
                } /*Thanos alcançou o jogador*/
            }

            /*Avancar o estado e desenhar*/
            if(resultado == -2) {
                j->offChao += v;  /*O chão anda (usado nas pedrinhas)*/
                j->metros += v * 0.25;  /*Distância percorrida (conta para a pontuação)*/
                j->frame++;  /*Próximo quadro (usado nas animações)*/
            

                /*Desenhar*/
                desenharJogo(j, f);  /*Monta o cenário e personagens*/
                desenharHud(j, f, idx);  /*Escreve o painel*/
                screenUpdate();  /*Envia tudo ao terminal*/
            }
        }
        if (resultado == -2) {
            usleep(2000); /*A cada volta do laço, o programa pausa por 2000 microsegundos; fim do laço*/
        }
    }
    listaLiberar(&j->lista);
    return resultado; 
}

/* ================= FIM DA SECAO 6 ================= */



/* ================= SECAO 7 - JOGARPARTIDA E MAIN (todos) ================= */

// roda as 6 fases em sequencia e chama o fim de jogo
void jogarPartida(Jogo *j, int sexo){
    int venceu = 0;
    int ultima = 0;
 
    j->sexo = sexo;
    j->metros = 0;
    j->fragTotal = 0;
    j->nJoias = 0;
    j->bonus = 0;
    for (int i = 0; i < NUM_FASES; i++) {
        j->joias[i] = 0;
    }
 
    // o recorde (HI) e o primeiro do ranking
    Score *r = rankingCarregar(ARQ_SCORES);
    if (r != NULL) {
        j->recorde = r->pontos;
    } else {
        j->recorde = 0;
    }
    rankingLiberar(r);
 
    for (int i = 0; i < NUM_FASES; i++) {
        ultima = i;
        int res = jogarFase(j, i);
 
        if (res < 0) {
            return;                 // saiu: volta ao menu sem salvar
        }
        if (res == 0) {
            break;                  // Thanos alcancou
        }
        j->joias[i] = 1;
        j->nJoias++;
        telaJoiaConquistada(j, i);
        if (i == NUM_FASES - 1) {
            venceu = 1;
        }
    }
    fimDeJogo(j, venceu, ultima);
}
 
#ifndef SEM_MAIN
int main(void){
    srand((unsigned)time(NULL));
    screenInit(0);
    keyboardInit();
    timerInit(50);
    limparTeclado();
 
    Jogo *j = xmalloc(sizeof(Jogo));
    memset(j, 0, sizeof(Jogo));
    j->tela = telaCriar(LARG, ALTP);
 
    int sair = 0;
    while (!sair) {
        int op = menuPrincipal();
        if (op == 1) {
            int s = escolherPersonagem();
            if (s >= 0) {
                jogarPartida(j, s);
            }
        } else if (op == 2) {
            telaRanking();
        } else if (op == 3) {
            telaComoJogar();
        } else {
            sair = 1;
        }
    }
 
    // limpeza: sem isso o terminal fica sem eco e sem cursor
    listaLiberar(&j->lista);
    telaLiberar(j->tela);
    free(j);
 
    keyboardDestroy();
    timerDestroy();
    screenDestroy();
    return 0;
}
#endif

/* ================= FIM DA SECAO 7 ================= */
