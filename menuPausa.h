#ifndef MENU_PAUSA_H
#define MENU_PAUSA_H

#include "raylib.h"

typedef enum
{
    PAUSA_CONTINUAR,
    PAUSA_REINICIAR,
    PAUSA_MENU_PRINCIPAL,
    PAUSA_SALIR,
    PAUSA_NINGUNA
} PausaOpcion;

PausaOpcion menuPausa()
{
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.65f));

    int fontSize = 48;
    const char *titulo = "JUEGO EN PAUSA";
    int textWidth = MeasureText(titulo, fontSize);
    int textX = (screenWidth - textWidth) / 2;

    DrawText(titulo, textX + 3, 123, fontSize, BLACK);
    DrawText(titulo, textX, 120, fontSize, ORANGE);

    int anchoBtn = 280;
    int altoBtn = 55;
    int x = screenWidth / 2 - anchoBtn / 2;
    int yInicial = 220;
    int espacio = 22;

    const char *opciones[] = {"Continuar", "Reiniciar Nivel", "Menu Principal", "Salir del Juego"};
    PausaOpcion resultados[] = {PAUSA_CONTINUAR, PAUSA_REINICIAR, PAUSA_MENU_PRINCIPAL, PAUSA_SALIR};

    for (int i = 0; i < 4; i++)
    {
        Rectangle btn = {
            (float)x,
            (float)(yInicial + i * (altoBtn + espacio)),
            (float)anchoBtn,
            (float)altoBtn};

        bool hover = CheckCollisionPointRec(GetMousePosition(), btn);
        Color color = hover ? ORANGE : DARKGRAY;

        DrawRectangleRec(btn, color);
        DrawRectangleLinesEx(btn, 3.0f, hover ? GOLD : BLACK);

        int optWidth = MeasureText(opciones[i], 22);
        DrawText(opciones[i], x + (anchoBtn - optWidth) / 2, (int)btn.y + 16, 22, WHITE);

        if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            return resultados[i];
        }
    }

    return PAUSA_NINGUNA;
}

#endif