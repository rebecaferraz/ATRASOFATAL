# PIF: Atraso Fatal

![C](https://img.shields.io/badge/C-language-blue) ![raylib](https://img.shields.io/badge/library-raylib-orange) ![platform](https://img.shields.io/badge/platform-macOS%20%7C%20Linux-lightgrey)

Runner 2D onde você é um estudante atrasado tentando chegar na aula de PIF da CESAR School. O cenário vai passando, você pula obstáculos e coleta itens pelo caminho. 
São 3 trechos até chegar no predio branco. (Brum -> tiradentes -> avenida -> Apolo)

<details>
<summary>Como compilar</summary>

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
<summary>Controles</summary>

- W ou espaço: pular
- A / D: mover

</details>

<details>
<summary>itens</summary>

- Café (marrom): +vida, fica mais rapido
- Trabalho surpresa (vermelho): -vida, fica mais lento
- Mini-prova (amarelo): -vida, perde pontos

Desvie dos cones, árvores e carros pulando 

</details>

<details>
<summary>Sobre a implementacao</summary>

Foi feito em C com raylib
Usa lista encadeada pra gerenciar os objetos na tela, matriz pra definir o layout dos obstáculos em cada trecho, e salva o ranking em arquivo txt.

</details>

Projeto da disciplina PIF | CESAR School 2026
