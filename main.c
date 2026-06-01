#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "raylib.h"

#define LARGURA   800
#define ALTURA    600
#define CHAO_Y    490
#define VEL_PULO -15.5f
#define GRAV      0.65f
#define SCENE_LEN 1800
#define DIST_FIM  5600
#define MAX_RANK  5

//tipos de objeto no mundo
#define CONE      1
#define ARVORE    2
#define CARRO     3
#define CAFE      4
#define TRABALHO  5
#define MINIPROVA 6

enum Tela { MENU, JOGO, GAMEOVER, RANKING, ULTIMACENA };

struct Estudante {
    float x, y;  
    // y = posicao dos pes
    float vy;
    int velocidade;     
    // 1 lento, 2 normal, 3 rapido
    int vida;
    int pontuacao;
    int noChao;
    int frameAnim;
    float timerAnim;
    float timerInvencivel;
};

struct Item {
    float x, y;
    int tipo;
    int ativo;
    struct Item *proximo;
};

//matriz de nivel: linha = cena (0,1,2), coluna = slot de spawn
//0=nada 1=cone 2=arvore 3=carro 4=cafe 5=trabalho 6=miniprova
int nivelLayout[3][15] = {
    {4, 0, 1, 5, 0, 4, 2, 1, 5, 1, 4, 0, 5, 1, 4},
    {0, 3, 4, 1, 5, 3, 4, 2, 5, 3, 1, 4, 5, 3, 2},
    {2, 5, 1, 4, 3, 5, 2, 4, 1, 5, 6, 4, 2, 1, 4},
};

struct Estudante jogador;
struct Item *listaObjetos = NULL;
float scrollTotal;
int   cenaAtual;
int   slotAtual;

//prototipos
enum Tela TelaMenu();
enum Tela TelaJogo();
enum Tela TelaGameOver();
enum Tela TelaRanking();
enum Tela TelaUltimaCena();
void iniciarJogo();
void desenharCenario(int cena, float scroll);
void desenharPersonagem(struct Estudante *e);
void desenharObjetos(struct Item *obj);
void adicionarObjeto(struct Item **lista, float x, float y, int tipo);
void atualizarObjetos(struct Item **lista, float vel);
void liberarObjetos(struct Item **lista);
void checaColisoes(struct Item **lista, struct Estudante *e);
void salvarScore(int pontos);
void carregarScores(int scores[], int *qtd);
void mostrarRankingRecursivo(int scores[], int total, int atual, int posY);


int main() {
    InitWindow(LARGURA, ALTURA, "PIF: Atraso Fatal");
    SetTargetFPS(60);
    SetExitKey(0); //desativa o esc fechar a janela automaticamente

    enum Tela telaAtual = MENU;
    while (!WindowShouldClose()) {
        switch (telaAtual) {
            case MENU:       telaAtual = TelaMenu();       break;
            case JOGO:       telaAtual = TelaJogo();       break;
            case GAMEOVER:   telaAtual = TelaGameOver();   break;
            case RANKING:    telaAtual = TelaRanking();    break;
            case ULTIMACENA: telaAtual = TelaUltimaCena(); break;
        }
    }
    CloseWindow();
    return 0;
}



//telas

enum Tela TelaMenu() {
    Rectangle btnJogar   = {(LARGURA - 220)/2.0f, 305, 220, 52};
    Rectangle btnRanking = {(LARGURA - 220)/2.0f, 375, 220, 52};
    Vector2 mouse = GetMousePosition();
    int hJ = CheckCollisionPointRec(mouse, btnJogar);
    int hR = CheckCollisionPointRec(mouse, btnRanking);

    BeginDrawing();
        ClearBackground((Color){250, 249, 247, 255});

        //faixa laranja de cima
        DrawRectangle(0, 0, LARGURA, 10, (Color){232, 96, 10, 255});
        DrawRectangle(0, 10, LARGURA, 4, (Color){255, 140, 60, 255});

        //titulo centralizado
        const char *t1 = "PIF: Atraso Fatal";
        DrawText(t1, (LARGURA - MeasureText(t1, 54))/2, 110, 54, (Color){232, 96, 10, 255});

        const char *t2 = "chegue a aula antes que seja tarde!";
        DrawText(t2, (LARGURA - MeasureText(t2, 19))/2, 182, 19, (Color){130, 125, 120, 255});

        DrawRectangle((LARGURA-360)/2, 220, 360, 2, (Color){232, 96, 10, 60});

        //botao JOGAR
        Color bgJ = hJ ? (Color){200, 78, 5, 255} : (Color){232, 96, 10, 255};
        DrawRectangleRounded(btnJogar, 0.28f, 8, bgJ);
        const char *tJ = "JOGAR";
        DrawText(tJ, btnJogar.x + (btnJogar.width - MeasureText(tJ,26))/2, btnJogar.y+13, 26, WHITE);

        //botao RANKING
        Color bgR = hR ? (Color){232,96,10,255} : WHITE;
        Color txR = hR ? WHITE : (Color){232,96,10,255};
        DrawRectangleRounded(btnRanking, 0.28f, 8, bgR);
        DrawRectangleRoundedLines(btnRanking, 0.28f, 8, (Color){232,96,10,255});
        const char *tR = "RANKING";
        DrawText(tR, btnRanking.x + (btnRanking.width - MeasureText(tR,26))/2, btnRanking.y+13, 26, txR);

        //legenda dos itens na parte de baixo
        int legendaY = 455;
        int blocoW   = 520;
        int blocoX   = (LARGURA - blocoW) / 2;

        DrawCircle(blocoX + 12, legendaY + 8, 10, (Color){100, 65, 25, 255});
        DrawText("cafe: +vida +vel", blocoX + 28, legendaY, 15, (Color){100,100,100,255});

        DrawRectangleRounded((Rectangle){blocoX + 180, legendaY - 2, 18, 18}, 0.3f, 4, (Color){200,40,40,255});
        DrawText("trabalho surpresa: -vida -vel", blocoX + 205, legendaY, 15, (Color){100,100,100,255});

        DrawRectangleRounded((Rectangle){blocoX + 2, legendaY + 28, 18, 18}, 0.3f, 4, (Color){220,175,0,255});
        DrawText("mini-prova: -vida -pts", blocoX + 26, legendaY + 30, 15, (Color){100,100,100,255});

        const char *hint = "W ou ESPACO pra pular    A e D pra mover";
        DrawText(hint, (LARGURA - MeasureText(hint,15))/2, legendaY + 60, 15, (Color){170,165,160,255});

        //faixa laranja de baixo
        DrawRectangle(0, ALTURA-10, LARGURA, 10, (Color){232,96,10,255});
        DrawRectangle(0, ALTURA-14, LARGURA, 4, (Color){255,140,60,255});
    EndDrawing();

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (hJ) { iniciarJogo(); return JOGO; }
        if (hR) return RANKING;
    }
    return MENU;
}


enum Tela TelaJogo() {
    float dt = GetFrameTime();

    float velMundo = 4.0f;
    if (jogador.velocidade == 1) velMundo = 2.5f;
    if (jogador.velocidade == 3) velMundo = 6.0f;

    scrollTotal += velMundo;
    jogador.pontuacao += (int)(velMundo * 0.4f);

    cenaAtual = (int)(scrollTotal / SCENE_LEN);
    if (cenaAtual > 2) cenaAtual = 2;

    //spawn de obstaculos usando matriz (de nivel)
    int novoSlot = (int)(scrollTotal / 120);
    if (novoSlot > slotAtual) {
        slotAtual = novoSlot;
        int cenaSlot = novoSlot / 15;
        int indSlot  = novoSlot % 15;
        if (cenaSlot < 3) {
            int tipo = nivelLayout[cenaSlot][indSlot];
            if (tipo != 0) {
                adicionarObjeto(&listaObjetos, LARGURA + 60, (float)CHAO_Y, tipo);
            }
        }
    }

    //pulo
    if ((IsKeyPressed(KEY_W) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP)) && jogador.noChao) {
        jogador.vy = VEL_PULO;
        jogador.noChao = 0;
    }

    //movimento horizontal leve (A/D)
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  jogador.x -= 2;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) jogador.x += 2;
    if (jogador.x < 60)  jogador.x = 60;
    if (jogador.x > 320) jogador.x = 320;

    //fisica
    jogador.vy += GRAV;
    jogador.y  += jogador.vy;
    if (jogador.y >= CHAO_Y) {
        jogador.y    = CHAO_Y;
        jogador.vy   = 0;
        jogador.noChao = 1;
    }

    //animacao
    jogador.timerAnim += dt;
    if (jogador.timerAnim >= 0.12f) {
        jogador.timerAnim = 0;
        jogador.frameAnim = (jogador.frameAnim + 1) % 2;
    }

    //timer invencivel
    if (jogador.timerInvencivel > 0) jogador.timerInvencivel -= dt;

    atualizarObjetos(&listaObjetos, velMundo);
    checaColisoes(&listaObjetos, &jogador);

    if (scrollTotal >= DIST_FIM) {
        jogador.pontuacao += jogador.vida * 200;
        salvarScore(jogador.pontuacao);
        liberarObjetos(&listaObjetos);
        return ULTIMACENA;
    }

    if (jogador.vida <= 0) {
        salvarScore(jogador.pontuacao);
        liberarObjetos(&listaObjetos);
        return GAMEOVER;
    }

    BeginDrawing();
        desenharCenario(cenaAtual, scrollTotal);
        desenharObjetos(listaObjetos);

        //personagem fica piscando quando fica invencivel
        if (jogador.timerInvencivel <= 0 || (int)(jogador.timerInvencivel * 10) % 2 == 0) {
            desenharPersonagem(&jogador);
        }

        //HUD 
        DrawRectangle(0, 0, LARGURA, 46, (Color){232, 96, 10, 255});
        DrawRectangle(0, 46, LARGURA, 3, (Color){255, 140, 60, 255});

        //coracoes
        for (int i = 0; i < jogador.vida; i++) {
            DrawText("<3", 12 + i*30, 13, 22, WHITE);
        }

        //pontuacao no HUD
        const char *pts = TextFormat("pts: %d", jogador.pontuacao);
        DrawText(pts, (LARGURA - MeasureText(pts, 20))/2, 14, 20, WHITE);

        //barra de progresso
        float prog = scrollTotal / DIST_FIM;
        if (prog > 1) prog = 1;
        DrawRectangle(590, 16, 190, 14, (Color){255,255,255,50});
        DrawRectangle(590, 16, (int)(190 * prog), 14, WHITE);
        DrawText(TextFormat("%d%%", (int)(prog*100)), 738, 13, 18, WHITE);

        if (jogador.velocidade == 3) DrawText("RAPIDO!", (LARGURA - MeasureText("RAPIDO!", 18))/2, 55, 18, (Color){80,200,80,255});
        if (jogador.velocidade == 1) DrawText("LENTO...", (LARGURA - MeasureText("LENTO...", 18))/2, 55, 18, (Color){220,180,50,255});
    EndDrawing();

    return JOGO;
}


enum Tela TelaGameOver() {
    Rectangle btnNovo = {(LARGURA - 220)/2.0f, 390, 220, 52};
    Vector2 mouse = GetMousePosition();
    int hover = CheckCollisionPointRec(mouse, btnNovo);

    BeginDrawing();
        ClearBackground((Color){250, 249, 247, 255});

        DrawRectangle(0, 0, LARGURA, 10, (Color){200, 40, 40, 255});
        DrawRectangle(0, 10, LARGURA, 4, (Color){230, 80, 80, 255});

        const char *sub = "que pena...";
        DrawText(sub, (LARGURA - MeasureText(sub, 26))/2, 105, 26, (Color){160,155,150,255});

        const char *titulo = "ATRASO FATAL";
        DrawText(titulo, (LARGURA - MeasureText(titulo, 58))/2, 148, 58, (Color){200, 40, 40, 255});

        DrawRectangle((LARGURA-340)/2, 224, 340, 2, (Color){200,40,40,60});

        char pts[60];
        sprintf(pts, "pontuacao final: %d", jogador.pontuacao);
        DrawText(pts, (LARGURA - MeasureText(pts, 26))/2, 244, 26, (Color){70,65,60,255});

        char cenas[60];
        sprintf(cenas, "voce chegou ao trecho %d de 3", cenaAtual + 1);
        DrawText(cenas, (LARGURA - MeasureText(cenas, 18))/2, 282, 18, (Color){140,135,130,255});

        //botao
        Color bgBtn = hover ? (Color){200,78,5,255} : (Color){232,96,10,255};
        DrawRectangleRounded(btnNovo, 0.28f, 8, bgBtn);
        const char *tBtn = "jogar de novo";
        DrawText(tBtn, btnNovo.x + (btnNovo.width - MeasureText(tBtn,22))/2, btnNovo.y+15, 22, WHITE);

        const char *esc = "ESC para o menu";
        DrawText(esc, (LARGURA - MeasureText(esc, 18))/2, 460, 18, (Color){160,155,150,255});

        DrawRectangle(0, ALTURA-10, LARGURA, 10, (Color){200,40,40,255});
        DrawRectangle(0, ALTURA-14, LARGURA, 4, (Color){230,80,80,255});
    EndDrawing();

    if (IsKeyPressed(KEY_ENTER) || (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && hover)) {
        iniciarJogo();
        return JOGO;
    }
    if (IsKeyPressed(KEY_ESCAPE)) return MENU;
    return GAMEOVER;
}


enum Tela TelaRanking() {
    int scores[MAX_RANK];
    int qtd = 0;
    carregarScores(scores, &qtd);

    BeginDrawing();
        ClearBackground((Color){250, 249, 247, 255});

        DrawRectangle(0, 0, LARGURA, 10, (Color){232,96,10,255});
        DrawRectangle(0, 10, LARGURA, 4, (Color){255,140,60,255});

        const char *titulo = "RANKING";
        DrawText(titulo, (LARGURA - MeasureText(titulo, 52))/2, 55, 52, (Color){232,96,10,255});
        DrawRectangle((LARGURA-360)/2, 125, 360, 2, (Color){232,96,10,60});

        if (qtd == 0) {
            const char *vazio = "nenhuma pontuacao ainda!";
            DrawText(vazio, (LARGURA - MeasureText(vazio, 22))/2, 285, 22, (Color){160,155,150,255});
        } else {
            mostrarRankingRecursivo(scores, qtd, 0, 155);
        }

        const char *esc = "ESC para voltar";
        DrawText(esc, (LARGURA - MeasureText(esc, 18))/2, 546, 18, (Color){160,155,150,255});

        DrawRectangle(0, ALTURA-10, LARGURA, 10, (Color){232,96,10,255});
        DrawRectangle(0, ALTURA-14, LARGURA, 4, (Color){255,140,60,255});
    EndDrawing();

    if (IsKeyPressed(KEY_ESCAPE)) return MENU;
    return RANKING;
}


enum Tela TelaUltimaCena() {
    BeginDrawing();
        ClearBackground((Color){250, 249, 247, 255});

        DrawRectangle(0, 0, LARGURA, 10, (Color){232,96,10,255});
        DrawRectangle(0, 10, LARGURA, 4, (Color){255,140,60,255});

        //ceu
        DrawRectangle(0, 14, LARGURA, 320, (Color){200, 228, 248, 255});

        //chao/calcada
        DrawRectangle(0, 334, LARGURA, 50, (Color){210,208,205,255});
        DrawRectangle(0, 384, LARGURA, 100, (Color){190,188,185,255});

        //predio APOLO
        DrawRectangle(200, 60, 400, 280, WHITE);
        DrawRectangleLines(200, 60, 400, 280, (Color){210,210,210,255});

        //janelas
        for (int c = 0; c < 4; c++) {
            for (int r = 0; r < 4; r++) {
                DrawRectangle(220 + c*88, 80 + r*52, 60, 36, (Color){173,216,230,200});
                DrawRectangleLines(220 + c*88, 80 + r*52, 60, 36, (Color){150,200,225,255});
            }
        }

        //placa da CESAR
        DrawRectangle(310, 65, 180, 28, (Color){232,96,10,255});
        const char *placa = "CESAR School";
        DrawText(placa, 310 + (180 - MeasureText(placa,16))/2, 72, 16, WHITE);

        //porta vermelha
        DrawRectangleRounded((Rectangle){362, 296, 76, 44}, 0.1f, 4, (Color){180,38,38,255});
        DrawCircle(430, 318, 5, (Color){200,160,50,255});

        // personagem chegando
        int px = 165, py = 334;
        DrawEllipse(px, py+5, 12, 4, (Color){0,0,0,40});
        DrawLine(px-10, py, px+8, py-4, (Color){50,50,90,255});
        DrawLine(px-4, py-22, px-10, py, (Color){50,50,90,255});
        DrawLineEx((Vector2){px-4, py-22}, (Vector2){px-4, py-40}, 5, (Color){232,96,10,255});
        DrawLine(px-4, py-38, px-16, py-30, (Color){240,200,150,255});
        DrawLine(px-4, py-38, px+8, py-46, (Color){240,200,150,255});
        DrawCircle(px-4, py-50, 10, (Color){240,200,150,255});
        DrawRectangleRounded((Rectangle){px+2, py-42, 9, 13}, 0.3f, 4, (Color){200,75,5,255});
        DrawRectangleRounded((Rectangle){px-14, py-56, 20, 8}, 0.5f, 4, (Color){50,35,15,255});

        //sala de aula no fundo (janela aberta mostrando dentro)
        DrawRectangle(220, 182, 360, 108, (Color){245,248,252,255});
        DrawCircle(330, 236, 28, (Color){232,96,10,50});
        DrawCircle(330, 236, 22, (Color){232,96,10,90});
        DrawCircle(470, 236, 28, (Color){232,96,10,50});
        DrawCircle(470, 236, 22, (Color){232,96,10,90});
        DrawRectangleRounded((Rectangle){315, 254, 28, 16}, 0.5f, 4, (Color){232,96,10,200});
        DrawRectangleRounded((Rectangle){455, 254, 28, 16}, 0.5f, 4, (Color){232,96,10,200});

        //textos centralizados
        const char *chegou = "CHEGOU A TEMPO!";
        DrawText(chegou, (LARGURA - MeasureText(chegou, 46))/2, 408, 46, (Color){50,180,80,255});

        DrawRectangle((LARGURA-340)/2, 466, 340, 2, (Color){50,180,80,60});

        char pts[60];
        sprintf(pts, "pontuacao final: %d", jogador.pontuacao);
        DrawText(pts, (LARGURA - MeasureText(pts, 24))/2, 478, 24, (Color){70,65,60,255});

        const char *enter = "ENTER para o ranking";
        DrawText(enter, (LARGURA - MeasureText(enter, 18))/2, 518, 18, (Color){160,155,150,255});

        DrawRectangle(0, ALTURA-10, LARGURA, 10, (Color){232,96,10,255});
        DrawRectangle(0, ALTURA-14, LARGURA, 4, (Color){255,140,60,255});
    EndDrawing();

    if (IsKeyPressed(KEY_ENTER))  return RANKING;
    if (IsKeyPressed(KEY_ESCAPE)) return MENU;
    return ULTIMACENA;
}


//jogo

void iniciarJogo() {
    jogador.x = 150;
    jogador.y = CHAO_Y;
    jogador.vy = 0;
    jogador.velocidade = 2;
    jogador.vida = 3;
    jogador.pontuacao = 0;
    jogador.noChao = 1;
    jogador.frameAnim = 0;
    jogador.timerAnim = 0;
    jogador.timerInvencivel = 0;
    scrollTotal = 0;
    cenaAtual   = 0;
    slotAtual   = -1;
    liberarObjetos(&listaObjetos);
}


void desenharCenario(int cena, float scroll) {
    //deslocamento lento para elementos de fundo: paralaxe?
    int bg = (int)(scroll * 0.2f);

    if (cena == 0) {
        //cena 0: saindo do BRUM
        DrawRectangle(0, 0, LARGURA, CHAO_Y, (Color){195, 220, 242, 255}); //ceu claro

        //BRUM saindo pela esquerda (deslocamento medio)
        int bx = 380 - (int)(scroll * 0.7f);
        if (bx > -320) {
            DrawRectangle(bx, 120, 300, 370, (Color){232, 96, 10, 255});
            DrawRectangle(bx, 120, 300, 28, (Color){200, 75, 5, 255});
            for (int c = 0; c < 3; c++)
                for (int r = 0; r < 4; r++)
                    DrawRectangle(bx+22+c*88, 165+r*52, 56, 34, (Color){30,30,30,255});
            DrawRectangleRounded((Rectangle){bx+105, 390, 90, 100}, 0.05f, 4, (Color){25,25,25,255});
        }

        //postes (deslocamento rapido)
        for (int i = 0; i < 3; i++) {
            int px = ((700 - bg*2 + i*350) % (LARGURA+200)) - 50;
            DrawRectangle(px, 360, 7, 130, (Color){90,88,85,255});
            DrawCircle(px+3, 357, 13, (Color){255,240,170,200});
        }

        //calcada
        DrawRectangle(0, 432, LARGURA, 58, (Color){185,183,180,255});
        DrawRectangle(0, 432, LARGURA, 4, (Color){160,158,155,255});
        //cantinhos da calcada
        for (int j = 0; j < 10; j++) {
            int jx = ((j*90 - bg*4) % (LARGURA+90) + LARGURA+90) % (LARGURA+90) - 90;
            DrawRectangle(jx, 432, 2, 58, (Color){165,163,160,255});
        }

        //chao
        DrawRectangle(0, CHAO_Y, LARGURA, ALTURA - CHAO_Y, (Color){160,158,155,255});

    } else if (cena == 1) {
        //cena 1:avenida
        DrawRectangle(0, 0, LARGURA, CHAO_Y, (Color){140, 198, 238, 255}); //ceu azul

        //arvores no fundo (deslocamento lento)
        for (int i = 0; i < 5; i++) {
            int tx = ((i*180 - bg/2) % (LARGURA+160) + LARGURA+160) % (LARGURA+160) - 80;
            DrawRectangle(tx+14, 330, 14, 100, (Color){90,58,22,255});
            DrawCircle(tx+21, 315, 30, (Color){55,140,55,255});
            DrawCircle(tx+21, 305, 22, (Color){70,165,60,255});
        }

        //TIRADENTES (deslocamento medio)
        int pbx = 680 - (int)((scroll - SCENE_LEN) * 0.25f);
        if (pbx > -220 && pbx < LARGURA + 50) {
            DrawRectangle(pbx, 280, 220, 152, (Color){232,96,10,255});
            DrawRectangle(pbx, 280, 220, 22, (Color){200,75,5,255});
            for (int c = 0; c < 2; c++)
                for (int r = 0; r < 2; r++)
                    DrawRectangle(pbx+18+c*90, 315+r*50, 58, 32, (Color){30,30,30,255});
            DrawRectangleRounded((Rectangle){pbx+80, 390, 60, 42}, 0.05f, 4, (Color){25,25,25,255});
        }

        //calcada lateral
        DrawRectangle(0, 415, LARGURA, 20, (Color){195,193,190,255});

        //pista da avenida
        DrawRectangle(0, 435, LARGURA, 55, (Color){72,72,76,255});

        //faixas centrais da pista (animadas)
        for (int m = 0; m < 10; m++) {
            int mx = ((m*130 - (int)(scroll*0.8f)) % (LARGURA+130) + LARGURA+130) % (LARGURA+130) - 130;
            DrawRectangle(mx, 458, 70, 8, WHITE);
        }

        //carro decorativo no fundo da pista (deslocamento medio)
        int cx = 750 - (int)((scroll - SCENE_LEN) * 0.55f);
        if (cx > -110 && cx < LARGURA + 20) {
            DrawRectangleRounded((Rectangle){cx, 437, 95, 32}, 0.2f, 4, (Color){65,95,175,255});
            DrawRectangleRounded((Rectangle){cx+12, 427, 65, 18}, 0.35f, 4, (Color){85,115,195,255});
            DrawCircle(cx+18, 471, 9, DARKGRAY);
            DrawCircle(cx+75, 471, 9, DARKGRAY);
        }

        //calcada d baixo
        DrawRectangle(0, CHAO_Y, LARGURA, ALTURA - CHAO_Y, (Color){195,193,190,255});

    } else {
        //cena 2:chegando no APOLO
        DrawRectangle(0, 0, LARGURA, CHAO_Y, (Color){175, 215, 245, 255}); // ceu mais claro

        //APOLO cehgando
        float sceneProg = (scroll - SCENE_LEN * 2) / SCENE_LEN;
        if (sceneProg < 0) sceneProg = 0;
        if (sceneProg > 1) sceneProg = 1;

        int bW = (int)(160 + sceneProg * 380);
        int bH = (int)(220 + sceneProg * 220);
        int bX = LARGURA - bW - 20;
        int bY = CHAO_Y - bH;

        DrawRectangle(bX, bY, bW, bH, WHITE);
        DrawRectangleLines(bX, bY, bW, bH, (Color){210,210,210,255});

        //janelas
        int jCols = 4, jRows = 4;
        int jW = bW / jCols - 10, jH = 28;
        for (int c = 0; c < jCols; c++)
            for (int r = 0; r < jRows; r++)
                DrawRectangle(bX+8+c*(bW/jCols), bY+15+r*42, jW, jH, (Color){173,216,230,220});

        //placa CESAR
        if (sceneProg > 0.25f) {
            int plW = (int)(80 + sceneProg * 100);
            DrawRectangle(bX + (bW-plW)/2, bY+5, plW, (int)(18+sceneProg*8), (Color){232,96,10,255});
            if (plW > 110) {
                const char *placa = "CESAR";
                DrawText(placa, bX+(bW-MeasureText(placa,14))/2, bY+8, 14, WHITE);
            }
        }

        //porta vermelha
        int dorW = (int)(28 + sceneProg * 42);
        int dorH = (int)(40 + sceneProg * 50);
        DrawRectangleRounded((Rectangle){bX+(bW-dorW)/2.0f, (float)(CHAO_Y-dorH), (float)dorW, (float)(dorH+4)},
                             0.1f, 4, (Color){180,38,38,255});
        DrawCircle(bX + (bW-dorW)/2 + dorW - 7, CHAO_Y - dorH/2, 4, (Color){200,160,50,255});

        //arvores laterais
        for (int i = 0; i < 3; i++) {
            int tx = ((i*200 - (int)((scroll - SCENE_LEN*2)*0.35f)) % (LARGURA+160) + LARGURA+160) % (LARGURA+160) - 80;
            if (tx > bX - 60) continue;
            DrawRectangle(tx+13, 340, 14, 90, (Color){90,58,22,255});
            DrawCircle(tx+20, 326, 28, (Color){55,140,55,255});
        }

        //calcada clara
        DrawRectangle(0, 432, LARGURA, 58, (Color){210,208,206,255});
        DrawRectangle(0, CHAO_Y, LARGURA, ALTURA - CHAO_Y, (Color){195,193,190,255});
    }
}


void desenharPersonagem(struct Estudante *e) {
    int x  = (int)e->x;
    int y  = (int)e->y;    
    //y = pes
    int f  = e->frameAnim;
    int nc = e->noChao;

    Color corPele   = {240, 200, 150, 255};
    Color corRoupa  = {232,  96,  10, 255}; 
    //laranja CESAR
    Color corCalca  = { 45,  45,  80, 255};
    Color corTenis  = {240, 240, 240, 255};
    Color corCabelo = { 50,  35,  15, 255};

    int hip  = y - 24;
    int sho  = y - 44;
    int head = y - 55;

    //sombra no chao
    if (nc) DrawEllipse(x, y+5, 14, 5, (Color){0,0,0,45});

    if (nc) {
        //correndo:pernas alternadas
        int lf = f == 0 ? x-13 : x+5;   //perna da frente
        int lb = f == 0 ? x+5  : x-10;  //perna de tras
        DrawLine(x, hip, lf, y, corCalca);
        DrawLine(x, hip, lb, y-4, corCalca);
        DrawCircle(lf, y, 5, corTenis);
        DrawCircle(lb, y-4, 5, corTenis);
    } else {
        //pulando:pernas levantadas
        DrawLine(x, hip, x-8, y-10, corCalca);
        DrawLine(x, hip, x+8, y-10, corCalca);
        DrawCircle(x-8, y-10, 5, corTenis);
        DrawCircle(x+8, y-10, 5, corTenis);
    }

    //corpo (blusa)
    DrawLineEx((Vector2){x, hip}, (Vector2){x, sho}, 7, corRoupa);

    //mochila
    DrawRectangleRounded((Rectangle){x+4, sho, 10, 14}, 0.35f, 4, (Color){200,75,5,255});

    //bracos alternados
    if (nc) {
        int ay1 = f == 0 ? sho-8 : sho+6;
        int ay2 = f == 0 ? sho+6 : sho-8;
        DrawLine(x, sho, x-13, ay1, corPele);
        DrawLine(x, sho, x+10, ay2, corPele);
    } else {
        DrawLine(x, sho, x-14, sho-12, corPele);
        DrawLine(x, sho, x+10, sho-12, corPele);
    }

    //cabeca
    DrawCircle(x, head, 11, corPele);

    //cabelo
    DrawRectangleRounded((Rectangle){x-11, head-12, 22, 9}, 0.5f, 4, corCabelo);

    //olhos
    DrawCircle(x-4, head-1, 2, (Color){40,40,40,255});
    DrawCircle(x+4, head-1, 2, (Color){40,40,40,255});
}


//funcao desenha objeto atual e chama p proximo
void desenharObjetos(struct Item *obj) {
    if (obj == NULL) return; 

    if (obj->ativo) {
        int x = (int)obj->x;
        int y = (int)obj->y; //y = base do objeto

        switch (obj->tipo) {
            case CONE:
                DrawTriangle(
                    (Vector2){x, y-32},
                    (Vector2){x-14, y+2},
                    (Vector2){x+14, y+2},
                    (Color){255, 115, 0, 255}
                );
                DrawRectangle(x-18, y, 36, 7, (Color){255,115,0,255});
                DrawRectangle(x-12, y-16, 24, 6, WHITE); //faixa branca
                break;

            case ARVORE:
                DrawRectangle(x-7, y-58, 14, 62, (Color){95,60,20,255});
                DrawCircle(x, y-68, 32, (Color){45,135,45,255});
                DrawCircle(x-8, y-72, 22, (Color){65,160,55,255});
                DrawCircle(x+8, y-65, 20, (Color){55,150,50,255});
                break;

            case CARRO:
                DrawRectangleRounded((Rectangle){x-50, y-30, 100, 28}, 0.2f, 4, (Color){65,95,175,255});
                DrawRectangleRounded((Rectangle){x-34, y-48, 68, 22}, 0.35f, 4, (Color){85,115,195,255});
                DrawRectangle(x+28, y-34, 24, 10, (Color){255,235,100,160});
                DrawCircle(x-28, y+2, 10, DARKGRAY); DrawCircle(x-28, y+2, 5, GRAY);
                DrawCircle(x+28, y+2, 10, DARKGRAY); DrawCircle(x+28, y+2, 5, GRAY);
                break;

            case CAFE:
                //xicara de cafe marrom
                DrawRectangleRounded((Rectangle){x-13, y-24, 26, 24}, 0.25f, 4, (Color){95,60,22,255});
                DrawRectangle(x-13, y-24, 26, 9, (Color){70,42,12,255}); //cafe
                DrawRectangleRounded((Rectangle){x-18, y-2, 36, 6}, 0.5f, 4, (Color){115,78,35,255}); //pires
                DrawLine(x+13, y-18, x+20, y-10, (Color){95,60,22,255}); //alca
                const char *tC = "C";
                DrawText(tC, x - MeasureText(tC,14)/2, y-22, 14, (Color){210,170,90,255});
                break;

            case TRABALHO:
                DrawRectangleRounded((Rectangle){x-18, y-30, 36, 30}, 0.08f, 4, (Color){200,38,38,255});
                DrawRectangle(x-14, y-26, 28, 2, (Color){230,75,75,255});
                DrawRectangle(x-14, y-20, 28, 2, (Color){230,75,75,255});
                DrawRectangle(x-14, y-14, 20, 2, (Color){230,75,75,255});
                const char *tT = "!";
                DrawText(tT, x - MeasureText(tT,22)/2, y-30, 22, WHITE);
                break;

            case MINIPROVA:
                DrawRectangleRounded((Rectangle){x-17, y-28, 34, 28}, 0.08f, 4, (Color){215,172,0,255});
                DrawRectangle(x-13, y-24, 26, 2, (Color){185,145,0,255});
                DrawRectangle(x-13, y-18, 20, 2, (Color){185,145,0,255});
                const char *tM = "?";
                DrawText(tM, x - MeasureText(tM,22)/2, y-30, 22, (Color){90,72,0,255});
                break;
        }
    }

    desenharObjetos(obj->proximo); //recursao
}


//lista encadeada

void adicionarObjeto(struct Item **lista, float x, float y, int tipo) {
    struct Item *novo = (struct Item*) malloc(sizeof(struct Item));
    novo->x = x;
    novo->y = y;
    novo->tipo = tipo;
    novo->ativo = 1;
    novo->proximo = *lista;
    *lista = novo;
}

void atualizarObjetos(struct Item **lista, float vel) {
    struct Item *atual = *lista;
    struct Item *anterior = NULL;

    while (atual != NULL) {
        atual->x -= vel;

        //remove se saiu da tela ou foi coletado
        if (atual->x < -120 || !atual->ativo) {
            struct Item *remover = atual;
            if (anterior == NULL) *lista = atual->proximo;
            else anterior->proximo = atual->proximo;
            atual = atual->proximo;
            free(remover);
        } else {
            anterior = atual;
            atual = atual->proximo;
        }
    }
}

void liberarObjetos(struct Item **lista) {
    struct Item *atual = *lista;
    while (atual != NULL) {
        struct Item *prox = atual->proximo;
        free(atual);
        atual = prox;
    }
    *lista = NULL;
}

void checaColisoes(struct Item **lista, struct Estudante *e) {
    if (e->timerInvencivel > 0) return; // invencivel, ignora

    struct Item *atual = *lista;
    Rectangle hb = {e->x - 12, e->y - 62, 24, 62};

    while (atual != NULL) {
        if (!atual->ativo) { atual = atual->proximo; continue; }

        Rectangle ob = {0};
        switch (atual->tipo) {
            case CONE:      ob = (Rectangle){atual->x-12, atual->y-32, 24, 34}; break;
            case ARVORE:    ob = (Rectangle){atual->x- 6, atual->y-58, 12, 60}; break;
            case CARRO:     ob = (Rectangle){atual->x-48, atual->y-48, 96, 50}; break;
            case CAFE:      ob = (Rectangle){atual->x-18, atual->y-26, 36, 28}; break;
            case TRABALHO:  ob = (Rectangle){atual->x-18, atual->y-30, 36, 32}; break;
            case MINIPROVA: ob = (Rectangle){atual->x-17, atual->y-28, 34, 30}; break;
        }

        if (CheckCollisionRecs(hb, ob)) {
            if (atual->tipo == CAFE) {
                e->velocidade = 3;
                if (e->vida < 5) e->vida++;
                e->pontuacao += 100;
            } else if (atual->tipo == TRABALHO) {
                e->velocidade = 1;
                e->vida--;
                e->timerInvencivel = 1.5f;
            } else if (atual->tipo == MINIPROVA) {
                e->vida--;
                e->pontuacao -= 60;
                if (e->pontuacao < 0) e->pontuacao = 0;
                e->timerInvencivel = 1.5f;
            } else {
                //cone, arvore, carro: obstaculo fisico
                e->vida--;
                e->velocidade = 1;
                e->timerInvencivel = 1.8f;
            }
            atual->ativo = 0;
            return;
        }
        atual = atual->proximo;
    }
}


//arquivo ranking

void carregarScores(int scores[], int *qtd) {
    *qtd = 0;
    FILE *f = fopen("ranking.txt", "r");
    if (f == NULL) return;
    fscanf(f, "%d", qtd);
    if (*qtd > MAX_RANK) *qtd = MAX_RANK;
    for (int i = 0; i < *qtd; i++) fscanf(f, "%d", &scores[i]);
    fclose(f);
}

void salvarScore(int pontos) {
    int scores[MAX_RANK];
    int qtd = 0;
    carregarScores(scores, &qtd);

    if (qtd < MAX_RANK) {
        scores[qtd++] = pontos;
    } else {
        int menor = 0;
        for (int i = 1; i < qtd; i++)
            if (scores[i] < scores[menor]) menor = i;
        if (pontos > scores[menor]) scores[menor] = pontos;
    }

    //ordena do maior pro menor 
    for (int i = 0; i < qtd-1; i++)
        for (int j = 0; j < qtd-1-i; j++)
            if (scores[j] < scores[j+1]) {
                int tmp = scores[j]; scores[j] = scores[j+1]; scores[j+1] = tmp;
            }

    FILE *f = fopen("ranking.txt", "w");
    if (f == NULL) return;
    fprintf(f, "%d\n", qtd);
    for (int i = 0; i < qtd; i++) fprintf(f, "%d\n", scores[i]);
    fclose(f);
}

//exibe cada linha do ranking
void mostrarRankingRecursivo(int scores[], int total, int atual, int posY) {
    if (atual >= total) return; 

    Color cor = atual == 0 ? (Color){232,96,10,255} : (Color){80,75,70,255};
    int   tam = atual == 0 ? 30 : 24;
    char  linha[50];
    sprintf(linha, "%d.   %d pontos", atual+1, scores[atual]);
    DrawText(linha, (LARGURA - MeasureText(linha, tam))/2, posY, tam, cor);

    mostrarRankingRecursivo(scores, total, atual+1, posY+48);
}
