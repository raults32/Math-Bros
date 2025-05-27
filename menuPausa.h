#include "raylib.h"

typedef enum
{
    PAUSA_CONTINUAR,
    PAUSA_MENU_PRINCIPAL,
    PAUSA_SALIR,
    PAUSA_NINGUNA
} PausaOpcion;

PausaOpcion menuPausa()
{
    int anchoBtn = 250;
    int altoBtn = 50;
    int x = GetScreenWidth() / 2 - anchoBtn / 2;
    int yInicial = 200;
    int espacio = 20;

    int fontSize = 40;
    int textWidth = MeasureText("JUEGO EN PAUSA", fontSize);
    int textX = (GetScreenWidth() - textWidth) / 2;

    const char *opciones[] = {"Continuar", "Menú Principal", "Salir del Juego"};
    PausaOpcion resultados[] = {PAUSA_CONTINUAR, PAUSA_MENU_PRINCIPAL, PAUSA_SALIR};

    // Capa semitransparente encima del juego
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(DARKGRAY, 0.050f));

    // Título
    DrawText("JUEGO EN PAUSA", textX, 120, fontSize, RED);

    // Botones
    for (int i = 0; i < 3; i++)
    {
        Rectangle btn = {
            (float)x,
            (float)(yInicial + i * (altoBtn + espacio)),
            (float)anchoBtn,
            (float)altoBtn
        };

        Color color = CheckCollisionPointRec(GetMousePosition(), btn) ? LIGHTGRAY : GRAY;

        DrawRectangleRec(btn, color);
        DrawText(opciones[i], x + 30, btn.y + 15, 20, DARKGRAY);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), btn))
        {
            return resultados[i];
        }
    }

    return PAUSA_NINGUNA;
}
