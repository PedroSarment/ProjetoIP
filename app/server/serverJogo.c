#include "server.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "ACore.h"


//DEFINIÇÕES DAS CONSTANTES
#define LOGIN_MAX_SIZE 13
#define MAX_CLIENTS 6
#define X_MAX 1280
#define Y_MAX 720

//INICIALIZAÇÃO DAS VARIÁVEIS GLOBAIS
char mapa [50][51];
Player players[6];
int qntJogadores = 0;
struct msg_ret_t input;
PROTOCOLO_JOGO envia_client, msg_lobby;
PROTOCOLO_ENVIO_CLIENT jogada;
double tempoInicio, tempoAtual;
int i, j, fim = 0, tp = 0, flagCongelamento = 0; 

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
    runGameTest();
   
    return 0;
}

void imprimeMatriz(){
    for(i=0;i<50;i++){
        printf("%s\n", mapa[i]);
    }
}

void inicializaMapa(){
    //LEGENDA:
    //N -> Player não pode andar
    //X -> Player anda e pode botar trap
    //S -> Player anda e não pode botar trap
    //V -> player anda camuflado e pode botar trap
    //Q -> bandeira vermelha
    //A - > bandeira azul
    //B -> trap azul
    //R -> trap vermelha

    strcpy(mapa[0],"NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN");
    strcpy(mapa[1],"NNNNNNNNNNNNNNNNNNNXXXXNNNNXXXXXXXXXXXXXNNNNNXNNNN");
    strcpy(mapa[2],"NNNNNNNNNNNNNNNNNNNXXXXNNNNXXXXXXXXXXXXXNNNNNXNNNN");
    strcpy(mapa[3],"NXXXXXXXXXXXXXXXXXXXXXXNNNNXXXXXXXXXXXXXNNNNSSSSSN");
    strcpy(mapa[4],"NXXXXXXXXXXXXXXXXXXXXXXNNNNXXXXXXXXXXXXXSNNZSSASSN");
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
    strcpy(mapa[45],"NSQSSSZZNSSXXXXXXXXXXXXNNNNXXXXXXXXXXXXXXXXXXXXXXN");
    strcpy(mapa[46],"NSSSSSZZNNSXXXXXXXXXXXXNNNNXXXXXXXXXXXXXXXXXXXXXXN");
    strcpy(mapa[47],"NNNSSNNNNNSXXXXXXXXXXXXNNNNXXXXNNNNNNNNNNNNNNNNNNN");
    strcpy(mapa[48],"NNNNSNNNNNSXXXXXXXXXXXXNNNNXXXXNNNNNNNNNNNNNNNNNNN");
    strcpy(mapa[49],"NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN");
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
                    players[input.client_id] = msg_client.jogador;

                    if(players[input.client_id].ready == 0){
                        qntJogadoresProntos++;
                        players[input.client_id].ready++;
                    }
                
                    if(input.client_id == 0){
                        msg_lobby.qntJogadores = qntJogadores;
                        msg_lobby.tipo = 'l';
                        if(players[0].ready == 1){
                            notReady = 0;                  
                        }
                        for(int i = 0; i < qntJogadores; i++){
                            msg_lobby.todosJogadores[i] = players[i];
                        }
                        broadcast(&msg_lobby, sizeof(PROTOCOLO_JOGO));     
                    }
                    else{
                        players[input.client_id].ready = 1;
                        msg_lobby.tipo = 'w';
                        for(int i = 0; i < qntJogadores; i++){
                            msg_lobby.todosJogadores[i] = players[i];
                        }
                        broadcast(&msg_lobby, sizeof(PROTOCOLO_JOGO)); 
                    }                       
                }
            }
        }
    }
}

void runGameTest(){

    for(i = 0; i < qntJogadores; i++){
        if(players[i].ready == 0){
            disconnectClient(i);
        }
    }
    while(!fim){
        input = recvMsg((PROTOCOLO_ENVIO_CLIENT *) &jogada);
        if(input.status != NO_MESSAGE){           
            if(!jogada.estaCongelado){
               //for(int i = 0; i < qntJogadores; i++){
                    players[input.client_id] = jogada;                   
                //}
                if(jogada.tipo == ANDAR_CIMA && mapa[jogada.position.x - 1] [ jogada.position.y] != 'N'){
                    if(jogada.team == 1 && mapa[jogada.position.x - 1] [ jogada.position.y] == 'R'  || jogada.team == 2 && mapa[jogada.position.x - 1] [ jogada.position.y] == 'B'){
                        players[input.client_id].estaCongelado = 1;
                        envia_client.acao = 'c';
                        envia_client.tipo = 'G';
                        envia_client.ganhou = 'n';
                        envia_client.id_acao = input.client_id;
                        for(int i = 0; i < qntJogadores; i++){
                            if(players[i].position.x == jogada.position.x - 1 && players[i].position.y == jogada.position.y)
                                envia_client.tipo = 'n';
                            envia_client.todosJogadores[i] = players[i];
                        }
                        broadcast(&envia_client, sizeof(PROTOCOLO_JOGO));
                    }
                    else if(jogada.team == 1 && mapa[jogada.position.x-1] [ jogada.position.y] == 'Q'  || jogada.team == 2 && mapa[jogada.position.x-1] [ jogada.position.y] == 'A'){
                        players[input.client_id].comBandeira = 1;
                        envia_client.acao = 'c';
                        envia_client.tipo = 'G';
                        envia_client.ganhou = 's';
                        envia_client.id_acao = input.client_id;
                        for(int i = 0; i < qntJogadores; i++){
                            if(players[i].position.x == jogada.position.x - 1 && players[i].position.y == jogada.position.y)
                                envia_client.tipo = 'n';
                            envia_client.todosJogadores[i] = players[i];
                        }
                        broadcast(&envia_client, sizeof(PROTOCOLO_JOGO));
                    }
                    else{
                        players[input.client_id].estaCongelado = 0;
                        envia_client.acao = 'c';
                        envia_client.tipo = 'G';
                        envia_client.ganhou = 'n';
                        envia_client.id_acao = input.client_id;
                        for(int i = 0; i < qntJogadores; i++){
                            if(players[i].position.x == jogada.position.x - 1 && players[i].position.y == jogada.position.y)
                                envia_client.tipo = 'n';
                            envia_client.todosJogadores[i] = players[i];
                        }
                        broadcast(&envia_client, sizeof(PROTOCOLO_JOGO));
                    } 
                }
                else if(jogada.tipo == ANDAR_BAIXO && mapa[jogada.position.x + 1] [ jogada.position.y] != 'N'){
                    if(jogada.team == 1 && mapa[jogada.position.x +1] [ jogada.position.y] == 'R' || jogada.team == 2 && mapa[jogada.position.x + 1] [ jogada.position.y] == 'B'){
                        players[input.client_id].estaCongelado = 1;
                        envia_client.acao = 'b';
                        envia_client.tipo = 'G';
                        envia_client.ganhou = 'n';
                        envia_client.id_acao = input.client_id;
                        for(int i = 0; i < qntJogadores; i++){
                            if(players[i].position.x == jogada.position.x + 1 && players[i].position.y == jogada.position.y)
                                envia_client.tipo = 'n';
                            envia_client.todosJogadores[i] = players[i];
                        }
                        broadcast(&envia_client, sizeof(PROTOCOLO_JOGO));
                    }
                    else if(jogada.team == 1 && mapa[jogada.position.x+1] [ jogada.position.y] == 'Q'  || jogada.team == 2 && mapa[jogada.position.x+1] [ jogada.position.y] == 'A'){
                        players[input.client_id].comBandeira = 1;
                        envia_client.acao = 'b';
                        envia_client.tipo = 'G';
                        envia_client.ganhou = 's';
                        envia_client.id_acao = input.client_id;
                        for(int i = 0; i < qntJogadores; i++){
                            if(players[i].position.x == jogada.position.x + 1 && players[i].position.y == jogada.position.y)
                                envia_client.tipo = 'n';
                            envia_client.todosJogadores[i] = players[i];
                        }
                        broadcast(&envia_client, sizeof(PROTOCOLO_JOGO));
                    }
                    else{
                        players[input.client_id].estaCongelado = 0;
                        envia_client.acao = 'b';
                        envia_client.tipo = 'G';
                        envia_client.ganhou = 'n';
                        envia_client.id_acao = input.client_id;
                        for(int i = 0; i < qntJogadores; i++){
                            if(players[i].position.x == jogada.position.x + 1 && players[i].position.y == jogada.position.y)
                                envia_client.tipo = 'n';
                            envia_client.todosJogadores[i] = players[i];
                        }
                        broadcast(&envia_client, sizeof(PROTOCOLO_JOGO));
                    } 
                }
                else if(jogada.tipo == ANDAR_ESQUERDA && mapa[jogada.position.x] [ jogada.position.y - 1] != 'N'){
                    if(jogada.team == 1 && mapa[jogada.position.x] [ jogada.position.y-1] == 'R' || jogada.team == 2 && mapa[jogada.position.x] [ jogada.position.y - 1] == 'B'){
                        players[input.client_id].estaCongelado = 1;
                        envia_client.acao = 'e';
                        envia_client.tipo = 'G';
                        envia_client.ganhou = 'n';
                        envia_client.id_acao = input.client_id;
                        for(int i = 0; i < qntJogadores; i++){
                            if(players[i].position.x == jogada.position.x && players[i].position.y == jogada.position.y - 1)
                                envia_client.tipo = 'n';
                            envia_client.todosJogadores[i] = players[i];
                        }
                        broadcast(&envia_client, sizeof(PROTOCOLO_JOGO));
                    }
                    else if(jogada.team == 1 && mapa[jogada.position.x] [ jogada.position.y-1] == 'Q'  || jogada.team == 2 && mapa[jogada.position.x] [ jogada.position.y-1] == 'A'){
                        players[input.client_id].comBandeira = 1;
                        envia_client.acao = 'e';
                        envia_client.tipo = 'G';
                        envia_client.ganhou = 's';
                        envia_client.id_acao = input.client_id;
                        for(int i = 0; i < qntJogadores; i++){
                            if(players[i].position.x == jogada.position.x && players[i].position.y == jogada.position.y - 1)
                                envia_client.tipo = 'n';
                            envia_client.todosJogadores[i] = players[i];
                        }
                        broadcast(&envia_client, sizeof(PROTOCOLO_JOGO));
                    }
                    else{
                        players[input.client_id].estaCongelado = 0;
                        envia_client.acao = 'e';
                        envia_client.tipo = 'G';
                        envia_client.ganhou = 'n';
                        envia_client.id_acao = input.client_id;
                        for(int i = 0; i < qntJogadores; i++){
                            if(players[i].position.x == jogada.position.x && players[i].position.y == jogada.position.y - 1)
                                envia_client.tipo = 'n';
                            envia_client.todosJogadores[i] = players[i];
                        }
                        broadcast(&envia_client, sizeof(PROTOCOLO_JOGO));
                    } 
                }
                else if(jogada.tipo == ANDAR_DIREITA && mapa[jogada.position.x] [ jogada.position.y + 1] != 'N'){
                    if(jogada.team == 1 && mapa[jogada.position.x] [ jogada.position.y+1] == 'R' || jogada.team == 2 && mapa[jogada.position.x] [ jogada.position.y + 1] == 'B'){
                        players[input.client_id].estaCongelado = 1;
                        envia_client.acao = 'd';
                        envia_client.tipo = 'G';
                        envia_client.ganhou = 'n';
                        envia_client.id_acao = input.client_id;
                        for(int i = 0; i < qntJogadores; i++){
                            if(players[i].position.x == jogada.position.x && players[i].position.y == jogada.position.y + 1)
                                envia_client.tipo = 'n';
                            envia_client.todosJogadores[i] = players[i];
                        }
                        broadcast(&envia_client, sizeof(PROTOCOLO_JOGO));
                    }
                    else if(jogada.team == 1 && mapa[jogada.position.x] [ jogada.position.y+1] == 'Q'  || jogada.team == 2 && mapa[jogada.position.x] [ jogada.position.y+1] == 'A'){
                        players[input.client_id].comBandeira = 1;
                        envia_client.acao = 'd';
                        envia_client.tipo = 'G';
                        envia_client.ganhou = 's';
                        envia_client.id_acao = input.client_id;
                        for(int i = 0; i < qntJogadores; i++){
                            if(players[i].position.x == jogada.position.x && players[i].position.y == jogada.position.y + 1)
                                envia_client.tipo = 'n';
                            envia_client.todosJogadores[i] = players[i];
                        }
                        broadcast(&envia_client, sizeof(PROTOCOLO_JOGO));
                    }
                    else{
                        players[input.client_id].estaCongelado = 0;
                        envia_client.acao = 'd';
                        envia_client.tipo = 'G';
                        envia_client.ganhou = 'n';
                        envia_client.id_acao = input.client_id;
                        for(int i = 0; i < qntJogadores; i++){
                            if(players[i].position.x == jogada.position.x && players[i].position.y == jogada.position.y + 1)
                                envia_client.tipo = 'n';
                            envia_client.todosJogadores[i] = players[i];
                        }
                        broadcast(&envia_client, sizeof(PROTOCOLO_JOGO));
                    } 
                }
                else if(jogada.tipo == CONGELA){
                    for(i = 0; i < qntJogadores; i++){
                        envia_client.todosJogadores[i] = players[i];
                    }
                    if(jogada.congelamentos > 0){
                        flagCongelamento = 0;
                        for(i = 0; i < qntJogadores; i++){
                            if(players[i].position.x == players[input.client_id].position.x + 1){
                                players[i].estaCongelado = 1;
                                players[input.client_id].congelamentos--;
                                players[input.client_id].congelou = 1;
                                envia_client.acao = 'n';
                                envia_client.tipo = 'G';
                                envia_client.ganhou = 'n';
                                envia_client.id_acao = i;
                                envia_client.todosJogadores[i] = players[i];
                                envia_client.todosJogadores[input.client_id] = players[input.client_id];
                                broadcast(&envia_client, sizeof(PROTOCOLO_JOGO));
                                flagCongelamento = 1;
                            }
                            else if(players[i].position.x == players[input.client_id].position.x - 1){
                                players[i].estaCongelado = 1;
                                players[input.client_id].congelamentos--;
                                players[input.client_id].congelou = 1;
                                envia_client.acao = 'n';
                                envia_client.tipo = 'G';
                                envia_client.ganhou = 'n';
                                envia_client.id_acao = i;
                                envia_client.todosJogadores[i] = players[i];
                                envia_client.todosJogadores[input.client_id] = players[input.client_id];
                                broadcast(&envia_client, sizeof(PROTOCOLO_JOGO));
                                flagCongelamento = 1;
                                
                            }
                            else if(players[i].position.y == players[input.client_id].position.y + 1){
                                players[i].estaCongelado = 1;
                                players[input.client_id].congelamentos--;
                                players[input.client_id].congelou = 1;
                                envia_client.acao = 'n';
                                envia_client.tipo = 'G';
                                envia_client.ganhou = 'n';
                                envia_client.id_acao = i;
                                envia_client.todosJogadores[i] = players[i];
                                envia_client.todosJogadores[input.client_id] = players[input.client_id];
                                broadcast(&envia_client, sizeof(PROTOCOLO_JOGO));
                                flagCongelamento = 1;
                            }
                            else if(players[i].position.y == players[input.client_id].position.y - 1){
                                players[i].estaCongelado = 1;
                                players[input.client_id].congelamentos--;
                                players[input.client_id].congelou = 1;
                                envia_client.acao = 'n';
                                envia_client.tipo = 'G';
                                envia_client.ganhou = 'n';
                                envia_client.id_acao = i;
                                envia_client.todosJogadores[i] = players[i];
                                envia_client.todosJogadores[input.client_id] = players[input.client_id];
                                broadcast(&envia_client, sizeof(PROTOCOLO_JOGO));
                                flagCongelamento = 1;
                            }
                        }
                        if(!flagCongelamento){
                            players[i].estaCongelado = 0;
                            players[input.client_id].congelamentos--;
                            players[input.client_id].congelou = 1;
                            envia_client.acao = 'n';
                            envia_client.tipo = 'G';
                            envia_client.ganhou = 'n';
                            envia_client.id_acao = i;
                            envia_client.todosJogadores[i] = players[i];
                            envia_client.todosJogadores[input.client_id] = players[input.client_id];
                            broadcast(&envia_client, sizeof(PROTOCOLO_JOGO));
                        }
                    }
                    else{
                        players[i].estaCongelado = 0;
                        players[input.client_id].congelou = 0;
                        envia_client.acao = 'n';
                        envia_client.tipo = 'G';
                        envia_client.ganhou = 'n';
                        envia_client.id_acao = i;
                        envia_client.todosJogadores[i] = players[i];
                        envia_client.todosJogadores[input.client_id] = players[input.client_id];
                        broadcast(&envia_client, sizeof(PROTOCOLO_JOGO));
                    }                    
                }
                else if(jogada.tipo == DESCONGELA && jogada.estaCongelado == 0){
                    players[input.client_id].estaCongelado = 0;
                    envia_client.acao = 'n';
                    envia_client.tipo = 'G';
                    envia_client.ganhou = 'n';
                    envia_client.id_acao = input.client_id;
                    for(int i = 0; i < qntJogadores; i++){
                        envia_client.todosJogadores[i] = players[i];
                    }
                    broadcast(&envia_client, sizeof(PROTOCOLO_JOGO));

                }
                else if(jogada.tipo == BOTARTRAPS && mapa[jogada.position.x] [jogada.position.y] != 'S'){
                    if(mapa[jogada.position.x][ jogada.position.y] != 'Z'){
                        if(jogada.team == 1 && mapa[jogada.position.x] [ jogada.position.y] != 'B' && jogada.armadilhas > 0){
                            for(int i = 0; i < qntJogadores; i++){
                                envia_client.todosJogadores[i] = players[i];
                            }
                            envia_client.acao = 't';
                            envia_client.tipo = 'G';
                            envia_client.id_acao = input.client_id;
                            envia_client.traps[tp].posiT.x= jogada.position.x;
                            envia_client.traps[tp].posiT.y= jogada.position.y;
                            envia_client.traps[tp].team = 1;
                            envia_client.tp++;
                            tp++;
                            mapa[jogada.position.x][jogada.position.y] = 'B';
                            broadcast(&envia_client, sizeof(PROTOCOLO_JOGO));
                        }else if(jogada.team == 2 && mapa[jogada.position.x] [ jogada.position.y] != 'R' && jogada.armadilhas > 0){
                            for(int i = 0; i < qntJogadores; i++){
                                envia_client.todosJogadores[i] = players[i];
                            }
                            envia_client.acao = 't';
                            envia_client.tipo = 'G';
                            envia_client.id_acao = input.client_id;
                            envia_client.traps[tp].posiT.x = jogada.position.x;
                            envia_client.traps[tp].posiT.y = jogada.position.y;
                            envia_client.traps[tp].team = 2;
                            envia_client.tp++;
                            tp++;
                            mapa[jogada.position.x] [ jogada.position.y] = 'R';
                            broadcast(&envia_client, sizeof(PROTOCOLO_JOGO));
                        }
                    }
                }    
                else if(jogada.tipo == ENDGAME){
                    envia_client.acao = 'n';
                    envia_client.tipo = 'G';
                    envia_client.id_acao = input.client_id;
                    disconnectClient(input.client_id);
                    printf("%s disconnected id = %d\n", players[input.client_id].name, input.client_id);
                    for(i=0;i<qntJogadores;i++){
                            envia_client.todosJogadores[i] = players[i];
                    }
                    envia_client.todosJogadores[input.client_id].id = -1;
                    broadcast(&envia_client, sizeof(PROTOCOLO_JOGO));
                }
                else{
                    envia_client.acao = 'n';                 
                }
            }            
        }
    }
}
