#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

int main() 
{
    al_init();
    al_init_primitives_addon();
    al_install_keyboard();
    al_init_image_addon();

    // ventana del allegro (ancho, alto)
    ALLEGRO_DISPLAY *pantalla = al_create_display(1280, 720);
    

    ALLEGRO_EVENT_QUEUE *eventos = al_create_event_queue();
    al_register_event_source(eventos, al_get_display_event_source(pantalla));
    al_register_event_source(eventos, al_get_keyboard_event_source());



    ALLEGRO_BITMAP *fondo = al_load_bitmap("fondominecraft.jpg");
    
    ALLEGRO_BITMAP *personaje = al_load_bitmap("personaje.png");


    if (!fondo || !personaje) 
    {
        printf("No se encontró una de las fotos\n");
        return 0; 
    }

    float posX = 0; 
    float posY = 0;

    bool jugando = true;

    while (jugando) 
    {
        
        al_clear_to_color(al_map_rgb(0, 0, 0));
        
        al_draw_bitmap(fondo, 0, 0, 0); 
        
        al_draw_bitmap(personaje, posX, posY, 0); 
        
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

// gcc cuadradoimagen1.c -o imagen1 -lallegro -lallegro_primitives -lallegro_image

