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
#define MAX_ENEMIGOS 5


int ValorAbsoluto(int Numero);
int Inicio(ALLEGRO_DISPLAY** Pantalla, ALLEGRO_EVENT_QUEUE** Cola, ALLEGRO_TIMER** Temporizador);
int ValidarMovimiento(char Matriz[FILAS][COLUMNAS], int DestinoX, int DestinoY);
void ObtenerPosicionSegura(char Matriz[FILAS][COLUMNAS], int JugX, int JugY, int* ResX, int* ResY);
void CargarMapa(char Matriz[FILAS][COLUMNAS], int* JugadorX, int* JugadorY, int EnemigosX[], int EnemigosY[], int* CantEnemigos);
void CalcularSiguientePaso(char Matriz[FILAS][COLUMNAS], int StartX, int StartY, int TargetX, int TargetY, int* SigX, int* SigY);
void DibujarMapa(char Matriz[FILAS][COLUMNAS]);
void DibujarEnemigos(int JugX, int JugY, int EnemigosX[], int EnemigosY[], int CantEnemigos, int Atacando);


int main()
{
	ALLEGRO_DISPLAY* Pantalla;
	ALLEGRO_EVENT_QUEUE* Cola;
	ALLEGRO_TIMER* Temporizador;
	ALLEGRO_EVENT Evento;

	char Mapa[FILAS][COLUMNAS];
	int JugadorX;
	int JugadorY;
	
	// Para enemigos
	int ListaEnemigosX[MAX_ENEMIGOS];
	int ListaEnemigosY[MAX_ENEMIGOS];
	int CantidadEnemigos;
	
	int Corriendo;
	int Dibujar;
	int NuevoX;
	int NuevoY;
	int SigEneX;
	int SigEneY;
	int I;
	int J;
	int SpawnX;
	int SpawnY;

	int VidasJugador;
	int Atacando;
	int RedondeoAtaque; 
	int RedondeoEnemigo; 
	
	Pantalla = NULL;
	Cola = NULL;
	Temporizador = NULL;

	srand(time(NULL));

	if (!Inicio(&Pantalla, &Cola, &Temporizador))
	{
		return -1;
	}

	CargarMapa(Mapa, &JugadorX, &JugadorY, ListaEnemigosX, ListaEnemigosY, &CantidadEnemigos);

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

			RedondeoEnemigo++;
			if (RedondeoEnemigo >= 60)
			{
				RedondeoEnemigo = 0;
				
				for (I = 0; I < CantidadEnemigos; I++)
				{
					CalcularSiguientePaso(Mapa, ListaEnemigosX[I], ListaEnemigosY[I], JugadorX, JugadorY, &SigEneX, &SigEneY);
					ListaEnemigosX[I] = SigEneX;
					ListaEnemigosY[I] = SigEneY;

					if (ListaEnemigosX[I] == JugadorX && ListaEnemigosY[I] == JugadorY)
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
				else if (Evento.keyboard.keycode == ALLEGRO_KEY_Z)
				{
					Atacando = 1;
					RedondeoAtaque = 0;

					for (I = CantidadEnemigos - 1; I >= 0; I--)
					{
						if (ValorAbsoluto(ListaEnemigosX[I] - JugadorX) <= 2 && ValorAbsoluto(ListaEnemigosY[I] - JugadorY) <= 2)
						{
							for (J = I; J < CantidadEnemigos - 1; J++)
							{
								ListaEnemigosX[J] = ListaEnemigosX[J + 1];
								ListaEnemigosY[J] = ListaEnemigosY[J + 1];
							}
							CantidadEnemigos--;

							if (CantidadEnemigos + 2 <= MAX_ENEMIGOS)
							{
								ObtenerPosicionSegura(Mapa, JugadorX, JugadorY, &SpawnX, &SpawnY);
								ListaEnemigosX[CantidadEnemigos] = SpawnX;
								ListaEnemigosY[CantidadEnemigos] = SpawnY;
								CantidadEnemigos++;
								
								ObtenerPosicionSegura(Mapa, JugadorX, JugadorY, &SpawnX, &SpawnY);
								ListaEnemigosX[CantidadEnemigos] = SpawnX;
								ListaEnemigosY[CantidadEnemigos] = SpawnY;
								CantidadEnemigos++;
							}
							else if (CantidadEnemigos + 1 <= MAX_ENEMIGOS)
							{
								ObtenerPosicionSegura(Mapa, JugadorX, JugadorY, &SpawnX, &SpawnY);
								ListaEnemigosX[CantidadEnemigos] = SpawnX;
								ListaEnemigosY[CantidadEnemigos] = SpawnY;
								CantidadEnemigos++;
							}
						}
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
			DibujarEnemigos(JugadorX, JugadorY, ListaEnemigosX, ListaEnemigosY, CantidadEnemigos, Atacando);
			
			al_flip_display();
		}
	}

	al_destroy_timer(Temporizador);
	al_destroy_event_queue(Cola);
	al_destroy_display(Pantalla);

	return 0;
}






int ValorAbsoluto(int Numero)
{
	if (Numero < 0)
	{
		return -Numero;
	}
	return Numero;
}






int Inicio(ALLEGRO_DISPLAY** Pantalla, ALLEGRO_EVENT_QUEUE** Cola, ALLEGRO_TIMER** Temporizador)
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
	al_register_event_source(*Cola, al_get_display_event_source(*Pantalla));
	al_register_event_source(*Cola, al_get_timer_event_source(*Temporizador));

	return 1;
}









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






void CargarMapa(char Matriz[FILAS][COLUMNAS], int* JugadorX, int* JugadorY, int EnemigosX[], int EnemigosY[], int* CantEnemigos)
{
	FILE* Archivo;
	int F;
	int C;
	char Caracter;
	int SpawnX;
	int SpawnY;

	Archivo = fopen("mapa.txt", "r");
	if (Archivo == NULL)
	{
		printf("Error: No se encontro el archivo mapa.txt\n");
		exit(1);
	}

	F = 0;
	C = 0;

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

	*CantEnemigos = 1;
	ObtenerPosicionSegura(Matriz, *JugadorX, *JugadorY, &SpawnX, &SpawnY);
	EnemigosX[0] = SpawnX;
	EnemigosY[0] = SpawnY;
}




void ObtenerPosicionSegura(char Matriz[FILAS][COLUMNAS], int JugX, int JugY, int* ResX, int* ResY)
{
	// Eliminamos structs usando arreglos paralelos para las coordenadas validas
	int OpcionesValidasX[FILAS * COLUMNAS];
	int OpcionesValidasY[FILAS * COLUMNAS];
	int CantidadOpciones;
	int F;
	int C;
	int DistanciaX;
	int DistanciaY;
	int IndiceElegido;

	CantidadOpciones = 0;

	for (F = 0; F < FILAS; F++)
	{
		for (C = 0; C < COLUMNAS; C++)
		{
			if (Matriz[F][C] == '0')
			{
				DistanciaX = ValorAbsoluto(C - JugX);
				DistanciaY = ValorAbsoluto(F - JugY);

				if (DistanciaX >= 10 || DistanciaY >= 10)
				{
					OpcionesValidasX[CantidadOpciones] = C;
					OpcionesValidasY[CantidadOpciones] = F;
					CantidadOpciones++;
				}
			}
		}
	}

	if (CantidadOpciones > 0)
	{
		IndiceElegido = rand() % CantidadOpciones;
		*ResX = OpcionesValidasX[IndiceElegido];
		*ResY = OpcionesValidasY[IndiceElegido];
	}
	else
	{
		*ResX = COLUMNAS - 2;
		*ResY = FILAS - 2;
	}
}





void CalcularSiguientePaso(char Matriz[FILAS][COLUMNAS], int StartX, int StartY, int TargetX, int TargetY, int* SigX, int* SigY)
{
	if (StartX == TargetX && StartY == TargetY)
	{
		*SigX = StartX;
		*SigY = StartY;
		return;
	}

	// Cola del BFS implementada con dos arreglos paralelos simples en vez de struct
	int ColaBfsX[FILAS * COLUMNAS];
	int ColaBfsY[FILAS * COLUMNAS];
	int Visitado[FILAS][COLUMNAS];
	int PadreX[FILAS][COLUMNAS];
	int PadreY[FILAS][COLUMNAS];
	int InicioCola;
	int FinCola;
	int F;
	int C;
	int ActualX;
	int ActualY;
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
			PadreX[F][C] = -1;
			PadreY[F][C] = -1;
		}
	}

	ColaBfsX[FinCola] = TargetX;
	ColaBfsY[FinCola] = TargetY;
	FinCola++;
	Visitado[TargetY][TargetX] = 1;

	while (InicioCola < FinCola)
	{
		ActualX = ColaBfsX[InicioCola];
		ActualY = ColaBfsY[InicioCola];
		InicioCola++;

		if (ActualX == StartX && ActualY == StartY)
		{
			break;
		}

		for (I = 0; I < 4; I++)
		{
			NextX = ActualX + DirX[I];
			NextY = ActualY + DirY[I];

			if (ValidarMovimiento(Matriz, NextX, NextY) && !Visitado[NextY][NextX])
			{
				Visitado[NextY][NextX] = 1;
				PadreX[NextY][NextX] = ActualX;
				PadreY[NextY][NextX] = ActualY;
				ColaBfsX[FinCola] = NextX;
				ColaBfsY[FinCola] = NextY;
				FinCola++;
			}
		}
	}

	if (PadreX[StartY][StartX] != -1)
	{
		*SigX = PadreX[StartY][StartX];
		*SigY = PadreY[StartY][StartX];
	}
	else
	{
		*SigX = StartX;
		*SigY = StartY;
	}
}









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
				al_draw_filled_rectangle(X, Y, X + TAMANO_CASILLA, Y + TAMANO_CASILLA, al_map_rgb(70, 70, 70));
				al_draw_rectangle(X, Y, X + TAMANO_CASILLA, Y + TAMANO_CASILLA, al_map_rgb(45, 45, 45), 1);
			}
			else
			{
				al_draw_filled_rectangle(X, Y, X + TAMANO_CASILLA, Y + TAMANO_CASILLA, al_map_rgb(15, 15, 15));
			}
		}
	}
}









void DibujarEnemigos(int JugX, int JugY, int EnemigosX[], int EnemigosY[], int CantEnemigos, int Atacando)
{
	float JX;
	float JY;
	float EX;
	float EY;
	int I;
	ALLEGRO_COLOR ColorJugador;

	JX = JugX * TAMANO_CASILLA;
	JY = JugY * TAMANO_CASILLA;

	for (I = 0; I < CantEnemigos; I++)
	{
		EX = EnemigosX[I] * TAMANO_CASILLA;
		EY = EnemigosY[I] * TAMANO_CASILLA;
		al_draw_filled_rectangle(EX + 4, EY + 4, EX + TAMANO_CASILLA - 4, EY + TAMANO_CASILLA - 4, al_map_rgb(50, 50, 220));
	}

	if (Atacando)
	{
		ColorJugador = al_map_rgb(255, 0, 0); 
	}
	else
	{
		ColorJugador = al_map_rgb(50, 220, 50); 
	}

	al_draw_filled_rectangle(JX + 2, JY + 2, JX + TAMANO_CASILLA - 2, JY + TAMANO_CASILLA - 2, ColorJugador);

	if (Atacando)
	{
		al_draw_rectangle(JX - (TAMANO_CASILLA * 2), JY - (TAMANO_CASILLA * 2), JX + TAMANO_CASILLA + (TAMANO_CASILLA * 2), JY + TAMANO_CASILLA + (TAMANO_CASILLA * 2), al_map_rgb(255, 255, 0), 2);
	}
}
