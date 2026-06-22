#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

void color(unsigned char *r, unsigned char *g, unsigned char *b, unsigned char nuevo_r, unsigned char nuevo_g, unsigned char nuevo_b);

int main() 
{
    al_init();
    al_init_primitives_addon();
    al_install_keyboard();

    ALLEGRO_DISPLAY *pantalla = al_create_display(600, 480);
    
    ALLEGRO_EVENT_QUEUE *eventos = al_create_event_queue();
    al_register_event_source(eventos, al_get_display_event_source(pantalla));
    al_register_event_source(eventos, al_get_keyboard_event_source());

    float posX = 0; 
    float posY = 0;
    float lado = 50; 

    unsigned char rojo, verde, azul;
    
    color(&rojo, &verde, &azul, 255, 0, 128);

    bool jugando = true;

    while (jugando) 
    {
        
        al_clear_to_color(al_map_rgb(0, 0, 0));
        
        al_draw_filled_rectangle(posX, posY, posX + lado, posY + lado, al_map_rgb(rojo, verde, azul)); 
        
        al_flip_display();

      
        ALLEGRO_EVENT ev;
        al_wait_for_event(eventos, &ev);


        if (ev.type == ALLEGRO_EVENT_KEY_DOWN) 
	{
            switch (ev.keyboard.keycode)
	    {
                case ALLEGRO_KEY_UP:    
                    posY -= 50;
                    break;
                case ALLEGRO_KEY_DOWN:  
                    posY += 50;
                    break;
                case ALLEGRO_KEY_LEFT:  
                    posX -= 50;
                    break;
                case ALLEGRO_KEY_RIGHT: 
                    posX += 50;
                    break;
                case ALLEGRO_KEY_ESCAPE: 
                    jugando = false;
                    break;
            }
        }
        else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) 
	{
            jugando = false;
        }
    }




    return 0;
}

void color(unsigned char *r, unsigned char *g, unsigned char *b, unsigned char nuevo_r, unsigned char nuevo_g, unsigned char nuevo_b) 
{
    *r = nuevo_r;
    *g = nuevo_g;
    *b = nuevo_b;
}


// gcc movercuadrado1.c -o mi_juego -lallegro -lallegro_primitives //
