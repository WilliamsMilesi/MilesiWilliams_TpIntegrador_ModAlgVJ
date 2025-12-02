#ifndef ESCENARIOS_HPP
#define ESCENARIOS_HPP

#include "raylib.h"
#include "Plataforma.hpp"
#include "Caja.hpp"
#include "Player.hpp"
#include <vector>

// ============================================================================
// ESCENARIO BASE (fondo + suelo + pinchos + plataformas + cajas)
// Llamado durante el estado JUGANDO y en transiciones.
// ============================================================================
void EscenarioBase(
    Texture2D TexturaFondo,
    Texture2D TexturaSuelo,
    Texture2D TexturaPinchos,
    Texture2D TexturaArbol,
    const std::vector<Plataforma*>& plataformas,
    const std::vector<Caja*>& cajas,
    int TilesNecesarios
);

// ============================================================================
// ESCENARIO FINAL — GANASTE
// Versión simplificada del fondo + suelo sin obstáculos.
// ============================================================================
void EscenarioFinalGanaste(
    Texture2D TexturaFondo,
    Texture2D TexturaSuelo,
    int TilesNecesarios
);

// ============================================================================
// ESCENARIO FINAL — PERDISTE
// El suelo completo se reemplaza por pinchos decorativos.
// ============================================================================
void EscenarioFinalPerdiste(
    Texture2D TexturaFondo,
    Texture2D TexturaPinchos,
    int TilesNecesarios
);

// ============================================================================
// HUD COMPLETO
// Dibuja el reloj, posición del jugador y contador de saltos.
// ============================================================================
void DibujarHUD(
    Font PixelFont,
    Texture2D TexturaReloj,
    Texture2D TexturaPosicion,
    Texture2D TexturaSaltos,
    float tiempoJugado,
    const Player& Jugador
);

#endif