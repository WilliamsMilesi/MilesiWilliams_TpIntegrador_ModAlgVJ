#pragma once
#include "raylib.h"

// ============================================================================
// CLASE PUERTA
// ============================================================================
// Representa la puerta final del nivel. Puede estar abierta o cerrada,
// muestra un cuadro de diálogo cuando el jugador está cerca
// y permite detectar clic para abrirla.
// ============================================================================
class Puerta
{
public:

	// Posición en el mundo
	Vector2 Posicion;

	// Texturas de la puerta y el cuadro de diálogo
	Texture2D TexturaCerrada;
	Texture2D TexturaAbierta;
	Texture2D TextDialogo;

	// Estados de interacción
	bool EstaAbierta;		// Se activa cuando el jugador la abre
	bool MostrarDialogo;	// Muestra mensaje "haz click para abrir"

	// Escalado y fuente para dibujar texto
	float Escala;
	Font PixelFont;

	// =========================================================================
	// MÉTODOS PRINCIPALES
	// =========================================================================
	Puerta();							// Carga texturas y configura el estado inicial
	void SetFont(Font f);				// Fuente usada para el mensaje emergente
	void Draw() const;					// Dibuja la puerta + cuadro de diálogo si corresponde
	Rectangle GetRect() const;			// Hitbox para detectar proximidad del jugador
	void IntAbrir(const Rectangle& RectJugador);	// Lógica de interacción (hover + clic)

	~Puerta();			// Libera texturas cargadas
};