#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

// Definimos los portes de la pantalla y las casillas
#define PANTALLA_ANCHO 1280
#define PANTALLA_ALTO 720
#define TAMANIO_CASILLA 40
#define MAX_FILAS 18   
#define MAX_COLUMNAS 32 

// estructura para mover el cuadrado
struct Jugador 
{
    int x; 
    int y; 
};


int MatrizMapa[MAX_FILAS][MAX_COLUMNAS];


void IniciarTodo();
void CrearVentana();
void CargarMapa();
void DibujarParedes();
void DibujarPersonaje();
int ValidarChoque();
void ModificarMapa();

ALLEGRO_DISPLAY* Pantalla = NULL;
ALLEGRO_EVENT_QUEUE* ColaEventos = NULL;
struct Jugador MiJugador;

int main() 
{
    ALLEGRO_EVENT Evento;
    int Terminar;
    int Redibujar;
    Terminar = 0;
    Redibujar = 1;
    IniciarTodo();
    CrearVentana();
    CargarMapa();
    
	printf("--------------------------------------------------------------------------------\nSE PUEDE MOVER CON: WASD y con las flechas de IZQUIERDA, DERECHA, ARRIBA Y ABAJO\n--------------------------------------------------------------------------------\n");


    //  la casilla (1,1) esta libre CAMBIAR SI CAMBIO EL TXT
    MiJugador.x = 1;
    MiJugador.y = 1;

    al_register_event_source(ColaEventos, al_get_display_event_source(Pantalla));
    al_register_event_source(ColaEventos, al_get_keyboard_event_source());

    while (Terminar == 0) 
	{
        al_wait_for_event(ColaEventos, &Evento);

	// aca se recibe los inputs del usuario, y hacen un movimiento por cada click, no por tick 
	
        if (Evento.type == ALLEGRO_EVENT_KEY_DOWN) 
		{
            if (Evento.keyboard.keycode == ALLEGRO_KEY_W || Evento.keyboard.keycode == ALLEGRO_KEY_UP) 
			{
                if (ValidarChoque(MiJugador.x, MiJugador.y - 1) == 0) 
				{
                    MiJugador.y = MiJugador.y - 1;
                }
            }
            if (Evento.keyboard.keycode == ALLEGRO_KEY_S || Evento.keyboard.keycode == ALLEGRO_KEY_DOWN) 
			{
                if (ValidarChoque(MiJugador.x, MiJugador.y + 1) == 0) 
				{
                    MiJugador.y = MiJugador.y + 1;
                }
            }
            if (Evento.keyboard.keycode == ALLEGRO_KEY_A || Evento.keyboard.keycode == ALLEGRO_KEY_LEFT) 
			{
                if (ValidarChoque(MiJugador.x - 1, MiJugador.y) == 0) 
				{
                    MiJugador.x = MiJugador.x - 1;
                }
            }
            if (Evento.keyboard.keycode == ALLEGRO_KEY_D || Evento.keyboard.keycode == ALLEGRO_KEY_RIGHT) 
			{
                if (ValidarChoque(MiJugador.x + 1, MiJugador.y) == 0) 
				{
                    MiJugador.x = MiJugador.x + 1;
                }
            }
            // CERRAR CON ESCAPE
            if (Evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) 
			{
                Terminar = 1;
            }
            
            Redibujar = 1;
        } 
        // CERRAR VENTANA :D con la x
        else if (Evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) 
		{
            Terminar = 1;
        }

        // Si la cola está vacía, dibuja
        if (Redibujar == 1 && al_is_event_queue_empty(ColaEventos)) 
		{
            al_clear_to_color(al_map_rgb(0, 0, 0)); // CAMBIAR PARA COLOR DEL FONDO

            DibujarParedes();
            DibujarPersonaje();

            al_flip_display();
            Redibujar = 0;
        }
    }


    al_destroy_display(Pantalla);
    al_destroy_event_queue(ColaEventos);
    // se limpia la memoria (aprendi a la mala)
    return 0;
}


void IniciarTodo() 
{
    // Encendemos Allegro y los addons básicos
    if (!al_init()) 
	{
        printf("Error: No se pudo iniciar Allegro\n");
        exit(-1);
    }
    if (!al_install_keyboard()) 
	{
        printf("Error: No se detectó teclado\n");
        exit(-1);
    }
    if (!al_init_primitives_addon()) 
	{
        printf("Error: No cargaron las primitivas\n");
        exit(-1);
    }
}

void CrearVentana() 
{
    // Creamos la pantalla con el tamaño pedido y la cola
    Pantalla = al_create_display(PANTALLA_ANCHO, PANTALLA_ALTO);
    ColaEventos = al_create_event_queue();
    
    if (!Pantalla || !ColaEventos) 
	{
        printf("Error al crear la ventana o los eventos\n");
        exit(-1);
    }
}

void CargarMapa() 
{
    FILE* Archivo;
    int f;
    int c;
    char Caracter;

    // mapa.txt sin rotacion
    Archivo = fopen("mapa.txt", "r");
    f = 0;
    c = 0;
    Caracter = '\0';

    if (Archivo == NULL) 
	{
        printf("Error: No encontré el archivo mapa.txt, rompiendo...\n");
        exit(-1);
    }

    // bloc de notas 
    for (f = 0; f < MAX_FILAS; f++) 
	{
        for (c = 0; c < MAX_COLUMNAS; c++) 
		{
            do 
			{
                Caracter = fgetc(Archivo);
            } while (Caracter == '\n' || Caracter == '\r');

            if (Caracter == EOF) 
			{
                MatrizMapa[f][c] = 0; 
            } else {
                MatrizMapa[f][c] = Caracter-'0';
            }
        }
    }
    fclose(Archivo);
}

void DibujarParedes() 
{
    int f;
    int c;

    f = 0;
    c = 0;

    // 1 es pared gris, 0 no es pared
    for (f = 0; f < MAX_FILAS; f++) 
	{
        for (c = 0; c < MAX_COLUMNAS; c++) 
		{
            if (MatrizMapa[f][c] == 1) 
			{
                al_draw_filled_rectangle(
                    c * TAMANIO_CASILLA, 
                    f * TAMANIO_CASILLA, 
                    (c + 1) * TAMANIO_CASILLA, 
                    (f + 1) * TAMANIO_CASILLA, 
                    al_map_rgb(150, 150, 150)
                );
            }
        }
    }
}

void DibujarPersonaje() 
{
    // PERSONAJE
    al_draw_filled_rectangle(
        MiJugador.x * TAMANIO_CASILLA, 
        MiJugador.y * TAMANIO_CASILLA, 
        (MiJugador.x + 1) * TAMANIO_CASILLA, 
        (MiJugador.y + 1) * TAMANIO_CASILLA, 
        al_map_rgb(220, 50, 255)
    );
}

int ValidarChoque(int NuevaX, int NuevaY) 
{

    if (NuevaX < 0 || NuevaX >= MAX_COLUMNAS || NuevaY < 0 || NuevaY >= MAX_FILAS) 
	{
	    // esto es por si llega al limite del mapa
	    return 1;
    }
    
    // si matriz es 1 hay pared
    if (MatrizMapa[NuevaY][NuevaX] == 1) 
	{
        return 1; // retorna 1 si choca con pared
    }

    return 0; // retorna 0 si no choca
}

void ModificarMapa(int Fila, int Columna, int NuevoValor) 
{
    // para cambiar pared
    if (Fila >= 0 && Fila < MAX_FILAS && Columna >= 0 && Columna < MAX_COLUMNAS) 
	{
        MatrizMapa[Fila][Columna] = NuevoValor;
    }
}
