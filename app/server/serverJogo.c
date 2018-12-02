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
char mapa [50][51];
Player players[6];
int qntJogadores = 0;
PROTOCOLO_JOGO jogada_client, jogada_server, tempo;                 // Protocolo de envio a ser enviado para o cliente com as infos do jogo;
struct msg_ret_t input;
PROTOCOLO_JOGO jogada, updateServer;
PROTOCOLO_TESTE teste_envia, teste_lobby;
double tempoInicio, tempoAtual;
int i, j, fim = 0, tp=0; 

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

    // puts("saiu");

    //JOGO
    runGameTest();
   
    return 0;
}

void imprimeMatriz(){
    for(i=0;i<50;i++){
        printf("%s\n", mapa[i]);
    }
}

void inicializaMapa(){
    
    strcpy(mapa[0],"NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN");
    strcpy(mapa[1],"NNNNNNNNNNNNNNNNNNNXXXXNNNNXXXXXXXXXXXXXNNNNNXNNNN");
    strcpy(mapa[2],"NNNNNNNNNNNNNNNNNNNXXXXNNNNXXXXXXXXXXXXXNNNNNXNNNN");
    strcpy(mapa[3],"NXXXXXXXXXXXXXXXXXXXXXXNNNNXXXXXXXXXXXXXNNNNSSSSSN");
    strcpy(mapa[4],"NXXXXXXXXXXXXXXXXXXXXXXNNNNXXXXXXXXXXXXXSNNZSSSSSN");
    strcpy(mapa[5],"NXXXXXXXXXXXXXXXXXXXXXXNNNNXXXXXXXXXXXXXSSNZZSSSSN");
    strcpy(mapa[6],"NXXXXXXXXXXXXXXXXXXXXXXNNNNXXXXXXXXXXXXXSSNZZSSSSN");
    strcpy(mapa[7],"NXXXXXXXXXXXXXXXXXXXXXXNNNNXXXXXXXXXXXXXXXSSZZSNNN");
    strcpy(mapa[8],"NXXXXXXXXXXXXXXXXXXXXSSSSSSSSXXXXXXXXXXXXXSSNZZSSN");
    strcpy(mapa[9],"NXXXXXXXXXXXXXXXXXXXXSSSSSSSSXXXXXXXXXXXXXSSNNZNSN");
    strcpy(mapa[10],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXSNNNNN");
    strcpy(mapa[11],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXSNNNNN");
    strcpy(mapa[12],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[13],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[14],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[15],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[16],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[17],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[18],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[19],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[20],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[21],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[22],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[23],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[24],"NXXXXXXXXXXXVVVVVXXVVSSSSSSSSXXXXXXXXXXXXXXXXXXXXN");
    strcpy(mapa[25],"NXXXXXXXXXXXVVVVVXXVVSSSSSSSSXXXXXXXXXXXXXXXXXXXXN");
    strcpy(mapa[26],"NXXXXXXXXXXXVVVVVXXVVSSSSSSSSXXXXXXXXXXXXXXXXXXXXN");
    strcpy(mapa[27],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[28],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[29],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[30],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[31],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[32],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[33],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[34],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[35],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[36],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[37],"NXXXXXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[38],"NNNNSXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[39],"NNNNSXXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[40],"NNZNNSXXXXXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[41],"NSSZNNSSXXXXVVVVVXXVVVSSSSSSXXXXXXXXXXXXXXXXXXXXXN");
    strcpy(mapa[42],"NNSZZSSSXXXXXXXXXXXXXXSSSSSSXXXXXXXXXXXXXXXXXXXXXN");
    strcpy(mapa[43],"NNSSZZZNXXXXXXXXXXXXXXXNNNNXXXXXXXXXXXXXXXXXXXXXXN");
    strcpy(mapa[44],"NSSSSZZNSSXXVVVVVXXVVVVNNNNVVVVXXVVVVVXXXXXXXXXXXN");
    strcpy(mapa[45],"NSSSSSZZNSSXXXXXXXXXXXXNNNNXXXXXXXXXXXXXXXXXXXXXXN");
    strcpy(mapa[46],"NSSSSSZZNNSXXXXXXXXXXXXNNNNXXXXXXXXXXXXXXXXXXXXXXN");
    strcpy(mapa[47],"NNNSSNNNNNSXXXXXXXXXXXXNNNNXXXXNNNNNNNNNNNNNNNNNNN");
    strcpy(mapa[48],"NNNNSNNNNNSXXXXXXXXXXXXNNNNXXXXNNNNNNNNNNNNNNNNNNN");
    strcpy(mapa[49],"NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN");

//   for(i = 0; i < 48; i++){
//       for(j = 0; j < 24; j++){
//           printf("%c",mapa[i][j]);
//       }
//       printf("\n");
//   }
//   printf("\n");
}


void inicializaJogadores(){
    int notReady = 1;
    int qntJogadoresProntos = 0;
    struct msg_ret_t input;

    while (notReady) {
        PROTOCOLO_INICIAL msg_client, msg_inicial_server;
        int id = acceptConnection();
        input = recvMsg((PROTOCOLO_INICIAL *) &msg_client);
        if(input.status != NO_MESSAGE){
            if (id != NO_CONNECTION) {
                    // Recebe o nick, capacete e id das novas conexões
                    if(msg_client.tipo == INICIAL ){
                        ++qntJogadores;
                        
                        if(qntJogadores < MAX_CLIENTS){
                            strcpy(players[id].name, msg_client.jogador.name);     // Salva o nick
                            players[id].helmet = msg_client.jogador.helmet;        // Salva o capacete
                            players[id].id = id;                                   // Salva o id         
                        
                            printf("%s connected id = %d\n", players[id].name, id);
                        
                            // Inicializando os demais atributos do player atual
                            if(qntJogadores%2 == 0){
                                players[id].team = 1;
                                players[id].position.x = 30;
                                players[id].position.y = 30;
                                players[id].posicaoPrint.x = players[id].position.x * 19;
                                players[id].posicaoPrint.y = players[id].position.y * 19;
                            }
                            else{
                                players[id].team = 2;
                                players[id].position.x = 20;
                                players[id].position.y = 20;
                                players[id].posicaoPrint.x = players[id].position.x * 19;
                                players[id].posicaoPrint.y = players[id].position.y * 19;
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

                            //qntJogadores++;                                       
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
                    //puts("loopou");
                    if(msg_client.tipo == ENDGAME){
                        printf("%s disconnected id = %d\n", players[msg_client.jogador.id].name, msg_client.jogador.id);
                        disconnectClient(msg_client.jogador.id);
                        msg_client.tipo = -1;
                    }
                    if(msg_client.tipo == COMECOU){
                        //puts("comecou?");
                        players[input.client_id] = msg_client.jogador;

                        if(players[input.client_id].ready == 0){
                            qntJogadoresProntos++;
                            //printf("id = %d",input.client_id);
                            players[input.client_id].ready++;
                        }
                    
                        if(input.client_id == 0){
                            // if(qntJogadoresProntos >= 4){
                                //puts("fooi");
                                //printf("qnt = %d", qntJogadores);
                                teste_lobby.qntJogadores = qntJogadores;
                                teste_lobby.tipo = 'l';
                                if(players[0].ready == 1){
                                    notReady = 0;
                                   // puts("break");
                                    //break;
                                }
                                for(int i = 0; i < qntJogadores; i++){
                                    teste_lobby.todosJogadores[i] = players[i];
                                    //printf("%d - %d, %d", i, jogada_server.todosJogadores[i].position.x,jogada_server.todosJogadores[i].position.y );
                                 }
                                broadcast(&teste_lobby, sizeof(PROTOCOLO_TESTE)); 
                      
                            // }    
                        }
                        else{
                            players[input.client_id].ready = 1;
                            //puts("waiting");
                            teste_lobby.tipo = 'w';
                            for(int i = 0; i < qntJogadores; i++){
                                teste_lobby.todosJogadores[i] = players[i];
                            //printf("%d - %d, %d", i, jogada_server.todosJogadores[i].position.x,jogada_server.todosJogadores[i].position.y );
                            }
                            broadcast(&teste_lobby, sizeof(PROTOCOLO_TESTE)); 
                        }

                           
                    }
                }
        }
  
    }
}

void runGame(){
    
    tempoInicio = al_get_time();
    while( (al_get_time() - tempoInicio < TEMPO_LIMITE) && !fim){
        // printf("loop");
        input = recvMsg((PROTOCOLO_JOGO *) &jogada_client);
        if(input.status != NO_MESSAGE){
            // puts("recebeu");
        // for(input.client_id = 0; input.client_id < qntJogadores; input.client_id++){ 
            //if((players[input.client_id].id == input.client_id)){ 
                // Atualiza o player do server com o player enviado pelo client
                //players[input.client_id] = jogada_client.todosJogadores[0];
                // Verifica se o jogador não está congelado
                if(!players[input.client_id].estaCongelado){
                    //puts("recebe");
                    //printf("x = %d, Y = %d", players[input.client_id].position.x, players[input.client_id].position.y);
                    // Verifica se o jogador andou para cima
                    if(jogada_client.tipo == ANDAR_CIMA){

                        // printf("cima\n");
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
                                
                                // jogada_server.qntJogadores=input.client_id;
                                // jogada_server.winner='c';
                                teste_envia.acao='c';
                                teste_envia.id_acao=input.client_id;
                                // players[input.client_id].position.y -= 4;
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

                        // printf("1");
                        //printf("tipo - %d\n", jogada_server.tipo);
                        //printf("2 - %d %d \n", players[input.client_id].position.x, players[input.client_id].position.y);
                        //printf("pos x = %d, pos y = %d\n", jogada_server.todosJogadores[input.client_id].position.x, jogada_server.todosJogadores[input.client_id].position.y);
                        teste_envia.tipo='G';
                        
                        for(int i = 0; i < qntJogadores; i++){
                            teste_envia.todosJogadores[i] = players[i];
                            // printf("%d", i);
                        }
                        // sendMsgToClient((PROTOCOLO_JOGO *) &jogada_server, sizeof(PROTOCOLO_JOGO), 0);
                        // printf("%d %c\n ", jogada_server.qntJogadores, jogada_server.winner);
                        //printf("pos x = %d, pos y = %d\n", jogada_server.todosJogadores[input.client_id].position.x, jogada_server.todosJogadores[input.client_id].position.y);
                        sendMsgToClient((PROTOCOLO_TESTE *) &teste_envia, sizeof(PROTOCOLO_TESTE), 0);
                        printf("%d %c\n ", teste_envia.id_acao, teste_envia.acao);
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

        // printf("2");
        //printf("tipo - %d\n", jogada_server.tipo);
        // printf("2 - %d %d \n", players[input.client_id].position.x, players[input.client_id].position.y);
        //printf("posi x = %d, posi y = %d\n", jogada_server.todosJogadores[nput.client_id].position.x, jogada_server.todosJogadores[input.client_id].position.y);
       
       
        sendMsgToClient((PROTOCOLO_JOGO *) &jogada_server, sizeof(PROTOCOLO_JOGO), 0);
        //broadcast((PROTOCOLO_JOGO *) &tempo, sizeof(PROTOCOLO_JOGO)); 
    }
}

void runGameTest(){

    for(i = 0; i < qntJogadores; i++){
        //puts("oi");
        if(players[i].ready == 0){
            //printf("id disconnect %d", players[i].id);
            disconnectClient(i);
        }
    }
   // printf("qnt = %d", qntJogadores);
    while(!fim){
        input = recvMsg((PROTOCOLO_JOGO *) &jogada);
        // printf("acao = %c, id = %d", jogada.winner, jogada.qntJogadores);
       // players[input.client_id] = jogada.todosJogadores[input.client_id];
        // for(int i = 0; i < qntJogadores; i++){
        //     players[i] = jogada.todosJogadores[i];
        //    // printf("id = %d team = %d", players[i].id, players[i].team);
        // }
        // printf("x = %d, Y = %d", jogada.todosJogadores[input.client_id].position.x, jogada.todosJogadores[input.client_id].position.y);

        if(input.status != NO_MESSAGE){
            // printf("X = %d, Y = %d,", jogada.todosJogadores[input.client_id].position.x, jogada.todosJogadores[input.client_id].position.y);
            // printf(" Mc = %c",  mapa[jogada.todosJogadores[input.client_id].position.x -1] [ jogada.todosJogadores[input.client_id].position.y]);
            // printf(" Mb = %c",  mapa[jogada.todosJogadores[input.client_id].position.x +1] [ jogada.todosJogadores[input.client_id].position.y]);
            // printf(" Me = %c",  mapa[jogada.todosJogadores[input.client_id].position.x] [ jogada.todosJogadores[input.client_id].position.y -1]);
            // printf(" Md = %c\n",  mapa[jogada.todosJogadores[input.client_id].position.x] [ jogada.todosJogadores[input.client_id].position.y + 1]);

             for(int i = 0; i < qntJogadores; i++){
                players[i] = jogada.todosJogadores[i];
                printf("posicao print::: x: %d, y:%d, posicao:: x: %d, y:%d\n", jogada.todosJogadores[i].posicaoPrint.x, jogada.todosJogadores[i].posicaoPrint.y, jogada.todosJogadores[i].position.x, jogada.todosJogadores[i].position.y);
           // printf("id = %d team = %d", players[i].id, players[i].team);
            }
            // printf("x = %d, Y = %d", players[input.client_id].position.x, players[input.client_id].position.y);
            if(jogada.tipo == ANDAR_CIMA && mapa[jogada.todosJogadores[input.client_id].position.x - 1] [ jogada.todosJogadores[input.client_id].position.y] != 'N' && jogada.todosJogadores[input.client_id].estaCongelado != 1){
                // imprimeMatriz();
                if(jogada.todosJogadores[input.client_id].team == 1 && mapa[jogada.todosJogadores[input.client_id].position.x - 1] [ jogada.todosJogadores[input.client_id].position.y] != 'R'  || jogada.todosJogadores[input.client_id].team == 2 && mapa[jogada.todosJogadores[input.client_id].position.x - 1] [ jogada.todosJogadores[input.client_id].position.y] != 'B'){
                    jogada.todosJogadores[input.client_id].estaCongelado = 1;
                    teste_envia.acao = 'c';
                    teste_envia.tipo = 'G';
                    teste_envia.id_acao = input.client_id;
                    //printf("acao = %c, id = %d", jogada.winner, jogada.qntJogadores);
                    for(int i = 0; i < qntJogadores; i++){
                        teste_envia.todosJogadores[i] = players[i];
                    // printf("id = %d team = %d", players[i].id, players[i].team);
                    }
                    broadcast(&teste_envia, sizeof(PROTOCOLO_TESTE));
                }else{
                    jogada.todosJogadores[input.client_id].estaCongelado = 0;
                    teste_envia.acao = 'c';
                    teste_envia.tipo = 'G';
                    teste_envia.id_acao = input.client_id;
                    //printf("acao = %c, id = %d", jogada.winner, jogada.qntJogadores);
                    for(int i = 0; i < qntJogadores; i++){
                        teste_envia.todosJogadores[i] = players[i];
                    // printf("id = %d team = %d", players[i].id, players[i].team);
                    }
                    broadcast(&teste_envia, sizeof(PROTOCOLO_TESTE));
                } 
            }
            else if(jogada.tipo == ANDAR_BAIXO && mapa[jogada.todosJogadores[input.client_id].position.x + 1] [ jogada.todosJogadores[input.client_id].position.y] != 'N' && jogada.todosJogadores[input.client_id].estaCongelado != 1){
                if(jogada.todosJogadores[input.client_id].team == 1 && mapa[jogada.todosJogadores[input.client_id].position.x - 1] [ jogada.todosJogadores[input.client_id].position.y] != 'R' || jogada.todosJogadores[input.client_id].team == 2 && mapa[jogada.todosJogadores[input.client_id].position.x - 1] [ jogada.todosJogadores[input.client_id].position.y] != 'B'){
                    jogada.todosJogadores[input.client_id].estaCongelado = 1;
                    teste_envia.acao = 'b';
                    teste_envia.tipo = 'G';
                    teste_envia.id_acao = input.client_id;
                    //printf("acao = %c, id = %d", jogada.winner, jogada.qntJogadores);
                    for(int i = 0; i < qntJogadores; i++){
                        teste_envia.todosJogadores[i] = players[i];
                    // printf("id = %d team = %d", players[i].id, players[i].team);
                    }
                    broadcast(&teste_envia, sizeof(PROTOCOLO_TESTE));
                }else{
                    jogada.todosJogadores[input.client_id].estaCongelado = 0;
                    teste_envia.acao = 'b';
                    teste_envia.tipo = 'G';
                    teste_envia.id_acao = input.client_id;
                    //printf("acao = %c, id = %d", jogada.winner, jogada.qntJogadores);
                    for(int i = 0; i < qntJogadores; i++){
                        teste_envia.todosJogadores[i] = players[i];
                    // printf("id = %d team = %d", players[i].id, players[i].team);
                    }
                    broadcast(&teste_envia, sizeof(PROTOCOLO_TESTE));
                } 
            }
            else if(jogada.tipo == ANDAR_ESQUERDA && mapa[jogada.todosJogadores[input.client_id].position.x] [ jogada.todosJogadores[input.client_id].position.y - 1] != 'N' && jogada.todosJogadores[input.client_id].estaCongelado != 1){
                if(jogada.todosJogadores[input.client_id].team == 1 && mapa[jogada.todosJogadores[input.client_id].position.x - 1] [ jogada.todosJogadores[input.client_id].position.y] != 'R' || jogada.todosJogadores[input.client_id].team == 2 && mapa[jogada.todosJogadores[input.client_id].position.x - 1] [ jogada.todosJogadores[input.client_id].position.y] != 'B'){
                    jogada.todosJogadores[input.client_id].estaCongelado = 1;
                    teste_envia.acao = 'e';
                    teste_envia.tipo = 'G';
                    teste_envia.id_acao = input.client_id;
                    //printf("acao = %c, id = %d", jogada.winner, jogada.qntJogadores);
                    for(int i = 0; i < qntJogadores; i++){
                        teste_envia.todosJogadores[i] = players[i];
                    // printf("id = %d team = %d", players[i].id, players[i].team);
                    }
                    broadcast(&teste_envia, sizeof(PROTOCOLO_TESTE));
                }else{
                    jogada.todosJogadores[input.client_id].estaCongelado = 0;
                    teste_envia.acao = 'e';
                    teste_envia.tipo = 'G';
                    teste_envia.id_acao = input.client_id;
                    //printf("acao = %c, id = %d", jogada.winner, jogada.qntJogadores);
                    for(int i = 0; i < qntJogadores; i++){
                        teste_envia.todosJogadores[i] = players[i];
                    // printf("id = %d team = %d", players[i].id, players[i].team);
                    }
                    broadcast(&teste_envia, sizeof(PROTOCOLO_TESTE));
                } 
            }
            else if(jogada.tipo == ANDAR_DIREITA && mapa[jogada.todosJogadores[input.client_id].position.x] [ jogada.todosJogadores[input.client_id].position.y + 1] != 'N' && jogada.todosJogadores[input.client_id].estaCongelado != 1){
                if(jogada.todosJogadores[input.client_id].team == 1 && mapa[jogada.todosJogadores[input.client_id].position.x - 1] [ jogada.todosJogadores[input.client_id].position.y] != 'R' || jogada.todosJogadores[input.client_id].team == 2 && mapa[jogada.todosJogadores[input.client_id].position.x - 1] [ jogada.todosJogadores[input.client_id].position.y] != 'B'){
                    jogada.todosJogadores[input.client_id].estaCongelado = 1;
                    teste_envia.acao = 'd';
                    teste_envia.tipo = 'G';
                    teste_envia.id_acao = input.client_id;
                    //printf("acao = %c, id = %d", jogada.winner, jogada.qntJogadores);
                    for(int i = 0; i < qntJogadores; i++){
                        teste_envia.todosJogadores[i] = players[i];
                    // printf("id = %d team = %d", players[i].id, players[i].team);
                    }
                    broadcast(&teste_envia, sizeof(PROTOCOLO_TESTE));
                }else{
                    jogada.todosJogadores[input.client_id].estaCongelado = 0;
                    teste_envia.acao = 'd';
                    teste_envia.tipo = 'G';
                    teste_envia.id_acao = input.client_id;
                    //printf("acao = %c, id = %d", jogada.winner, jogada.qntJogadores);
                    for(int i = 0; i < qntJogadores; i++){
                        teste_envia.todosJogadores[i] = players[i];
                    // printf("id = %d team = %d", players[i].id, players[i].team);
                    }
                    broadcast(&teste_envia, sizeof(PROTOCOLO_TESTE));
                } 
            }
            else if(jogada.tipo == BOTARTRAPS && mapa[jogada.todosJogadores[input.client_id].position.x] [jogada.todosJogadores[input.client_id].position.y] != 'S' && jogada.todosJogadores[input.client_id].estaCongelado != 1){
                if(mapa[jogada.todosJogadores[input.client_id].position.x][ jogada.todosJogadores[input.client_id].position.y] != 'Z'){
                    if(jogada.todosJogadores[input.client_id].team == 1 && mapa[jogada.todosJogadores[input.client_id].position.x] [ jogada.todosJogadores[input.client_id].position.y] != 'B' && jogada.todosJogadores[input.client_id].armadilhas > 0){
                        // puts("traps");
                        for(int i = 0; i < qntJogadores; i++){
                            teste_envia.todosJogadores[i] = players[i];
                        }
                        teste_envia.acao = 't';
                        teste_envia.tipo = 'G';
                        teste_envia.id_acao = input.client_id;
                        teste_envia.traps[tp].posiT.x= jogada.todosJogadores[input.client_id].position.x;
                        teste_envia.traps[tp].posiT.y= jogada.todosJogadores[input.client_id].position.y;
                        teste_envia.traps[tp].team = 1;
                        teste_envia.tp++;
                        // printf("%d %d\n", teste_envia.traps[tp].posiT.x, teste_envia.traps[tp].posiT.y);
                        tp++;
                        mapa[jogada.todosJogadores[input.client_id].position.x][jogada.todosJogadores[input.client_id].position.y] = 'B';
                    // printf("acao = %c, id = %d", teste_envia.acao, teste_envia.id_acao);
                        broadcast(&teste_envia, sizeof(PROTOCOLO_TESTE));
                    }else if(jogada.todosJogadores[input.client_id].team == 2 && mapa[jogada.todosJogadores[input.client_id].position.x] [ jogada.todosJogadores[input.client_id].position.y] != 'R' && jogada.todosJogadores[input.client_id].armadilhas > 0){
                        // puts("traps");
                        for(int i = 0; i < qntJogadores; i++){
                            teste_envia.todosJogadores[i] = players[i];
                        }
                        teste_envia.acao = 't';
                        teste_envia.tipo = 'G';
                        teste_envia.id_acao = input.client_id;
                        teste_envia.traps[tp].posiT.x = jogada.todosJogadores[input.client_id].position.x;
                        teste_envia.traps[tp].posiT.y = jogada.todosJogadores[input.client_id].position.y;
                        teste_envia.traps[tp].team = 2;
                        teste_envia.tp++;
                        // printf("%d %d\n", teste_envia.traps[tp].posiT.x, teste_envia.traps[tp].posiT.y);
                        tp++;
                        mapa[jogada.todosJogadores[input.client_id].position.x] [ jogada.todosJogadores[input.client_id].position.y] = 'R';
                    // printf("acao = %c, id = %d", teste_envia.acao, teste_envia.id_acao);
                        broadcast(&teste_envia, sizeof(PROTOCOLO_TESTE));
                    }
                    // printf("%s \n", mapa[jogada.todosJogadores[input.client_id].position.x]);
                }
            }
            // else if(jogada.tipo == CONGELA){
            //     teste_envia.acao = 'f';
            //     teste_envia.tipo = 'G';
            //     for(int i = 0; i < qntJogadores; i++);
            // }
            else if(jogada.tipo == ENDGAME){
                disconnectClient(input.client_id);
                printf("%s disconnected id = %d\n", players[input.client_id].name, input.client_id);
            }
            else{
                teste_envia.acao = 'n';

            //     teste_envia.acao = 'n';
            //     teste_envia.tipo = 'G';
            //     teste_envia.id_acao = input.client_id;
            //     for(int i = 0; i < qntJogadores; i++){
            //         teste_envia.todosJogadores[i] = players[i];
            //     }
            //    // printf("acao = %c, id = %d", teste_envia.acao, teste_envia.id_acao);
            //     broadcast(&teste_envia, sizeof(PROTOCOLO_TESTE));          
            }
        }
    }
}