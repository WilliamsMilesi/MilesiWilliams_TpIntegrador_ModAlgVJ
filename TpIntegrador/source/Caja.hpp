#pragma once
#include "raylib.h"

// ============================================================================
// CLASE CAJA
// ============================================================================
// Representa una caja estática del escenario.
// El jugador puede pararse encima y colisiona igual que con una plataforma.
// ============================================================================
class Caja
{
public:
	Vector2 Posicion;		// Posición en pantalla
	Texture2D Textura;		// Imagen de la caja
	float Escala;			// Factor de escala visual

	// Constructor: inicializa posición, textura y escala
	Caja(float x, float y);

	// Devuelve el rectángulo de colisión de la caja
	Rectangle GetRect() const;

	// Dibuja la caja en pantalla
	void Draw() const;

	// Liberación de recursos
	~Caja();
};