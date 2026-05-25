#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "raylib.h"

enum Tela{ //lista de opcoes exclusivas com nomes , tipo um semáforo  
    MENU,
    JOGO,
    GAMEOVER,
    RANKING,
    ULTIMACENA
};
//funcoes protótipos
enum Tela TelaMenu(); //retorna a funçao 
enum Tela TelaJogo();
enum Tela TelaGameOver();
enum Tela TelaRanking();
enum Tela TelaUltimaCena();

int main (){
    InitWindow(800, 600, "PIF: Atraso Fatal"); //iniciando janela
    SetTargetFPS(60); //decidindo o fps
    enum Tela telaAtual = MENU;
    while (!WindowShouldClose()){ //loop raylib
        switch (telaAtual){ //pq switch? pq é uma variavel  e várias opcoes fixas. 
            //if/else seria melhor se as condicoes fossem variadas
            case MENU:
                telaAtual = TelaMenu();
                break;
            case JOGO:
                telaAtual = TelaJogo();
                break;
            case GAMEOVER:
                telaAtual = TelaGameOver();
                break;
            case RANKING:
                telaAtual = TelaRanking();
                break;
            case ULTIMACENA:
                telaAtual = TelaUltimaCena();
                break;
        }
    }
    CloseWindow(); //fecha a janela

    return 0;
}