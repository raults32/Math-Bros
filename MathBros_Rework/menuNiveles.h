#ifndef MENU_NIVELES_H
#define MENU_NIVELES_H

#include "mainMenu.h"

bool btnsNiveles(const char textoBoton[], const char subtitulo[], int rectX, int rectY, bool desbloqueado, int highScore);

bool btnsNiveles(const char textoBoton[], const char subtitulo[], int rectX, int rectY, bool desbloqueado, int highScore)
{
    int rectWidth = 290;
    int rectHeight = 200;
    Rectangle rect = {(float)rectX, (float)rectY, (float)rectWidth, (float)rectHeight};

    Vector2 mousePoint = GetMousePosition();
    bool hover = CheckCollisionPointRec(mousePoint, rect) && desbloqueado;
    bool pressed = hover && IsMouseButtonDown(MOUSE_LEFT_BUTTON);
    bool clicked = hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    float time = (float)GetTime();
    float scale = hover ? 1.05f : 1.0f;
    float bounceY = hover ? sinf(time * 7.5f) * 5.0f : 0.0f;
    if (pressed) scale = 0.97f;

    float w = rect.width * scale;
    float h = rect.height * scale;
    float x = rect.x - (w - rect.width) / 2.0f;
    float y = rect.y - (h - rect.height) / 2.0f + bounceY;

    // Color temático según cada Mundo
    Color worldColor = MARIO_GREEN;
    if (strstr(textoBoton, "2") != NULL) worldColor = MARIO_BLUE;
    else if (strstr(textoBoton, "3") != NULL) worldColor = MARIO_RED;

    if (!desbloqueado) worldColor = (Color){75, 75, 85, 255};

    DrawMarioPanel((Rectangle){x, y, w, h}, worldColor, hover ? MARIO_YELLOW : WHITE);

    // Banner superior estilo Mundo de Mario
    Rectangle headerRect = {x + 14, y + 14, w - 28, 46};
    DrawRectangleRounded(headerRect, 0.3f, 6, Fade(BLACK, 0.35f));

    const char *label = desbloqueado ? textoBoton : "BLOQUEADO";
    int labelSize = desbloqueado ? 34 : 26;
    int textWidth = MeasureText(label, labelSize);
    int textX = (int)(x + (w - textWidth) / 2.0f);
    int textY = (int)(y + 21);

    DrawText(label, textX + 2, textY + 2, labelSize, BLACK);
    DrawText(label, textX, textY, labelSize, desbloqueado ? MARIO_YELLOW : LIGHTGRAY);

    // Subtítulo del mundo
    int subWidth = MeasureText(subtitulo, 18);
    DrawText(subtitulo, (int)(x + (w - subWidth) / 2.0f) + 1, (int)(y + 76) + 1, 18, BLACK);
    DrawText(subtitulo, (int)(x + (w - subWidth) / 2.0f), (int)(y + 76), 18, desbloqueado ? WHITE : GRAY);

    // Estrellas decorativas de progreso
    int starsEarned = (highScore >= 1200) ? 3 : ((highScore >= 600) ? 2 : ((highScore > 0) ? 1 : 0));
    for (int s = 0; s < 3; s++)
    {
        int sx = (int)(x + w / 2.0f - 45 + s * 45);
        int sy = (int)(y + 118);
        Color sCol = (desbloqueado && s < starsEarned) ? MARIO_YELLOW : Fade(BLACK, 0.45f);
        DrawCircle(sx, sy, 13, BLACK);
        DrawCircle(sx, sy, 10, sCol);
    }

    // Placa de récord o indicación de bloqueo
    Rectangle scoreBadge = {x + 20, y + h - 52, w - 40, 34};
    DrawRectangleRounded(scoreBadge, 0.4f, 6, Fade(BLACK, 0.50f));
    const char *scoreText = desbloqueado ? TextFormat("RECORD: %d PTS", highScore) : "Supera el nivel anterior";
    int scoreWidth = MeasureText(scoreText, 17);
    DrawText(scoreText, (int)(x + (w - scoreWidth) / 2.0f), (int)(scoreBadge.y + 9), 17, desbloqueado ? GOLD : LIGHTGRAY);

    // Botón flotante "¡JUGAR!" al pasar el cursor
    if (hover)
    {
        const char *goTxt = "► ¡ENTRAR AL MUNDO! ◄";
        int goW = MeasureText(goTxt, 18);
        DrawText(goTxt, (int)(x + (w - goW) / 2.0f) + 2, (int)(y - 26) + 2, 18, BLACK);
        DrawText(goTxt, (int)(x + (w - goW) / 2.0f), (int)(y - 26), 18, MARIO_YELLOW);
    }

    return clicked;
}

#endif