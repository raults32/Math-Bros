#ifndef MENU_NIVELES_H
#define MENU_NIVELES_H

#include <raylib.h>
#include <string.h>
#include <stdbool.h>

bool btnsNiveles(const char textoBoton[], int rectX, int rectY, bool desbloqueado, int highScore);

bool btnsNiveles(const char textoBoton[], int rectX, int rectY, bool desbloqueado, int highScore)
{
    int rectWidth = 220;
    int rectHeight = 140;
    Rectangle rect = {(float)rectX, (float)rectY, (float)rectWidth, (float)rectHeight};

    Vector2 mousePoint = GetMousePosition();
    bool mouseSobreBoton = CheckCollisionPointRec(mousePoint, rect) && desbloqueado;

    Color colorFondo;
    if (!desbloqueado)
        colorFondo = Fade(DARKGRAY, 0.85f);
    else if (mouseSobreBoton)
        colorFondo = Fade(SKYBLUE, 0.95f);
    else
        colorFondo = Fade(BLUE, 0.80f);

    DrawRectangleRec(rect, colorFondo);
    DrawRectangleLinesEx(rect, 5.0f, desbloqueado ? (mouseSobreBoton ? GOLD : DARKBLUE) : BLACK);

    int fontsize = 36;
    const char *label = desbloqueado ? textoBoton : "BLOQUEADO";
    int labelSize = desbloqueado ? fontsize : 26;
    int textWidth = MeasureText(label, labelSize);
    int textX = (int)(rect.x + (rect.width - textWidth) / 2);
    int textY = (int)(rect.y + 30);

    DrawText(label, textX + 2, textY + 2, labelSize, BLACK);
    DrawText(label, textX, textY, labelSize, desbloqueado ? WHITE : LIGHTGRAY);

    const char *scoreText = desbloqueado ? TextFormat("Record: %d pts", highScore) : "Supera el anterior";
    int scoreWidth = MeasureText(scoreText, 18);
    DrawText(scoreText, (int)(rect.x + (rect.width - scoreWidth) / 2), (int)(rect.y + 90), 18, desbloqueado ? YELLOW : GRAY);

    return (mouseSobreBoton && IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
}

#endif