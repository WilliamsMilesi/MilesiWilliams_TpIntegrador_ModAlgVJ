#include "Player.hpp"
#include "Plataforma.hpp"
#include "Caja.hpp"
#include "raylib.h"

// ============================================================================
// CONSTRUCTOR DEL JUGADOR
// ============================================================================
Player::Player()
{
    // Posición inicial del personaje en el nivel
    Posicion = { 25.0f, 568.0f };
    Velocidad = { 0.0f, 0.0f };

    // Textura principal del personaje
    TexturaCaballero = LoadTexture("Caballero.png");

    // Dirección inicial: mirando a la derecha
    MirandoDerecha = true;

    // Escala del sprite (tamaño final en pantalla)
    Escala = 0.15f;

    // Dimensiones del jugador escalado
    Ancho = TexturaCaballero.width * Escala;
    Alto = TexturaCaballero.height * Escala;

    // Estado físico inicial
    EnSuelo = false;
    BufferSalto = 0;
    ContadorSaltos = 0;

    // CARGA DE SONIDOS
    SonidoCaminarPasto = LoadSound("Caminando.mp3");
    SonidoCaminarCaja = LoadSound("CaeCaja.mp3");
    SonidoSaltoPasto = LoadSound("SaltoPasto.mp3");
    SonidoSaltoCaja = LoadSound("SaltoCaja.mp3");

    // Tipo de superficie actual
    TipoActual = SUELO_AIRE;

    // Tiempo entre pasos (para evitar spam de sonido)
    TimerPaso = 0;
    IntervaloPaso = 0.33f; // 330ms entre sonidos de pasos
}

// ============================================================================
// SALTO DEL JUGADOR
// ============================================================================
void Player::Jump()
{
    const float FuerzaSalto = 450.0f;

    if (EnSuelo)
    {
        Velocidad.y = -FuerzaSalto;
        EnSuelo = false;

        // Sonido según la superficie desde la que saltó
        if (TipoActual == SUELO_PASTO) PlaySound(SonidoSaltoPasto);
        if (TipoActual == SUELO_CAJA) PlaySound(SonidoSaltoCaja);

        ContadorSaltos++;
    }
}

// ============================================================================
// RECTÁNGULO DE COLISIÓN DEL JUGADOR
// ============================================================================
Rectangle Player::GetRect() const
{
    // Se recorta horizontalmente para ajustar mejor al sprite
    float margen = Ancho * 0.26f;
    return { Posicion.x + margen, Posicion.y, Ancho - margen * 2, Alto };
}

// ============================================================================
// UPDATE PRINCIPAL DEL JUGADOR
// ============================================================================
void Player::Update(float dt, const Rectangle& piso,
    const std::vector<Plataforma*>& plataformas,
    const std::vector<Caja*>& cajas)
{
    TimerPaso += dt;

    float velMov = 200.0f;
    Velocidad.x = 0;

    if (BufferSalto > 0) BufferSalto -= dt;

    // --- MOVIMIENTO HORIZONTAL ---
    bool moviendo = false;

    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
    {
        Velocidad.x = velMov;
        MirandoDerecha = true;
        moviendo = true;
    }
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
    {
        Velocidad.x = -velMov;
        MirandoDerecha = false;
        moviendo = true;
    }

    // Buffer para salto más responsivo
    if (IsKeyPressed(KEY_SPACE))
        BufferSalto = 0.12f;

    // Gravedad
    if (!EnSuelo)
        Velocidad.y += 900 * dt;

    Rectangle r = GetRect();

    // --- MOVER EN X ---
    Posicion.x += Velocidad.x * dt;
    r = GetRect();

    // Colisión con plataformas
    for (auto* p : plataformas)
    {
        Rectangle pr = p->GetRect();
        if (CheckCollisionRecs(r, pr))
        {
            if (Velocidad.x > 0) Posicion.x = pr.x - Ancho;
            else if (Velocidad.x < 0) Posicion.x = pr.x + pr.width;
            r = GetRect();
        }
    }

    // Colisión con cajas
    for (auto* c : cajas)
    {
        Rectangle cr = c->GetRect();
        if (CheckCollisionRecs(r, cr))
        {
            if (Velocidad.x > 0) Posicion.x = cr.x - Ancho;
            else if (Velocidad.x < 0) Posicion.x = cr.x + cr.width;
            r = GetRect();
        }
    }

    // --- MOVER EN Y ---
    EnSuelo = false;
    TipoSuelo nuevoTipo = SUELO_AIRE;

    Posicion.y += Velocidad.y * dt;
    r = GetRect();

    // Suelo principal
    if (CheckCollisionRecs(r, piso))
    {
        EnSuelo = true;
        nuevoTipo = SUELO_PASTO;
        Velocidad.y = 0;
        Posicion.y = piso.y - Alto;
        r = GetRect();
    }

    // Colisión con plataformas
    for (auto* p : plataformas)
    {
        Rectangle pr = p->GetRect();
        if (CheckCollisionRecs(r, pr))
        {
            if (Velocidad.y > 0)
            {
                EnSuelo = true;
                nuevoTipo = SUELO_PASTO;
                Velocidad.y = 0;
                Posicion.y = pr.y - Alto;
            }
            r = GetRect();
        }
    }

    // Colisión con cajas
    for (auto* c : cajas)
    {
        Rectangle cr = c->GetRect();
        if (CheckCollisionRecs(r, cr))
        {
            if (Velocidad.y > 0)
            {
                EnSuelo = true;
                nuevoTipo = SUELO_CAJA;
                Velocidad.y = 0;
                Posicion.y = cr.y - Alto;
            }
            r = GetRect();
        }
    }

    TipoActual = nuevoTipo;

    // --- Salto buffer ---
    if (EnSuelo && BufferSalto > 0)
    {
        Jump();
        BufferSalto = 0;
    }

    // --- SONIDO DE PASOS ---
    if (EnSuelo && moviendo)
    {
        if (TimerPaso >= IntervaloPaso)
        {
            TimerPaso = 0;

            if (TipoActual == SUELO_PASTO)
                PlaySound(SonidoCaminarPasto);

            if (TipoActual == SUELO_CAJA)
                PlaySound(SonidoCaminarCaja);
        }
    }

    // --- Limites pantalla ---
    if (Posicion.x < 0) Posicion.x = 0;
    if (Posicion.x > 1024 - Ancho) Posicion.x = 1024 - Ancho;
}

// ============================================================================
// DIBUJAR PERSONAJE
// ============================================================================
void Player::Draw() const
{
    if (MirandoDerecha)
        DrawTextureEx(TexturaCaballero, Posicion, 0, Escala, WHITE);
    else
    {
        // Invertido horizontalmente
        Rectangle src = { (float)TexturaCaballero.width, 0, -TexturaCaballero.width, (float)TexturaCaballero.height };
        Rectangle dst = { Posicion.x, Posicion.y, TexturaCaballero.width * Escala, TexturaCaballero.height * Escala };
        DrawTexturePro(TexturaCaballero, src, dst, { 0,0 }, 0, WHITE);
    }
}

// ============================================================================
// REINICIAR VALORES DEL JUGADOR
// ============================================================================
void Player::Reiniciar()
{
    Posicion = { 25, 568 };
    Velocidad = { 0, 0 };
    EnSuelo = false;
    ContadorSaltos = 0;
    MirandoDerecha = true;
    TipoActual = SUELO_AIRE;
    TimerPaso = 0;
}

// ============================================================================
// DESTRUCTOR — LIBERAR TEXTURAS Y SONIDOS
// ============================================================================
Player::~Player()
{
    UnloadTexture(TexturaCaballero);
    UnloadSound(SonidoCaminarPasto);
    UnloadSound(SonidoCaminarCaja);
    UnloadSound(SonidoSaltoPasto);
    UnloadSound(SonidoSaltoCaja);
}