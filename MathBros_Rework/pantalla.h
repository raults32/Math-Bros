#ifndef PANTALLA_H
#define PANTALLA_H

#include <raylib.h>
#include <math.h>

// El juego siempre se dibuja a 1280x720 en un lienzo interno, que luego se escala
// a la ventana (manteniendo la proporción con franjas negras) para poder maximizarla.
#define PANTALLA_ANCHO 1280
#define PANTALLA_ALTO 720

static RenderTexture2D gLienzo = {0};
static float gEscala = 1.0f;
static Vector2 gDesplazamiento = {0.0f, 0.0f};

static inline void InicializarPantalla(void)
{
    gLienzo = LoadRenderTexture(PANTALLA_ANCHO, PANTALLA_ALTO);
    SetTextureFilter(gLienzo.texture, TEXTURE_FILTER_BILINEAR);
    SetWindowMinSize(PANTALLA_ANCHO / 2, PANTALLA_ALTO / 2);
}

static inline void CerrarPantalla(void)
{
    UnloadRenderTexture(gLienzo);
    SetMouseOffset(0, 0);
    SetMouseScale(1.0f, 1.0f);
}

// Sustituye a BeginDrawing(): todo lo que se dibuje después va al lienzo de 1280x720
static inline void ComenzarDibujo(void)
{
    // F11 o Alt+Enter: alternar pantalla completa
    bool alt = IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT);
    if (IsKeyPressed(KEY_F11) || (alt && IsKeyPressed(KEY_ENTER)))
        ToggleBorderlessWindowed();

    float anchoVentana = (float)GetScreenWidth();
    float altoVentana = (float)GetScreenHeight();
    gEscala = fminf(anchoVentana / PANTALLA_ANCHO, altoVentana / PANTALLA_ALTO);
    if (gEscala <= 0.0f) gEscala = 1.0f;
    gDesplazamiento.x = (anchoVentana - PANTALLA_ANCHO * gEscala) / 2.0f;
    gDesplazamiento.y = (altoVentana - PANTALLA_ALTO * gEscala) / 2.0f;

    // El ratón se traduce a coordenadas del lienzo para que los botones sigan funcionando
    SetMouseOffset((int)-gDesplazamiento.x, (int)-gDesplazamiento.y);
    SetMouseScale(1.0f / gEscala, 1.0f / gEscala);

    BeginTextureMode(gLienzo);
}

// Sustituye a EndDrawing(): escala el lienzo a la ventana real
static inline void TerminarDibujo(void)
{
    EndTextureMode();

    BeginDrawing();
    ClearBackground(BLACK);
    Rectangle origen = {0.0f, 0.0f, (float)PANTALLA_ANCHO, -(float)PANTALLA_ALTO};
    Rectangle destino = {gDesplazamiento.x, gDesplazamiento.y, PANTALLA_ANCHO * gEscala, PANTALLA_ALTO * gEscala};
    DrawTexturePro(gLienzo.texture, origen, destino, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
    EndDrawing();
}

#endif
