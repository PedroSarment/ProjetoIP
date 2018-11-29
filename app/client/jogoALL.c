#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_native_dialog.h>
#include "client.h"
#include "ACore.h"

#define LARGURA_TELA 1280
#define ALTURA_TELA 720

#define MSG_MAX_SIZE 350
#define BUFFER_SIZE (MSG_MAX_SIZE + 100)
#define LOGIN_MAX_SIZE 13
#define HIST_MAX_SIZE 200

#define MAX_LOG_SIZE 17

//gcc Jogo.c -lm -lallegro -lallegro_image -lallegro_primitives -lallegro_font -lallegro_ttf -lallegro_audio -lallegro_acodec

ALLEGRO_FONT *fonte = NULL;
ALLEGRO_DISPLAY *janela = NULL;
ALLEGRO_AUDIO_STREAM *musica_fundo = NULL;
ALLEGRO_BITMAP *personagem = NULL;
ALLEGRO_BITMAP *personagemm = NULL;
ALLEGRO_BITMAP *mapa = NULL;
ALLEGRO_EVENT_QUEUE *fila_eventos = NULL;
ALLEGRO_BITMAP *logo = NULL;
ALLEGRO_KEYBOARD *teclado = NULL;
ALLEGRO_EVENT_QUEUE *eventsQueue2;

PROTOCOLO_INICIAL sendPlayer, rcvPlayer;
Player jogador;
char loginP[14], ip[30];
int sair = 0;
bool login = true;
bool ipAd = true;
int current_x = 0, current_y = 0;
int checkType = 1;

void error_msg(char *text){
	ALLEGRO_DISPLAY *error = NULL;
    error = al_create_display(400, 300);
    al_set_window_position(error,860,600);
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
    al_init_font_addon();
    if(!al_init_ttf_addon()){ 
        printf("Falha ao iniciar o ttf addon.");
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
    musica_fundo = al_load_audio_stream("/home/CIN/ersa/Desktop/projetoip/app/Resources/Musics/Musica_fundo.ogg",4,1024);
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
    eventsQueue2 = al_create_event_queue();

    if(!fila_eventos){
        error_msg("Erro ao criar fila de eventos.");
        al_destroy_display(janela);
        return 0;
    }
    if(!eventsQueue2){
        error_msg("Erro ao criar fila de eventos.");
        al_destroy_display(janela);
        return 0;
    }
    if (!al_install_mouse())
    {
        printf("Falha ao inicializar o mouse.\n");
        al_destroy_display(janela);
        return 0;
    }
    if (!al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT))
    {
        printf("Falha ao atribuir ponteiro do mouse.\n");
        al_destroy_display(janela);
        return 0;
    }
    al_clear_to_color(al_map_rgb(255,255,255));
    personagem = al_load_bitmap("/home/CIN/ersa/Desktop/projetoip/app/Resources/Characters/Personagem(1).png");
    personagemm = al_load_bitmap("/home/CIN/ersa/Desktop/projetoip/app/Resources/Characters/Personagem_C3R.png");
    logo = al_load_bitmap("/home/CIN/ersa/Desktop/projetoip/app/Resources/Characters/index.png");
    mapa = al_load_bitmap("/home/CIN/ersa/Desktop/projetoip/app/Resources/Characters/mapa.jpeg");
    fonte = al_load_font("/home/CIN/ersa/Desktop/projetoip/app/Resources/Fontes/OldLondon.ttf", 24, 0);

    if(!personagem){
        printf("Nao foi possivel carregar o personagem.");
        al_destroy_audio_stream(musica_fundo);
        return 0;
    }
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    al_register_event_source(fila_eventos, al_get_mouse_event_source());
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    return 1;
}


void startScreen(){
    al_draw_bitmap(logo,0,0,0);
    fonte = al_load_font("/home/CIN/ersa/Desktop/projetoip/app/Resources/Fontes/Minecraft.ttf", 48, 0);
    al_draw_text(fonte, al_map_rgb(0, 0, 0), 640, 600, ALLEGRO_ALIGN_CENTRE, "PRESS START!");
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
                    case ALLEGRO_KEY_SPACE:
                        sair = 1;
                        break;
                }
            }
        }
    }
}

// void menuScreen(){
// }

void runGame(){
    int tecla;
    al_draw_bitmap(mapa,0,0,0);
    al_draw_bitmap(personagem,current_x,current_y,0);
    al_flip_display();
    
    while(!al_event_queue_is_empty(fila_eventos)){
        ALLEGRO_EVENT evento;
        al_wait_for_event(fila_eventos,&evento);
        if (evento.type == ALLEGRO_EVENT_KEY_DOWN){
            //verifica qual tecla foi pressionada
            switch(evento.keyboard.keycode){
                //seta para cima
                case ALLEGRO_KEY_UP:
                    al_draw_bitmap(mapa,0,0,0);
                    current_y-=4;
                    al_draw_bitmap(personagem,current_x,current_y,ALLEGRO_FLIP_HORIZONTAL);
                    break;
                //Baixo
                case ALLEGRO_KEY_DOWN:
                    al_draw_bitmap(mapa,0,0,0);
                    current_y+=4;
                    al_draw_bitmap(personagem,current_x,current_y,0);
                    break;
                //Esquerda
                case ALLEGRO_KEY_LEFT:
                    al_draw_bitmap(mapa,0,0,0);
                    current_x-=4;
                    al_draw_bitmap(personagem,current_x,current_y,0);
                    break;
                //Direita.
                case ALLEGRO_KEY_RIGHT:
                    al_draw_bitmap(mapa,0,0,0);
                    current_x+=4;
                    al_draw_bitmap(personagem,current_x,current_y,0);
                //esc. sair=1 faz com que o programa saia do loop principal
                case ALLEGRO_KEY_J:
                    tecla = 5;
                    break;
                case ALLEGRO_KEY_ESCAPE:
                    sair = 0;
            }
            al_flip_display();
        }
    }
}

void endGame(){
    al_destroy_display(janela);
    al_destroy_audio_stream(musica_fundo);
    al_destroy_event_queue(fila_eventos);
}

void readInputIP(ALLEGRO_EVENT event, char str[], int limit){
    if (event.type == ALLEGRO_EVENT_KEY_CHAR)
    {
        if (strlen(str) <= limit)
        {
            char temp[] = {event.keyboard.unichar, '\0'};
            if (event.keyboard.unichar == ' ')
            {
                strcat(str, temp);
            }
            else if (event.keyboard.unichar >= '!' &&
                     event.keyboard.unichar <= '?')
            {
                strcat(str, temp);
            }
            else if (event.keyboard.unichar >= 'A' &&
                     event.keyboard.unichar <= 'Z')
            {
                strcat(str, temp);
            }
            else if (event.keyboard.unichar >= 'a' &&
                     event.keyboard.unichar <= 'z')
            {
                strcat(str, temp);
            }
        }

        if (event.keyboard.keycode == ALLEGRO_KEY_BACKSPACE && strlen(str) != 0)
        {
            str[strlen(str) - 1] = '\0';
        }
         al_clear_to_color(al_map_rgb(255,255,255));
        //al_draw_bitmap(logo,0,0,0);
        fonte = al_load_font("./app/Resources/Fontes/OldLondon.ttf", 48, 0);
        al_draw_text(fonte, al_map_rgb(0, 0, 0), LARGURA_TELA/2, ALTURA_TELA/3, ALLEGRO_ALIGN_CENTRE, "IP adress: ");
        al_draw_textf(fonte, al_map_rgb(0, 0, 0), LARGURA_TELA/2, ALTURA_TELA/2, ALLEGRO_ALIGN_CENTRE, "%s", ip);
        al_flip_display();
    }
}

void readInput2(ALLEGRO_EVENT event, char str[], int limit){
    if (event.type == ALLEGRO_EVENT_KEY_CHAR)
    {
        if (strlen(str) <= limit)
        {
            char temp[] = {event.keyboard.unichar, '\0'};
            if (event.keyboard.unichar == ' ')
            {
                strcat(str, temp);
            }
            else if (event.keyboard.unichar >= '!' &&
                     event.keyboard.unichar <= '?')
            {
                strcat(str, temp);
            }
            else if (event.keyboard.unichar >= 'A' &&
                     event.keyboard.unichar <= 'Z')
            {
                strcat(str, temp);
            }
            else if (event.keyboard.unichar >= 'a' &&
                     event.keyboard.unichar <= 'z')
            {
                strcat(str, temp);
            }
        }

        if (event.keyboard.keycode == ALLEGRO_KEY_BACKSPACE && strlen(str) != 0)
        {
            str[strlen(str) - 1] = '\0';
        }
         al_clear_to_color(al_map_rgb(255,255,255));
        //al_draw_bitmap(logo,0,0,0);
        fonte = al_load_font("./app/Resources/Fontes/OldLondon.ttf", 48, 0);
        al_draw_text(fonte, al_map_rgb(0, 0, 0), LARGURA_TELA/2, ALTURA_TELA/3, ALLEGRO_ALIGN_CENTRE, "Login: ");
        al_draw_textf(fonte, al_map_rgb(0, 0, 0), LARGURA_TELA/2, ALTURA_TELA/2, ALLEGRO_ALIGN_CENTRE, "%s", loginP);
        al_flip_display();
    }
}

void readLogin(){
    al_clear_to_color(al_map_rgb(255,255,255));
    //al_draw_bitmap(logo,0,0,0);
    fonte = al_load_font("./app/Resources/Fontes/OldLondon.ttf", 48, 0);
    al_draw_text(fonte, al_map_rgb(0, 0, 0), LARGURA_TELA/2, ALTURA_TELA/3, ALLEGRO_ALIGN_CENTRE, "Login: ");
    al_flip_display();
    ALLEGRO_EVENT_QUEUE *eventsQueue3;
    eventsQueue3 = al_create_event_queue();
    if(!eventsQueue3){
        puts("Erro ao criar fila de eventos.");
        al_destroy_display(janela);
    }

    while(login){
        while(!al_is_event_queue_empty(fila_eventos))
        {
            ALLEGRO_EVENT loginEvent;
            al_wait_for_event(fila_eventos, &loginEvent);
            readInput2(loginEvent, loginP, LOGIN_MAX_SIZE);
            
            

            if (loginEvent.type == ALLEGRO_EVENT_KEY_DOWN)
            {
                switch(loginEvent.keyboard.keycode)
                {
                    case ALLEGRO_KEY_ENTER:
                        if(strlen(loginP) > 1){
                            strcpy(jogador.name, loginP);
                            puts(jogador.name);
                            login = false;
                        }else{
                            al_clear_to_color(al_map_rgb(255,255,255));
                            al_draw_text(fonte, al_map_rgb(0, 0, 0), LARGURA_TELA/2, ALTURA_TELA/3, ALLEGRO_ALIGN_CENTRE, "Login: ");
                            al_draw_textf(fonte, al_map_rgb(0, 0, 0), LARGURA_TELA/2, ALTURA_TELA/2, ALLEGRO_ALIGN_CENTRE, "%s", loginP);
                            al_draw_text(fonte, al_map_rgb(255,0,0), LARGURA_TELA/2, ALTURA_TELA/4, ALLEGRO_ALIGN_CENTRE, "Digite um nick de no minimo 2 caracteres!");
                            al_flip_display();
                            al_rest(2.0);
                        }
                        break;
                    case ALLEGRO_KEY_PAD_ENTER:
                        if(strlen(loginP) > 1){
                            strcpy(jogador.name, loginP);
                            puts(jogador.name);
                            login = false;
                        }else{
                            al_clear_to_color(al_map_rgb(255,255,255));
                            al_draw_text(fonte, al_map_rgb(0, 0, 0), LARGURA_TELA/2, ALTURA_TELA/3, ALLEGRO_ALIGN_CENTRE, "Login: ");
                            al_draw_textf(fonte, al_map_rgb(0, 0, 0), LARGURA_TELA/2, ALTURA_TELA/2, ALLEGRO_ALIGN_CENTRE, "%s", loginP);
                            al_draw_text(fonte, al_map_rgb(255,0,0), LARGURA_TELA/2, ALTURA_TELA/4, ALLEGRO_ALIGN_CENTRE, "Digite um nick de no minimo 2 caracteres!");
                            al_flip_display();
                            al_rest(2.0);
                        }
                        break;
                }
            }

            if(loginEvent.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                login = false;
            }
        }

    }
    
}

void readHelmet(){
    int capacetesAzul, capacetesVerm, capacetes, capaCheck=1;
    int foi=1, capaEscolhido;
    
    capacetesAzul = al_load_bitmap("./app/Resources/capacetes/chapeusfinalizados.png");
    capacetesVerm = al_load_bitmap("./app/Resources/capacetes/chapeusfinalizados.png");

    if(jogador.team==1) capacetes = capacetesAzul;
    else capacetes = capacetesVerm;

    al_clear_to_color(al_map_rgb(255,255,255));
    //al_draw_bitmap(logo,0,0,0);
    fonte = al_load_font("./app/Resources/Fontes/OldLondon.ttf", 32, 0);
    al_draw_text(fonte, al_map_rgb(0, 0, 0), LARGURA_TELA/2, ALTURA_TELA/6, ALLEGRO_ALIGN_CENTRE, "Escolha seu capacete");
    al_draw_bitmap(capacetes, 0,0,0);
    al_flip_display();

    while(capaCheck){
        while(!al_is_event_queue_empty(fila_eventos)){
            al_draw_text(fonte, al_map_rgb(0, 0, 0), LARGURA_TELA/2, ALTURA_TELA/6, ALLEGRO_ALIGN_CENTRE, "Escolha seu capacete");           
            al_draw_bitmap(capacetes, 0,0,0);
            al_flip_display();
            ALLEGRO_EVENT evento;
            int tecla;

            al_wait_for_event(fila_eventos,&evento);
            if (evento.type == ALLEGRO_EVENT_KEY_DOWN){
                //verifica qual tecla foi pressionada
                switch(evento.keyboard.keycode){
                    
                    case ALLEGRO_KEY_1:
                        tecla = 1;
                        foi=0;
                        break;
                    
                    case ALLEGRO_KEY_2:
                        tecla = 2;
                        foi=0;
                        break;
                    
                    case ALLEGRO_KEY_3:
                        tecla = 3;
                        foi=0;
                        break;
                    
                    case ALLEGRO_KEY_4:
                        tecla = 4;
                        foi=0;
                        break;
                    //esc. sair=1 faz com que o programa saia do loop principal
                    case ALLEGRO_KEY_PAD_1:
                        tecla=1;
                        foi=0;
                        break;
                    case ALLEGRO_KEY_PAD_2:
                        tecla=2;
                        foi=0;
                        break;
                    case ALLEGRO_KEY_PAD_3:
                        tecla=3;
                        foi=0;
                        break;
                    case ALLEGRO_KEY_PAD_4:
                        tecla=4;
                        foi=0;
                        break;
                }
            }
            if(!foi){
                al_draw_text(fonte, al_map_rgb(0, 0, 0), LARGURA_TELA/2, ALTURA_TELA/6, ALLEGRO_ALIGN_CENTRE, "Tem certeza?");            
                al_draw_text(fonte, al_map_rgb(0, 0, 0), LARGURA_TELA/2, ALTURA_TELA/5, ALLEGRO_ALIGN_CENTRE, "1-Sim     2-Nao");            
                al_draw_bitmap(capacetes, 0,0,0);
                al_flip_display();
                ALLEGRO_EVENT evento1;
                

                al_wait_for_event(fila_eventos,&evento1);
                if (evento1.type == ALLEGRO_EVENT_KEY_DOWN){
                    //verifica qual tecla foi pressionada
                    switch(evento1.keyboard.keycode){
                        
                        case ALLEGRO_KEY_1:                      
                            foi=1;
                            capaCheck=0;
                            break;
                        
                        case ALLEGRO_KEY_2:                           
                            foi=1;
                            break;
                    
                        case ALLEGRO_KEY_PAD_1:                           
                            foi=0;
                            break;
                        case ALLEGRO_KEY_PAD_2:
                            foi=0;
                            break;
                    
                    }
                }
            }        
            capaEscolhido=tecla;    
        }
    }
    jogador.helmet=capaEscolhido;
}

void readIP(){
    enum conn_ret_t serverConnection;
    al_clear_to_color(al_map_rgb(255,255,255));
    //al_draw_bitmap(logo,0,0,0);
    fonte = al_load_font("./app/Resources/Fontes/OldLondon.ttf", 48, 0);
    al_draw_text(fonte, al_map_rgb(0, 0, 0), LARGURA_TELA/2, ALTURA_TELA/3, ALLEGRO_ALIGN_CENTRE, "IP adress: ");
    al_flip_display();

    while(ipAd){
        while(!al_is_event_queue_empty(fila_eventos))
        {
            ALLEGRO_EVENT ipEvent;
            al_wait_for_event(fila_eventos, &ipEvent);
            readInputIP(ipEvent, ip, LOGIN_MAX_SIZE);
            
            

            if (ipEvent.type == ALLEGRO_EVENT_KEY_DOWN)
            {
                switch(ipEvent.keyboard.keycode)
                {
                    case ALLEGRO_KEY_ENTER:
                        puts(ip);
                        ipAd = false;
                        break;
                    case ALLEGRO_KEY_PAD_ENTER:
                        puts(ip);
                        ipAd = false;
                        break;
                }
            }

            if(ipEvent.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                ipAd = false;
            }
        }
        serverConnection = connectToServer(ip);
        if(serverConnection == SERVER_UP){
            sendPlayer.jogador = jogador;
            sendPlayer.tipo = INICIAL;
            int ret = sendMsgToServer((PROTOCOLO_INICIAL *) &sendPlayer, sizeof(PROTOCOLO_INICIAL));
            al_clear_to_color(al_map_rgb(255,255,255));
            al_draw_text(fonte, al_map_rgb(0, 100 , 200), LARGURA_TELA/2, ALTURA_TELA/3, ALLEGRO_ALIGN_LEFT, "Servidor conectado!");
            al_flip_display();
            while(checkType){
                recvMsgFromServer((PROTOCOLO_INICIAL *) &rcvPlayer, WAIT_FOR_IT);
                if(rcvPlayer.tipo==INICIAL){
                    checkType=0;
                    jogador = rcvPlayer.jogador;
                } 
            }
            if (ret == SERVER_DISCONNECTED){
                return;
            }
        }
        else if(serverConnection == SERVER_DOWN){
            al_clear_to_color(al_map_rgb(255,255,255));
            al_draw_text(fonte, al_map_rgb(255,0,0), LARGURA_TELA/2, ALTURA_TELA/3, ALLEGRO_ALIGN_LEFT, "Servidor fora do ar, tente novamente");
            al_flip_display();
            rest(2.0);
            ipAd=true;
            strcpy(ip, "");
        }
        else if(serverConnection == SERVER_FULL){
            al_clear_to_color(al_map_rgb(255,255,255));
            al_draw_text(fonte, al_map_rgb(255,0,0), LARGURA_TELA/2, ALTURA_TELA/3, ALLEGRO_ALIGN_LEFT, "Servidor cheio!");
            al_flip_display();
            rest(2.0);        
        }
        else if(serverConnection == SERVER_CLOSED){
            al_clear_to_color(al_map_rgb(255,255,255));
            al_draw_text(fonte, al_map_rgb(255,0,0), LARGURA_TELA/2, ALTURA_TELA/3, ALLEGRO_ALIGN_LEFT, "Servidor fechado!");
            al_flip_display();
            rest(2.0);
        }
        else if(serverConnection == SERVER_TIMEOUT){
            al_clear_to_color(al_map_rgb(255,255,255));
            al_draw_text(fonte, al_map_rgb(255,0,0), LARGURA_TELA/2, ALTURA_TELA/3, ALLEGRO_ALIGN_LEFT, "Tempo excedido! Tente novamente");
            al_flip_display();
            rest(2.0);
            ipAd=true;
            strcpy(ip, "");
        }  
        al_flip_display(); 
    }
    
}

void lobby(){
    int telaMenu;

    telaMenu = al_load_bitmap("./app/Resources/Etc/index.png");
}

int main(){
    iniciar();
    startScreen();
    readLogin();
    readHelmet();
    readIP();
    lobby();
    al_flip_display();
    while(sair){
        runGame();
    }
    endGame();
    return 0;
}