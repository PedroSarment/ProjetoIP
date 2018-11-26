#ifndef GAME_H
#define GAME_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define FPS 60
#define WIDTH  640
#define HEIGHT 480
#define IP_MAX_SIZE 100
#define LOGIN_MAX_SIZE 13

#define CLIENT_TO_SERVER 0
#define CLIENT_TO_CLIENT 1

#define NICK 0
#define CAPACETE 1
#define CHAT 2
#define COMECOU 3
#define ANDAR 4
#define BOTARTRAPS 5
#define GAME 6
#define ENDGAME 7


double startingTime;

ALLEGRO_DISPLAY *main_window;
ALLEGRO_EVENT_QUEUE *eventsQueue;

//========================
//FONT AND BITMAP POINTERS
ALLEGRO_FONT *ubuntu;
ALLEGRO_FONT *start;

ALLEGRO_BITMAP *objects;
ALLEGRO_BITMAP *menuScreen;
//========================
//========================

//EXAMPLE STRUCT
typedef struct DADOS
{
    int tipo;
    char mensagem[100];
    int valor, tecla;
}DADOS;

typedef struct{
    int tipo, msg;
    char mensagem[100];
    int funcao;
}DADOS_LOBBY;


typedef struct{
    int x, y;
}Position;

typedef struct{
    char name[LOGIN_MAX_SIZE];
    int helmet, team, armadilhas;
    int id;
    int comBandeira;
    int congelado;
    Position position;
}Player;

typedef struct{
    int tipo; 
    int xAnterior, yAnterior;
    char itemAnterior;
    char teclado;
    Player jogadorAtual;
}PROTOCOLO_JOGO;


//MAIN ALLEGRO FUNCTIONS
bool coreInit();
bool windowInit(int W, int H, char title[]);
bool inputInit();
void allegroEnd();

//FPS CONTROL FUNCTIONS
void startTimer();
double getTimer();
void FPSLimit();

//RESOURCE LOADING FUNCTIONS
bool loadGraphics();
bool fontInit();

//INPUT READING FUNCTION
void readInput(ALLEGRO_EVENT event, char str[], int limit);


#endif