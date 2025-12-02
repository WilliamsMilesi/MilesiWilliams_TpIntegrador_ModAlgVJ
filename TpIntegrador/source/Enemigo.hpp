#pragma once
#include "raylib.h"

// ============================================================================
// CLASE ENEMIGO (Murciélago)
// ============================================================================
// Representa un enemigo que se mueve horizontalmente entre dos límites.
// El jugador pierde si colisiona con él.
// La clase maneja: posición, movimiento, textura y reinicio.
// ============================================================================
class Enemigo
{
public:
    Vector2 Posicion;                   // Posición actual del enemigo en pantalla
    Vector2 PosicionInicial;            // Posición donde inicia (para reinicios)

    float Velocidad;                    // Velocidad actual de movimiento
    float VelocidadInicial;             // Velocidad original guardada para reiniciar

    float Escala;                       // Escalado del sprite del enemigo

    // Límites de movimiento horizontal
    float MinX, MaxX;
    float MinXInicial, MaxXInicial;

    Texture2D Textura;                  // Sprite del enemigo

    // Constructor: define posición inicial y rango de movimiento.
    Enemigo(float x, float y, float minX, float maxX);

    // Actualiza posición del enemigo según deltaTime.
    // Recorre de lado a lado entre minX y maxX.
    void Update(float dt);

    // Dibuja el sprite del enemigo en pantalla.
    void Draw() const;

    // Devuelve el rectángulo de colisión.
    Rectangle GetRect() const;

    // Restaura todas las variables a su estado inicial.
    void Reiniciar();

    // Libera la textura asociada.
    ~Enemigo();
};