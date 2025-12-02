#include "Caja.hpp"

// CONSTRUCTOR: carga textura y fija posición
Caja::Caja(float x, float y)
{
	Textura = LoadTexture("Caja.png");	// Carga la textura de la caja
	Escala = 1.0f;						// Tamaño sin cambios
	Posicion = { x, y };				// Ubicación inicial en pantalla
}

// RECTÁNGULO DE COLISIÓN
Rectangle Caja::GetRect() const
{
	return { Posicion.x, Posicion.y, Textura.width * Escala, Textura.height * Escala };
}

// DIBUJADO EN PANTALLA
void Caja::Draw() const
{
	DrawTextureEx(Textura, Posicion, 0, Escala, WHITE);
}

// DESTRUCTOR: libera la textura de la caja
Caja::~Caja()
{
	UnloadTexture(Textura);
}