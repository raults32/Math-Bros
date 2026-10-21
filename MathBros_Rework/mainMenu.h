#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <raylib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "halloween.h"

// Paleta de colores estilo Super Mario Bros
#define MARIO_RED      (Color){230, 35, 35, 255}
#define MARIO_GREEN    (Color){40, 185, 55, 255}
#define MARIO_BLUE     (Color){30, 145, 250, 255}
#define MARIO_YELLOW   (Color){255, 215, 0, 255}
#define MARIO_ORANGE   (Color){255, 130, 20, 255}
#define MARIO_PURPLE   (Color){155, 60, 220, 255}
#define MARIO_BRICK    (Color){185, 75, 30, 255}

// Prototipos
void menuTitulo(const char textotitulo[]);
bool btnsMenu(const char textoBoton[], int rectY);
bool DrawMarioButton(Rectangle rect, const char *texto, Color baseColor, Color hoverColor, int fontSize);
void DrawMarioPanel(Rectangle rect, Color bodyColor, Color borderColor);
void DrawMarioBackground(Texture2D bgTex, int theme, float scrollX);
void fondoMenu(Texture2D fondoTexture);
Texture2D LoadTextureSmart(const char *path, int targetWidth, int targetHeight);

// Cargador inteligente que prueba .png, .jpg y .bmp para que nunca falte una imagen
Texture2D LoadTextureSmart(const char *path, int targetWidth, int targetHeight)
{
    // Si el formato no esta soportado (p. ej. raylib compilada sin JPG en macOS),
    // tambien se intenta con la siguiente extension.
    Image img = {0};
    if (FileExists(path))
    {
        img = LoadImage(path);
    }

    char altPath[128] = {0};
    strncpy(altPath, path, sizeof(altPath) - 1);
    char *dot = strrchr(altPath, '.');
    const char *extensiones[] = {".png", ".jpg", ".bmp"};
    for (int i = 0; dot != NULL && img.data == NULL && i < 3; i++)
    {
        strcpy(dot, extensiones[i]);
        if (strcmp(altPath, path) != 0 && FileExists(altPath)) img = LoadImage(altPath);
    }

    if (img.data != NULL)
    {
        if (targetWidth > 0 && targetHeight > 0)
            ImageResize(&img, targetWidth, targetHeight);
        Texture2D tex = LoadTextureFromImage(img);
        UnloadImage(img);
        return tex;
    }

    Texture2D empty = {0};
    return empty;
}

// Dibuja un panel estilo bloque de Super Mario con remaches en las esquinas
void DrawMarioPanel(Rectangle rect, Color bodyColor, Color borderColor)
{
    Rectangle shadowRect = {rect.x + 6, rect.y + 8, rect.width, rect.height};
    DrawRectangleRounded(shadowRect, 0.15f, 8, Fade(BLACK, 0.50f));

    Rectangle outerRect = {rect.x - 4, rect.y - 4, rect.width + 8, rect.height + 8};
    DrawRectangleRounded(outerRect, 0.15f, 8, BLACK);
    DrawRectangleRounded(rect, 0.15f, 8, borderColor);

    Rectangle innerRect = {rect.x + 5, rect.y + 5, rect.width - 10, rect.height - 10};
    DrawRectangleRounded(innerRect, 0.12f, 8, bodyColor);

    // Franja superior de brillo
    Rectangle glossRect = {innerRect.x + 6, innerRect.y + 4, innerRect.width - 12, innerRect.height * 0.25f};
    DrawRectangleRounded(glossRect, 0.3f, 6, Fade(WHITE, 0.22f));

    // 4 remaches estilo bloque [?] de Mario
    int boltOffset = 14;
    Vector2 bolts[4] = {
        {rect.x + boltOffset, rect.y + boltOffset},
        {rect.x + rect.width - boltOffset, rect.y + boltOffset},
        {rect.x + boltOffset, rect.y + rect.height - boltOffset},
        {rect.x + rect.width - boltOffset, rect.y + rect.height - boltOffset}};

    for (int i = 0; i < 4; i++)
    {
        DrawCircle((int)bolts[i].x + 1, (int)bolts[i].y + 1, 5, BLACK);
        DrawCircle((int)bolts[i].x, (int)bolts[i].y, 4, GOLD);
        DrawCircle((int)bolts[i].x - 1, (int)bolts[i].y - 1, 1.5f, WHITE);
    }
}

// Botón interactivo estilo Mario Bros con rebote, escala, bisel 3D y monedas animadas
bool DrawMarioButton(Rectangle rect, const char *texto, Color baseColor, Color hoverColor, int fontSize)
{
    Vector2 mousePoint = GetMousePosition();
    bool hover = CheckCollisionPointRec(mousePoint, rect);
    bool pressed = hover && IsMouseButtonDown(MOUSE_LEFT_BUTTON);
    bool clicked = hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    float time = (float)GetTime();
    float scale = hover ? (1.06f + sinf(time * 9.0f) * 0.02f) : 1.0f;
    if (pressed) scale = 0.97f;

    float bounceY = hover ? (sinf(time * 8.0f) * 3.5f) : 0.0f;
    float pressOffsetY = pressed ? 5.0f : 0.0f;

    float w = rect.width * scale;
    float h = rect.height * scale;
    float x = rect.x - (w - rect.width) / 2.0f;
    float y = rect.y - (h - rect.height) / 2.0f + bounceY + pressOffsetY;

    // Sombra 3D inferior
    if (!pressed)
    {
        Rectangle shadow = {x + 4, y + 8, w, h};
        DrawRectangleRounded(shadow, 0.35f, 10, Fade(BLACK, 0.55f));
    }

    // Borde negro grueso estilo caricatura
    Rectangle blackBorder = {x - 4, y - 4, w + 8, h + 8};
    DrawRectangleRounded(blackBorder, 0.38f, 10, BLACK);

    // Borde interior dorado si el cursor está encima
    Color rimColor = hover ? MARIO_YELLOW : WHITE;
    Rectangle rimRect = {x - 1, y - 1, w + 2, h + 2};
    DrawRectangleRounded(rimRect, 0.36f, 10, rimColor);

    // Cuerpo principal del botón
    Color fill = hover ? hoverColor : baseColor;
    Rectangle mainRect = {x, y, w, h};
    DrawRectangleRounded(mainRect, 0.35f, 10, fill);

    // Bisel inferior oscuro
    Rectangle bottomShade = {x + 4, y + h * 0.65f, w - 8, h * 0.30f};
    DrawRectangleRounded(bottomShade, 0.4f, 8, Fade(BLACK, 0.22f));

    // Brillo superior blanco
    Rectangle topGloss = {x + 8, y + 5, w - 16, h * 0.32f};
    DrawRectangleRounded(topGloss, 0.5f, 8, Fade(WHITE, 0.38f));

    // Remaches laterales estilo bloque de Mario
    DrawCircle((int)(x + 14), (int)(y + h / 2), 5, BLACK);
    DrawCircle((int)(x + 14), (int)(y + h / 2), 3.5f, hover ? MARIO_YELLOW : LIGHTGRAY);
    DrawCircle((int)(x + w - 14), (int)(y + h / 2), 5, BLACK);
    DrawCircle((int)(x + w - 14), (int)(y + h / 2), 3.5f, hover ? MARIO_YELLOW : LIGHTGRAY);

    // Texto con borde negro y sombra
    int scaledFont = (int)(fontSize * scale);
    int textWidth = MeasureText(texto, scaledFont);
    int textX = (int)(x + (w - textWidth) / 2.0f);
    int textY = (int)(y + (h - scaledFont) / 2.0f);

    DrawText(texto, textX + 3, textY + 3, scaledFont, BLACK);
    DrawText(texto, textX - 1, textY - 1, scaledFont, BLACK);
    DrawText(texto, textX, textY, scaledFont, hover ? MARIO_YELLOW : WHITE);

    // Cursores animados estilo moneda de Mario cuando estás sobre el botón
    if (hover)
    {
        float coinSpin = fabsf(cosf(time * 7.0f));
        float coinW = 12.0f * coinSpin + 3.0f;
        int coinY = (int)(y + h / 2);

        // Moneda izquierda
        DrawEllipse((int)(x - 24), coinY, coinW + 2, 14, BLACK);
        DrawEllipse((int)(x - 24), coinY, coinW, 12, MARIO_YELLOW);
        DrawRectangle((int)(x - 25), coinY - 6, 2, 12, ORANGE);

        // Moneda derecha
        DrawEllipse((int)(x + w + 24), coinY, coinW + 2, 14, BLACK);
        DrawEllipse((int)(x + w + 24), coinY, coinW, 12, MARIO_YELLOW);
        DrawRectangle((int)(x + w + 23), coinY - 6, 2, 12, ORANGE);
    }

    return clicked;
}

// Título multicolor letra por letra estilo Super Mario Bros
void menuTitulo(const char textotitulo[])
{
    int fontSize = 96;
    int len = (int)strlen(textotitulo);
    int spacing = 6;
    int totalWidth = 0;

    char temp[2] = {0, 0};
    for (int i = 0; i < len; i++)
    {
        temp[0] = textotitulo[i];
        totalWidth += MeasureText(temp, fontSize) + spacing;
    }

    int startX = (1280 - totalWidth) / 2;
    int baseY = 42;

#if MODO_HALLOWEEN
    Color marioPalette[5] = {HW_ORANGE, HW_PURPLE, HW_SLIME, HW_ORANGE, (Color){235, 235, 255, 255}};
#else
    Color marioPalette[5] = {MARIO_RED, MARIO_GREEN, MARIO_YELLOW, MARIO_BLUE, MARIO_ORANGE};
#endif

    int curX = startX;
    float time = (float)GetTime();

    for (int i = 0; i < len; i++)
    {
        temp[0] = textotitulo[i];
        int charW = MeasureText(temp, fontSize);

        if (temp[0] != ' ')
        {
            float waveY = sinf(time * 4.8f + i * 0.55f) * 7.0f;
            int drawY = baseY + (int)waveY;
            Color c = marioPalette[i % 5];

            // Extrusión 3D negra gruesa
            for (int ox = -4; ox <= 4; ox += 2)
            {
                for (int oy = -4; oy <= 4; oy += 2)
                {
                    DrawText(temp, curX + ox, drawY + oy, fontSize, BLACK);
                }
            }
            DrawText(temp, curX + 6, drawY + 8, fontSize, Fade(BLACK, 0.65f));
            DrawText(temp, curX, drawY, fontSize, c);
            DrawText(temp, curX, drawY - 2, fontSize, Fade(WHITE, 0.25f));
        }
        curX += charW + spacing;
    }
}

// Motor de fondos estilo Super Mario Bros (Garantiza fondo en Nivel 3 y en todas las pantallas)
// theme: 0 = Menú/Niveles, 1 = Nivel 1 (Pradera), 2 = Nivel 2 (Cañón), 3 = Nivel 3 (Castillo Jefe), 4 = Tienda
void DrawMarioBackground(Texture2D bgTex, int theme, float scrollX)
{
    const int sw = 1280;
    const int sh = 720;
    float time = (float)GetTime();

    // 1. Cielo degradado según el tema
    if (theme == 3)
    {
        // Tema Nivel 3: Castillo / Fortaleza del Guardián (Aunque no exista nivel3.jpg se ve increíble)
        DrawRectangleGradientV(0, 0, sw, sh, (Color){25, 10, 40, 255}, (Color){115, 25, 20, 255});
    }
    else if (theme == 4)
    {
        // Tema Tienda: Casa de Toad / Estrellas
        DrawRectangleGradientV(0, 0, sw, sh, (Color){20, 35, 95, 255}, (Color){85, 40, 140, 255});
    }
    else if (theme == 2)
    {
        // Tema Nivel 2: Atardecer en el Cañón
        DrawRectangleGradientV(0, 0, sw, sh, (Color){45, 115, 210, 255}, (Color){245, 165, 95, 255});
    }
    else
    {
        // Tema Menú / Nivel 1: Reino Champiñón soleado
        DrawRectangleGradientV(0, 0, sw, sh, (Color){65, 155, 255, 255}, (Color){175, 230, 255, 255});
    }

    // 2. Si la textura original cargó bien, dibujarla con soporte de parallax
    if (bgTex.id != 0)
    {
        Color tint = (theme == 3) ? (Color){255, 200, 200, 225} : WHITE;
        DrawTexture(bgTex, 0, 0, tint);
    }

    // 3. Elementos decorativos animados estilo Super Mario según el escenario
    float parallaxSlow = fmodf(scrollX * 0.18f, (float)sw);
    if (parallaxSlow < 0) parallaxSlow += sw;

    float parallaxMid = fmodf(scrollX * 0.35f, (float)sw);
    if (parallaxMid < 0) parallaxMid += sw;

    if (theme == 3)
    {
        // Decoración Fortaleza de Bowser / Guardián para Nivel 3
        for (int i = -1; i < 6; i++)
        {
            int cx = (int)(i * 280 - parallaxSlow);
            // Torres del castillo de fondo
            DrawRectangle(cx, 260, 160, 460, (Color){40, 25, 45, 220});
            DrawRectangleLinesEx((Rectangle){(float)cx, 260, 160, 460}, 3.0f, (Color){20, 10, 25, 255});
            // Almenas del castillo
            for (int b = 0; b < 4; b++)
            {
                DrawRectangle(cx + b * 42, 235, 30, 25, (Color){50, 30, 55, 230});
            }
            // Ventanas encendidas por la lava
            DrawRectangle(cx + 55, 320, 50, 80, (Color){220, 80, 20, 180});
            DrawCircle(cx + 80, 320, 25, (Color){220, 80, 20, 180});
        }

        // Resplandor de lava hirviendo en el fondo inferior y brasas subiendo
        float lavaWave = sinf(time * 4.0f) * 8.0f;
        DrawRectangleGradientV(0, 580 + (int)lavaWave, sw, 150, Fade(MARIO_ORANGE, 0.55f), Fade(MARIO_RED, 0.85f));
        for (int e = 0; e < 18; e++)
        {
            float ex = fmodf((float)(e * 97) + time * 35.0f, (float)sw);
            float ey = 680.0f - fmodf((float)(e * 53) + time * (55.0f + (e % 4) * 15.0f), 580.0f);
            float r = 3.0f + (e % 4);
            DrawCircle((int)ex, (int)ey, r, Fade(MARIO_YELLOW, 0.75f));
            DrawCircle((int)ex, (int)ey, r + 3.0f, Fade(MARIO_ORANGE, 0.35f));
        }
    }
    else if (theme == 4)
    {
        // Decoración Tienda de Personajes: Estrellas doradas flotantes y cortinas reales
        for (int s = 0; s < 24; s++)
        {
            float sx = fmodf((float)(s * 73) + time * 20.0f, (float)sw);
            float sy = 80.0f + (s % 6) * 95.0f + sinf(time * 3.0f + s) * 12.0f;
            float rad = 4.0f + sinf(time * 5.0f + s) * 2.0f;
            DrawCircle((int)sx, (int)sy, rad, Fade(MARIO_YELLOW, 0.75f));
        }
        // Piso ajedrezado estilo Mario Bros 3
        for (int x = 0; x < sw; x += 64)
        {
            Color tileCol = ((x / 64) % 2 == 0) ? (Color){40, 25, 75, 220} : (Color){240, 220, 170, 200};
            DrawRectangle(x, 650, 64, 70, tileCol);
        }
        DrawRectangle(0, 646, sw, 6, GOLD);
    }
    else
    {
        // Nubes sonrientes estilo Super Mario y colinas redondeadas
        for (int i = -1; i < 5; i++)
        {
            float cloudX = i * 330.0f - fmodf(time * 22.0f + parallaxSlow, 330.0f);
            float cloudY = 95.0f + (i % 2) * 55.0f + sinf(time * 2.0f + i) * 6.0f;

            DrawCircle((int)cloudX, (int)cloudY, 34, Fade(WHITE, 0.90f));
            DrawCircle((int)cloudX + 28, (int)cloudY - 10, 40, Fade(WHITE, 0.90f));
            DrawCircle((int)cloudX + 60, (int)cloudY, 32, Fade(WHITE, 0.90f));
            // Ojos clásicos de las nubes de Mario
            DrawRectangle((int)cloudX + 20, (int)cloudY - 16, 4, 12, BLACK);
            DrawRectangle((int)cloudX + 34, (int)cloudY - 16, 4, 12, BLACK);
        }

        // Si el fondo de imagen no está o estamos en el menú, dibujar colinas y bloques decorativos
        for (int h = -1; h < 5; h++)
        {
            int hx = (int)(h * 380 - parallaxMid);
            Color hillColor = (theme == 2) ? (Color){180, 95, 50, 140} : (Color){45, 175, 65, 145};
            DrawCircle(hx + 160, 660, 150, hillColor);
            DrawCircleLines(hx + 160, 660, 150, Fade(BLACK, 0.35f));
        }

        // En el menú principal y selector de niveles, agregar bloques [?] flotantes y suelo de ladrillos
        if (theme == 0)
        {
            float blockBounce = sinf(time * 4.5f) * 6.0f;
            int blockPositionsX[4] = {90, 210, 1020, 1140};
            for (int b = 0; b < 4; b++)
            {
                Rectangle qBlock = {(float)blockPositionsX[b], 180.0f + ((b % 2) * 45.0f) + blockBounce, 48, 48};
                DrawRectangleRec((Rectangle){qBlock.x + 4, qBlock.y + 4, 48, 48}, Fade(BLACK, 0.45f));
                DrawRectangleRec(qBlock, MARIO_YELLOW);
                DrawRectangleLinesEx(qBlock, 3.5f, BLACK);
                DrawText("?", (int)qBlock.x + 15, (int)qBlock.y + 8, 34, MARIO_BRICK);
            }

            // Franja inferior de bloques estilo Super Mario
            for (int bx = 0; bx < sw; bx += 48)
            {
                DrawRectangle(bx, 668, 48, 52, MARIO_BRICK);
                DrawRectangleLines(bx, 668, 48, 52, BLACK);
                DrawRectangle(bx, 668, 48, 12, MARIO_GREEN);
            }
        }
    }

    // 4. Capa de Halloween (noche, luna, murciélagos, telarañas)
    DrawHalloweenSky(theme, scrollX);
    if (theme == 0)
        DrawHalloweenMenuDecor();
}

void fondoMenu(Texture2D fondoTexture)
{
    DrawMarioBackground(fondoTexture, 0, 0.0f);
}

bool btnsMenu(const char textoBoton[], int rectY)
{
    int rectWidth = 300;
    int rectHeight = 78;
    float rectX = (1280 - rectWidth) / 2.0f;
    Rectangle rect = {rectX, (float)rectY, (float)rectWidth, (float)rectHeight};

    Color baseCol = MARIO_RED;
    Color hoverCol = MARIO_ORANGE;

    if (strcmp(textoBoton, "Jugar") == 0)
    {
        baseCol = MARIO_GREEN;
        hoverCol = (Color){60, 215, 75, 255};
    }
    else if (strcmp(textoBoton, "Tienda") == 0)
    {
        baseCol = MARIO_BLUE;
        hoverCol = SKYBLUE;
    }
    else if (strcmp(textoBoton, "Volver") == 0)
    {
        baseCol = MARIO_ORANGE;
        hoverCol = GOLD;
    }

    return DrawMarioButton(rect, textoBoton, baseCol, hoverCol, 34);
}

#endif