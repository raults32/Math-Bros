#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <raylib.h>
#include <string.h>
#include <stdbool.h>

void menuTitulo(const char textotitulo[]);
bool btnsMenu(const char textoBoton[], int rectY);
void fondoMenu(Texture2D fondoTexture);

void menuTitulo(const char textotitulo[])
{
    int fontSize = 110;
    int textWidth = MeasureText(textotitulo, fontSize);
    int textX = (1280 - textWidth) / 2;
    int textY = 45;

    DrawText(textotitulo, textX + 5, textY + 5, fontSize, BLACK);
    DrawText(textotitulo, textX, textY, fontSize, ORANGE);
}

void fondoMenu(Texture2D fondoTexture)
{
    DrawTexture(fondoTexture, 0, 0, WHITE);
}

bool btnsMenu(const char textoBoton[], int rectY)
{
    int rectWidth = 260;
    int rectHeight = 85;
    float rectX = (1280 - rectWidth) / 2.0f;
    Rectangle rect = {rectX, (float)rectY, (float)rectWidth, (float)rectHeight};

    Vector2 mousePoint = GetMousePosition();
    bool mouseSobreBoton = CheckCollisionPointRec(mousePoint, rect);

    Color colorFondo = mouseSobreBoton ? Fade(ORANGE, 0.90f) : Fade(DARKGRAY, 0.80f);
    Color colorBorde = mouseSobreBoton ? GOLD : BLACK;

    DrawRectangleRec(rect, colorFondo);
    DrawRectangleLinesEx(rect, 5.0f, colorBorde);

    int fontsize = 36;
    int textWidth = MeasureText(textoBoton, fontsize);
    int textX = (int)(rect.x + (rect.width - textWidth) / 2);
    int textY = (int)(rect.y + (rect.height - fontsize) / 2);

    DrawText(textoBoton, textX + 2, textY + 2, fontsize, BLACK);
    DrawText(textoBoton, textX, textY, fontsize, WHITE);

    return (mouseSobreBoton && IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
}

#endif