#include "Enemigo.hpp"
#include "raylib.h"

// ============================================================================
// CONSTRUCTOR — Inicializa posición, límites, velocidad y textura
// ============================================================================
Enemigo::Enemigo(float x, float y, float minX, float maxX)
{
    Posicion = { x, y };                // Posición actual en pantalla
    PosicionInicial = { x, y };         // Guardamos la posición original

    Velocidad = 80.0f;                  // Velocidad horizontal del murciélago
    VelocidadInicial = Velocidad;       // Se guarda para Reiniciar()

    MinX = minX;                        // Límite izquierdo del movimiento
    MaxX = maxX;                        // Límite derecho del movimiento
    MinXInicial = minX;                 // Valores originales
    MaxXInicial = maxX;

    Textura = LoadTexture("Murcielago.png");    // Sprite del enemigo

    Escala = 0.1f;          // Tamaño reducido
}

// ============================================================================
// UPDATE — Movimiento horizontal entre límites
// ============================================================================
void Enemigo::Update(float dt)
{
    Posicion.x += Velocidad * dt;       // Movimiento continuo

    // Rebote en los límites
    if (Posicion.x < MinX)
    {
        Posicion.x = MinX;
        Velocidad *= -1;                // Cambia dirección
    }
    else if (Posicion.x > MaxX)
    {
        Posicion.x = MaxX;
        Velocidad *= -1;
    }
}

// ============================================================================
// DRAW — Dibuja el murciélago en pantalla
// ============================================================================
void Enemigo::Draw() const
{
    DrawTextureEx(Textura, Posicion, 0, Escala, WHITE);
}

// ============================================================================
// GetRect — Caja de colisión del enemigo
// ============================================================================
Rectangle Enemigo::GetRect() const
{
    return {
        Posicion.x,
        Posicion.y,
        Textura.width * Escala,
        Textura.height * Escala
    };
}

// ============================================================================
// Reiniciar — Restaura el murciélago a sus valores originales
// ============================================================================
void Enemigo::Reiniciar()
{
    Posicion = PosicionInicial;
    Velocidad = VelocidadInicial;
    MinX = MinXInicial;
    MaxX = MaxXInicial;
}

// ============================================================================
// DESTRUCTOR — Libera la textura del enemigo
// ============================================================================
Enemigo::~Enemigo()
{
    UnloadTexture(Textura);
}