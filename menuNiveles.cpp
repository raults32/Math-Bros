#include "menuNiveles.h"

bool btnsNiveles(const char textoBoton[], int rectX, int rectY)
{
    int rectWidth = 200;
    int rectHeight = 120;
    Rectangle rect = {rectX, (float)rectY, (float)rectWidth, (float)rectHeight};

    int fontsize = 40;
    int textWidth = MeasureText(textoBoton, fontsize);
    int textX = rect.x + (rect.width - textWidth) / 2;
    int textY = rect.y + (rect.height - fontsize) / 2;

    Vector2 mousePoint = GetMousePosition();
    bool mouseSobreBoton = CheckCollisionPointRec(mousePoint, rect);

    Color colorFondo = mouseSobreBoton ? Fade(DARKBLUE, 0.8f) : Fade(BLUE, 0.7f);
    DrawRectangle(rectX, rectY, rectWidth, rectHeight, colorFondo);

    DrawRectangleLinesEx(rect, 6.0f, DARKBLUE);
    DrawRectangle(rectX, rectY, rectWidth, rectHeight, Fade(SKYBLUE, 0.70f));
    DrawText(textoBoton, textX, textY, fontsize, WHITE);

    return (mouseSobreBoton && IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
}
