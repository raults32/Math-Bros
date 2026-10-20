#ifndef MENU_PAUSA_H
#define MENU_PAUSA_H

#include "mainMenu.h"

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

    // Oscurecer el nivel congelado de fondo
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.65f));

    // Tablero central estilo Mario Bros
    Rectangle panel = {(screenWidth - 460) / 2.0f, 110, 460, 500};
    DrawMarioPanel(panel, (Color){35, 65, 135, 250}, MARIO_YELLOW);

    int fontSize = 44;
    const char *titulo = "PAUSA";
    int textWidth = MeasureText(titulo, fontSize);
    int textX = (screenWidth - textWidth) / 2;

    DrawText(titulo, textX + 4, 138, fontSize, BLACK);
    DrawText(titulo, textX, 134, fontSize, MARIO_YELLOW);

    int anchoBtn = 320;
    int altoBtn = 62;
    int x = screenWidth / 2 - anchoBtn / 2;
    int yInicial = 210;
    int espacio = 24;

    const char *opciones[] = {"Continuar", "Reiniciar Nivel", "Menu Principal", "Salir del Juego"};
    Color coloresBase[] = {MARIO_GREEN, MARIO_BLUE, MARIO_ORANGE, MARIO_RED};
    Color coloresHover[] = {(Color){60, 220, 80, 255}, SKYBLUE, GOLD, (Color){255, 70, 70, 255}};
    PausaOpcion resultados[] = {PAUSA_CONTINUAR, PAUSA_REINICIAR, PAUSA_MENU_PRINCIPAL, PAUSA_SALIR};

    for (int i = 0; i < 4; i++)
    {
        Rectangle btn = {
            (float)x,
            (float)(yInicial + i * (altoBtn + espacio)),
            (float)anchoBtn,
            (float)altoBtn};

        if (DrawMarioButton(btn, opciones[i], coloresBase[i], coloresHover[i], 24))
        {
            return resultados[i];
        }
    }

    return PAUSA_NINGUNA;
}

#endif