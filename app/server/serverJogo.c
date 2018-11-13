#include "server.h"
#include <stdio.h>
#include <string.h>

#define MSG_MAX_SIZE 350
#define BUFFER_SIZE (MSG_MAX_SIZE + 100)
#define LOGIN_MAX_SIZE 13
#define MAX_CLIENTS 10
#define QTD_TIMES 2
#define MAX_CLIENTS_TIMES 5
#define X_MAX 500
#define Y_MAX 500
#define N_ARMADILHAS 20

typedef struct{
  int x, y;
}Position;

typedef struct{
  char name[LOGIN_MAX_SIZE];
  int helmet, team, armadilhas;
  Position position;
}Player;

int main() {
  char client_names[MAX_CLIENTS][LOGIN_MAX_SIZE];
  char str_buffer[BUFFER_SIZE], aux_buffer[BUFFER_SIZE];
  Player players[10];
  char helmetChoice[] = ""; 
  int jogadores=0, capacete, ready=0;

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
      }
      else{
        strcpy(players[jogadores].name, client_names[id]);
        players[jogadores].team=2;
        players[jogadores].position.x= X_MAX;
        players[jogadores].position.y= Y_MAX;
        players[jogadores].helmet=capacete;
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

  





}
