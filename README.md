# PIF: Atraso Fatal

![C](https://img.shields.io/badge/C-language-blue) ![raylib](https://img.shields.io/badge/library-raylib-orange) ![platform](https://img.shields.io/badge/platform-macOS%20%7C%20Linux-lightgrey)

o jogo se passa em uma corrida até a CESAR School, onde o jogador precisa chegar a tempo de apresentar o trabalho. o cenário vai avançando automaticamente e durante o percurso diversos obstáculos aparecem, como cones, árvores e carros, exigindo atenção e reflexo pra pular na hora certa.

o jogador também pode encontrar itens especiais: o café concede vantagens temporárias como aumento de velocidade e recuperação de vida, enquanto o trabalho surpresa e a mini-prova atrapalham o avanço reduzindo vida e pontuação.

<details>
<summary>como compilar</summary>

**mac**

```bash
brew install raylib
make
make run
```

**windows (WSL)**

```bash
sudo apt update && sudo apt install libraylib-dev
gcc main.c -o jogo -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
./jogo
```

</details>

<details>
<summary>controles</summary>

- W ou espaco: pular
- A / D: mover

</details>

<details>
<summary>sobre a implementacao</summary>

feito em C com raylib. usa lista encadeada pra gerenciar os objetos na tela, matriz pra definir o layout dos obstaculos em cada trecho, e salva o ranking em arquivo txt.

</details>

---

projeto da disciplina PIF, CESAR School 2026
