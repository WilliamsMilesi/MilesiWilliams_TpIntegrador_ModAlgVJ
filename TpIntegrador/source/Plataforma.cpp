#include "Plataforma.hpp"
#include "raylib.h"

// ============================================================================
// CONSTRUCTOR: Carga textura y posiciona la plataforma
// ============================================================================
Plataforma::Plataforma(float x, float y)
{
	Textura = LoadTexture("PisoFlotante.png");		// Sprite del piso flotante
	Escala = 0.80f;									// Tamaño reducido para estética del nivel
	Posicion = { x, y };							// Ubicación exacta en el mundo
}

// ============================================================================
// RECTÁNGULO DE COLISIÓN
// ============================================================================
Rectangle Plataforma::GetRect() const
{
	return { Posicion.x, Posicion.y, Textura.width * Escala, Textura.height * Escala };
}

// ============================================================================
// DIBUJADO EN PANTALLA
// ============================================================================
void Plataforma::Draw() const
{
	DrawTextureEx(Textura, Posicion, 0, Escala, WHITE);
}

// ============================================================================
// LIBERACIÓN DE RECURSOS
// ============================================================================
Plataforma::~Plataforma()
{
	UnloadTexture(Textura);		// Evita pérdidas de memoria
}