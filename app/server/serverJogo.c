#include "server.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "ACore.h"


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
#define DIREITA 0x44            //D
#define ESQUERDA 0x41           //A
#define CIMA 0x57               //W
#define BAIXO 0x53              //S
#define VAZIO 0x30              //0
#define BANDEIRA 0x42           //B 
#define PLAYER 0x50             //P
#define TRAP_TEAM_BLUE 0x41     //A
#define TRAP_TEAM_RED 0x56      //V
#define PLAYER_COM_TRAP 0x51    //Q
#define BOTAR_TRAP 0x4A         //J
#define X_FLAG_BLUE 1280
#define Y_FLAG_BLUE 0
#define X_FLAG_RED 0
#define Y_FLAG_RED 720
#define X_ENTRADA_1_BLUE 1275
#define Y_ENTRADA_1_BLUE 8
#define X_ENTRADA_2_BLUE 1273
#define Y_ENTRADA_2_BLUE 5
#define X_ENTRADA_3_BLUE 1270
#define Y_ENTRADA_3_BLUE 2
#define X_ENTRADA_1_RED 0
#define Y_ENTRADA_1_RED 719
#define X_ENTRADA_2_RED 5
#define Y_ENTRADA_2_RED 715
#define X_ENTRADA_3_RED 8
#define Y_ENTRADA_3_RED 710


int main() {
    char client_names[MAX_CLIENTS][LOGIN_MAX_SIZE];
    char str_buffer[BUFFER_SIZE], aux_buffer[BUFFER_SIZE];

    Player players[10];

    char helmetChoice[] = ""; 
    int jogadores = 0, capacete, ready = 0;
    double tempoInicio;
    int fim = 0,comecou = 1;
    int teclado;
    int time_1 = 0,time_2 = 0; //quantidade de indivíduos nas equipes
    int armadilhas_1,armadilhas_2; //quantidade de armadilha por pessoa
    char mapa [X_MAX][Y_MAX];
    int xAnterior,yAnterior;
    int scoreBlue = 0, scoreRed = 0;
    int i, j;

    PROTOCOLO_JOGO jogada;  // Protocolo de envio a ser enviado para o cliente com as infos do jogo;

    struct msg_ret_t input;

    //CRIANDO MAPA

    //

    serverInit(MAX_CLIENTS);
    puts("Server is running!!");
    while (jogadores < MAX_CLIENTS || (jogadores < 4 && ready != jogadores)) {
        int id = acceptConnection();
        if (id != NO_CONNECTION) {
            recvMsgFromClient(client_names[id], id, WAIT_FOR_IT);
            strcpy(str_buffer, client_names[id]);
            strcat(str_buffer, " connected");
            broadcast(str_buffer, (int)strlen(str_buffer) + 1);
            printf("%s connected id = %d\n", client_names[id], id);

            // Escolha do capacete
            
            recvMsgFromClient(&capacete, id, WAIT_FOR_IT);
            //char msg[20];
            DADOS_LOBBY protocolo;
            sprintf(&protocolo, "Seu capacete: %d", capacete);
            sendMsgToClient(&protocolo, sizeof(protocolo) +1, id);

            //inicializando jogadores
            if(jogadores%2 == 0){
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
    for(i = 0; i < jogadores; i++){
        if(players[i].team == 1)
            players[i].armadilhas = armadilhas_1;
        else
            players[i].armadilhas = armadilhas_2;
    }


    tempoInicio = al_get_time();
    while( (al_get_time() - tempoInicio < TEMPO_LIMITE) && !fim){
        input = recvMsg(&teclado);
        for(i = 0; i < players; i++){ 
            if(players[i].id == input.client_id){
                if(!players[i].congelado){

                    if(teclado == CIMA){
                        jogada.modo = 0; // Modo andar = 0
                        if(players[i].position.y != 0){ // espaco livre
                            if(mapa[players[i].position.x][players[i].position.y-1] == VAZIO){ //considerando 0 um espaco livre
                                if(mapa[players[i].position.x][players[i].position.y] == PLAYER_COM_TRAP){
                                    if(players[i].team == 1)
                                        mapa[players[i].position.x][players[i].position.y] == TRAP_TEAM_BLUE;
                                    else
                                        mapa[players[i].position.x][players[i].position.y] == TRAP_TEAM_RED;
                                }
                                else{
                                    mapa[players[i].position.x][players[i].position.y] == VAZIO;
                                }
                                
                                jogada.xAnterior = players[i].position.x;
                                jogada.yAnterior = players[i].position.y;
                                jogada.itemAnterior = mapa[players[i].position.x][players[i].position.y];

                                players[i].position.y--;

                                if(players[i].team == 1){
                                    if(mapa[players[i].position.x][players[i].position.y] == TRAP_TEAM_RED)
                                        players[i].congelado = 1;
                                    mapa[players[i].position.x][players[i].position.y] = PLAYER; //jogador time1
                                }
                                else{
                                    if(mapa[players[i].position.x][players[i].position.y] == TRAP_TEAM_BLUE)
                                        players[i].congelado = 1;
                                    mapa[players[i].position.x][players[i].position.y] = PLAYER; //jogador time 2
                                }   
                            }
                        } 
                    }
                    else if(teclado == BAIXO){
                        if(players[i].position.y != Y_MAX){
                            jogada.modo = 0; // Modo andar = 0
                            if(mapa[players[i].position.x][players[i].position.y+1] == VAZIO){
                                if(mapa[players[i].position.x][players[i].position.y] == PLAYER_COM_TRAP){
                                    if(players[i].team == 1)
                                        mapa[players[i].position.x][players[i].position.y] == TRAP_TEAM_BLUE;
                                    else
                                        mapa[players[i].position.x][players[i].position.y] == TRAP_TEAM_RED;
                                }
                                else{
                                    mapa[players[i].position.x][players[i].position.y] == VAZIO;
                                }
                                
                                jogada.xAnterior = players[i].position.x;
                                jogada.yAnterior = players[i].position.y;
                                jogada.itemAnterior = mapa[players[i].position.x][players[i].position.y];

                                players[i].position.y++;
                                
                                if(players[i].team == 1){
                                    if(mapa[players[i].position.x][players[i].position.y] == TRAP_TEAM_RED)
                                        players[i].congelado = 1;
                                    mapa[players[i].position.x][players[i].position.y] = PLAYER; //jogador time1
                                }
                                else{
                                    if(mapa[players[i].position.x][players[i].position.y] == TRAP_TEAM_BLUE)
                                        players[i].congelado = 1;
                                    mapa[players[i].position.x][players[i].position.y] = PLAYER; //jogador time 2
                                } 
                            }  
                        }
                    }
                    else if(teclado == DIREITA){
                        if(players[i].position.x != X_MAX){
                            if(mapa[players[i].position.x+1][players[i].position.y] == VAZIO){
                                jogada.modo = 0; // Modo andar = 0
                                if(mapa[players[i].position.x][players[i].position.y] == PLAYER_COM_TRAP){
                                    if(players[i].team == 1)
                                        mapa[players[i].position.x][players[i].position.y] == TRAP_TEAM_BLUE;
                                    else
                                        mapa[players[i].position.x][players[i].position.y] == TRAP_TEAM_RED;
                                }
                                else{
                                    mapa[players[i].position.x][players[i].position.y] == VAZIO;
                                }
                                
                                jogada.xAnterior = players[i].position.x;
                                jogada.yAnterior = players[i].position.y;
                                jogada.itemAnterior = mapa[players[i].position.x][players[i].position.y];

                                players[i].position.x++;
                                
                                if(players[i].team == 1){
                                    if(mapa[players[i].position.x][players[i].position.y] == TRAP_TEAM_RED)
                                        players[i].congelado = 1;
                                    mapa[players[i].position.x][players[i].position.y] = PLAYER; //jogador time1
                                }
                                else{
                                    if(mapa[players[i].position.x][players[i].position.y] == TRAP_TEAM_BLUE)
                                        players[i].congelado = 1;
                                    mapa[players[i].position.x][players[i].position.y] = PLAYER; //jogador time 2
                                } 
                            }
                        }
                    }
                    else if(teclado == ESQUERDA){
                        if(players[i].position.x != 0){
                            if(mapa[players[i].position.x-1][players[i].position.y] == VAZIO){
                                jogada.modo = 0; // Modo andar = 0
                                if(mapa[players[i].position.x][players[i].position.y] == PLAYER_COM_TRAP){
                                    if(players[i].team == 1)
                                        mapa[players[i].position.x][players[i].position.y] == TRAP_TEAM_BLUE;
                                    else
                                        mapa[players[i].position.x][players[i].position.y] == TRAP_TEAM_RED;
                                }
                                else{
                                    mapa[players[i].position.x][players[i].position.y] == VAZIO;
                                }
                                
                                jogada.xAnterior = players[i].position.x;
                                jogada.yAnterior = players[i].position.y;
                                jogada.itemAnterior = mapa[players[i].position.x][players[i].position.y];

                                players[i].position.x--;
                                
                                if(players[i].team == 1){
                                    if(mapa[players[i].position.x][players[i].position.y] == TRAP_TEAM_RED)
                                        players[i].congelado =1;
                                    mapa[players[i].position.x][players[i].position.y] = PLAYER; //jogador time1
                                }
                                else{
                                    if(mapa[players[i].position.x][players[i].position.y] == TRAP_TEAM_BLUE)
                                        players[i].congelado = 1;
                                    mapa[players[i].position.x][players[i].position.y] = PLAYER; //jogador time 2
                                } 
                            }
                        }
                    }
                    else if(teclado == BOTAR_TRAP){ 
                        if(players[i].armadilhas > 0){
                            jogada.modo = 1; // Modo botar armadilha = 1
                            if(players[i].team == 1){
                                if(mapa[players[i].position.x][players[i].position.y] != TRAP_TEAM_BLUE){
                                    mapa[players[i].position.x][players[i].position.y] = PLAYER_COM_TRAP;
                                    players[i].armadilhas--;
                                }
                                else{
                                    char msg[20] = "armadilha ja existente";
                                    sendMsgToClient(msg, sizeof(msg) + 1, players[i].id);
                                }
                            }    
                            else{
                                if(mapa[players[i].position.x][players[i].position.y] != TRAP_TEAM_RED){
                                    mapa[players[i].position.x][players[i].position.y] = PLAYER_COM_TRAP;
                                    players[i].armadilhas--;
                                }
                                else{
                                    char msg[20] = "armadilha ja existente";
                                    sendMsgToClient(msg, sizeof(msg) + 1, players[i].id);
                                }
                            }
                            jogada.itemAnterior = mapa[players[i].position.x][players[i].position.y];
                            jogada.xAnterior = players[i].position.x;
                            jogada.yAnterior = players[i].position.y;
                        }
                    }
                    // Se ele chegou na bandeira vermelha e ele é do time azul ou Se ele chegou na bandeira azul e ele é do time vermelho = ele tá quase ganhando!
                    if((players[i].position.x == X_FLAG_RED && players[i].position.y == Y_FLAG_RED && players[i].team == 1) || (players[i].position.x == X_FLAG_BLUE && players[i].position.y == Y_FLAG_BLUE && players[i].team == 2)){
                        if(mapa[players[i].position.x][players[i].position.y] == BANDEIRA){
                            players[i].comBandeira = 1;
                            mapa[players[i].position.x][players[i].position.y] = PLAYER;
                        }
                            
                    }
                    // Se ele chegou na base vermelha, ele é do time vermelho e está com a bandeira azul = ele ganhou uma partida!
                    else if(((players[i].position.x == X_ENTRADA_1_RED && players[i].position.y == Y_ENTRADA_1_RED) || (players[i].position.x == X_ENTRADA_2_RED && players[i].position.y == Y_ENTRADA_2_RED) || (players[i].position.x == X_ENTRADA_3_RED && players[i].position.y == Y_ENTRADA_3_RED)) && (players[i].team == 2) && (players[i].comBandeira == 1)){
                        //scoreRed++;
                        //if(scoreRed >= 2){
                            char msg[] = "Time Vermelho Ganhou!";
                            broadcast((char *)msg, sizeof(msg));
                            fim = 1;
                       // }
                    }
                    // Se ele chegou na base azul com a bandeira vermelha e ele é do time azul = ele ganhou uma partida!
                    else if(((players[i].position.x == X_ENTRADA_1_BLUE && players[i].position.y == Y_ENTRADA_1_BLUE) || (players[i].position.x == X_ENTRADA_2_BLUE && players[i].position.y == Y_ENTRADA_2_BLUE) || (players[i].position.x == X_ENTRADA_3_BLUE && players[i].position.y == Y_ENTRADA_3_BLUE)) && (players[i].team == 1) && (players[i].comBandeira == 1)){
                        //scoreBlue++;
                        //if(scoreBlue >= 2){
                            char msg[] = "Time Azul Ganhou!";
                            broadcast((char *)msg, sizeof(msg));
                            fim = 1;
                        //}
                    }
                    
                    jogada.jogadorAtual = players[i];
                    broadcast((PROTOCOLO_JOGO *)&jogada, sizeof(PROTOCOLO_JOGO));
                }
            }
        }
    }

    return 0;
}