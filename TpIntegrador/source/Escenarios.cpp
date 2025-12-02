#include "Escenarios.hpp"
#include "raylib.h"

// ============================================================================
// ESCENARIO BASE DEL NIVEL
// ============================================================================

// Dibuja el fondo, árbol decorativo, suelo inicial, pinchos, plataformas y cajas.
// Es la vista principal utilizada durante el gameplay.
void EscenarioBase(
    Texture2D TexturaFondo,
    Texture2D TexturaSuelo,
    Texture2D TexturaPinchos,
    Texture2D TexturaArbol,
    const std::vector<Plataforma*>& plataformas,
    const std::vector<Caja*>& cajas,
    int TilesNecesarios
)
{
    // Fondo completo
    DrawTexture(TexturaFondo, 0, 0, WHITE);

    // Árbol decorativo (escalado y ajustado al suelo)
    float escalaArbol = 0.8f;
    DrawTextureEx(TexturaArbol, { 5, 640 - (TexturaArbol.height * escalaArbol) }, 0, escalaArbol, WHITE);

    // Primeros 5 tiles = suelo → caminable
    for (int i = 0; i < 5; i++)
        DrawTexture(TexturaSuelo, i * 64.0f, 640, WHITE);

    // Resto del suelo = pinchos → zona peligrosa
    for (int i = 5; i < TilesNecesarios; i++)
        DrawTexture(TexturaPinchos, i * 64.0f, 623, WHITE);

    // Plataformas del nivel
    for (auto* p : plataformas) p->Draw();

    // Cajas
    for (auto* c : cajas) c->Draw();
}

// ============================================================================
// ESCENARIO FINAL — GANASTE
// ============================================================================

// Se dibuja suelo completo normal (sin pinchos ni árbol).
void EscenarioFinalGanaste(Texture2D TexturaFondo, Texture2D TexturaSuelo, int TilesNecesarios)
{
    DrawTexture(TexturaFondo, 0, 0, WHITE);

    // Suelo completo caminable
    for (int i = 0; i < TilesNecesarios; i++)
        DrawTexture(TexturaSuelo, i * 64.0f, 640, WHITE);
}

// ============================================================================
// ESCENARIO FINAL — PERDISTE
// ============================================================================

// Todo el suelo se reemplaza por pinchos (estético, no colisiona aquí).
void EscenarioFinalPerdiste(
    Texture2D TexturaFondo,
    Texture2D TexturaPinchos,
    int TilesNecesarios
)
{
    DrawTexture(TexturaFondo, 0, 0, WHITE);

    // Suelo completo de pinchos
    for (int i = 0; i < TilesNecesarios; i++)
        DrawTexture(TexturaPinchos, i * 64.0f, 623, WHITE);
}

// ============================================================================
// HUD COMPLETO (tiempo, posición, contador de saltos)
// ============================================================================

// Se dibuja el HUD superior e inferior con información de la partida.
void DibujarHUD(
    Font PixelFont,
    Texture2D TexturaReloj,
    Texture2D TexturaPosicion,
    Texture2D TexturaSaltos,
    float tiempoJugado,
    const Player& Jugador
)
{
    // FORMATO TIEMPO TOTAL
    int segundos = (int)tiempoJugado;
    int centesimas = (int)((tiempoJugado - segundos) * 100.0f);

    if (centesimas < 0) centesimas = 0;
    if (centesimas > 99) centesimas = 99;

    const char* tiempoStr = TextFormat("%02i:%02i|20:00", segundos, centesimas);

    DrawTextureEx(TexturaReloj, { 780, 10 }, 0, 0.15f, WHITE);
    DrawTextEx(PixelFont, tiempoStr, { 830, 32 }, 15, 1, BLACK);

    // POSICIÓN DEL JUGADOR
    DrawTextureEx(TexturaPosicion, { 10, 10 }, 0, 0.1f, WHITE);
    DrawTextEx(PixelFont,
        TextFormat("X:%i Y:%i", (int)Jugador.Posicion.x, (int)Jugador.Posicion.y),
        { 50, 25 },
        15,
        0,
        BLACK
    );

    // CONTADOR DE SALTOS
    DrawTextureEx(TexturaSaltos, { 844, 690 }, 0, 0.08f, WHITE);
    const char* saltoStr = TextFormat("%i|10", Jugador.ContadorSaltos);

    Vector2 sizeSaltos = MeasureTextEx(PixelFont, saltoStr, 14, 1);

    DrawTextEx(
        PixelFont,
        saltoStr,
        { 1024 - sizeSaltos.x - 30, 768 - sizeSaltos.y - 40 },
        14,
        1,
        BLACK
    );
}