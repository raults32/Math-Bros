#ifndef MENU_NIVELES_H
#define MENU_NIVELES_H

#include <raylib.h>
#include <stdbool.h>
#include <string.h>

static inline bool btnsNiveles(const char *textoBoton, const char *subtitulo,
                               int rectX, int rectY, bool desbloqueado, int highScore)
{
    const int ancho = 260;
    const int alto = 165;
    Rectangle rect = {(float)rectX, (float)rectY, (float)ancho, (float)alto};
    const bool hover = desbloqueado && CheckCollisionPointRec(GetMousePosition(), rect);
    const Color fondo = !desbloqueado ? Fade(DARKGRAY, 0.88f)
        : (hover ? Fade(SKYBLUE, 0.95f) : Fade(BLUE, 0.82f));
    DrawRectangleRec(rect, fondo);
    DrawRectangleLinesEx(rect, 4.0f, hover ? GOLD : DARKBLUE);

    const char *titulo = desbloqueado ? textoBoton : "BLOQUEADO";
    const int tamTitulo = 30;
    DrawText(titulo, rectX + (ancho - MeasureText(titulo, tamTitulo)) / 2, rectY + 20,
             tamTitulo, desbloqueado ? WHITE : LIGHTGRAY);
    DrawText(subtitulo, rectX + (ancho - MeasureText(subtitulo, 16)) / 2,
             rectY + 70, 16, RAYWHITE);
    const char *record = desbloqueado ? TextFormat("Record: %d pts", highScore)
                                      : "Supera el nivel previo";
    DrawText(record, rectX + (ancho - MeasureText(record, 17)) / 2,
             rectY + 120, 17, desbloqueado ? YELLOW : GRAY);
    return hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

#endif
