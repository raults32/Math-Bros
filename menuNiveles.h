#include <raylib.h>
#include <string.h>
#include <stdbool.h>

//PROTOTIPOS
bool btnsNiveles(const char textoBoton[], int rectX, int rectY);

bool btnsNiveles(const char textoBoton[], int rectX, int rectY)
{
    int rectWidth = 200;                                                         // Ancho del rectángulo
    int rectHeight = 120;                                                        // Alto del rectángulo                  // Calcula la posición X del rectángulo para centrarlo en la pantalla
    Rectangle rect = {(float)rectX, (float)rectY, (float)rectWidth, (float)rectHeight}; // Crea un rectángulo con las dimensiones y posición especificadas

    // Centrar el texto con el boton
    int fontsize = 40;                                 // Tamaño de la fuente
    int textWidth = MeasureText(textoBoton, fontsize); // Mide el ancho del texto
    int textX = rect.x + (rect.width - textWidth) / 2; // Calcula la posición X del texto para centrarlo en el rectángulo
    int textY = rect.y + (rect.height - fontsize) / 2; // Calcula la posición Y del texto para centrarlo en el rectángulo

    // Chequea si el mouse está sobre el botón
    Vector2 mousePoint = GetMousePosition(); // Obtiene la posición del mouse
    bool mouseSobreBoton = CheckCollisionPointRec(mousePoint, rect);

    Color colorFondo = mouseSobreBoton ? Fade(DARKBLUE, 0.8f) : Fade(BLUE, 0.7f);
    DrawRectangle(rectX, rectY, rectWidth, rectHeight, colorFondo);

    DrawRectangleLinesEx(rect, 6.0f, DARKBLUE);                               // Dibuja el borde del rectángulo con un grosor de 10 píxeles
    DrawRectangle(rectX, rectY, rectWidth, rectHeight, Fade(SKYBLUE, 0.70f)); // Dibuja el rectángulo con un color de fondo
    DrawText(textoBoton, textX, textY, fontsize, WHITE);                      // Dibuja el texto del botón en el centro del rectángulo

    return (mouseSobreBoton && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)); // Devuelve verdadero si el mouse está sobre el botón y se ha hecho clic
}