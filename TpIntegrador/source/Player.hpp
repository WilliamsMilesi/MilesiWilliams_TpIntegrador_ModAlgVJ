#pragma once
#include "raylib.h"
#include <vector>
#include "Plataforma.hpp"

class Plataforma;   // Declaraciones anticipadas para evitar includes pesados. 
class Caja;         // (Las clases se definen en otros archivos)

// ============================================================================
// TIPO DE SUPERFICIE BAJO EL JUGADOR
// ============================================================================
// Permite saber si está pisando pasto, una caja o está en el aire.
// Esto afecta sonidos y comportamiento de salto.
// ============================================================================
enum TipoSuelo { SUELO_PASTO, SUELO_CAJA, SUELO_AIRE };

class Player
{
public:
    // ========================================================================
    // PROPIEDADES PRINCIPALES DEL PERSONAJE
    // ========================================================================
    Vector2 Posicion;               // Posición actual
    Vector2 Velocidad;              // Velocidad en X/Y
    float Ancho;                    // Dimensiones base del sprite escalado
    float Alto;
    bool EnSuelo;                   // Verdadero si está tocando alguna superficie
    float BufferSalto;              // Permite "guardar" el salto por unos ms

    Texture2D TexturaCaballero;     // Sprite del jugador
    bool MirandoDerecha;            // Para espejar el sprite

    float Escala;                   // Tamaño del sprite

    int ContadorSaltos;             // Para perder si se pasa de 10

    // ========================================================================
    // SONIDOS DEL JUGADOR
    // ========================================================================
    Sound SonidoCaminarPasto;
    Sound SonidoCaminarCaja;
    Sound SonidoSaltoPasto;
    Sound SonidoSaltoCaja;

    // ========================================================================
    // SONIDOS DEL JUGADOR
    // ========================================================================
    float TimerPaso;
    float IntervaloPaso;

    // Tipo de superficie donde está parado actualmente
    TipoSuelo TipoActual;

    // ========================================================================
    // MÉTODOS PRINCIPALES
    // ========================================================================
    Player();   // Constructor

    // Actualiza movimiento, gravedad, colisiones y sonidos
    void Update(float dt, const Rectangle& piso,
                const std::vector<Plataforma*>& plataformas,
                const std::vector<Caja*>& cajas);

    // Dibuja el sprite con orientación correcta
    void Draw() const;

    // Lógica interna de salto
    void Jump();

    // Rectángulo de colisión del jugador
    Rectangle GetRect() const;

    // Restablece valores para reintentar
    void Reiniciar();

    // Libera recursos
    ~Player();
};