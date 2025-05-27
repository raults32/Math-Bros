#include <raylib.h>
#include <string.h>
#include <stdbool.h>
//**********************************************************************************
//** Prototipos **//
//**********************************************************************************
void menuTitulo(const char textotitulo[]);
bool btnsMenu(const char textoBoton[], int rectY);
void fondoMenu(Texture2D fondoTexture);

//**********************************************************************************
//** Funciones **//
//**********************************************************************************

//*** TITULO DEL MENU ***//
void menuTitulo(const char textotitulo[])
{
    
    int fontSize = 100;
    int textWidth = MeasureText(textotitulo, fontSize);
    int textX = (1280 - textWidth) / 2;
    int textY = 50;

    DrawText(textotitulo, textX, textY, fontSize, ORANGE);
}

//*** FONDO DEL MENU ***//
void fondoMenu(Texture2D fondoTexture)
{
    DrawTexture(fondoTexture, 0, 0, WHITE); // Dibuja la textura de fondo en la posición (0, 0)
}

//*** BOTONES DEL MENU ***//
bool btnsMenu(const char textoBoton[], int rectY)
{
    int rectWidth = 250;                                    // Ancho del rectángulo
    int rectHeight = 100;                                   // Alto del rectángulo
    float rectX = (1280 - rectWidth) / 2;                     // Calcula la posición X del rectángulo para centrarlo en la pantalla
    Rectangle rect = {(float)rectX,(float)rectY, (float)rectWidth, (float)rectHeight}; // Crea un rectángulo con las dimensiones y posición especificadas

    int fontsize = 40;                                 // Tamaño de la fuente
    int textWidth = MeasureText(textoBoton, fontsize); // Mide el ancho del texto
    int textX = rect.x + (rect.width - textWidth) / 2; // Calcula la posición X del texto para centrarlo en el rectángulo
    int textY = rect.y + (rect.height - fontsize) / 2; // Calcula la posición Y del texto para centrarlo en el rectángulo

    Vector2 mousePoint = GetMousePosition(); // Obtiene la posición del mouse
    bool mouseSobreBoton = CheckCollisionPointRec(mousePoint, rect);

    Color colorFondo = mouseSobreBoton ? Fade(DARKGRAY, 0.8f) : Fade(GRAY, 0.7f);
    DrawRectangle(rectX, rectY, rectWidth, rectHeight, colorFondo);

    DrawRectangleLinesEx(rect, 6.0f, BLACK); // Dibuja el borde del rectángulo con un grosor de 10 píxeles
    DrawRectangle(rectX, rectY, rectWidth, rectHeight, Fade(GRAY, 0.70f));
    DrawText(textoBoton, textX, textY, fontsize, WHITE); // Dibuja el texto del botón en el centro del rectángulo

    return (mouseSobreBoton && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)); // Devuelve verdadero si el mouse está sobre el botón y se ha hecho clic
}