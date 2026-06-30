#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#define FILAS 18
#define COLUMNAS 32
#define TAMANO_CASILLA 40
#define ANCHO_PANTALLA 1280
#define ALTO_PANTALLA 720
#define FPS 60

// Estructura simple para el BFS (Cola) sin usar typedef
struct Punto
{
    int X;
    int Y;
};

// Funcion propia para sacar el valor absoluto de un entero
int ValorAbsoluto(int Numero)
{
    if (Numero < 0)
    {
        return -Numero;
    }
    return Numero;
}

// Inicializa Allegro, addon de primitivas, teclado y timer
int InicializarTodo(ALLEGRO_DISPLAY** Pantalla, ALLEGRO_EVENT_QUEUE** Cola, ALLEGRO_TIMER** Temporizador)
{
    if (!al_init())
    {
        printf("No se pudo iniciar Allegro.\n");
        return 0;
    }

    if (!al_init_primitives_addon())
    {
        printf("No se pudo iniciar el addon de primitivas.\n");
        return 0;
    }

    if (!al_install_keyboard())
    {
        printf("No se pudo instalar el teclado.\n");
        return 0;
    }

    *Temporizador = al_create_timer(1.0 / FPS);
    if (!*Temporizador)
    {
        printf("No se pudo crear el timer principal.\n");
        return 0;
    }

    *Pantalla = al_create_display(ANCHO_PANTALLA, ALTO_PANTALLA);
    if (!*Pantalla)
    {
        printf("No se pudo crear la pantalla.\n");
        return 0;
    }

    *Cola = al_create_event_queue();
    if (!*Cola)
    {
        printf("No se pudo crear la cola de eventos.\n");
        return 0;
    }

    al_register_event_source(*Cola, al_get_keyboard_event_source());
    // CORREGIDO: Ahora le pasamos la pantalla creada como argumento
    al_register_event_source(*Cola, al_get_display_event_source(*Pantalla));
    al_register_event_source(*Cola, al_get_timer_event_source(*Temporizador));

    return 1;
}


// Carga el archivo .txt en la matriz y busca posiciones iniciales
void CargarMapa(char Matriz[FILAS][COLUMNAS], int* JugadorX, int* JugadorY, int* EnemigoX, int* EnemigoY)
{
    FILE* Archivo;
    int F;
    int C;
    char Caracter;

    Archivo = fopen("mapa.txt", "r");
    if (Archivo == NULL)
    {
        printf("Error: No se encontro el archivo mapa.txt\n");
        exit(1);
    }

    F = 0;
    C = 0;
    *EnemigoX = 15;
    *EnemigoY = 9;

    while (F < FILAS)
    {
        Caracter = fgetc(Archivo);
        if (Caracter == EOF)
        {
            break;
        }
        if (Caracter == '\n' || Caracter == '\r')
        {
            continue;
        }

        Matriz[F][C] = Caracter;
        
        if (Caracter == 'P')
        {
            *JugadorX = C;
            *JugadorY = F;
            Matriz[F][C] = '0'; 
        }

        C++;
        if (C >= COLUMNAS)
        {
            C = 0;
            F++;
        }
    }

    fclose(Archivo);
}

// Verifica si una casilla es caminable (no es pared)
int ValidarMovimiento(char Matriz[FILAS][COLUMNAS], int DestinoX, int DestinoY)
{
    if (DestinoX < 0 || DestinoX >= COLUMNAS || DestinoY < 0 || DestinoY >= FILAS)
    {
        return 0;
    }

    if (Matriz[DestinoY][DestinoX] == '1')
    {
        return 0;
    }

    return 1;
}

// Encuentra el siguiente paso (1 casilla, no diagonal) usando BFS
void CalcularSiguientePaso(char Matriz[FILAS][COLUMNAS], int StartX, int StartY, int TargetX, int TargetY, int* SigX, int* SigY)
{
    if (StartX == TargetX && StartY == TargetY)
    {
        *SigX = StartX;
        *SigY = StartY;
        return;
    }

    struct Punto ColaBfs[FILAS * COLUMNAS];
    int Visitado[FILAS][COLUMNAS];
    struct Punto Padre[FILAS][COLUMNAS];
    int InicioCola;
    int FinCola;
    int F;
    int C;
    struct Punto Actual;
    int DirX[4] = {0, 0, -1, 1};
    int DirY[4] = {-1, 1, 0, 0};
    int I;
    int NextX;
    int NextY;

    InicioCola = 0;
    FinCola = 0;

    for (F = 0; F < FILAS; F++)
    {
        for (C = 0; C < COLUMNAS; C++)
        {
            Visitado[F][C] = 0;
            Padre[F][C].X = -1;
            Padre[F][C].Y = -1;
        }
    }

    // Busqueda inversa desde el jugador para trazar la ruta directa hacia el enemigo
    ColaBfs[FinCola].X = TargetX;
    ColaBfs[FinCola].Y = TargetY;
    FinCola++;
    Visitado[TargetY][TargetX] = 1;

    while (InicioCola < FinCola)
    {
        Actual = ColaBfs[InicioCola];
        InicioCola++;

        if (Actual.X == StartX && Actual.Y == StartY)
        {
            break;
        }

        for (I = 0; I < 4; I++)
        {
            NextX = Actual.X + DirX[I];
            NextY = Actual.Y + DirY[I];

            if (ValidarMovimiento(Matriz, NextX, NextY) && !Visitado[NextY][NextX])
            {
                Visitado[NextY][NextX] = 1;
                Padre[NextY][NextX] = Actual;
                ColaBfs[FinCola].X = NextX;
                ColaBfs[FinCola].Y = NextY;
                FinCola++;
            }
        }
    }

    if (Padre[StartY][StartX].X != -1)
    {
        *SigX = Padre[StartY][StartX].X;
        *SigY = Padre[StartY][StartX].Y;
    }
    else
    {
        *SigX = StartX;
        *SigY = StartY;
    }
}

// Dibuja el mapa usando figuras nativas (Paredes gris, suelo negro)
void DibujarMapa(char Matriz[FILAS][COLUMNAS])
{
    int F;
    int C;
    float X;
    float Y;

    for (F = 0; F < FILAS; F++)
    {
        for (C = 0; C < COLUMNAS; C++)
        {
            X = C * TAMANO_CASILLA;
            Y = F * TAMANO_CASILLA;

            if (Matriz[F][C] == '1')
            {
                // Paredes
                al_draw_filled_rectangle(X, Y, X + TAMANO_CASILLA, Y + TAMANO_CASILLA, al_map_rgb(80, 80, 80));
                al_draw_rectangle(X, Y, X + TAMANO_CASILLA, Y + TAMANO_CASILLA, al_map_rgb(40, 40, 40), 1);
            }
            else
            {
                // Suelo libre
                al_draw_filled_rectangle(X, Y, X + TAMANO_CASILLA, Y + TAMANO_CASILLA, al_map_rgb(20, 20, 20));
            }
        }
    }
}

// Dibuja los personajes y efectos aplicando el cambio de color al atacar
void DibujarEntidades(int JugX, int JugY, int EneX, int EneY, int Atacando)
{
    float JX;
    float JY;
    float EX;
    float EY;
    ALLEGRO_COLOR ColorJugador;

    JX = JugX * TAMANO_CASILLA;
    JY = JugY * TAMANO_CASILLA;
    EX = EneX * TAMANO_CASILLA;
    EY = EneY * TAMANO_CASILLA;

    // Dibujar Enemigo (Cuadrado Azul)
    al_draw_filled_rectangle(EX + 4, EY + 4, EX + TAMANO_CASILLA - 4, EY + TAMANO_CASILLA - 4, al_map_rgb(50, 50, 220));

    // Determinar el color del jugador segun su estado de ataque
    if (Atacando)
    {
        ColorJugador = al_map_rgb(255, 0, 0); // Rojo intenso al atacar
    }
    else
    {
        ColorJugador = al_map_rgb(50, 220, 50); // Verde (u otro color base) cuando esta normal
    }

    // Dibujar Jugador
    al_draw_filled_rectangle(JX + 2, JY + 2, JX + TAMANO_CASILLA - 2, JY + TAMANO_CASILLA - 2, ColorJugador);

    // Si esta atacando, dibuja una linea amarilla alrededor del rango (2 casillas)
    if (Atacando)
    {
        al_draw_rectangle(JX - (TAMANO_CASILLA * 2), JY - (TAMANO_CASILLA * 2), JX + TAMANO_CASILLA + (TAMANO_CASILLA * 2), JY + TAMANO_CASILLA + (TAMANO_CASILLA * 2), al_map_rgb(255, 255, 0), 2);
    }
}

int main()
{
    ALLEGRO_DISPLAY* Pantalla;
    ALLEGRO_EVENT_QUEUE* Cola;
    ALLEGRO_TIMER* Temporizador;
    ALLEGRO_EVENT Evento;

    char Mapa[FILAS][COLUMNAS];
    int JugadorX;
    int JugadorY;
    int EnemigoX;
    int EnemigoY;
    
    int Corriendo;
    int Dibujar;
    int NuevoX;
    int NuevoY;
    int SigEneX;
    int SigEneY;

    int VidasJugador;
    int Atacando;
    int RedondeoAtaque; 
    int RedondeoEnemigo; 
    
    Pantalla = NULL;
    Cola = NULL;
    Temporizador = NULL;

    if (!InicializarTodo(&Pantalla, &Cola, &Temporizador))
    {
        return -1;
    }

    CargarMapa(Mapa, &JugadorX, &JugadorY, &EnemigoX, &EnemigoY);

    VidasJugador = 3; 
    Atacando = 0;
    RedondeoAtaque = 0;
    RedondeoEnemigo = 0;
    Corriendo = 1;
    Dibujar = 1;

    al_start_timer(Temporizador);

    while (Corriendo)
    {
        al_wait_for_event(Cola, &Evento);

        if (Evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            Corriendo = 0;
        }
        
        else if (Evento.type == ALLEGRO_EVENT_TIMER)
        {
            Dibujar = 1;

            if (Atacando)
            {
                RedondeoAtaque++;
                if (RedondeoAtaque >= 15)
                {
                    Atacando = 0;
                    RedondeoAtaque = 0;
                }
            }

            // Movimiento del enemigo cada 1 segundo (60 FPS)
            RedondeoEnemigo++;
            if (RedondeoEnemigo >= 60)
            {
                RedondeoEnemigo = 0;
                
                CalcularSiguientePaso(Mapa, EnemigoX, EnemigoY, JugadorX, JugadorY, &SigEneX, &SigEneY);
                EnemigoX = SigEneX;
                EnemigoY = SigEneY;

                if (EnemigoX == JugadorX && EnemigoY == JugadorY)
                {
                    VidasJugador--;
                    if (VidasJugador <= 0)
                    {
                        printf("¡El jugador fue eliminado!\n");
                        Corriendo = 0;
                    }
                }
            }
        }
        
        else if (Evento.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            NuevoX = JugadorX;
            NuevoY = JugadorY;

            if (!Atacando)
            {
                if (Evento.keyboard.keycode == ALLEGRO_KEY_UP || Evento.keyboard.keycode == ALLEGRO_KEY_W)
                {
                    NuevoY--;
                }
                else if (Evento.keyboard.keycode == ALLEGRO_KEY_DOWN || Evento.keyboard.keycode == ALLEGRO_KEY_S)
                {
                    NuevoY++;
                }
                else if (Evento.keyboard.keycode == ALLEGRO_KEY_LEFT || Evento.keyboard.keycode == ALLEGRO_KEY_A)
                {
                    NuevoX--;
                }
                else if (Evento.keyboard.keycode == ALLEGRO_KEY_RIGHT || Evento.keyboard.keycode == ALLEGRO_KEY_D)
                {
                    NuevoX++;
                }
                
                // Ataque con Z
                else if (Evento.keyboard.keycode == ALLEGRO_KEY_Z)
                {
                    Atacando = 1;
                    RedondeoAtaque = 0;

                    if (ValorAbsoluto(EnemigoX - JugadorX) <= 2 && ValorAbsoluto(EnemigoY - JugadorY) <= 2)
                    {
                        // Si es golpeado, reaparece lejos (esquina inferior derecha)
                        EnemigoX = 30; 
                        EnemigoY = 16;
                    }
                }
                else if (Evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                {
                    Corriendo = 0;
                }

                if (NuevoX != JugadorX || NuevoY != JugadorY)
                {
                    if (ValidarMovimiento(Mapa, NuevoX, NuevoY))
                    {
                        JugadorX = NuevoX;
                        JugadorY = NuevoY;
                    }
                }
            }
        }

        if (Dibujar && al_is_event_queue_empty(Cola))
        {
            Dibujar = 0;
            al_clear_to_color(al_map_rgb(0, 0, 0));
            
            DibujarMapa(Mapa);
            DibujarEntidades(JugadorX, JugadorY, EnemigoX, EnemigoY, Atacando);
            
            al_flip_display();
        }
    }

    al_destroy_timer(Temporizador);
    al_destroy_event_queue(Cola);
    al_destroy_display(Pantalla);

    return 0;
}
