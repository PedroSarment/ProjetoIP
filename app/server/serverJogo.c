#include "server.h"
#include <stdio.h>
#include <string.h>
#include <math.h>


// Definição de constantes (NOMES MAIÚSCULOS)
#define MSG_MAX_SIZE 350
#define BUFFER_SIZE (MSG_MAX_SIZE + 100)
#define LOGIN_MAX_SIZE 13
#define MAX_CLIENTS 10
#define QTD_TIMES 2
#define MAX_CLIENTS_TIMES 5
#define X_MAX 1280
#define Y_MAX 720
#define N_ARMADILHAS 20
#define TEMPO_LIMITE 120
#define DIREITA 0x44
#define ESQUERDA 0x41
#define CIMA 0x57
#define BAIXO 0x53
#define VAZIO 0x30
#define PLAYER_TEAM_1 0x31
#define PLAYER_TEAM_2 0x32
#define BOTAR_TRAP 0
#define TRAP_TEAM1 0
#define TRAP_TEAM2 0
#define X_FLAG_RED 0
#define Y_FLAG_RED 720
#define X_FLAG_BLUE 1280
#define Y_FLAG_BLUE 0

typedef struct{
    int x, y;
}Position;

typedef struct{
    char name[LOGIN_MAX_SIZE];
    int helmet, team, armadilhas;
    int id;
    int congelado;
    Position position;
}Player;

int main() {
    char client_names[MAX_CLIENTS][LOGIN_MAX_SIZE];
    char str_buffer[BUFFER_SIZE], aux_buffer[BUFFER_SIZE];

    Player players[10];

    char helmetChoice[] = ""; 
    int jogadores=0, capacete, ready=0;
    double tempoInicio;
    int fim=0,comecou=1;
    int teclado, i, j;
    int time_1=0,time_2=0; //quantidade de indivíduos nas equipes
    int armadilhas_1,armadilhas_2; //quantidade de armadilha por pessoa
    char mapa [X_MAX][Y_MAX];
    int xAnterior,yAnterior;

    struct msg_ret_t input;

    //CRIANDO MAPA

    //

    serverInit(MAX_CLIENTS);
    puts("Server is running!!");
    while (jogadores<MAX_CLIENTS || (jogadores<4 && ready!=jogadores)) {
        int id = acceptConnection();
        if (id != NO_CONNECTION) {
            recvMsgFromClient(client_names[id], id, WAIT_FOR_IT);
            strcpy(str_buffer, client_names[id]);
            strcat(str_buffer, " connected");
            broadcast(str_buffer, (int)strlen(str_buffer) + 1);
            printf("%s connected id = %d\n", client_names[id], id);

            //escolha do capacete
            
            recvMsgFromClient(&capacete, id, WAIT_FOR_IT);
            char msg[20];
            sprintf(msg, "Seu capacete: %d", capacete);
            sendMsgToClient(msg, sizeof(msg) +1, id);

            //inicializando jogadores
            if(jogadores%2==0){
            strcpy(players[jogadores].name, client_names[id]);
            players[jogadores].team = 1;
            players[jogadores].position.x = 0;
            players[jogadores].position.y = 0;
            players[jogadores].helmet = capacete;
            players[jogadores].id = id;
            players[jogadores].congelado = 0;
            time_1++;
            }
            else{
            strcpy(players[jogadores].name, client_names[id]);
            players[jogadores].team = 2;
            players[jogadores].position.x = X_MAX;
            players[jogadores].position.y = Y_MAX;
            players[jogadores].helmet = capacete;
            players[jogadores].id = id;
            time_2++;
            players[jogadores].congelado = 0;
            }
            jogadores++;
        }
        //chat
        struct msg_ret_t msg_ret = recvMsg(aux_buffer);
        if (msg_ret.status == MESSAGE_OK) {
            sprintf(str_buffer, "%s-%d: %s", client_names[msg_ret.client_id],
                    msg_ret.client_id, aux_buffer);
            broadcast(str_buffer, (int)strlen(str_buffer) + 1);
        } else if (msg_ret.status == DISCONNECT_MSG) {
            sprintf(str_buffer, "%s disconnected", client_names[msg_ret.client_id]);
            printf("%s disconnected, id = %d is free\n",
                    client_names[msg_ret.client_id], msg_ret.client_id);
            broadcast(str_buffer, (int)strlen(str_buffer) + 1);
        }
    }

    //jogo
    armadilhas_1 = ceil((float)N_ARMADILHAS/(float)time_1);
    armadilhas_2 = ceil((float)N_ARMADILHAS/(float)time_2);
    for(i=0;i<jogadores;i++){
        if(players[i].team == 1)
            players[i].armadilhas = armadilhas_1;
        else
            players[i].armadilhas = armadilhas_2;
    }


    tempoInicio = al_get_time();
    while( (al_get_time() - tempoInicio < TEMPO_LIMITE) && !fim){

        input = recvMsg(&teclado);
        for(i=0;i<players;i++){ 
            if(players[i].id == input.client_id){
                if( !players[i].congelado ){

                    if(teclado == CIMA){
                        if(players[i].position.y != Y_MAX){ // espaco livre
                            if(mapa[players[i].position.x][players[i].position.y+1] == VAZIO){ //considerando 0 um espaco livre
                                players[i].position.y++;
                                if(players[i].team==1){
                                    if(mapa[players[i].position.x][players[i].position.y] == TRAP_TEAM2)
                                        players[i].congelado=1;
                                    mapa[players[i].position.x][players[i].position.y] = PLAYER_TEAM_1; //jogador time1
                                }
                                else{
                                    if(mapa[players[i].position.x][players[i].position.y] == TRAP_TEAM1)
                                        players[i].congelado=1;
                                    mapa[players[i].position.x][players[i].position.y] = PLAYER_TEAM_2; //jogador time 2
                                }   

                                mapa[players[i].position.x+1][players[i].position.y] = VAZIO;
                                //posicao antiga do personagem
                                broadcast((int)players[i].position.x+1,sizeof(int));
                                broadcast((int)players[i].position.y,sizeof(int));
                                broadcast((char)mapa[players[i].position.x+1][players[i].position.y],sizeof(char));

                                //posicao atual do personagem
                                broadcast((int)players[i].position.x,sizeof(int));
                                broadcast((int)players[i].position.y,sizeof(int));
                                broadcast((char)mapa[players[i].position.x][players[i].position.y],sizeof(char));
                            }
                        } 
                    }
                    else if(teclado == BAIXO){
                        if(players[i].position.x != X_MAX){
                            if(mapa[players[i].position.x+1][players[i].position.y] == VAZIO){

                                players[i].position.x++;
                                if(players[i].team==1){
                                    if(mapa[players[i].position.x][players[i].position.y] == TRAP_TEAM2)
                                        players[i].congelado=1;
                                    mapa[players[i].position.x][players[i].position.y] = PLAYER_TEAM_1; //jogador time1
                                }
                                else{
                                    if(mapa[players[i].position.x][players[i].position.y] == TRAP_TEAM1)
                                        players[i].congelado=1;
                                    mapa[players[i].position.x][players[i].position.y] = PLAYER_TEAM_2; //jogador time 2
                                } 

                                mapa[players[i].position.x-1][players[i].position.y] = VAZIO;
                                //posicao antiga do personagem
                                broadcast((int)players[i].position.x-1,sizeof(int));
                                broadcast((int)players[i].position.y,sizeof(int));
                                broadcast((char)mapa[players[i].position.x-1][players[i].position.y],sizeof(char));

                                //posicao atual do personagem
                                broadcast((int)players[i].position.x,sizeof(int));
                                broadcast((int)players[i].position.y,sizeof(int));
                                broadcast((char)mapa[players[i].position.x][players[i].position.y],sizeof(char));
                            }
                        }
                    }
                    else if(teclado == DIREITA){
                        if(players[i].position.y != Y_MAX){
                            if(mapa[players[i].position.x][players[i].position.y+1] == VAZIO){
                                players[i].position.y++;
                                
                                if(players[i].team==1){
                                    if(mapa[players[i].position.x][players[i].position.y] == TRAP_TEAM2)
                                        players[i].congelado=1;
                                    mapa[players[i].position.x][players[i].position.y] = PLAYER_TEAM_1; //jogador time1
                                }
                                else{
                                    if(mapa[players[i].position.x][players[i].position.y] == TRAP_TEAM1)
                                        players[i].congelado=1;
                                    mapa[players[i].position.x][players[i].position.y] = PLAYER_TEAM_2; //jogador time 2
                                } 

                                mapa[players[i].position.x][players[i].position.y-1] = VAZIO;
                                //posicao antiga do personagem
                                broadcast((int)players[i].position.x,sizeof(int));
                                broadcast((int)players[i].position.y-1,sizeof(int));
                                broadcast((char)mapa[players[i].position.x][players[i].position.y-1],sizeof(char)); //deixando vazio o espaço antigo

                                //posicao atual do personagem
                                broadcast((int)players[i].position.x,sizeof(int));
                                broadcast((int)players[i].position.y,sizeof(int));
                                broadcast((char)mapa[players[i].position.x][players[i].position.y],sizeof(char));
                            }
                        }
                    }
                    else if(teclado == ESQUERDA){
                        if(players[i].position.y != 0){
                            if(mapa[players[i].position.x][players[i].position.y-1] == VAZIO){
                            
                                players[i].position.y--;
                                
                                if(players[i].team==1){
                                    if(mapa[players[i].position.x][players[i].position.y] == TRAP_TEAM2)
                                        players[i].congelado=1;
                                    mapa[players[i].position.x][players[i].position.y] = PLAYER_TEAM_1; //jogador time1
                                }
                                else{
                                    if(mapa[players[i].position.x][players[i].position.y] == TRAP_TEAM1)
                                        players[i].congelado=1;
                                    mapa[players[i].position.x][players[i].position.y] = PLAYER_TEAM_2; //jogador time 2
                                } 

                                mapa[players[i].position.x][players[i].position.y+1] = VAZIO;
                                //posicao antiga do personagem
                                broadcast((int)players[i].position.x,sizeof(int));
                                broadcast((int)players[i].position.y+1,sizeof(int));
                                broadcast((char)mapa[players[i].position.x][players[i].position.y+1],sizeof(char)); //deixando vazio o espaço antigo

                                //posicao atual do personagem
                                broadcast((int)players[i].position.x,sizeof(int));
                                broadcast((int)players[i].position.y,sizeof(int));
                                broadcast((char)mapa[players[i].position.x][players[i].position.y],sizeof(char));
                            }
                        }
                    }
                    if(teclado == BOTAR_TRAP){ 
                        if(players[i].armadilhas>0){

                            if(players[i].team == 1){
                                if(mapa[players[i].position.x][players[i].position.y] != TRAP_TEAM1){
                                    mapa[players[i].position.x][players[i].position.y] = TRAP_TEAM1;
                                    players[i].armadilhas--;
                                }
                                else{
                                    char msg[20] = "armadilha ja existente";
                                    sendMsgToClient(msg, sizeof(msg) +1, players[i].id);
                                }
                            }    
                            else{
                                if(mapa[players[i].position.x][players[i].position.y] != TRAP_TEAM2){
                                    mapa[players[i].position.x][players[i].position.y] = TRAP_TEAM2;
                                    players[i].armadilhas--;
                                }
                                else{
                                    char msg[20] = "armadilha ja existente";
                                    sendMsgToClient(msg, sizeof(msg) +1, players[i].id);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return 0;
}