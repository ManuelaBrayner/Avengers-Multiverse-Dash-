# Avengers: Multiverse Dash

# Avengers Run
 
Jogo de corrida lateral no terminal, estilo "jogo do dinossauro". Fuja de **Thanos**, desvie dos obstáculos e junte as **6 Joias do Infinito**, uma por fase.
 
Projeto em **C** com a biblioteca **cli-lib**, feito em equipe para a disciplina de Programação Imperativa e Funcional.
 
## Integrantes
 
| Integrante | Responsabilidade |
|---|---|
| A – Maria Júlia Dionísio | Utilitários e tela de pixels (seções 1 e 2 do `jogo.c`), `tipos.h` e Makefile |
| B – Milena Araújo | Mundo, lista de entidades, colisão e spawn (seção 3) |
| C – Rafaela Dubeux | Sprites, fases e laço do jogo (`dados.h` e seção 6) |
| D – Manuela Brayner | Ranking, menus e documentação (seções 4 e 5, README) |
| Todos | Integração (seção 7: `jogarPartida` e `main`), balanceamento e testes |

 
## Como o jogo funciona
 
Em cada fase (uma Joia do Infinito):
 
1. Junte os **fragmentos** para a joia aparecer.
2. Pegue a **joia** para passar de fase.
3. Bater em um obstáculo **aproxima Thanos**.
4. Se Thanos te alcançar, o jogo acaba.
Pegar fragmentos afasta Thanos um pouco. A velocidade aumenta ao longo de cada fase e de uma fase para a outra.
 
### Fases
 
| # | Cenário | Joia |
|---|---|---|
| 1 | Nova York | Joia do Espaço |
| 2 | Sokovia | Joia da Mente |
| 3 | Asgard | Joia da Realidade |
| 4 | Xandar | Joia do Poder |
| 5 | Kamar-Taj | Joia do Tempo |
| 6 | Vormir | Joia da Alma |
 
### Pontuação
 
`metros percorridos + 10 por fragmento + 500 por joia + 2000 de bônus se pegar as 6 joias`
 
Os 8 melhores resultados ficam salvos no arquivo `scores.txt`.
- Com o nome do jogador e a quantidade de pontos feitos na partida jogada. 
 
## Controles
 
| Tecla | Ação |
|---|---|
| `W`, `Espaço` ou seta para cima | pular |
| `S` ou seta para baixo | agachar (no ar: cair rápido) |
| `P` | pausar |
| `Q` | sair da fase e voltar ao menu |
| `Enter` | avançar nas telas |
 
Obstáculos de chão pedem **pulo**. Obstáculos voadores pedem **agachar**.
 
## Requisitos
 
- Linux, macOS ou **WSL** (a cli-lib usa recursos de terminal Unix; não roda direto no CMD ou PowerShell)
- `gcc`, `make` e `git`
- Terminal com pelo menos **80 colunas por 24 linhas** e codificação **UTF-8**
No Ubuntu/WSL:
 
```bash
sudo apt update
sudo apt install build-essential git
```
 
## Como compilar e jogar
 
```bash
git clone https://github.com/ManuelaBrayner/Avengers-Multiverse-Dash-.git
cd Avengers-Multiverse-Dash-
make
./avengers_run
```
 
Um aviso sobre `read` em `cli-lib/src/keyboard.c` vem da própria biblioteca e pode ser ignorado.
 
### Outros comandos
 
| Comando | O que faz |
|---|---|
| `make` | compila o jogo |
| `make asan` | compila com o AddressSanitizer (detecta erros de memória); rode com `./av_asan` |
| `make clean` | apaga os executáveis |
 
Se o terminal ficar sem eco depois de fechar o jogo de forma errada, digite `reset` e Enter.
 
## Estrutura do projeto
 
```
tipos.h      constantes (#define) e structs
dados.h      sprites, tabela de fases e caracteres de bloco (static const)
jogo.c       todo o resto, dividido em 7 seções
Makefile     compilação
cli-lib/     biblioteca de tela, teclado e timer
```
 
O `jogo.c` segue esta ordem (cada seção só usa o que veio antes):
 
| Seção | Conteúdo |
|---|---|
| 1 | utilitários: `xmalloc`, `rnd`, teclado, `escrever`, `centro`, `limparTela`, `esperarEnter` |
| 2 | tela de pixels: `telaCriar`, `telaLiberar`, `telaPut`, `telaSprite`, `telaDesenhar`, `spritePreview` |
| 3 | mundo: lista de entidades, colisão, spawn, `atualizarMundo`, `atualizarJogador` |
| 4 | ranking: lista ordenada e arquivo `scores.txt` |
| 5 | menus e telas de texto |
| 6 | desenho do jogo, HUD e `jogarFase` |
| 7 | `jogarPartida` e `main` |
 
### Como o desenho funciona
 
Cada célula do terminal mostra **2 pixels na vertical**, usando os caracteres espaço, `▀`, `▄` e `█`. O jogo desenha em uma matriz de 72 por 34 pixels e depois converte cada par vertical em um caractere. Assim os pixels ficam quadrados.
 
## Mapa dos requisitos do trabalho
 
| Requisito | Onde está |
|---|---|
| **Structs** | `tipos.h`: `Sprite`, `Tela`, `Entidade`, `Jogador`, `Perseguidor`, `Modelo`, `Fase`, `Jogo`, `Score` |
| **Ponteiros** | `Jogo *j` passado às funções; `Entidade **pp` para inserir e remover nós; `const Sprite *` para sprites compartilhados |
| **Alocação dinâmica** | `malloc` e `free` em `xmalloc`, `telaCriar`/`telaLiberar`, `entidadeNova`/`listaLiberar`, `rankingInserir`/`rankingLiberar` e no `Jogo` do `main` |
| **Listas encadeadas** | `Entidade` (objetos do mundo) e `Score` (ranking ordenado) |
| **Matrizes** | `Tela.m` (matriz dinâmica de pixels) e `SPR_JOG[2][4]` (sprites por personagem e pose) |
| **Arquivo** | `scores.txt`, lido com `fscanf` e gravado com `fprintf` |
| **Biblioteca** | cli-lib: `screen.h` (tela e cores), `keyboard.h` (teclado sem Enter) e `timer.h` (20 quadros por segundo) |
 
## Como trabalhamos em equipe
 
- Uma **branch por etapa**, nunca commit direto na `main`.
- Cada pessoa escreve **só na sua seção** do `jogo.c`.
- Pull request revisado por uma colega antes do merge.
- Antes de começar: `git pull`. Antes de enviar: compilar e testar.
## Testes
 
- Compilação com `-Wall -Wextra` sem avisos no código do grupo.
- AddressSanitizer (`make asan`) sem erros nem vazamentos.
- Jogo testado do menu até o ranking, nas 6 fases.
## Limitações
 
- Funciona apenas em terminais Unix (Linux, macOS e WSL).
- Exige terminal de 80 por 24 com UTF-8.
- Nomes do ranking não podem ter espaço (apenas letras, números e hífen).
## Possíveis melhorias
 
- Dividir o `jogo.c` em módulos (`.c` e `.h`), um por seção.
- Mais obstáculos por fase e modo para dois jogadores.
- Guardar mais informações no ranking (data e personagem).
## Equipe:
| Nome Completo | E-mail Institucional |
|---|---|
| Manuela Brayner Medeiros de Araújo  | mbma3@cesar.school |
| Maria Júlia Oliveira Dionísio  | mjod@cesar.school |
| Milena Siqueira Araújo  | msa4@cesar.school |
| Rafaela Dubeux Godoy  | rdg@cesar.school |
