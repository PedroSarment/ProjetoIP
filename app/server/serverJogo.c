#include "server.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "ACore.h"


//DEFINIÇÕES DAS CONSTANTES
// #define MSG_MAX_SIZE 350
// #define BUFFER_SIZE (MSG_MAX_SIZE + 100)
#define LOGIN_MAX_SIZE 13
#define MAX_CLIENTS 6
#define QTD_TIMES 2
#define X_MAX 1280
#define Y_MAX 720
#define TEMPO_LIMITE 120
#define DIREITA 0x44            //D
#define ESQUERDA 0x41           //A
#define CIMA 0x57               //W
#define BAIXO 0x53              //S
#define VAZIO 0x30              //0
#define INVALIDO 0x49           //input.client_id
#define BANDEIRA_BLUE 0x42      //B 
#define BANDEIRA_RED 0x50       //P
#define CONGELAR 0x20           //SPACE
#define TRAP_TEAM_BLUE 0x54     //T
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

//INICIALIZAÇÃO DAS VARIÁVEIS GLOBAIS
char mapa [X_MAX][Y_MAX];
Player players[6];
int qntJogadores = 0;
PROTOCOLO_JOGO jogada_client, jogada_server, tempo;                 // Protocolo de envio a ser enviado para o cliente com as infos do jogo;


//INICIALIZAÇÃO DAS FUNÇÕES
void inicializaMapa();
void inicializaJogadores();
void runGame();
void runGameTest();


int main() {
    //CRIANDO MAPA 
    inicializaMapa();
        
    //INICIALIZAÇÃO DO SERVER
    serverInit(MAX_CLIENTS);
    puts("Server is running!!");
    
    //INICIALIZAÇÃO DOS JOGADORES
    inicializaJogadores();

    //JOGO
    runGame();
   
    return 0;
}

void inicializaMapa(){
    int i, j;
    for(i = 0; i < X_MAX; i++){
        for(j = 0; j < Y_MAX; j++){
            mapa[i][j] = VAZIO;
        }
    }
}

void inicializaJogadores(){
    int notReady = 1;
    int qntJogadoresProntos = 0;
    struct msg_ret_t input;

    while (notReady) {
        PROTOCOLO_INICIAL msg_client, msg_inicial_server;
        int id = acceptConnection();
        input = recvMsg((PROTOCOLO_INICIAL *) &msg_client);
        if (id != NO_CONNECTION) {
            // Recebe o nick, capacete e id das novas conexões
            if(msg_client.tipo == INICIAL ){
                if(qntJogadores < MAX_CLIENTS){
                    strcpy(players[id].name, msg_client.jogador.name);     // Salva o nick
                    players[id].helmet = msg_client.jogador.helmet;        // Salva o capacete
                    players[id].id = id;                                   // Salva o id         
                
                    printf("%s connected id = %d\n", players[id].name, id);
                
                    // Inicializando os demais atributos do player atual
                    if(qntJogadores%2 == 0){
                        players[id].team = 1;
                        players[id].position.x = 100;
                        players[id].position.y = 100;
                       }
                    else{
                        players[id].team = 2;
                        players[id].position.x = 200;
                        players[id].position.y = 200;
                    }                    
                    players[id].comBandeira = 0;
                    players[id].ready = 0;
                    players[id].estaCongelado = 0;
                    players[id].congelou = 0;
                    players[id].congelamentos = 4;
                    players[id].armadilhas = 3;

                    // Atualizando o player do client após as inicializações dos atributos
                    msg_inicial_server.tipo = INICIAL;
                    msg_inicial_server.jogador = players[id];                
                    sendMsgToClient((PROTOCOLO_INICIAL *) &msg_inicial_server, sizeof(PROTOCOLO_INICIAL), id);

                    qntJogadores++;                                       
                }
                else{
                    printf("Numero max de clientes conectados ja foi atingido!\n");
                    printf("%s disconnected id = %d\n", players[id].name, id);
                    disconnectClient(id);
                    msg_client.tipo = -1;
                } 
            }
        }
        else{
            if(msg_client.tipo == ENDGAME){
                printf("%s disconnected id = %d\n", players[msg_client.jogador.id].name, msg_client.jogador.id);
                disconnectClient(msg_client.jogador.id);
                msg_client.tipo = -1;
            }
            if(msg_client.tipo == COMECOU){
                qntJogadoresProntos++;
                players[msg_client.jogador.id] = msg_client.jogador;

                if(msg_client.jogador.id == 0){
                    // if(qntJogadoresProntos >= 4){
                        jogada_server.qntJogadores = qntJogadores;
                        jogada_server.tipo = COMECOU;
                        notReady = 0;
                    // }    
                }
                else{
                    jogada_server.tipo = WAITING;
                    jogada_server.qntJogadores = qntJogadoresProntos;
                }

                for(int i = 0; i < qntJogadores; i++){
                    jogada_server.todosJogadores[i] = players[i];
                    //printf("%d - %d, %d", i, jogada_server.todosJogadores[i].position.x,jogada_server.todosJogadores[i].position.y );
                }
                broadcast((PROTOCOLO_JOGO *) &jogada_server, sizeof(PROTOCOLO_JOGO));    
            }
        }
    }
}

void runGame(){
    double tempoInicio, tempoAtual;
    struct msg_ret_t input;
    int i, j, fim = 0; 

    tempoInicio = al_get_time();
    while( (al_get_time() - tempoInicio < TEMPO_LIMITE) && !fim){
        //printf("loop");
        input = recvMsg((PROTOCOLO_JOGO *) &jogada_client);
        if(input.status != NO_MESSAGE){
        // for(input.client_id = 0; input.client_id < qntJogadores; input.client_id++){ 
            //if((players[input.client_id].id == input.client_id)){ 
                // Atualiza o player do server com o player enviado pelo client
                //players[input.client_id] = jogada_client.todosJogadores[0];
                // Verifica se o jogador não está congelado
                if(!players[input.client_id].estaCongelado){
                    // Verifica se o jogador andou para cima
                    if(jogada_client.tipo == ANDAR_CIMA){
                        //printf("cima");
                        //jogada_server.tipo = ANDAR_CIMA; 
                        if(players[input.client_id].position.y - 4 >= 0){   
                            //printf("cima1");   
                            if(mapa[players[input.client_id].position.x][players[input.client_id].position.y-4] == VAZIO || mapa[players[input.client_id].position.x][players[input.client_id].position.y-4] == TRAP_TEAM_BLUE 
                            || mapa[players[input.client_id].position.x][players[input.client_id].position.y-4] == TRAP_TEAM_RED ||  mapa[players[input.client_id].position.x][players[input.client_id].position.y-4] == BANDEIRA_BLUE 
                            || mapa[players[input.client_id].position.x][players[input.client_id].position.y-4] == BANDEIRA_RED ){
                                 //considerando 0 um espaco livre
                                 //printf("cima2"); 
                                if(mapa[players[input.client_id].position.x][players[input.client_id].position.y] == (char)((players[input.client_id].id + 97) + 10)){ // se o mapa = indicador do id do player + indicador da trap
                                    //printf("cima3"); 
                                    if(players[input.client_id].team == 1)
                                        mapa[players[input.client_id].position.x][players[input.client_id].position.y] = TRAP_TEAM_BLUE;
                                    else
                                        mapa[players[input.client_id].position.x][players[input.client_id].position.y] = TRAP_TEAM_RED;
                                }
                                else{
                                    
                                    //printf("1 - %d %d \n", players[input.client_id].position.x, players[input.client_id].position.y);
                                    mapa[players[input.client_id].position.x][players[input.client_id].position.y] = VAZIO;
                                }
                                
                                jogada_server.xAnterior = players[input.client_id].position.x;
                                jogada_server.yAnterior = players[input.client_id].position.y;
                                jogada_server.itemAnterior = mapa[players[input.client_id].position.x][players[input.client_id].position.y];
                                
                                
                                players[input.client_id].position.y -= 4;
                               // printf("2 - %d %d \n", players[input.client_id].position.x, players[input.client_id].position.y);
                                mapa[players[input.client_id].position.x][players[input.client_id].position.y] = (char)(players[input.client_id].id + 97); // mapa = indicador do id do player

                                if(players[input.client_id].team == 1){
                                    if(mapa[players[input.client_id].position.x][players[input.client_id].position.y] == TRAP_TEAM_RED){
                                        players[input.client_id].estaCongelado = 1;
                                        mapa[players[input.client_id].position.x][players[input.client_id].position.y] = (char)((players[input.client_id].id + 97));
                                    }
                                    else if(mapa[players[input.client_id].position.x][players[input.client_id].position.y] == TRAP_TEAM_BLUE){
                                        mapa[players[input.client_id].position.x][players[input.client_id].position.y] = (char)((players[input.client_id].id + 97) + 10);
                                    }
                                }
                                else{
                                    if(mapa[players[input.client_id].position.x][players[input.client_id].position.y] == TRAP_TEAM_BLUE){
                                        players[input.client_id].estaCongelado = 1;
                                        mapa[players[input.client_id].position.x][players[input.client_id].position.y] = (char)((players[input.client_id].id + 97));
                                    }
                                    else if(mapa[players[input.client_id].position.x][players[input.client_id].position.y] == TRAP_TEAM_RED){
                                        mapa[players[input.client_id].position.x][players[input.client_id].position.y] = (char)((players[input.client_id].id + 97) + 10);
                                    }
                                }  
                                
                            }
                        } 
                    }
                    // Verifica se o jogador andou para baixo
                    else if(jogada_client.tipo == ANDAR_BAIXO){
                        //printf("baixo");
                        if(players[input.client_id].position.y + 4 <= Y_MAX){
                            jogada_server.tipo = ANDAR_BAIXO;
                            if(mapa[players[input.client_id].position.x][players[input.client_id].position.y+4] == VAZIO || mapa[players[input.client_id].position.x][players[input.client_id].position.y+4] == TRAP_TEAM_BLUE
                            || mapa[players[input.client_id].position.x][players[input.client_id].position.y+4] == TRAP_TEAM_RED || mapa[players[input.client_id].position.x][players[input.client_id].position.y+4] == BANDEIRA_BLUE
                            || mapa[players[input.client_id].position.x][players[input.client_id].position.y+4] == BANDEIRA_RED){
                                
                              if(mapa[players[input.client_id].position.x][players[input.client_id].position.y] == (char)((players[input.client_id].id + 97) + 10)){ // se o mapa = indicador do id do player + indicador da trap
                                    if(players[input.client_id].team == 1)
                                        mapa[players[input.client_id].position.x][players[input.client_id].position.y] = TRAP_TEAM_BLUE;
                                    else
                                        mapa[players[input.client_id].position.x][players[input.client_id].position.y] = TRAP_TEAM_RED;
                                }
                                else{
                                    mapa[players[input.client_id].position.x][players[input.client_id].position.y] = VAZIO;
                                }
                                
                                jogada_server.xAnterior = players[input.client_id].position.x;
                                jogada_server.yAnterior = players[input.client_id].position.y;
                                jogada_server.itemAnterior = mapa[players[input.client_id].position.x][players[input.client_id].position.y];

                                players[input.client_id].position.y += 4;
                                mapa[players[input.client_id].position.x][players[input.client_id].position.y] = (char)(players[input.client_id].id + 97); // mapa = indicador do id do player

                                if(players[input.client_id].team == 1){
                                    if(mapa[players[input.client_id].position.x][players[input.client_id].position.y] == TRAP_TEAM_RED){
                                        players[input.client_id].estaCongelado = 1;
                                        mapa[players[input.client_id].position.x][players[input.client_id].position.y] = (char)((players[input.client_id].id + 97));
                                    }
                                    else if(mapa[players[input.client_id].position.x][players[input.client_id].position.y] == TRAP_TEAM_BLUE){
                                        mapa[players[input.client_id].position.x][players[input.client_id].position.y] = (char)((players[input.client_id].id + 97) + 10);
                                    }
                                }
                                else{
                                    if(mapa[players[input.client_id].position.x][players[input.client_id].position.y] == TRAP_TEAM_BLUE){
                                        players[input.client_id].estaCongelado = 1;
                                        mapa[players[input.client_id].position.x][players[input.client_id].position.y] = (char)((players[input.client_id].id + 97));
                                    }
                                    else if(mapa[players[input.client_id].position.x][players[input.client_id].position.y] == TRAP_TEAM_RED){
                                        mapa[players[input.client_id].position.x][players[input.client_id].position.y] = (char)((players[input.client_id].id + 97) + 10);
                                    }
                                }  
                            }  
                        }
                    }
                    // Verifica se o jogador andou para direita
                    else if(jogada_client.tipo == ANDAR_DIREITA){
                        //printf("dir");
                        if(players[input.client_id].position.x + 4 <= X_MAX){
                            if(mapa[players[input.client_id].position.x+4][players[input.client_id].position.y] == VAZIO || mapa[players[input.client_id].position.x+4][players[input.client_id].position.y] == TRAP_TEAM_BLUE 
                            || mapa[players[input.client_id].position.x+4][players[input.client_id].position.y] == TRAP_TEAM_RED || mapa[players[input.client_id].position.x+4][players[input.client_id].position.y] == BANDEIRA_BLUE 
                            || mapa[players[input.client_id].position.x+4][players[input.client_id].position.y] == BANDEIRA_RED){
                                
                              if(mapa[players[input.client_id].position.x][players[input.client_id].position.y] == (char)((players[input.client_id].id + 97) + 10)){ // se o mapa = indicador do id do player + indicador da trap
                                    if(players[input.client_id].team == 1)
                                        mapa[players[input.client_id].position.x][players[input.client_id].position.y] = TRAP_TEAM_BLUE;
                                    else
                                        mapa[players[input.client_id].position.x][players[input.client_id].position.y] = TRAP_TEAM_RED;
                                }
                                else{
                                    mapa[players[input.client_id].position.x][players[input.client_id].position.y] = VAZIO;
                                }
                                
                                jogada_server.xAnterior = players[input.client_id].position.x;
                                jogada_server.yAnterior = players[input.client_id].position.y;
                                jogada_server.itemAnterior = mapa[players[input.client_id].position.x][players[input.client_id].position.y];

                                players[input.client_id].position.x += 4;
                                mapa[players[input.client_id].position.x][players[input.client_id].position.y] = (char)(players[input.client_id].id + 97); // mapa = indicador do id do player

                                if(players[input.client_id].team == 1){
                                    if(mapa[players[input.client_id].position.x][players[input.client_id].position.y] == TRAP_TEAM_RED){
                                        players[input.client_id].estaCongelado = 1;
                                        mapa[players[input.client_id].position.x][players[input.client_id].position.y] = (char)((players[input.client_id].id + 97));
                                    }
                                    else if(mapa[players[input.client_id].position.x][players[input.client_id].position.y] == TRAP_TEAM_BLUE){
                                        mapa[players[input.client_id].position.x][players[input.client_id].position.y] = (char)((players[input.client_id].id + 97) + 10);
                                    }
                                }
                                else{
                                    if(mapa[players[input.client_id].position.x][players[input.client_id].position.y] == TRAP_TEAM_BLUE){
                                        players[input.client_id].estaCongelado = 1;
                                        mapa[players[input.client_id].position.x][players[input.client_id].position.y] = (char)((players[input.client_id].id + 97));
                                    }
                                    else if(mapa[players[input.client_id].position.x][players[input.client_id].position.y] == TRAP_TEAM_RED){
                                        mapa[players[input.client_id].position.x][players[input.client_id].position.y] = (char)((players[input.client_id].id + 97) + 10);
                                    }
                                }  
                            }  
                        }
                    }
                    // Verifica se o jogador andou para a esuqerda
                    else if(jogada_client.tipo == ANDAR_ESQUERDA){
                       // printf("esq");
                        if(players[input.client_id].position.x - 4 >= 0){
                            if(mapa[players[input.client_id].position.x-4][players[input.client_id].position.y] == VAZIO || mapa[players[input.client_id].position.x-4][players[input.client_id].position.y] == TRAP_TEAM_BLUE 
                            || mapa[players[input.client_id].position.x-4][players[input.client_id].position.y] == TRAP_TEAM_RED || mapa[players[input.client_id].position.x-4][players[input.client_id].position.y] == BANDEIRA_BLUE 
                            || mapa[players[input.client_id].position.x-4][players[input.client_id].position.y] == BANDEIRA_RED){
                                
                              if(mapa[players[input.client_id].position.x][players[input.client_id].position.y] == (char)((players[input.client_id].id + 97) + 10)){ // se o mapa = indicador do id do player + indicador da trap
                                    if(players[input.client_id].team == 1)
                                        mapa[players[input.client_id].position.x][players[input.client_id].position.y] = TRAP_TEAM_BLUE;
                                    else
                                        mapa[players[input.client_id].position.x][players[input.client_id].position.y] = TRAP_TEAM_RED;
                                }
                                else{
                                    mapa[players[input.client_id].position.x][players[input.client_id].position.y] = VAZIO;
                                }
                                
                                jogada_server.xAnterior = players[input.client_id].position.x;
                                jogada_server.yAnterior = players[input.client_id].position.y;
                                jogada_server.itemAnterior = mapa[players[input.client_id].position.x][players[input.client_id].position.y];

                                players[input.client_id].position.x -= 4;
                                mapa[players[input.client_id].position.x][players[input.client_id].position.y] = (char)(players[input.client_id].id + 97); // mapa = indicador do id do player

                                if(players[input.client_id].team == 1){
                                    if(mapa[players[input.client_id].position.x][players[input.client_id].position.y] == TRAP_TEAM_RED){
                                        players[input.client_id].estaCongelado = 1;
                                        mapa[players[input.client_id].position.x][players[input.client_id].position.y] = (char)((players[input.client_id].id + 97));
                                    }
                                    else if(mapa[players[input.client_id].position.x][players[input.client_id].position.y] == TRAP_TEAM_BLUE){
                                        mapa[players[input.client_id].position.x][players[input.client_id].position.y] = (char)((players[input.client_id].id + 97) + 10);
                                    }
                                }
                                else{
                                    if(mapa[players[input.client_id].position.x][players[input.client_id].position.y] == TRAP_TEAM_BLUE){
                                        players[input.client_id].estaCongelado = 1;
                                        mapa[players[input.client_id].position.x][players[input.client_id].position.y] = (char)((players[input.client_id].id + 97));
                                    }
                                    else if(mapa[players[input.client_id].position.x][players[input.client_id].position.y] == TRAP_TEAM_RED){
                                        mapa[players[input.client_id].position.x][players[input.client_id].position.y] = (char)((players[input.client_id].id + 97) + 10);
                                    }
                                }  
                            }  
                        }
                    }
                    // Verifica se o jogador botou alguma armadilha
                    else if(jogada_client.tipo == BOTARTRAPS){ 
                       // printf("trap");
                        // Verifica se tem armadilhas para botar
                        if(players[input.client_id].armadilhas > 0){                                                          
                            jogada_server.tipo = BOTARTRAPS;                                                    
                            // Se for do time azul
                            if(players[input.client_id].team == 1){                                                           
                                if(mapa[players[input.client_id].position.x][players[input.client_id].position.y] != TRAP_TEAM_BLUE){
                                    mapa[players[input.client_id].position.x][players[input.client_id].position.y] = (char)((players[input.client_id].id + 97) + 10); // mapa = indicador do id do player + indicador da trap
                                    players[input.client_id].armadilhas--;
                                }
                                else{
                                    char msg[] = "armadilha ja existente";
                                    mapa[players[input.client_id].position.x][players[input.client_id].position.y] = (char)((players[input.client_id].id + 97));    // mapa = indicador do id do player
                                    sendMsgToClient((char *) msg, sizeof(msg) + 1, players[input.client_id].id);
                                }
                            } 
                            // Se for do time vermelho   
                            else{                                                                               
                                if(mapa[players[input.client_id].position.x][players[input.client_id].position.y] != TRAP_TEAM_RED){
                                    mapa[players[input.client_id].position.x][players[input.client_id].position.y] = (char)((players[input.client_id].id + 97) + 10); // mapa = indicador do id do player + indicador da trap
                                    players[input.client_id].armadilhas--;
                                }
                                else{
                                    char msg[] = "armadilha ja existente";
                                    mapa[players[input.client_id].position.x][players[input.client_id].position.y] = (char)((players[input.client_id].id + 97));    // mapa = indicador do id do player
                                    sendMsgToClient((char *) msg, sizeof(msg) + 1, players[input.client_id].id);
                                }
                            }
                            jogada_server.itemAnterior = mapa[players[input.client_id].position.x][players[input.client_id].position.y];
                            jogada_server.xAnterior = players[input.client_id].position.x;
                            jogada_server.yAnterior = players[input.client_id].position.y;
                        }
                    }
                    // Verifica se o jogador congelou/descongelou alguem 
                    else if(jogada_client.tipo == CONGELAR){
                        //printf("congela");
                        int posi = input.client_id + 97;
                        int flag = 1;
                        
                        if(players[input.client_id].congelamentos > 0){
                            // Verifica se tem algum player ao redor do player atual e salva a posição dele caso tenha algo
                            if(mapa[players[input.client_id].position.x+1][players[input.client_id].position.y] >= 97 
                            && mapa[players[input.client_id].position.x+1][players[input.client_id].position.y] <= 106)
                                posi = (int) mapa[players[input.client_id].position.x+1][players[input.client_id].position.y];   
                            else if(mapa[players[input.client_id].position.x-1][players[input.client_id].position.y] >= 97 
                            && mapa[players[input.client_id].position.x-1][players[input.client_id].position.y] <= 106)
                                posi = (int) mapa[players[input.client_id].position.x-1][players[input.client_id].position.y];
                            else if(mapa[players[input.client_id].position.x][players[input.client_id].position.y+1] >= 97 
                            && mapa[players[input.client_id].position.x][players[input.client_id].position.y+1] <= 106)
                                posi = (int) mapa[players[input.client_id].position.x][players[input.client_id].position.y+1];
                            else if(mapa[players[input.client_id].position.x][players[input.client_id].position.y-1] >= 97 
                            && mapa[players[input.client_id].position.x][players[input.client_id].position.y-1] <= 106)
                                posi = (int) mapa[players[input.client_id].position.x][players[input.client_id].position.y-1];
                    
                            // Se o player encontrado foi do time adversário, ele é congelado
                            if(players[posi - 97].team != players[input.client_id].team){
                                players[posi - 97].estaCongelado = 1;
                                
                                //jogada_server.todosJogadores = players[posi - 97]; 
                                jogada_server.tipo = CONGELA;
                                //broadcast((PROTOCOLO_JOGO *) &jogada_server, sizeof(PROTOCOLO_JOGO));
                                
                                flag = 0;
                            }
                        
                            if(flag){
                                posi = input.client_id + 97;

                                // Verifica se tem algum player congelado ao redor do player atual e salva a posição dele caso tenha algo
                                if(mapa[players[input.client_id].position.x+1][players[input.client_id].position.y] >= 107 
                                && mapa[players[input.client_id].position.x+1][players[input.client_id].position.y] <= 116)
                                    posi = (int) mapa[players[input.client_id].position.x+1][players[input.client_id].position.y];   
                                else if(mapa[players[input.client_id].position.x-1][players[input.client_id].position.y] >= 107 
                                && mapa[players[input.client_id].position.x-1][players[input.client_id].position.y] <= 116)
                                    posi = (int) mapa[players[input.client_id].position.x-1][players[input.client_id].position.y];
                                else if(mapa[players[input.client_id].position.x][players[input.client_id].position.y+1] >= 107 
                                && mapa[players[input.client_id].position.x][players[input.client_id].position.y+1] <= 116)
                                    posi = (int) mapa[players[input.client_id].position.x][players[input.client_id].position.y+1];
                                else if(mapa[players[input.client_id].position.x][players[input.client_id].position.y-1] >= 107 
                                && mapa[players[input.client_id].position.x][players[input.client_id].position.y-1] <= 116)
                                    posi = (int) mapa[players[input.client_id].position.x][players[input.client_id].position.y-1];

                                // Se o player encontrado foi do mesmo time, ele é descongelado
                                if((players[posi - 97].team == players[input.client_id].team) && (posi != (input.client_id + 97))){
                                    players[posi - 97].estaCongelado = 0;
                                    //jogada_server.todosJogadores = players[posi - 97]; 
                                    jogada_server.tipo = DESCONGELA;
                                    //broadcast((PROTOCOLO_JOGO *) &jogada_server, sizeof(PROTOCOLO_JOGO));      
                                }
                            }
                            players[input.client_id].congelou = 1;
                            players[input.client_id].congelamentos--;   
                        }
                                              
                    }
                    else if(jogada_client.tipo == ENDGAME){
                        //printf("oi");
                        printf("%s disconnected id = %d\n", players[input.client_id].name, input.client_id);
                        disconnectClient(input.client_id);
                        jogada_client.tipo = -1;
                    }

                    // Se ele chegou na bandeira vermelha e ele é do time azul ou Se ele chegou na bandeira azul e ele é do time vermelho = ele tá quase ganhando!
                    if((players[input.client_id].position.x == X_FLAG_RED && players[input.client_id].position.y == Y_FLAG_RED && players[input.client_id].team == 1) || (players[input.client_id].position.x == X_FLAG_BLUE && players[input.client_id].position.y == Y_FLAG_BLUE && players[input.client_id].team == 2)){
                        if(mapa[players[input.client_id].position.x][players[input.client_id].position.y] == BANDEIRA_RED){
                            players[input.client_id].comBandeira = 1;
                            mapa[players[input.client_id].position.x][players[input.client_id].position.y] = (char)(players[input.client_id].id + 97);
                        }
                            
                    }
                    // Se ele chegou na base vermelha, ele é do time vermelho e está com a bandeira azul = ele ganhou uma partida!
                    else if(((players[input.client_id].position.x == X_ENTRADA_1_RED && players[input.client_id].position.y == Y_ENTRADA_1_RED) || (players[input.client_id].position.x == X_ENTRADA_2_RED && players[input.client_id].position.y == Y_ENTRADA_2_RED) || (players[input.client_id].position.x == X_ENTRADA_3_RED && players[input.client_id].position.y == Y_ENTRADA_3_RED)) && (players[input.client_id].team == 2) && (players[input.client_id].comBandeira == 1)){
                        PROTOCOLO_JOGO msg_final;
                        msg_final.winner = 'v';
                        msg_final.tipo = ENDGAME;
                        broadcast((PROTOCOLO_JOGO *) &msg_final, sizeof(PROTOCOLO_JOGO));
                        fim = 1;
                    }
                    // Se ele chegou na base azul com a bandeira vermelha e ele é do time azul = ele ganhou uma partida!
                    else if(((players[input.client_id].position.x == X_ENTRADA_1_BLUE && players[input.client_id].position.y == Y_ENTRADA_1_BLUE) || (players[input.client_id].position.x == X_ENTRADA_2_BLUE && players[input.client_id].position.y == Y_ENTRADA_2_BLUE) || (players[input.client_id].position.x == X_ENTRADA_3_BLUE && players[input.client_id].position.y == Y_ENTRADA_3_BLUE)) && (players[input.client_id].team == 1) && (players[input.client_id].comBandeira == 1)){
                        PROTOCOLO_JOGO msg_final;
                        msg_final.winner = 'b';
                        msg_final.tipo = ENDGAME;
                        broadcast((PROTOCOLO_JOGO *) &msg_final, sizeof(PROTOCOLO_JOGO));
                        fim = 1;
                    }
                    
                    if(fim != 1){
                        jogada_server.tipo = GAME;
                        for(int i = 0; i < qntJogadores; i++){
                            jogada_server.todosJogadores[i] = players[i];
                            // printf("%d", i);
                        }

                        //printf("1");
                        //printf("tipo - %d\n", jogada_server.tipo);
                        //printf("2 - %d %d \n", players[input.client_id].position.x, players[input.client_id].position.y);
                        //printf("pos x = %d, pos y = %d\n", jogada_server.todosJogadores[input.client_id].position.x, jogada_server.todosJogadores[input.client_id].position.y);
                        
                        broadcast((PROTOCOLO_JOGO *) &jogada_server, sizeof(PROTOCOLO_JOGO));
                    }
                }
            // }
        // }
        }
       
        tempoAtual = al_get_time() - tempoInicio;
        tempo.tipo = TEMPO;
       // sprintf(tempo.mensagem, "%lf", tempoAtual);
        jogada_server.tipo = GAME;
        for(int i = 0; i < qntJogadores; i++){
            jogada_server.todosJogadores[i] = players[i];
            //printf("posi x = %d, posi y = %d\n", players[i].position.x, players[i].position.y);
            //printf("%d", i);
        }

        //printf("2");
        //printf("tipo - %d\n", jogada_server.tipo);
        // printf("2 - %d %d \n", players[input.client_id].position.x, players[input.client_id].position.y);
        //printf("posi x = %d, posi y = %d\n", jogada_server.todosJogadores[nput.client_id].position.x, jogada_server.todosJogadores[input.client_id].position.y);
       
       
        broadcast((PROTOCOLO_JOGO *) &jogada_server, sizeof(PROTOCOLO_JOGO));
        //broadcast((PROTOCOLO_JOGO *) &tempo, sizeof(PROTOCOLO_JOGO)); 
    }
}

void runGameTest(){
    int fim = 0;
    struct msg_ret_t input;
    PROTOCOLO_JOGO jogada, updateServer;

    while(!fim){
        input = recvMsgFromClient((PROTOCOLO_JOGO *) &jogada, 0, WAIT_FOR_IT);
       
            if(jogada.tipo == ANDAR_CIMA){
                updateServer.tipo=GAME;
                updateServer.todosJogadores[0]=jogada.todosJogadores[0];
                updateServer.todosJogadores[0].position.y-=4;
                broadcast((PROTOCOLO_JOGO *) &jogada, sizeof(PROTOCOLO_JOGO));
                //players[input.client_id].position.y -= 4;
            }
            printf("tipo %d\n", jogada.tipo);
        
    }
}