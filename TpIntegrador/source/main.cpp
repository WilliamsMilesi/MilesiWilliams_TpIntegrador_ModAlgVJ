#ifndef NDEBUG
#include <vld.h> // Visual Leak Detector, útil en modo Debug para detectar fugas de memoria
#endif

#include "raylib.h"         // Biblioteca principal del motor gráfico
#include "Player.hpp"       // Lógica y render del jugador
#include "Puerta.hpp"       // Objeto que define el final del nivel
#include "Plataforma.hpp"   // Plataformas sólidas del escenario
#include "Caja.hpp"         // Cajas que actúan como superficie secundaria
#include "Escenarios.hpp"   // Funciones de dibujo del escenario completo
#include "Enemigo.hpp"      // Enemigos móviles (murciélagos)
#include <vector>           // Estructura dinámica usada para almacenar plataformas/cajas

// ============================================================================
// ESTADOS DEL JUEGO
// ============================================================================
// Representan las diferentes pantallas y transiciones del flujo general:
// - MENU: pantalla principal con botones
// - JUGANDO: loop principal de juego
// - TRANSICION_GANASTE → GANASTE: animación + pantalla final de victoria
// - TRANSICION_PERDISTE → PERDISTE: animación + pantalla final de derrota
// ============================================================================
enum EstadoJuego { MENU, JUGANDO, TRANSICION_GANASTE, GANASTE, TRANSICION_PERDISTE, PERDISTE};

int main(void)
{
    // ============================================================================
    // SISTEMA DE AUDIO Y CARGA DE SONIDOS
    // ============================================================================

    // Inicializamos el sistema de audio (Necesario para reproducir sonidos)
    InitAudioDevice();

    // Agregamos la musica y los sonidos
    Music MusicaFondo = LoadMusicStream("Musica.mp3");
    PlayMusicStream(MusicaFondo); // Iniciamos la reproducción en loop

    // Cargamos los efectos de sonido usados en botones y transiciones
    Sound SonidoBoton = LoadSound("Boton.mp3");         // Hover sobre botones
    Sound SonidoTocaBoton = LoadSound("TocaBoton.mp3"); // Click confirmado
    Sound SonidoPierde = LoadSound("Pierde.mp3");       // Efecto al perder
    Sound SonidoGanaste = LoadSound("Ganaste.mp3");     // Efecto al ganar

    // ============================================================================
    // CONFIGURACIÓN DE LA VENTANA PRINCIPAL
    // ============================================================================

    // Inicializamos una ventana de 1024x768 píxeles con un título personalizado
    InitWindow(1024, 768, "Plataformas 2D - TP Integrador, Milesi Williams");

    // Configuramos el framerate deseado
    SetTargetFPS(60);

    // ============================================================================
    // CARGA DE TEXTURAS PRINCIPALES
    // ============================================================================

    Texture2D TexturaFondo = LoadTexture("Fondo.png");              // Fondo general del nivel
    Texture2D TexturaSuelo = LoadTexture("Suelo.png");              // Suelo del escenario (tiles inferiores)
    Texture2D TexturaBoton = LoadTexture("Boton.png");              // Botón base reutilizado para el menú
    Texture2D TexturaTrofeo = LoadTexture("Trofeo.png");            // Trofeo mostrado al ganar
    Texture2D TexturaMarcoFinal = LoadTexture("MarcoFinal.png");    // Marco decorativo para la pantalla de victoria

    // Texturas para pantallas de control, HUD y elementos decorativos
    Texture2D TexturaControles1 = LoadTexture("controles1.png");
    Texture2D TexturaControles2 = LoadTexture("Controles2.png");
    Texture2D TexturaReloj = LoadTexture("Reloj.png");
    Texture2D TexturaPosicion = LoadTexture("posicion.png");
    Texture2D TexturaArbol = LoadTexture("Arbol.png");
    Texture2D TexturaPinchos = LoadTexture("Pinchos.png");
    Texture2D TexturaMarcoPerdiste = LoadTexture("MarcoPerdiste.png");
    Texture2D TexturaFlecha = LoadTexture("Flecha.png");
    Texture2D TexturaSaltos = LoadTexture("Saltos.png");

    // ============================================================================
    // FUENTE PIXELADA PARA TEXTOS DEL HUD Y MENÚ
    // ============================================================================
    
    Font PixelFont = LoadFont("PressStart2P.ttf");             // Cargamos la fuente principal tipo "pixel art"
    SetTextureFilter(PixelFont.texture, TEXTURE_FILTER_POINT); // Forzamos filtrado punto para conservar estética retro

    // ============================================================================
    // VARIABLES DE CONTROL Y ESTADO GENERAL
    // ============================================================================

    // Alternar la visualización de controles con la tecla M
    bool mostrarControles = false;

    // Acumula el tiempo de la partida actual
    float tiempoJugado = 0.0f;

    // Guarda el tiempo logrado cuando se gana o pierde
    float tiempoFinal = 0.0f;

    // Identifica la causa de la derrota
    int motivoPerdida = 0;

    // Control de hover en pantalla PERDISTE
    bool hoverRetryPrev = false;
    bool hoverMenuPrev = false;

    // Escala base de los botones
    float escalaBoton = 0.4f;
    
    // Dimensiones escaladas para los botones del menú
    int W = TexturaBoton.width * escalaBoton;
    int H = TexturaBoton.height * escalaBoton;

    // Rectángulos interactivos del botón PLAY y EXIT
    Rectangle rectPlay = { 512 - W / 2, 400, W, H };    // Botón PLAY centrado
    Rectangle rectExit = { 512 - W / 2, 500, W, H };    // Botón EXIT centrado

    // Estados previos del hover, usados para reproducir sonidos solo una vez
    bool hoverPlayPrev = false;
    bool hoverExitPrev = false;

    // Estado general del juego (menú, jugando, transiciones, etc.)
    EstadoJuego estado = MENU;

    // Temporizador usado en pantallas de transición
    float temporizadorFinal = 0.0f;

    // ============================================================================
    // DEFINICIÓN DEL SUELO PRINCIPAL Y TILEO SUPERIOR
    // ============================================================================

    // Rectángulo físico del piso donde el jugador puede pararse
    Rectangle Suelo = { 0, 640, (float)GetScreenWidth(), 128 };

    // Cálculo dinámico de cuántos tiles se necesitan para cubrir el ancho
    int TilesNecesarios = GetScreenWidth() / 64 + 2;

    // ============================================================================
    // ENTIDADES PRINCIPALES DEL JUGADOR Y PUERTA
    // ============================================================================

    // Instancia principal del jugador
    Player Jugador;

    // Instancia de la puerta final del nivel
    Puerta LaPuerta;

    // Asignamos la fuente pixelada a la puerta para sus diálogos
    LaPuerta.SetFont(PixelFont);

    // ============================================================================
    // PLATAFORMAS DEL NIVEL (OBJETOS ESTÁTICOS)
    // ============================================================================

    // Creamos plataformas con posiciones precisas del diseño original
    Plataforma p1(155, 550);
    Plataforma p2(455, 500);
    Plataforma p3(755, 450);
    Plataforma p4(585, 250);
    Plataforma p5(285, 300);

    // Vector que almacena todas las plataformas para recorrerlas fácilmente
    std::vector<Plataforma*> Plataformas = { &p1, &p2, &p3, &p4, &p5 };

    // ============================================================================
    // CAJAS (OBJETOS ESTÁTICOS)
    // ============================================================================

    // Caja principal usada como superficie y obstáculo
    Caja caja1(865, 350);

    // Vector que permite agregar más cajas sin modificar el código base
    std::vector<Caja*> Cajas = { &caja1 };

    // ============================================================================
    // ENEMIGOS MÓVILES: MURCIÉLAGOS
    // ============================================================================

    // Murciélago en plataformas (movimiento horizontal en un rango definido)
    Enemigo Murcielago(
        585, 200,     // posición inicial
        545, 735      // límites en X para que se mueva de lado a lado
    );
    Enemigo Murcielago2(
        455, 450,
        415, 605
    );

    // ============================================================================
    // BUCLE PRINCIPAL DEL JUEGO (se repite hasta que se cierre la ventana)
    // ============================================================================
    while (!WindowShouldClose())
    {
        UpdateMusicStream(MusicaFondo); // Actualizamos la música en loop
        
        BeginDrawing(); // Iniciamos la etapa de dibujo

        // Alternar visualización de controles presionando M
        if (IsKeyPressed(KEY_M))
            mostrarControles = !mostrarControles;

        // ============================================================================
        // ESTADO: MENÚ PRINCIPAL
        // ============================================================================
        if (estado == MENU)
        {
            DrawTexture(TexturaFondo, 0, 0, WHITE); // Fondo del menú
            Vector2 mouse = GetMousePosition(); // Posición actual del cursor

            // --- TÍTULO PRINCIPAL DEL JUEGO ---
            const char* titulo = "Plataformas 2D - TP Integrador";
            int fontSize = 25;
            float spacing = 1;

            Vector2 sizeTitulo = MeasureTextEx(PixelFont, titulo, fontSize, spacing);

            float tituloX = 512 - sizeTitulo.x / 2;
            float tituloY = 190;

            DrawTextEx(
                PixelFont,
                titulo,
                { tituloX, tituloY },
                fontSize,
                spacing,
                BLACK
            );

            // --- ANIMACIÓN DE BOTONES (cambio de escala según hover) ---  // "Hover" = cuando el cursor está por encima de un botón.
            float escalaNormal = 0.40f;                                     // Cambiamos la escala del botón para dar feedback visual y reproducimos un sonido
            float escalaHover = 0.36f;                                      // solo cuando el mouse entra en su área por primera vez.

            // Hover sobre "PLAY"
            float escalaPlay = CheckCollisionPointRec(mouse, rectPlay)
                ? escalaHover : escalaNormal;

            // Detectar entrada del mouse en PLAY
            bool hoverPlay = CheckCollisionPointRec(mouse, rectPlay);
            if (hoverPlay && !hoverPlayPrev)
                PlaySound(SonidoBoton);   // Reproduce sonido solo al entrar en hover
            
            hoverPlayPrev = hoverPlay;

            // Hover sobre "EXIT"
            float escalaExit = CheckCollisionPointRec(mouse, rectExit)
                ? escalaHover : escalaNormal;

            // Detectar entrada del mouse en EXIT
            bool hoverExit = CheckCollisionPointRec(mouse, rectExit);
            if (hoverExit && !hoverExitPrev)
            {
                PlaySound(SonidoBoton);   // Reproduce sonido solo al entrar en hover
            }
            hoverExitPrev = hoverExit;

            // Recalcular tamaños según la escala actual
            float playW = TexturaBoton.width * escalaPlay;
            float playH = TexturaBoton.height * escalaPlay;
            float exitW = TexturaBoton.width * escalaExit;
            float exitH = TexturaBoton.height * escalaExit;

            // Recalcular posiciones centradas
            float playX = 512 - playW / 2;
            float playY = rectPlay.y;
            float exitX = 512 - exitW / 2;
            float exitY = rectExit.y;

            // --- DIBUJAR BOTÓN PLAY ---
            DrawTextureEx(TexturaBoton, { playX, playY }, 0, escalaPlay, WHITE);

            const char* txtPlay = "PLAY";
            int fsPlay = 28;
            Vector2 szPlay = MeasureTextEx(PixelFont, txtPlay, fsPlay, 1);
            
            // Texto centrado en el botón
            DrawTextEx(
                PixelFont,
                txtPlay,
                { playX + playW / 2 - szPlay.x / 2,
                  playY + playH / 2 - szPlay.y / 2 + 3 },
                fsPlay, 1, WHITE
            );

            // Si clickea PLAY → comienza el juego  
            if (CheckCollisionPointRec(mouse, rectPlay) &&
                IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                PlaySound(SonidoTocaBoton);
                estado = JUGANDO;
                tiempoJugado = 0.0f;
                Jugador.Reiniciar();
                Murcielago.Reiniciar();
                Murcielago2.Reiniciar();
            }

            // --- DIBUJAR BOTÓN EXIT ---
            DrawTextureEx(TexturaBoton, { exitX, exitY }, 0, escalaExit, WHITE);

            const char* txtExit = "EXIT";
            int fsExit = 28;
            Vector2 szExit = MeasureTextEx(PixelFont, txtExit, fsExit, 1);
            
            // Texto centrado en el botón
            DrawTextEx(
                PixelFont,
                txtExit,
                { exitX + exitW / 2 - szExit.x / 2,
                  exitY + exitH / 2 - szExit.y / 2 + 3 },
                fsExit, 1, WHITE
            );

            // Si clickea EXIT → cerrar la ventana
            if (CheckCollisionPointRec(mouse, rectExit) &&
                IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                CloseWindow();
                return 0;
            }

            // Finalizamos el frame
            EndDrawing();
            continue;
        }

        // ============================================================================
        // ESTADO PRINCIPAL DEL JUEGO: JUGANDO
        // ============================================================================

        if (estado == JUGANDO)
        {
            // Cálculo del delta time (con límite para evitar saltos bruscos)
            float dt = GetFrameTime();
            if (dt > 0.03f) dt = 0.03f; // Limitamos dt para evitar saltos bruscos si el juego se traba un frame.

            // Cálculo del delta time (con límite para evitar saltos bruscos)
            Jugador.Update(dt, Suelo, Plataformas, Cajas);
            LaPuerta.IntAbrir(Jugador.GetRect());

            // Sumamos tiempo total jugado
            tiempoJugado += dt;

            // --- Perder por saltos ---
            if (Jugador.ContadorSaltos > 10)
            {
                motivoPerdida = 1;              // Saltaste demasiado
                PlaySound(SonidoPierde);        // Sonido de derrota
                estado = TRANSICION_PERDISTE;   // Lleva a la transicion de pantalla
                temporizadorFinal = 0.0f;
                tiempoFinal = tiempoJugado;
            }

            // --- Perder por límite de tiempo ---
            if (tiempoJugado >= 20.0f)
            {
                motivoPerdida = 2;
                PlaySound(SonidoPierde);
                estado = TRANSICION_PERDISTE;
                temporizadorFinal = 0.0f;
                tiempoFinal = tiempoJugado;
            }

            // --- Condiciín de victoria (abrir la puerta) ---
            if (LaPuerta.EstaAbierta)
            {
                PlaySound(SonidoGanaste);
                estado = TRANSICION_GANASTE;
                temporizadorFinal = 0.0f;
                tiempoFinal = tiempoJugado;
            }

            // --- Reiniciar la partida (tecla R) ---
            if (IsKeyPressed(KEY_R))
            {
                PlaySound(SonidoTocaBoton);
                Jugador.Reiniciar();
                Murcielago.Reiniciar();
                Murcielago2.Reiniciar();
                LaPuerta.EstaAbierta = false;
                tiempoJugado = 0.0f;
            }

            // ACTUALIZACIÓN DE ENEMIGOS
            Murcielago.Update(dt);
            Murcielago2.Update(dt);

            // DIBUJADO DEL ESCENARIO BASE
            EscenarioBase(
                TexturaFondo,
                TexturaSuelo,
                TexturaPinchos,
                TexturaArbol,
                Plataformas,
                Cajas,
                TilesNecesarios
            );

            // DIBUJO DE CONTROLES EN PANTALLA
            DrawTextureEx(TexturaControles1, { 20, 690 }, 0, 0.08f, WHITE);

            if (mostrarControles)
                DrawTextureEx(TexturaControles2, { 20, 200 }, 0, 0.1f, WHITE);

            // HUD COMPLETO: tiempo, posición, saltos
            DibujarHUD(
                PixelFont,
                TexturaReloj,
                TexturaPosicion,
                TexturaSaltos,
                tiempoJugado,
                Jugador
            );

            // ENEMIGOS + JUGADOR + PUERTA (orden correcto de renderizado)
            Murcielago.Draw();
            Murcielago2.Draw();
            LaPuerta.Draw();
            Jugador.Draw();

            // DETECCIÓN DE COLISIONES (trampas + murciélagos)
            Rectangle rectJugador = Jugador.GetRect();
            Rectangle rectPinchos = { 5 * 64, 623, (TilesNecesarios - 5) * 64, TexturaPinchos.height }; // Definimos el rectángulo de pinchos del suelo (comienza en tile 5).

            // Murciélago 1 o 2 → pérdida inmediata
            bool golpeMurcielago =
                CheckCollisionRecs(Jugador.GetRect(), Murcielago.GetRect()) ||
                CheckCollisionRecs(Jugador.GetRect(), Murcielago2.GetRect());

            // --- Perder por contacto con murcielagos ---
            if (golpeMurcielago)
            {
                motivoPerdida = 3;
                PlaySound(SonidoPierde);
                estado = TRANSICION_PERDISTE;
                temporizadorFinal = 0.0f;
                tiempoFinal = tiempoJugado;
            }

            // --- Perder por contacto con pinchos ---
            if (CheckCollisionRecs(rectJugador, rectPinchos))
            {
                motivoPerdida = 4;
                PlaySound(SonidoPierde);
                estado = TRANSICION_PERDISTE;
                temporizadorFinal = 0.0f;
                tiempoFinal = tiempoJugado;
            }

            // Finalizamos el frame
            EndDrawing();
            continue;
        }

        // ============================================================================
        // ESTADO: TRANSICIÓN HACIA LA PANTALLA DE VICTORIA
        // ============================================================================
        
        if (estado == TRANSICION_GANASTE)   // Esta fase muestra una animación breve antes de pasar a la pantalla GANASTE.
        {
            // Tiempo transcurrido en esta transición
            float dt = GetFrameTime();
            temporizadorFinal += dt;

            // --- ESCENARIO COMPLETO DEL NIVEL ---
            // Se dibuja el fondo, el suelo, los pinchos, plataformas y caja, tal como en
            // el estado JUGANDO, usando la función modular del archivo Escenarios.cpp.
            // El jugador NO aparece durante esta transición.
            EscenarioBase(
                TexturaFondo,
                TexturaSuelo,
                TexturaPinchos,
                TexturaArbol,
                Plataformas,
                Cajas,
                TilesNecesarios
            );

            // --- SE DIBUJA ÚNICAMENTE LA PUERTA ---
            LaPuerta.Draw();    // La puerta permanece visible para reforzar que se activó antes de ganar.

            // --- AVANCE AUTOMÁTICO A LA PANTALLA DE VICTORIA ---
            if (temporizadorFinal >= 1.0f)  // Pasados 1.0 segundos, se cambia al estado GANASTE.
                estado = GANASTE;

            // Finalizamos el dibujo de este frame y volvemos al loop
            EndDrawing();
            continue;
        }

        // ============================================================================
        // ESTADO: PANTALLA DE LA VICTORIA
        // ============================================================================

        if (estado == GANASTE)
        {
            // --- ESCENARIO DE VICTORIA ---
            // Se dibuja el fondo y un suelo normal (sin pinchos, sin árbol)
            EscenarioFinalGanaste(
                TexturaFondo,
                TexturaSuelo,
                TilesNecesarios
            );

            // MARCO DECORATIVO DE LA PANTALLA GANADORA
            float marcoX = 525 - TexturaMarcoFinal.width / 2;
            float marcoY = 90;
            DrawTexture(TexturaMarcoFinal, marcoX, marcoY, WHITE);

            // --- TEXTO PRINCIPAL "GANASTE!" ---
            // Con sombra para darle efecto visual más destacado
            const char* msg = "GANASTE!";
            int fontSize = 45;
            float spacing = 1;

            Vector2 size = MeasureTextEx(PixelFont, msg, fontSize, spacing);
            float textoX = 525 - size.x / 2;
            float textoY = marcoY + 185;

            Color AmarilloClaro = { 255, 255, 150, 255 };
            Color NaranjaOscuro = { 255, 140, 0, 255 };

            DrawTextEx(PixelFont, msg, { textoX, textoY - 3 }, fontSize, spacing, AmarilloClaro);
            DrawTextEx(PixelFont, msg, { textoX, textoY + 3 }, fontSize, spacing, NaranjaOscuro);
            DrawTextEx(PixelFont, msg, { textoX, textoY }, fontSize, spacing, YELLOW);

            // TROFEO CENTRAL (solo decorativo)
            float trofeoX = 512 - TexturaTrofeo.width / 2;
            float trofeoY = 640 - TexturaTrofeo.height;
            DrawTexture(TexturaTrofeo, trofeoX, trofeoY, WHITE);

            // DIBUJAR PLAYER PARADO EN LA ESCENA DE VICTORIA (Siempre aparece en la esquina inferior izquierda es decorativo)
            Vector2 posCab = { 25.0f, 568.0f };
            DrawTextureEx(Jugador.TexturaCaballero, posCab, 0, Jugador.Escala, WHITE);

            // CUADRO DE DIÁLOGO FINAL — INVERTIDO HORIZONTALMENTE (Acompaña al personaje cuando completa el nivel)
            float escalaDialogo = 0.40f;
            float dw = LaPuerta.TextDialogo.width * escalaDialogo;
            float dh = LaPuerta.TextDialogo.height * escalaDialogo;

            Vector2 posDialogo;
            posDialogo.x = posCab.x - 20;
            posDialogo.y = posCab.y - dh + 10;

            Rectangle src = {
                (float)LaPuerta.TextDialogo.width,  // Volteado
                0,
                -(float)LaPuerta.TextDialogo.width,
                (float)LaPuerta.TextDialogo.height
            };

            Rectangle dst = {
                posDialogo.x,
                posDialogo.y,
                LaPuerta.TextDialogo.width * escalaDialogo,
                LaPuerta.TextDialogo.height * escalaDialogo
            };

            DrawTexturePro(LaPuerta.TextDialogo, src, dst, { 0,0 }, 0, WHITE);

            // MENSAJE FINAL (volver al menú)
            const char* textoFin = "Presiona R para volver al menu";
            int fs = 8;
            Vector2 ts = MeasureTextEx(PixelFont, textoFin, fs, 1);

            Vector2 posTexto = {
                posDialogo.x + dw / 2 - ts.x / 2,
                posDialogo.y + dh / 2 - ts.y / 2 - 10
            };

            DrawTextEx(PixelFont, textoFin, posTexto, fs, 1, BLACK);

            // TIEMPO FINAL FORMATEADO (MM:SS)
            int sFin = (int)tiempoFinal;
            int cFin = (int)((tiempoFinal - sFin) * 100.0f);
            if (cFin < 0) cFin = 0;
            if (cFin > 99) cFin = 99;

            const char* tiempoFinalStr = TextFormat("%02i:%02i", sFin, cFin);

            DrawTextureEx(TexturaReloj, { 810, 10 }, 0, 0.15f, WHITE);
            DrawTextEx(PixelFont, tiempoFinalStr, { 860, 25 }, 25, 1, BLACK);

            // REINICIAR DESDE LA PANTALLA DE GANASTE (R vuelve al menú principal y reinicia entidades)
            if (IsKeyPressed(KEY_R))
            {
                PlaySound(SonidoTocaBoton);
                LaPuerta.EstaAbierta = false;
                estado = MENU;
                tiempoJugado = 0.0f;
                Jugador.Reiniciar();
                Murcielago.Reiniciar();
                Murcielago2.Reiniciar();
            }

            // Finalizamos el frame
            EndDrawing();
            continue;
        }

        // ============================================================================
        // ESTADO: TRANSICIÓN A PANTALLA DE DERROTA
        // ============================================================================
        
        if (estado == TRANSICION_PERDISTE)  // Esta fase muestra el escenario estático durante 1 segundo antes de pasar a la pantalla PERDISTE.
        {                                   // La puerta se fuerza a estar cerrada y el jugador NO se dibuja (para reforzar el impacto visual de la derrota).
            float dt = GetFrameTime();
            temporizadorFinal += dt;        // Avanzamos la cuenta para la transición

            // --- ESCENARIO COMPLETO (fondo + suelo + pinchos + plataformas + caja) ---
            // Se usa el mismo escenario base del juego, pero congelado y sin jugador.
            EscenarioBase(
                TexturaFondo,
                TexturaSuelo,
                TexturaPinchos,
                TexturaArbol,
                Plataformas,
                Cajas,
                TilesNecesarios
            );


            // --- PUERTA FORZADA A ESTAR CERRADA ---
            LaPuerta.EstaAbierta = false;   // Durante esta transición la puerta nunca debe abrirse
            LaPuerta.Draw();

            // --- AVANZAMOS A LA PANTALLA PERDISTE LUEGO DE 1 SEGUNDO ---
            if (temporizadorFinal >= 1.0f)
                estado = PERDISTE;

            // Finalizamos el dibujo de este frame y volvemos al loop
            EndDrawing();
            continue;
        }

        // ============================================================================
        // ESTADO: PERDISTE
        // ============================================================================
        // Se ejecuta cuando el jugador pierde por cualquiera de los motivos:
        // tiempo agotado, exceso de saltos, colisión con pinchos o con murciélagos.
        // Muestra cartel, motivo y botones para reintentar o ir al menú.
        // ============================================================================
        if (estado == PERDISTE)
        {
            // --- ESCENARIO PERDISTE ---
            // El fondo se mantiene igual, el suelo completo es remplazado por pinchos decorativos.
            EscenarioFinalPerdiste(
                TexturaFondo,
                TexturaPinchos,
                TilesNecesarios
            );

            // --- MARCO Y TEXTO "PERDISTE" ---
            float marcoX = 525 - TexturaMarcoPerdiste.width / 2;
            float marcoY = 10;
            DrawTexture(TexturaMarcoPerdiste, marcoX, marcoY, WHITE);

            const char* msg = "PERDISTE";
            int fontSize = 40;
            float spacing = 1;

            Vector2 size = MeasureTextEx(PixelFont, msg, fontSize, spacing);

            float textoX = 520 - size.x / 2;
            float textoY = marcoY + 185;

            // Colores con sombras para efecto visual
            Color RojoSombra = { 120, 0, 0, 255 };
            Color RojoLuz = { 255, 180, 180, 255 };
            Color RojoPrincipal = { 255, 50, 50, 255 };

            DrawTextEx(PixelFont, msg, { textoX, textoY + 3 }, fontSize, spacing, RojoSombra);
            DrawTextEx(PixelFont, msg, { textoX, textoY - 3 }, fontSize, spacing, RojoLuz);
            DrawTextEx(PixelFont, msg, { textoX, textoY }, fontSize, spacing, RojoPrincipal);


            // --- MOTIVO DE LA DERROTA (dependiendo del tipo de pérdida) ---
            const char* msgMotivo = "";

            if (motivoPerdida == 1)      msgMotivo = "Saltaste demasiado!";
            else if (motivoPerdida == 2) msgMotivo = "Te quedaste sin tiempo!";
            else if (motivoPerdida == 3) msgMotivo = "Te mordio un murcielago!";
            else if (motivoPerdida == 4) msgMotivo = "Caiste en las trampas!";

            int fontMotivo = 18;
            Vector2 sizeMotivo = MeasureTextEx(PixelFont, msgMotivo, fontMotivo, 1);

            float motivoX = 512 - sizeMotivo.x / 2;
            float motivoY = textoY + 110;

            DrawTextEx(PixelFont, msgMotivo, { motivoX, motivoY }, fontMotivo, 1, BLACK);


            // --- BOTONES: REINTENTAR y MENU ---
            const char* tRetry = "REINTENTAR";
            const char* tMenu = "MENU";

            float fontBoton = 22;

            // Posiciones centradas
            Vector2 posRetry = {
                512 - MeasureTextEx(PixelFont, tRetry, fontBoton, 1).x / 2,
                420
            };

            Vector2 posMenu = {
                512 - MeasureTextEx(PixelFont, tMenu, fontBoton, 1).x / 2,
                470
            };

            // rectRetry - rectMenu definen las áreas clickeables
            Rectangle rectRetry = { posRetry.x, posRetry.y, 250, 35 };
            Rectangle rectMenu = { posMenu.x,  posMenu.y,  250, 35 };

            Vector2 mouse = GetMousePosition();

            bool hoverRetry = CheckCollisionPointRec(mouse, rectRetry);
            bool hoverMenu = CheckCollisionPointRec(mouse, rectMenu);

            // Sonido al entrar por primera vez al hover de cada botón
            if (hoverRetry && !hoverRetryPrev) PlaySound(SonidoBoton);
            if (hoverMenu && !hoverMenuPrev) PlaySound(SonidoBoton);
   
            // Guardar estados del frame anterior
            hoverRetryPrev = hoverRetry;
            hoverMenuPrev = hoverMenu;

            // Dibujar texto de botones
            DrawTextEx(PixelFont, tRetry, posRetry, fontBoton, 1, BLACK);
            DrawTextEx(PixelFont, tMenu, posMenu, fontBoton, 1, BLACK);

            // Flecha animada cuando el mouse pasa por encima
            if (hoverRetry)
            {
                float escF = 0.25f;
                float fx = posRetry.x - (TexturaFlecha.width * escF) - 10;
                float fy = posRetry.y - 2;
                DrawTextureEx(TexturaFlecha, { fx, fy }, 0, escF, WHITE);
            }

            if (hoverMenu)
            {
                float escF = 0.25f;
                float fx = posMenu.x - (TexturaFlecha.width * escF) - 10;
                float fy = posMenu.y - 2;
                DrawTextureEx(TexturaFlecha, { fx, fy }, 0, escF, WHITE);
            }


            // --- ACCIONES DE BOTONES ---
            // → REINTENTAR: Reinicia jugador, murciélagos y estado
            if (hoverRetry && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                PlaySound(SonidoTocaBoton);
                LaPuerta.EstaAbierta = false;
                tiempoJugado = 0.0f;
                Jugador.Reiniciar();
                Murcielago.Reiniciar();
                Murcielago2.Reiniciar();
                estado = JUGANDO;
            }

            // → VOLVER AL MENU
            if (hoverMenu && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                PlaySound(SonidoTocaBoton);
                LaPuerta.EstaAbierta = false;
                tiempoJugado = 0.0f;
                Jugador.Reiniciar();
                Murcielago.Reiniciar();
                Murcielago2.Reiniciar();
                estado = MENU;
            }

            // Finalizamos el frame
            EndDrawing();
            continue;
        }

        EndDrawing();
    }

    // ============================================================================
    // LIBERACIÓN DE TEXTURAS UTILIZADAS EN TODO EL JUEGO
    // ============================================================================
    UnloadTexture(TexturaFondo);
    UnloadTexture(TexturaBoton);
    UnloadTexture(TexturaTrofeo);
    UnloadTexture(TexturaMarcoFinal);
    UnloadTexture(TexturaSuelo);
    UnloadTexture(TexturaControles1);
    UnloadTexture(TexturaControles2);
    UnloadTexture(TexturaReloj);
    UnloadTexture(TexturaPosicion);
    UnloadTexture(TexturaArbol);
    UnloadTexture(TexturaPinchos);
    UnloadTexture(TexturaMarcoPerdiste);
    UnloadTexture(TexturaFlecha);
    UnloadTexture(TexturaSaltos);
    
    // ============================================================================
    // LIBERACIÓN DE FUENTES
    // ============================================================================
    UnloadFont(PixelFont);

    // ============================================================================
    // LIBERACIÓN DE SONIDOS Y MÚSICA
    // ============================================================================
    UnloadSound(SonidoBoton);
    UnloadSound(SonidoTocaBoton);
    UnloadSound(SonidoPierde);
    UnloadSound(SonidoGanaste);
    UnloadMusicStream(MusicaFondo);

    // Cerramos la ventana y liberamos recursos
    CloseWindow();

    return 0;
}