#ifndef HALLOWEEN_H
#define HALLOWEEN_H

#include <raylib.h>
#include <math.h>

// Cambia a 0 para quitar todas las decoraciones de Halloween
#define MODO_HALLOWEEN 1

#define HW_ORANGE      (Color){255, 120, 15, 255}
#define HW_DARK_ORANGE (Color){200, 75, 10, 255}
#define HW_PURPLE      (Color){120, 45, 175, 255}
#define HW_NIGHT       (Color){20, 10, 45, 255}
#define HW_SLIME       (Color){120, 220, 40, 255}
#define HW_CANDLE      (Color){255, 225, 90, 255}

// Número pseudoaleatorio estable por índice (para que las decoraciones no salten cada frame)
static inline float HwHash(int n)
{
    unsigned int x = (unsigned int)n * 2654435761u;
    x ^= x >> 13;
    x *= 0x5bd1e995u;
    x ^= x >> 15;
    return (float)(x % 10000) / 10000.0f;
}

// Calabaza tallada con vela parpadeante. base = punto donde se apoya en el suelo
static inline void DrawJackOLantern(Vector2 base, float size, float time, int seed)
{
    float w = size;
    float h = size * 0.8f;
    float cx = base.x;
    float cy = base.y - h / 2.0f;
    float flicker = 0.75f + 0.25f * sinf(time * 13.0f + seed * 1.7f) * sinf(time * 7.3f + seed);

    DrawEllipse((int)cx, (int)base.y, w * 0.6f, h * 0.12f, Fade(BLACK, 0.35f));
    DrawCircle((int)cx, (int)cy, w * 0.9f, Fade(HW_ORANGE, 0.10f * flicker));

    // Gajos de la calabaza
    DrawEllipse((int)(cx - w * 0.28f), (int)cy, w * 0.30f, h * 0.48f, HW_DARK_ORANGE);
    DrawEllipse((int)(cx + w * 0.28f), (int)cy, w * 0.30f, h * 0.48f, HW_DARK_ORANGE);
    DrawEllipse((int)cx, (int)cy, w * 0.34f, h * 0.50f, HW_ORANGE);
    DrawEllipseLines((int)cx, (int)cy, w * 0.52f, h * 0.50f, Fade(BLACK, 0.55f));

    // Tallo
    DrawRectangle((int)(cx - w * 0.05f), (int)(cy - h * 0.62f), (int)(w * 0.10f) + 1, (int)(h * 0.18f) + 1, (Color){70, 120, 30, 255});

    // Cara iluminada desde dentro
    Color glow = Fade(HW_CANDLE, flicker);
    float ey = cy - h * 0.10f;
    DrawTriangle((Vector2){cx - w * 0.26f, ey + h * 0.10f}, (Vector2){cx - w * 0.08f, ey + h * 0.10f}, (Vector2){cx - w * 0.17f, ey - h * 0.10f}, glow);
    DrawTriangle((Vector2){cx + w * 0.08f, ey + h * 0.10f}, (Vector2){cx + w * 0.26f, ey + h * 0.10f}, (Vector2){cx + w * 0.17f, ey - h * 0.10f}, glow);
    DrawTriangle((Vector2){cx - w * 0.05f, cy + h * 0.05f}, (Vector2){cx + w * 0.05f, cy + h * 0.05f}, (Vector2){cx, cy - h * 0.04f}, glow);

    float my = cy + h * 0.15f;
    DrawRectangle((int)(cx - w * 0.28f), (int)my, (int)(w * 0.56f), (int)(h * 0.12f) + 1, glow);
    for (int t = 0; t < 3; t++)
    {
        float tx = cx - w * 0.18f + t * w * 0.18f;
        DrawRectangle((int)(tx - w * 0.03f), (int)my, (int)(w * 0.06f) + 1, (int)(h * 0.05f) + 1, HW_DARK_ORANGE);
    }
}

// Murciélago con alas que aletean
static inline void DrawBat(Vector2 pos, float size, float time, int seed)
{
    float flap = sinf(time * 14.0f + seed * 2.1f);
    float wingY = flap * size * 0.55f;
    Color c = (Color){15, 5, 25, 255};

    DrawTriangle((Vector2){pos.x - size * 0.2f, pos.y}, (Vector2){pos.x - size * 1.2f, pos.y - wingY}, (Vector2){pos.x - size * 0.5f, pos.y + size * 0.3f}, c);
    DrawTriangle((Vector2){pos.x - size * 1.2f, pos.y - wingY}, (Vector2){pos.x - size * 0.9f, pos.y + size * 0.25f - wingY * 0.3f}, (Vector2){pos.x - size * 0.5f, pos.y + size * 0.3f}, c);
    DrawTriangle((Vector2){pos.x + size * 0.2f, pos.y}, (Vector2){pos.x + size * 0.5f, pos.y + size * 0.3f}, (Vector2){pos.x + size * 1.2f, pos.y - wingY}, c);
    DrawTriangle((Vector2){pos.x + size * 1.2f, pos.y - wingY}, (Vector2){pos.x + size * 0.5f, pos.y + size * 0.3f}, (Vector2){pos.x + size * 0.9f, pos.y + size * 0.25f - wingY * 0.3f}, c);

    DrawCircle((int)pos.x, (int)pos.y, size * 0.32f, c);
    DrawTriangle((Vector2){pos.x - size * 0.28f, pos.y - size * 0.15f}, (Vector2){pos.x - size * 0.08f, pos.y - size * 0.25f}, (Vector2){pos.x - size * 0.22f, pos.y - size * 0.5f}, c);
    DrawTriangle((Vector2){pos.x + size * 0.08f, pos.y - size * 0.25f}, (Vector2){pos.x + size * 0.28f, pos.y - size * 0.15f}, (Vector2){pos.x + size * 0.22f, pos.y - size * 0.5f}, c);
    DrawCircle((int)(pos.x - size * 0.11f), (int)(pos.y - size * 0.05f), size * 0.06f + 1, RED);
    DrawCircle((int)(pos.x + size * 0.11f), (int)(pos.y - size * 0.05f), size * 0.06f + 1, RED);
}

// Fantasma flotante semitransparente
static inline void DrawGhost(Vector2 pos, float size, float time, int seed)
{
    float bob = sinf(time * 2.5f + seed) * size * 0.15f;
    float x = pos.x;
    float y = pos.y + bob;
    Color body = Fade(WHITE, 0.82f);

    DrawCircle((int)x, (int)y, size * 0.75f, Fade((Color){200, 220, 255, 255}, 0.18f));
    DrawCircle((int)x, (int)y, size * 0.5f, body);
    DrawRectangle((int)(x - size * 0.5f), (int)y, (int)size, (int)(size * 0.55f), body);
    for (int i = 0; i < 4; i++)
    {
        float wave = sinf(time * 6.0f + i * 1.4f + seed) * size * 0.05f;
        DrawCircle((int)(x - size * 0.375f + i * size * 0.25f), (int)(y + size * 0.55f + wave), size * 0.125f + 1, body);
    }

    DrawEllipse((int)(x - size * 0.17f), (int)(y - size * 0.05f), size * 0.08f + 1, size * 0.13f + 1, (Color){25, 20, 40, 255});
    DrawEllipse((int)(x + size * 0.17f), (int)(y - size * 0.05f), size * 0.08f + 1, size * 0.13f + 1, (Color){25, 20, 40, 255});
    DrawEllipse((int)x, (int)(y + size * 0.22f), size * 0.09f + 1, size * 0.11f + 1, (Color){25, 20, 40, 255});
}

// Telaraña en una esquina. dirX/dirY = hacia dónde se abre (+1 o -1)
static inline void DrawSpiderWeb(Vector2 corner, float radius, int dirX, int dirY)
{
    Color c = Fade(WHITE, 0.55f);
    const int spokes = 6;
    Vector2 ends[6];
    for (int s = 0; s < spokes; s++)
    {
        float a = (PI / 2.0f) * s / (spokes - 1);
        ends[s] = (Vector2){corner.x + dirX * cosf(a) * radius, corner.y + dirY * sinf(a) * radius};
        DrawLineEx(corner, ends[s], 1.5f, c);
    }
    for (int ring = 1; ring <= 4; ring++)
    {
        float f = ring / 4.0f;
        for (int s = 0; s < spokes - 1; s++)
        {
            Vector2 a = {corner.x + (ends[s].x - corner.x) * f, corner.y + (ends[s].y - corner.y) * f};
            Vector2 b = {corner.x + (ends[s + 1].x - corner.x) * f, corner.y + (ends[s + 1].y - corner.y) * f};
            Vector2 mid = {(a.x + b.x) / 2.0f - dirX * radius * 0.03f * f, (a.y + b.y) / 2.0f - dirY * radius * 0.03f * f};
            DrawLineEx(a, mid, 1.2f, c);
            DrawLineEx(mid, b, 1.2f, c);
        }
    }
}

// Araña colgando de un hilo que sube y baja
static inline void DrawHangingSpider(float x, float topY, float length, float time, int seed)
{
    float y = topY + length * (0.65f + 0.35f * sinf(time * 1.6f + seed));
    DrawLineEx((Vector2){x, topY}, (Vector2){x, y}, 1.2f, Fade(WHITE, 0.6f));
    Color c = (Color){20, 10, 25, 255};
    for (int l = 0; l < 4; l++)
    {
        float ly = y - 4 + l * 3.0f;
        float kick = sinf(time * 9.0f + l) * 2.0f;
        DrawLineEx((Vector2){x, ly}, (Vector2){x - 13, ly - 4 + kick}, 2.0f, c);
        DrawLineEx((Vector2){x, ly}, (Vector2){x + 13, ly - 4 - kick}, 2.0f, c);
    }
    DrawCircle((int)x, (int)y, 8, c);
    DrawCircle((int)x, (int)y - 8, 5, c);
    DrawCircle((int)x - 2, (int)y - 9, 1.5f, RED);
    DrawCircle((int)x + 2, (int)y - 9, 1.5f, RED);
}

// Lápida con "RIP". base = punto medio inferior
static inline void DrawTombstone(Vector2 base, float w, float h)
{
    Rectangle r = {base.x - w / 2.0f, base.y - h, w, h};
    DrawRectangleRounded((Rectangle){r.x + 4, r.y + 4, w, h}, 0.5f, 8, Fade(BLACK, 0.35f));
    DrawRectangleRounded((Rectangle){r.x - 2, r.y - 2, w + 4, h + 2}, 0.5f, 8, (Color){55, 55, 70, 255});
    DrawRectangleRounded(r, 0.5f, 8, (Color){125, 125, 140, 255});
    int fs = (int)(w * 0.32f);
    DrawText("RIP", (int)(base.x - MeasureText("RIP", fs) / 2.0f), (int)(r.y + h * 0.3f), fs, (Color){55, 55, 70, 255});
    DrawRectangle((int)(r.x - 3), (int)(base.y - 5), (int)(w + 6), 5, (Color){60, 110, 40, 255});
}

// Sombrero de bruja. base = centro del ala
static inline void DrawWitchHat(Vector2 base, float w)
{
    DrawEllipse((int)base.x, (int)base.y, w * 0.6f, w * 0.12f, (Color){25, 10, 40, 255});
    DrawTriangle((Vector2){base.x - w * 0.3f, base.y}, (Vector2){base.x + w * 0.3f, base.y}, (Vector2){base.x + w * 0.15f, base.y - w * 0.85f}, (Color){45, 15, 70, 255});
    DrawRectangle((int)(base.x - w * 0.28f), (int)(base.y - w * 0.16f), (int)(w * 0.56f), (int)(w * 0.1f) + 1, HW_ORANGE);
    DrawRectangle((int)(base.x - w * 0.06f), (int)(base.y - w * 0.17f), (int)(w * 0.12f) + 1, (int)(w * 0.12f) + 1, HW_SLIME);
}

// Capa nocturna en pantalla: oscurece el cielo, luna, murciélagos, telarañas y niebla.
// theme: igual que DrawMarioBackground (0 = menú, 1-3 = niveles, 4 = tienda)
static inline void DrawHalloweenSky(int theme, float scrollX)
{
#if MODO_HALLOWEEN
    const int sw = 1280;
    const int sh = 720;
    float time = (float)GetTime();

    float darkness = (theme == 3 || theme == 4) ? 0.30f : 0.62f;
    DrawRectangleGradientV(0, 0, sw, sh, Fade(HW_NIGHT, darkness), Fade((Color){90, 30, 70, 255}, darkness * 0.6f));

    // Estrellas titilantes
    for (int s = 0; s < 40; s++)
    {
        float sx = HwHash(s) * sw;
        float sy = HwHash(s + 100) * 300.0f;
        float a = 0.35f + 0.35f * sinf(time * 3.0f + s);
        DrawCircle((int)sx, (int)sy, 1.0f + (s % 3 == 0), Fade(WHITE, a));
    }

    // Luna llena con resplandor
    Vector2 moon = {(theme == 0) ? 1080.0f : 1130.0f, 130.0f};
    if (theme != 4)
    {
        DrawCircleV(moon, 95, Fade((Color){255, 240, 190, 255}, 0.10f));
        DrawCircleV(moon, 75, Fade((Color){255, 240, 190, 255}, 0.18f));
        DrawCircleV(moon, 58, (Color){255, 245, 200, 255});
        DrawCircle((int)moon.x - 18, (int)moon.y - 12, 11, (Color){235, 220, 170, 255});
        DrawCircle((int)moon.x + 20, (int)moon.y + 15, 8, (Color){235, 220, 170, 255});
        DrawCircle((int)moon.x + 6, (int)moon.y - 25, 5, (Color){235, 220, 170, 255});
    }

    // Bandada de murciélagos cruzando la luna
    for (int b = 0; b < 7; b++)
    {
        float speed = 60.0f + HwHash(b + 7) * 70.0f;
        float span = sw + 200.0f;
        float bx = fmodf(HwHash(b) * span + time * speed + scrollX * 0.1f, span) - 100.0f;
        float by = 70.0f + HwHash(b + 30) * 220.0f + sinf(time * 2.0f + b) * 18.0f;
        DrawBat((Vector2){bx, by}, 12.0f + HwHash(b + 50) * 10.0f, time, b);
    }

    // Telaraña en la esquina superior derecha y una araña colgando
    DrawSpiderWeb((Vector2){(float)sw, 0}, 150.0f, -1, 1);
    DrawHangingSpider(sw - 70.0f, 0.0f, 120.0f, time, 3);

    // Niebla espectral a ras de suelo
    if (theme != 3)
    {
        for (int f = 0; f < 6; f++)
        {
            float fx = fmodf(f * 260.0f + time * 18.0f - scrollX * 0.25f, sw + 400.0f);
            if (fx < 0) fx += sw + 400.0f;
            DrawEllipse((int)(fx - 200), sh - 40 + (int)(sinf(time + f) * 6), 230, 45, Fade((Color){200, 180, 230, 255}, 0.16f));
        }
    }
#else
    (void)theme;
    (void)scrollX;
#endif
}

// Decoración del suelo del menú: calabazas, lápidas y fantasmas
static inline void DrawHalloweenMenuDecor(void)
{
#if MODO_HALLOWEEN
    float time = (float)GetTime();
    DrawTombstone((Vector2){300, 668}, 54, 70);
    DrawTombstone((Vector2){850, 668}, 46, 60);
    DrawJackOLantern((Vector2){75, 668}, 62, time, 1);
    DrawJackOLantern((Vector2){180, 668}, 44, time, 2);
    DrawJackOLantern((Vector2){405, 668}, 50, time, 3);
    DrawJackOLantern((Vector2){940, 668}, 56, time, 4);
    DrawGhost((Vector2){170, 390 + sinf(time * 0.8f) * 20.0f}, 64, time, 1);
    DrawGhost((Vector2){380 + sinf(time * 0.6f) * 30.0f, 520}, 44, time, 2);
    DrawGhost((Vector2){930, 300 + sinf(time * 0.7f) * 25.0f}, 52, time, 3);
#endif
}

// Decoración sobre una plataforma del nivel (coordenadas del mundo)
static inline void DrawHalloweenPlatformDecor(Rectangle plat, int index)
{
#if MODO_HALLOWEEN
    float time = (float)GetTime();
    if (plat.width >= 1000.0f)
    {
        // Suelo largo: calabazas y lápidas repartidas
        for (int i = 0; i * 380.0f < plat.width; i++)
        {
            float x = plat.x + 150.0f + i * 380.0f + HwHash(i + index * 50) * 120.0f;
            if (x > plat.x + plat.width - 40.0f) break;
            if (HwHash(i * 3 + index) < 0.5f)
                DrawJackOLantern((Vector2){x, plat.y}, 38.0f + HwHash(i) * 14.0f, time, i);
            else
                DrawTombstone((Vector2){x, plat.y}, 34, 44);
        }
        return;
    }

    float roll = HwHash(index * 7 + 11);
    if (plat.width < 150.0f) return;
    if (roll < 0.45f)
        DrawJackOLantern((Vector2){plat.x + plat.width - 26.0f, plat.y}, 30.0f, time, index);
    else if (roll < 0.65f)
        DrawTombstone((Vector2){plat.x + plat.width - 26.0f, plat.y}, 28, 36);
    else if (roll < 0.85f)
        DrawHangingSpider(plat.x + plat.width * 0.7f, plat.y + plat.height, 50.0f, time, index);
#else
    (void)plat;
    (void)index;
#endif
}

#endif
