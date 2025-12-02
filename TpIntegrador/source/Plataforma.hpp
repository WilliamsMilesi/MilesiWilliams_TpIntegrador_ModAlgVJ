#pragma once
#include "raylib.h"

// ============================================================================
// CLASE: Plataforma
// ============================================================================
// Representa una plataforma estática del nivel.
// El jugador y enemigos pueden pararse sobre ella.
// ============================================================================
class  Plataforma
{
public:
	Vector2 Posicion;		// Posición en pantalla
	Texture2D Textura;		// Imagen de la plataforma
	float Escala;			// Factor de tamaño para el dibujo

	// Constructor: crea una plataforma en (x, y) y carga su textura
	Plataforma(float x, float y);

	// Dibuja la plataforma en pantalla
	void Draw() const;

	// Devuelve su rectángulo de colisión (para físicas)
	Rectangle GetRect() const;

	// Libera la textura
	~Plataforma();
};