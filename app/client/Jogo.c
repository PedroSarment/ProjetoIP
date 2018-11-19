#include <stdio.h>
#include <string.h>
#include "client.h"
#include "ACore.h"
#include <ctype.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_native_dialog.h>
//#include "server.h"
//#include "client.h"

#define MSG_MAX_SIZE 350
#define BUFFER_SIZE (MSG_MAX_SIZE + 100)
#define LOGIN_MAX_SIZE 13
#define HIST_MAX_SIZE 200

#define MAX_LOG_SIZE 17

#define LARGURA_TELA 1280
#define ALTURA_TELA 720

//gcc Jogo.c -lm -lallegro -lallegro_image -lallegro_primitives -lallegro_font -lallegro_ttf -lallegro_audio -lallegro_acodec
typedef struct{
    int helmet;
    int team;
    int traps;
    int freezes;
    char nick[13];
    int x,y;

}player;



ALLEGRO_FONT *fonte = NULL;
ALLEGRO_DISPLAY *janela = NULL;
ALLEGRO_AUDIO_STREAM *musica_fundo = NULL;
ALLEGRO_BITMAP *personagem = NULL;
ALLEGRO_BITMAP *capacetes = NULL;
ALLEGRO_BITMAP *personagemm = NULL;
ALLEGRO_BITMAP *mapa = NULL;
ALLEGRO_EVENT_QUEUE *fila_eventos = NULL;
ALLEGRO_BITMAP *logo = NULL;
ALLEGRO_KEYBOARD *teclado = NULL;

DADOS_LOBBY msg_chat;

void error_msg(char *text){
	ALLEGRO_DISPLAY *error = NULL;
    error = al_create_display(400, 300);

    al_set_window_position(error,860,600);
}

void runChat() {
  char str_buffer[BUFFER_SIZE], type_buffer[MSG_MAX_SIZE] = {0};
  char msg_history[HIST_MAX_SIZE][MSG_MAX_SIZE] = {{0}};
  int type_pointer = 0;
  
  while (1) {
    // LER UMA TECLA DIGITADA
    char ch = getch();
    if (ch == '\n') {
      type_buffer[type_pointer++] = '\0';
      int ret = sendMsgToServer((void *)type_buffer, type_pointer);
      if (ret == SERVER_DISCONNECTED) {
        return;
      }
      type_pointer = 0;
      type_buffer[type_pointer] = '\0';
    } else if (ch == 127 || ch == 8) {
      if (type_pointer > 0) {
        --type_pointer;
        type_buffer[type_pointer] = '\0';
      }
    } else if (ch != NO_KEY_PRESSED && type_pointer + 1 < MSG_MAX_SIZE) {
      type_buffer[type_pointer++] = ch;
      type_buffer[type_pointer] = '\0';
      
    }

    // LER UMA MENSAGEM DO SERVIDOR
    int ret = recvMsgFromServer(str_buffer, DONT_WAIT);
    if (ret == SERVER_DISCONNECTED) {
      return;
    } else if (ret != NO_MESSAGE) {
      int i;
      for (i = 0; i < HIST_MAX_SIZE - 1; ++i) {
        strcpy(msg_history[i], msg_history[i + 1]);
      }
      strcpy(msg_history[HIST_MAX_SIZE - 1], str_buffer);
    }

    // PRINTAR NOVO ESTADO DO CHAT
    system("clear");
    int i;
    for (i = 0; i < HIST_MAX_SIZE; ++i) {
      printf("%s\n", msg_history[i]);
    }
    printf("\nYour message: %s\n", type_buffer);
  }
}

void defineNick(ALLEGRO_FONT *fonte){
    char type_buffer[LOGIN_MAX_SIZE+1] = {0};
    int type_pointer = 0;

    al_draw_text(fonte, al_map_rgb(255, 0, 0), 10, 10, ALLEGRO_ALIGN_LEFT, "Nick: ");
    while (1) {
        // LER UMA TECLA DIGITADA
        char ch = getch();
        if (ch == '\n') {
            type_buffer[type_pointer++] = '\0';
            int ret = sendMsgToServer((void *)type_buffer, type_pointer);
        if (ret == SERVER_DISCONNECTED) {
            return;
        }
        type_pointer = 0;
        type_buffer[type_pointer] = '\0';

        } 
        else if (ch == 127 || ch == 8) {
            if (type_pointer > 0) {
                --type_pointer;
                type_buffer[type_pointer] = '\0';
            }
        } 
        else if (ch != NO_KEY_PRESSED && type_pointer + 1 < MSG_MAX_SIZE) {
            type_buffer[type_pointer++] = ch;
            type_buffer[type_pointer] = '\0';
        }
        al_draw_text(fonte, al_map_rgb(255, 0, 0), 10, 10, 7, type_buffer);
    }
}

void selectHelmet(ALLEGRO_FONT *fonte){
    capacetes = al_load_bitmap("/app/Resources/capacetes/chapeusfinalizados.png");

    int capacete;

    al_draw_text(fonte, al_map_rgb(0, 255, 0), LARGURA_TELA / 2, 90, ALLEGRO_ALIGN_CENTRE, "Selecione o Capacete");
    al_draw_bitmap(capacetes, 0,0,0);

    while(!al_event_queue_is_empty(fila_eventos)){
        ALLEGRO_EVENT evento;
        int tecla;

        al_wait_for_event(fila_eventos,&evento);
        if (evento.type == ALLEGRO_EVENT_KEY_CHAR){
            //verifica qual tecla foi pressionada
            switch(evento.keyboard.keycode){
                //seta para cima
                case ALLEGRO_KEY_1:
                    tecla = 1;
                    break;
                //Baixo
                case ALLEGRO_KEY_2:
                    tecla = 2;
                    break;
                //Esquerda
                case ALLEGRO_KEY_3:
                    tecla = 3;
                    break;
                //Direita.
                case ALLEGRO_KEY_4:
                    tecla = 4;
                    break;
                //esc. sair=1 faz com que o programa saia do loop principal
                case ALLEGRO_KEY_PAD_1:
                    tecla=1;
                    break;
                case ALLEGRO_KEY_PAD_2:
                    tecla=2;
                    break;
                case ALLEGRO_KEY_PAD_3:
                    tecla=3;
                    break;
                case ALLEGRO_KEY_PAD_4:
                    tecla=4;
                    break;
            }
        }    
        int ret = sendMsgToServer((int *)&tecla, (int)sizeof(int));
        if (ret == SERVER_DISCONNECTED) {
            return;
        }
    }
    al_destroy_font(fonte);
    al_destroy_bitmap(capacetes);
}

int iniciar(){
    if(!al_init()){
        printf("Tela nao foi iniciada.");
        return 0;
    }
    if(!al_install_audio()){
        printf("Falha ao iniciar o audio.");
        return 0;
    }
    if(!al_init_acodec_addon()){
        printf("Falha ao iniciar Codec de Audio.");
        return 0;
    }
    fonte = al_load_font("arial.ttf", 48, 0);
    if(!al_install_keyboard()){
        printf("Falha ao instalar o teclado.");
        return 0;
    }
    if(!al_init_image_addon()){
        printf("Nao foi possivel iniciar o addon de imagem.");
        return 0;
    }
    if(!al_reserve_samples(5)){
        printf("Nao foi possivel reservar os audios.");
        return 0;
    }
  
    musica_fundo = al_load_audio_stream("/app/Resources/Musics/Musica_fundo.ogg",4,1024);
    if(!musica_fundo){
        error_msg("Musica nao foi carregada.");
        return 0;
    }
    al_attach_audio_stream_to_mixer(musica_fundo, al_get_default_mixer());
    al_set_audio_stream_playmode(musica_fundo,ALLEGRO_PLAYMODE_LOOP);
    janela = al_create_display(1280,720);
    if(!janela){
        error_msg("Falha ao iniciar a janela");
        al_destroy_audio_stream(musica_fundo);
        return 0;
    }
    al_set_window_title(janela,"Barra Bandeira");
    fila_eventos = al_create_event_queue();
    if(!fila_eventos){
        error_msg("Erro ao criar fila de eventos.");
        al_destroy_display(janela);
        return 0;
    }
    al_clear_to_color(al_map_rgb(255,255,255));
    personagem = al_load_bitmap("/app/Resources/Characters/Personagem(1).png");
    personagemm = al_load_bitmap("/app/Resources/Characters/Personagem_C3R.png");
    logo = al_load_bitmap("/app/Resources/Characters/index.png");
    mapa = al_load_bitmap("/app/Resources/Characters/mapa.jpeg");

    if(!personagem){
        printf("Nao foi possivel carregar o personagem.");
        al_destroy_audio_stream(musica_fundo);
        return 0;
    }
    al_draw_bitmap(logo,0,0,0);
    al_flip_display();
    al_rest(10.0);
    al_destroy_bitmap(logo);
    al_clear_to_color(al_map_rgb(0,0,0));
    al_draw_bitmap(mapa,0,0,0);
    al_flip_display();
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    return 1;
}

int lobby(ALLEGRO_FONT *fonte, char *nick){
    int state=0;

    defineNick(fonte);
    selectHelmet(fonte);
    runChat();
    
    recvMsgFromServer(&state, DONT_WAIT);

    return state;
}

int main(){
    int state;

    state = lobby();
    iniciar();
    al_flip_display();
    int sair = 0,tecla = 0;
    int current_x = 0,current_y = 0;
    al_draw_bitmap(personagem,current_x,current_y,0);
    al_flip_display();

    while(!sair){
            //enquanto esc nao for pressionado
            while(!al_event_queue_is_empty(fila_eventos)){
                ALLEGRO_EVENT evento;
                al_wait_for_event(fila_eventos,&evento);
                if (evento.type == ALLEGRO_EVENT_KEY_CHAR){
                    //verifica qual tecla foi pressionada
                    switch(evento.keyboard.keycode){
                        //seta para cima
                        case ALLEGRO_KEY_UP:
                            tecla = 1;
                            break;
                        //Baixo
                        case ALLEGRO_KEY_DOWN:
                            tecla = 2;
                            break;
                        //Esquerda
                        case ALLEGRO_KEY_LEFT:
                            tecla = 3;
                            break;
                        //Direita.
                        case ALLEGRO_KEY_RIGHT:
                            tecla = 4;
                            break;
                        //esc. sair=1 faz com que o programa saia do loop principal
                        case ALLEGRO_KEY_J:
                            tecla = 5;
                            break;
                        case ALLEGRO_KEY_ESCAPE:
                            sair = 1;
                    }
                    
                }
                if(tecla){
                    switch(tecla){
                        case 1:
                            al_draw_bitmap(mapa,0,0,0);
                            current_y-=4;
                            al_draw_bitmap(personagem,current_x,current_y,0);
                            break;
                        case 2:
                            al_draw_bitmap(mapa,0,0,0);
                            current_y+=4;
                            al_draw_bitmap(personagem,current_x,current_y,0);
                            break;
                        case 3:
                            al_draw_bitmap(mapa,0,0,0);
                            current_x-=4;
                            al_draw_bitmap(personagem,current_x,current_y,0);
                            break;
                        case 4:
                            al_draw_bitmap(mapa,0,0,0);
                            current_x+=4;
                            al_draw_bitmap(personagem,current_x,current_y,0);
                            break;
                    }
                    tecla = 0;
                    al_flip_display();
                }
                
            }

        }
    al_destroy_display(janela);
    al_destroy_audio_stream(musica_fundo);
    al_destroy_event_queue(fila_eventos);
    return 0;
}