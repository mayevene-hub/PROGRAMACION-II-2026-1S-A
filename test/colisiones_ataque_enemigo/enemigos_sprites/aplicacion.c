#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

#define FILAS 18
#define COLUMNAS 32
#define TAMANO_CASILLA 40
#define ANCHO_PANTALLA 1280
#define ALTO_PANTALLA 720
#define FPS 60
#define MAX_ENEMIGOS 5

struct Coordenada
{
	int X;
	int Y;
};

struct Jugador
{
	struct Coordenada Pos;
	int Vidas;
	int Atacando;
	int RedondeoAtaque;
};

int InicioAllegro(ALLEGRO_DISPLAY** Pantalla, ALLEGRO_EVENT_QUEUE** Cola, ALLEGRO_TIMER** Temporizador);
int ValidarMovimiento(char Matriz[FILAS][COLUMNAS], struct Coordenada Destino);
void ObtenerPosicionSegura(char Matriz[FILAS][COLUMNAS], struct Coordenada PosJugador, struct Coordenada* Res);
void CargarMapa(char Matriz[FILAS][COLUMNAS], struct Jugador* P1, struct Coordenada Enemigos[], int* CantEnemigos);
void CalcularSiguientePaso(char Matriz[FILAS][COLUMNAS], struct Coordenada Start, struct Coordenada Target, struct Coordenada* Sig);
void DibujarMapa(char Matriz[FILAS][COLUMNAS], ALLEGRO_BITMAP* ImgMuro);
void DibujarEntidades(struct Jugador* P1, struct Coordenada Enemigos[], int CantEnemigos, ALLEGRO_BITMAP* ImgJugador, ALLEGRO_BITMAP* ImgEnemigo);

int main()
{
	ALLEGRO_DISPLAY* Pantalla = NULL;
	ALLEGRO_EVENT_QUEUE* Cola = NULL;
	ALLEGRO_TIMER* Temporizador = NULL;
	ALLEGRO_EVENT Evento;

	ALLEGRO_BITMAP* ImgJugador = NULL;
	ALLEGRO_BITMAP* ImgEnemigo = NULL;
	ALLEGRO_BITMAP* ImgMuro = NULL;

	char Mapa[FILAS][COLUMNAS];
	struct Jugador P1 = {{0, 0}, 3, 0, 0};
	struct Coordenada ListaEnemigos[MAX_ENEMIGOS];
	int CantidadEnemigos = 0;
	
	int Corriendo = 1;
	int Dibujar = 1;
	int RedondeoEnemigo = 0;
	int I, J;
	
	struct Coordenada NuevoPos, SigEne, SpawnPos;

	srand(time(NULL));

	if (!InicioAllegro(&Pantalla, &Cola, &Temporizador))
	{
		return -1;
	}

	ImgJugador = al_load_bitmap("jugador.png");
	ImgEnemigo = al_load_bitmap("enemigo.png");
	ImgMuro = al_load_bitmap("muro.png");

	CargarMapa(Mapa, &P1, ListaEnemigos, &CantidadEnemigos);

	al_start_timer(Temporizador);

	while (Corriendo)
	{
		// aca el juego se pausa esperando eventos de allegro (teclado, cerrar ventana o el timer a 60fps)
		al_wait_for_event(Cola, &Evento);

		if (Evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
			Corriendo = 0;
		}
		else if (Evento.type == ALLEGRO_EVENT_TIMER)
		{
			Dibujar = 1;

			if (P1.Atacando)
			{
				P1.RedondeoAtaque++;
				if (P1.RedondeoAtaque >= 15)
				{
					P1.Atacando = 0;
					P1.RedondeoAtaque = 0;
				}
			}

			RedondeoEnemigo++;
			if (RedondeoEnemigo >= 30)
			{
				RedondeoEnemigo = 0;
				
				for (I = 0; I < CantidadEnemigos; I++)
				{
					CalcularSiguientePaso(Mapa, ListaEnemigos[I], P1.Pos, &SigEne);
					ListaEnemigos[I] = SigEne;

					if (ListaEnemigos[I].X == P1.Pos.X && ListaEnemigos[I].Y == P1.Pos.Y)
					{
						P1.Vidas--;
						if (P1.Vidas <= 0)
						{
							printf("el jugador fue eliminado\n");
							Corriendo = 0;
						}
					}
				}
			}
		}
		else if (Evento.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			NuevoPos = P1.Pos;

			if (!P1.Atacando)
			{
				if (Evento.keyboard.keycode == ALLEGRO_KEY_UP || Evento.keyboard.keycode == ALLEGRO_KEY_W)
				{
					NuevoPos.Y--;
				}
				else if (Evento.keyboard.keycode == ALLEGRO_KEY_DOWN || Evento.keyboard.keycode == ALLEGRO_KEY_S)
				{
					NuevoPos.Y++;
				}
				else if (Evento.keyboard.keycode == ALLEGRO_KEY_LEFT || Evento.keyboard.keycode == ALLEGRO_KEY_A)
				{
					NuevoPos.X--;
				}
				else if (Evento.keyboard.keycode == ALLEGRO_KEY_RIGHT || Evento.keyboard.keycode == ALLEGRO_KEY_D)
				{
					NuevoPos.X++;
				}
				else if (Evento.keyboard.keycode == ALLEGRO_KEY_Z)
				{
					P1.Atacando = 1;
					P1.RedondeoAtaque = 0;

					for (I = CantidadEnemigos - 1; I >= 0; I--)
					{
						if (abs(ListaEnemigos[I].X - P1.Pos.X) <= 2 && abs(ListaEnemigos[I].Y - P1.Pos.Y) <= 2)
						{
							// si le pegamos a uno lo borramos del arreglo y spawnea 2 nuevos maximo
							for (J = I; J < CantidadEnemigos - 1; J++)
							{
								ListaEnemigos[J] = ListaEnemigos[J + 1];
							}
							CantidadEnemigos--;

							if (CantidadEnemigos + 2 <= MAX_ENEMIGOS)
							{
								ObtenerPosicionSegura(Mapa, P1.Pos, &SpawnPos);
								ListaEnemigos[CantidadEnemigos] = SpawnPos;
								CantidadEnemigos++;
								
								ObtenerPosicionSegura(Mapa, P1.Pos, &SpawnPos);
								ListaEnemigos[CantidadEnemigos] = SpawnPos;
								CantidadEnemigos++;
							}
							else if (CantidadEnemigos + 1 <= MAX_ENEMIGOS)
							{
								ObtenerPosicionSegura(Mapa, P1.Pos, &SpawnPos);
								ListaEnemigos[CantidadEnemigos] = SpawnPos;
								CantidadEnemigos++;
							}
						}
					}
				}
				else if (Evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
				{
					Corriendo = 0;
				}

				if (NuevoPos.X != P1.Pos.X || NuevoPos.Y != P1.Pos.Y)
				{
					if (ValidarMovimiento(Mapa, NuevoPos))
					{
						P1.Pos = NuevoPos;
					}
				}
			}
		}

		if (Dibujar && al_is_event_queue_empty(Cola))
		{
			Dibujar = 0;
			al_clear_to_color(al_map_rgb(0, 0, 0));
			
			// dibujamos todo en el buffer oculto y hacemos flip para mostrarlo de golpe sin tirones visuales
			DibujarMapa(Mapa, ImgMuro);
			DibujarEntidades(&P1, ListaEnemigos, CantidadEnemigos, ImgJugador, ImgEnemigo);
			
			al_flip_display();
		}
	}

	if (ImgJugador)
	{
		al_destroy_bitmap(ImgJugador);
	}
	if (ImgEnemigo)
	{
		al_destroy_bitmap(ImgEnemigo);
	}
	if (ImgMuro)
	{
		al_destroy_bitmap(ImgMuro);
	}

	al_destroy_timer(Temporizador);
	al_destroy_event_queue(Cola);
	al_destroy_display(Pantalla);

	return 0;
}

int InicioAllegro(ALLEGRO_DISPLAY** Pantalla, ALLEGRO_EVENT_QUEUE** Cola, ALLEGRO_TIMER** Temporizador)
{
	if (!al_init())
	{
		return 0;
	}
	if (!al_init_primitives_addon())
	{
		return 0;
	}
	if (!al_init_image_addon())
	{
		return 0;
	}
	if (!al_install_keyboard())
	{
		return 0;
	}

	*Temporizador = al_create_timer(1.0 / FPS);
	*Pantalla = al_create_display(ANCHO_PANTALLA, ALTO_PANTALLA);
	*Cola = al_create_event_queue();

	al_register_event_source(*Cola, al_get_keyboard_event_source());
	al_register_event_source(*Cola, al_get_display_event_source(*Pantalla));
	al_register_event_source(*Cola, al_get_timer_event_source(*Temporizador));
	
	return 1;
}

int ValidarMovimiento(char Matriz[FILAS][COLUMNAS], struct Coordenada Destino)
{
	if (Destino.X < 0 || Destino.X >= COLUMNAS || Destino.Y < 0 || Destino.Y >= FILAS)
	{
		return 0;
	}
	if (Matriz[Destino.Y][Destino.X] == '1')
	{
		return 0;
	}
	
	return 1;
}

// buscamos un pasillo vacio que este al menos a 10 bloques de distancia para un respawn seguro
void ObtenerPosicionSegura(char Matriz[FILAS][COLUMNAS], struct Coordenada PosJugador, struct Coordenada* Res)
{
	struct Coordenada OpcionesValidas[FILAS * COLUMNAS];
	int CantidadOpciones = 0;
	int F, C;

	for (F = 0; F < FILAS; F++)
	{
		for (C = 0; C < COLUMNAS; C++)
		{
			if (Matriz[F][C] == '0')
			{
				if (abs(C - PosJugador.X) >= 10 || abs(F - PosJugador.Y) >= 10)
				{
					OpcionesValidas[CantidadOpciones].X = C;
					OpcionesValidas[CantidadOpciones].Y = F;
					CantidadOpciones++;
				}
			}
		}
	}

	if (CantidadOpciones > 0)
	{
		*Res = OpcionesValidas[rand() % CantidadOpciones];
	}
	else
	{
		Res->X = COLUMNAS - 2;
		Res->Y = FILAS - 2;
	}
}

void CargarMapa(char Matriz[FILAS][COLUMNAS], struct Jugador* P1, struct Coordenada Enemigos[], int* CantEnemigos)
{
	FILE* Archivo = fopen("mapa.txt", "r");
	int F = 0, C = 0;
	char Caracter;
	struct Coordenada SpawnPos;

	if (!Archivo)
	{
		exit(1);
	}

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
			P1->Pos.X = C;
			P1->Pos.Y = F;
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
	ObtenerPosicionSegura(Matriz, P1->Pos, &SpawnPos);
	Enemigos[0] = SpawnPos;
}

// aplicamos busqueda en anchura (bfs) para encontrar el camino mas corto al jugador esquivando muros
void CalcularSiguientePaso(char Matriz[FILAS][COLUMNAS], struct Coordenada Start, struct Coordenada Target, struct Coordenada* Sig)
{
	if (Start.X == Target.X && Start.Y == Target.Y)
	{
		*Sig = Start;
		return;
	}

	struct Coordenada ColaBfs[FILAS * COLUMNAS];
	int Visitado[FILAS][COLUMNAS];
	struct Coordenada Padre[FILAS][COLUMNAS];
	
	int InicioCola = 0, FinCola = 0, F, C, I;
	struct Coordenada Actual, Next;
	
	int DirX[4] = {0, 0, -1, 1};
	int DirY[4] = {-1, 1, 0, 0};

	for (F = 0; F < FILAS; F++)
	{
		for (C = 0; C < COLUMNAS; C++)
		{
			Visitado[F][C] = 0;
			Padre[F][C].X = -1;
			Padre[F][C].Y = -1;
		}
	}

	ColaBfs[FinCola] = Target;
	FinCola++;
	Visitado[Target.Y][Target.X] = 1;

	while (InicioCola < FinCola)
	{
		Actual = ColaBfs[InicioCola];
		InicioCola++;

		if (Actual.X == Start.X && Actual.Y == Start.Y)
		{
			break;
		}

		for (I = 0; I < 4; I++)
		{
			Next.X = Actual.X + DirX[I];
			Next.Y = Actual.Y + DirY[I];

			if (ValidarMovimiento(Matriz, Next) && !Visitado[Next.Y][Next.X])
			{
				Visitado[Next.Y][Next.X] = 1;
				Padre[Next.Y][Next.X] = Actual;
				ColaBfs[FinCola] = Next;
				FinCola++;
			}
		}
	}

	if (Padre[Start.Y][Start.X].X != -1)
	{
		*Sig = Padre[Start.Y][Start.X];
	}
	else
	{
		*Sig = Start;
	}
}

void DibujarMapa(char Matriz[FILAS][COLUMNAS], ALLEGRO_BITMAP* ImgMuro)
{
	int F, C;
	float X, Y;
	
	for (F = 0; F < FILAS; F++)
	{
		for (C = 0; C < COLUMNAS; C++)
		{
			X = C * TAMANO_CASILLA; 
			Y = F * TAMANO_CASILLA;
			
			if (Matriz[F][C] == '1')
			{
				if (ImgMuro)
				{
					al_draw_bitmap(ImgMuro, X, Y, 0);
				}
				else
				{
					al_draw_filled_rectangle(X, Y, X + TAMANO_CASILLA, Y + TAMANO_CASILLA, al_map_rgb(70, 70, 70));
					al_draw_rectangle(X, Y, X + TAMANO_CASILLA, Y + TAMANO_CASILLA, al_map_rgb(45, 45, 45), 1);
				}
			}
			else
			{
				al_draw_filled_rectangle(X, Y, X + TAMANO_CASILLA, Y + TAMANO_CASILLA, al_map_rgb(15, 15, 15));
			}
		}
	}
}

void DibujarEntidades(struct Jugador* P1, struct Coordenada Enemigos[], int CantEnemigos, ALLEGRO_BITMAP* ImgJugador, ALLEGRO_BITMAP* ImgEnemigo)
{
	float JX = P1->Pos.X * TAMANO_CASILLA;
	float JY = P1->Pos.Y * TAMANO_CASILLA;
	float EX, EY;
	int I;

	for (I = 0; I < CantEnemigos; I++)
	{
		EX = Enemigos[I].X * TAMANO_CASILLA;
		EY = Enemigos[I].Y * TAMANO_CASILLA;
		
		if (ImgEnemigo)
		{
			al_draw_bitmap(ImgEnemigo, EX, EY, 0);
		}
		else
		{
			al_draw_filled_rectangle(EX + 4, EY + 4, EX + TAMANO_CASILLA - 4, EY + TAMANO_CASILLA - 4, al_map_rgb(50, 50, 220));
		}
	}

	if (ImgJugador)
	{
		if (P1->Atacando)
		{
			al_draw_tinted_bitmap(ImgJugador, al_map_rgb(255, 0, 0), JX, JY, 0);
		}
		else
		{
			al_draw_bitmap(ImgJugador, JX, JY, 0);
		}
	}
	else
	{
		ALLEGRO_COLOR ColorJugador = (P1->Atacando) ? al_map_rgb(255, 0, 0) : al_map_rgb(50, 220, 50);
		al_draw_filled_rectangle(JX + 2, JY + 2, JX + TAMANO_CASILLA - 2, JY + TAMANO_CASILLA - 2, ColorJugador);
	}

	if (P1->Atacando)
	{
		al_draw_rectangle(JX - (TAMANO_CASILLA * 2), JY - (TAMANO_CASILLA * 2), JX + TAMANO_CASILLA + (TAMANO_CASILLA * 2), JY + TAMANO_CASILLA + (TAMANO_CASILLA * 2), al_map_rgb(255, 255, 0), 2);
	}
}
