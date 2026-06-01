# PIF: Atraso Fatal

![](https://img.shields.io/badge/C-language-004482?style=for-the-badge&logo=c&logoColor=white) ![](https://img.shields.io/badge/raylib-library-E8600A?style=for-the-badge) ![](https://img.shields.io/badge/macOS%20%7C%20Linux-platform-555?style=for-the-badge)

o jogo se passa em uma corrida até a CESAR School, onde o jogador precisa chegar a tempo de apresentar o trabalho. o cenário vai avançando automaticamente e durante o percurso diversos obstáculos aparecem, como cones, árvores e carros, exigindo atenção e reflexo pra pular na hora certa.

o jogador também pode encontrar itens especiais: o café concede vantagens temporárias como aumento de velocidade e recuperação de vida, enquanto o trabalho surpresa e a mini-prova atrapalham o avanço reduzindo vida e pontuação.

## demo

acesse o vídeo demo aqui: [https://youtu.be/4RVp_btZU2U](https://youtu.be/4RVp_btZU2U)

> o arquivo de vídeo não foi incluído no repositório por causa do tamanho.

## como rodar

**mac**

1. instala o raylib: `brew install raylib`
2. compila: `make`
3. roda: `make run`

**windows (WSL)**

1. instala o raylib: `sudo apt install libraylib-dev`
2. compila: `gcc main.c -o jogo -lraylib -lGL -lm -lpthread -ldl -lrt -lX11`
3. roda: `./jogo`

## controles

- W ou espaço: pular
- A / D: mover

## itens

- cafe (marrom): +vida, fica mais rapido
- trabalho surpresa (vermelho): -vida, fica mais lento
- mini-prova (amarelo): -vida, perde pontos

desvie dos cones, arvores e carros pulando

<details>
<summary>sobre a implementacao</summary>

feito em C com raylib. usa lista encadeada pra gerenciar os objetos na tela, matriz pra definir o layout dos obstaculos em cada trecho, e salva o ranking em arquivo txt.

</details>

<p align="center">projeto da disciplina PIF, CESAR School 2026</p>
