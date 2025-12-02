#include "Puerta.hpp"
#include "raylib.h"

// ============================================================================
// CONSTRUCTOR: carga texturas y define posición base
// ============================================================================
Puerta::Puerta()
{
	TexturaCerrada = LoadTexture("PuertaCerrada.png");		// Sprite puerta cerrada
	TexturaAbierta = LoadTexture("PuertaAbierta.png");		// Sprite puerta abierta
	TextDialogo = LoadTexture("Dialogo.png");				// Cuadro de diálogo

	Escala = 1.2f;

	// Ubicación exacta según diseño (ajustada por la altura escalada)
	float AlturaEscalada = TexturaCerrada.height * Escala;

	Posicion = { 325.0f, 300.0f - AlturaEscalada };

	EstaAbierta = false;		// Estado inicial
	MostrarDialogo = false;		// El diálogo solo aparece si el jugador está cerca
}

// Asignación de fuente para escribir dentro del cuadro de diálogo
void Puerta::SetFont(Font f)
{
	PixelFont = f;
}

// Devuelve la hitbox exacta de la puerta
Rectangle Puerta::GetRect() const
{
	return { Posicion.x, Posicion.y, TexturaCerrada.width * Escala, TexturaCerrada.height * Escala };
}

// ============================================================================
// Interacción para abrir la puerta
// - Solo aparece el diálogo si el jugador está cerca
// - Solo abre si se hace *click directamente sobre ella*
// ============================================================================
void Puerta::IntAbrir(const Rectangle& RectJugador)
{
	if (CheckCollisionRecs(RectJugador,GetRect()))
	{
		MostrarDialogo = !EstaAbierta;		// Mostrar “Haz click…” solo si está cerrada

		Vector2 mouse = GetMousePosition();
		Rectangle rectPuerta = GetRect();

		// Click EXACTO encima de la puerta para abrir
		if (CheckCollisionPointRec(mouse, rectPuerta) &&
			IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
		{
			EstaAbierta = true;
			MostrarDialogo = false;
		}
	}
	else
	{
		MostrarDialogo = false;		// Si el jugador se aleja, se oculta el diálogo
	}
}

// ============================================================================
// Dibujo de la puerta y su cuadro de interacción
// ============================================================================
void Puerta::Draw() const
{
	// Sprite según estado
	if (EstaAbierta)
		DrawTextureEx(TexturaAbierta, Posicion, 0, Escala, WHITE);
	else
		DrawTextureEx(TexturaCerrada, Posicion, 0, Escala, WHITE);

	// Cuadro “Haz click para abrir la puerta”
	if (MostrarDialogo)
	{
		float escalaDialogo = 0.40f;

		float dw = TextDialogo.width * escalaDialogo;
		float dh = TextDialogo.height * escalaDialogo;

		// Posición del cuadro, centrado respecto a la puerta
		Vector2 posDialogo;
		posDialogo.x = Posicion.x - 180;
		posDialogo.y = Posicion.y - dh;

		DrawTextureEx(TextDialogo, posDialogo, 0, escalaDialogo, WHITE);

		// Texto dentro del cuadro
		const char* texto = "Haz click para abrir la puerta";
		int fontSize = 8;
		float spacing = 1;

		Vector2 ts = MeasureTextEx(PixelFont, texto, fontSize, spacing);

		Vector2 posTexto = {
			posDialogo.x + dw / 2 - ts.x / 2,
			posDialogo.y + dh / 2 - ts.y / 2 - 10
		};

		DrawTextEx(PixelFont, texto, posTexto, fontSize, spacing, BLACK);
	}
}

// ============================================================================
// Destructor: libera texturas
// ============================================================================
Puerta::~Puerta()	
{
	UnloadTexture(TexturaCerrada);
	UnloadTexture(TexturaAbierta);
	UnloadTexture(TextDialogo);
}