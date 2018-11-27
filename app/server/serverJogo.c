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
#define TRAP 0x4A               //J
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
    double tempoInicio, tempoAtual;
    int fim = 0,comecou = 1;
    int time_1 = 0,time_2 = 0;              // Quantidade de indivíduos nas equipes
    int armadilhas_1,armadilhas_2;          // Quantidade de armadilha por pessoa
    char mapa [X_MAX][Y_MAX] = {'0'};
    int xAnterior,yAnterior;
    int scoreBlue = 0, scoreRed = 0;
    int i, j;

    PROTOCOLO_JOGO jogada, jogada_server, tempo;           // Protocolo de envio a ser enviado para o cliente com as infos do jogo;
    PROTOCOLO_INICIAL msg_inicial;                  

    struct msg_ret_t input;

    //CRIANDO MAPA

    //
    DADOS_LOBBY msg, msg_server;
    
    serverInit(MAX_CLIENTS);
    puts("Server is running!!");
    while (jogadores < MAX_CLIENTS || (jogadores < 4 && !ready)) {
        
        int id = acceptConnection();
        if (id != NO_CONNECTION) {
            // Recebe o nick e o id das novas conexões
            recvMsgFromClient((DADOS_LOBBY *) &msg, id, WAIT_FOR_IT);
            if(msg.tipo == NICK){
                strcpy(players[jogadores].name, msg.mensagem); // Salvou o nick
                strcpy(players[jogadores].id, id);              // Salvou o id
            }
            strcpy(msg_server.mensagem, msg.mensagem);
            strcat(msg_server.mensagem, " connected");
            msg_server.tipo = CHAT;
            broadcast((DADOS_LOBBY *) &msg_server, (int)sizeof(DADOS_LOBBY));
            printf("%s connected id = %d\n", players[jogadores].name, id);

            // Escolha do capacete
            recvMsgFromClient((DADOS_LOBBY *)&msg, id, WAIT_FOR_IT);
            if(msg.tipo == CAPACETE){
                players[jogadores].helmet = msg.msg;
                sprintf(msg_server.mensagem, "Seu capacete: %d", msg.msg);
                msg_server.tipo = CHAT; // O modo chat seria o modo onde o server só manda uma msg pro client?
                sendMsgToClient((DADOS_LOBBY *) &msg_server, sizeof(DADOS_LOBBY), id);
            }
            
            // Inicializando jogadores
            if(jogadores%2 == 0){
                players[jogadores].team = 1;
                players[jogadores].position.x = 0;
                players[jogadores].position.y = 0;
                players[jogadores].congelado = 0;
                time_1++;
            }
            else{
                players[jogadores].team = 2;
                players[jogadores].position.x = X_MAX;
                players[jogadores].position.y = Y_MAX;
                players[jogadores].congelado = 0;
                time_2++;
            }
            
            msg_inicial.tipo = GAME;
            msg_inicial.jogador = players[jogadores];
            sendMsgToClient((PROTOCOLO_INICIAL *) &msg_inicial, sizeof(PROTOCOLO_INICIAL), id);
            
            jogadores++;
        }

        // Chat
        struct msg_ret_t msg_ret = recvMsg((char *)msg.mensagem);
        if (msg_ret.status == MESSAGE_OK) {
            if((strcasecmp(msg.mensagem, "start 1") == 0) && (msg_ret.client_id == 0)){  // Só o jogador inicial pode começar o jogo e ele começa qnd ele digitar "start 1"
               ready = 1; 
               if(jogadores < 4){
                   msg_server.tipo = CHAT;
                   strcpy(msg_server.mensagem,"Aguardando o numero min de jogadores para comecar!");
                   broadcast((DADOS_LOBBY *) &msg_server, (int)sizeof(DADOS_LOBBY));
               }           
            }
            else{
                sprintf(msg_server.mensagem, "%s(%d): %s", players[msg_ret.client_id].name,
                    msg_ret.client_id, msg.mensagem);
                 msg_server.tipo = CHAT;
                broadcast((DADOS_LOBBY *) &msg_server, (int)sizeof(DADOS_LOBBY));
            }          
        } else if (msg_ret.status == DISCONNECT_MSG) {
            sprintf(msg_server.mensagem, "%s disconnected", players[msg_ret.client_id].name);
            printf("%s disconnected, id = %d is free\n",
                    players[msg_ret.client_id].name, msg_ret.client_id);
            msg_server.tipo = CHAT;
            broadcast((DADOS_LOBBY *) &msg_server, (int)sizeof(DADOS_LOBBY));
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
        input = recvMsg((PROTOCOLO_JOGO *) &jogada);
        for(i = 0; i < players; i++){ 
            if((players[i].id == input.client_id) && (jogada.tipo == GAME)){
                if(!players[i].congelado){
                    if(jogada.teclado == CIMA){
                        jogada_server.tipo = ANDAR; // Modo andar = 4
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
                                
                                jogada_server.xAnterior = players[i].position.x;
                                jogada_server.yAnterior = players[i].position.y;
                                jogada_server.itemAnterior = mapa[players[i].position.x][players[i].position.y];

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
                    else if(jogada.teclado == BAIXO){
                        if(players[i].position.y != Y_MAX){
                            jogada_server.tipo = ANDAR; // Modo andar = 4
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
                                
                                jogada_server.xAnterior = players[i].position.x;
                                jogada_server.yAnterior = players[i].position.y;
                                jogada_server.itemAnterior = mapa[players[i].position.x][players[i].position.y];

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
                    else if(jogada.teclado == DIREITA){
                        if(players[i].position.x != X_MAX){
                            if(mapa[players[i].position.x+1][players[i].position.y] == VAZIO){
                                jogada_server.tipo = ANDAR; // Modo andar = 4
                                if(mapa[players[i].position.x][players[i].position.y] == PLAYER_COM_TRAP){
                                    if(players[i].team == 1)
                                        mapa[players[i].position.x][players[i].position.y] == TRAP_TEAM_BLUE;
                                    else
                                        mapa[players[i].position.x][players[i].position.y] == TRAP_TEAM_RED;
                                }
                                else{
                                    mapa[players[i].position.x][players[i].position.y] == VAZIO;
                                }
                                
                                jogada_server.xAnterior = players[i].position.x;
                                jogada_server.yAnterior = players[i].position.y;
                                jogada_server.itemAnterior = mapa[players[i].position.x][players[i].position.y];

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
                    else if(jogada.teclado == ESQUERDA){
                        if(players[i].position.x != 0){
                            if(mapa[players[i].position.x-1][players[i].position.y] == VAZIO){
                                jogada_server.tipo = ANDAR; // Modo andar = 4
                                if(mapa[players[i].position.x][players[i].position.y] == PLAYER_COM_TRAP){
                                    if(players[i].team == 1)
                                        mapa[players[i].position.x][players[i].position.y] == TRAP_TEAM_BLUE;
                                    else
                                        mapa[players[i].position.x][players[i].position.y] == TRAP_TEAM_RED;
                                }
                                else{
                                    mapa[players[i].position.x][players[i].position.y] == VAZIO;
                                }
                                
                                jogada_server.xAnterior = players[i].position.x;
                                jogada_server.yAnterior = players[i].position.y;
                                jogada_server.itemAnterior = mapa[players[i].position.x][players[i].position.y];

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
                    else if(jogada.teclado == TRAP){ 
                        if(players[i].armadilhas > 0){                                                          // Verifica se tem armadilhas para botar
                            jogada_server.tipo = BOTARTRAPS;                                                    // Modo botar armadilha = 5
                            if(players[i].team == 1){                                                           // Se for do time azul
                                if(mapa[players[i].position.x][players[i].position.y] != TRAP_TEAM_BLUE){
                                    mapa[players[i].position.x][players[i].position.y] = PLAYER_COM_TRAP;
                                    players[i].armadilhas--;
                                }
                                else{
                                    char msg[] = "armadilha ja existente";
                                    sendMsgToClient((char *) msg, sizeof(msg) + 1, players[i].id);
                                }
                            }    
                            else{                                                                               // Se for do time vermelho
                                if(mapa[players[i].position.x][players[i].position.y] != TRAP_TEAM_RED){
                                    mapa[players[i].position.x][players[i].position.y] = PLAYER_COM_TRAP;
                                    players[i].armadilhas--;
                                }
                                else{
                                    char msg[] = "armadilha ja existente";
                                    sendMsgToClient((char *) msg, sizeof(msg) + 1, players[i].id);
                                }
                            }
                            jogada_server.itemAnterior = mapa[players[i].position.x][players[i].position.y];
                            jogada_server.xAnterior = players[i].position.x;
                            jogada_server.yAnterior = players[i].position.y;
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
                            DADOS msg_final;
                            strcpy(msg.mensagem, "Time Vermelho Ganhou!");
                            msg_final.tipo = ENDGAME;
                            broadcast((DADOS *) &msg_final, sizeof(DADOS));
                            fim = 1;
                       // } 
                    }
                    // Se ele chegou na base azul com a bandeira vermelha e ele é do time azul = ele ganhou uma partida!
                    else if(((players[i].position.x == X_ENTRADA_1_BLUE && players[i].position.y == Y_ENTRADA_1_BLUE) || (players[i].position.x == X_ENTRADA_2_BLUE && players[i].position.y == Y_ENTRADA_2_BLUE) || (players[i].position.x == X_ENTRADA_3_BLUE && players[i].position.y == Y_ENTRADA_3_BLUE)) && (players[i].team == 1) && (players[i].comBandeira == 1)){
                        //scoreBlue++;
                        //if(scoreBlue >= 2){
                            DADOS msg_final;
                            strcpy(msg.mensagem, "Time Azul Ganhou!");
                            msg_final.tipo = ENDGAME;
                            broadcast((DADOS *) &msg, sizeof(DADOS));
                            fim = 1;
                        //}
                    }
                    
                    if(fim != 1){
                        jogada_server.jogadorAtual = players[i];
                        jogada_server.tipo = GAME;
                        broadcast((PROTOCOLO_JOGO *) &jogada_server, sizeof(PROTOCOLO_JOGO));
                    }
                }
            }
        }
        tempoAtual = al_get_time() - tempoInicio;
        tempo.tipo=TEMPO;
        sprintf(tempo.msg, "%lf", &tempoAtual);
        broadcast((PROTOCOLO_JOGO *) &tempo, sizeof(PROTOCOLO_JOGO)); 
    }

    return 0;
}