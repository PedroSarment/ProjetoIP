#include "server.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

#define MSG_MAX_SIZE 350
#define BUFFER_SIZE (MSG_MAX_SIZE + 100)
#define LOGIN_MAX_SIZE 13
#define MAX_CLIENTS 10
#define QTD_TIMES 2
#define MAX_CLIENTS_TIMES 5
#define X_MAX 500
#define Y_MAX 500
#define N_ARMADILHAS 20
#define TEMPO_LIMITE 120
#define direita 
#define esquerda
#define cima 
#define baixo


typedef struct{
  int x, y;
}Position;

typedef struct{
  char name[LOGIN_MAX_SIZE];
  int helmet, team, armadilhas;
  int id;
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
  int teclado,i;
  int time_1=0,time_2=0; //quantidade de indivíduos nas equipes
  int armadilhas_1,armadilhas_2; //quantidade de armadilha por pessoa
  char mapa [X_MAX][Y_MAX];

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
        players[jogadores].team=1;
        players[jogadores].position.x=0;
        players[jogadores].position.y=0;
        players[jogadores].helmet=capacete;
        players[jogadores].id = id;
        time_1++;
      }
      else{
        strcpy(players[jogadores].name, client_names[id]);
        players[jogadores].team=2;
        players[jogadores].position.x= X_MAX;
        players[jogadores].position.y= Y_MAX;
        players[jogadores].helmet=capacete;
        players[jogadores].id = id;
        time_2++;
      }
      jogadores++;
    }
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
    if(jogadores.team == 1)
      jogadores.armadilhas = armadilhas_1;
    else
      jogadores.armadilhas = armadilhas_2;

  }

  tempoInicio = al_get_time();
  while(al_get_time() - tempoInicio < TEMPO_LIMITE && !fim){
    input = recvMsg(&teclado);

    for(i=0;i<jogadores;i++){ 
      if(players[i].id == input.client_id){
        
        if(teclado == cima){

          if(players[i].position.y !=0){ // espaco livre
            if(mapa[players[i].position.x][players[i].position.y+1] == '0'){
              

            }

        } 

      }
      
    }
    
  }

  





}