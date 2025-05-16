 #include <stdio.h>
#include "mainMenu.h"
#include "personaje.h"
#include "menuNiveles.h"	

int main()
{
    //**********************************************************************************
    //** Inicializacion de la ventana y variables **//
    //**********************************************************************************
    const int screenWidth = 1280; // Ancho de la ventana X
    const int screenHeight = 720; // Alto de la ventana Y
    
    //Cambio de pantalla
    enum Pantalla { MENU_PRINCIPAL, MENU_NIVELES, MENU_TIENDA, SALIR };
    Pantalla pantallaActual = MENU_PRINCIPAL;

    Texture2D fondoTexture;
    InitWindow(screenWidth, screenHeight, "Math Bros"); // Inicializa la ventana con un tamaño de 1280x720 y el título "Math Bros"

    Image fondo = LoadImage("src/images/fondotest.png"); // Carga la imagen del fondo
    ImageResize(&fondo, screenWidth, screenHeight);      // Redimensiona la imagen del fondo al tamaño de la ventana
    fondoTexture = LoadTextureFromImage(fondo);          // Convierte la imagen a textura
    UnloadImage(fondo);                                  // Libera la imagen de memoria

    SetTargetFPS(60); // Establece la tasa de fotogramas por segundo

    while (!WindowShouldClose())
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

    switch (pantallaActual)
    {
        case MENU_PRINCIPAL:
            fondoMenu(fondoTexture);
            menuTitulo();

            if (btnsMenu("Jugar", 250)) pantallaActual = MENU_NIVELES;
            if (btnsMenu("Tienda", 370)) pantallaActual = MENU_TIENDA;
            if (btnsMenu("Salir", 490)) pantallaActual = SALIR;
            break;

        case MENU_NIVELES:
            DrawText("Estás en el MENÚ DE NIVELES", 100, 100, 30, DARKBLUE);
            if (btnsMenu("Volver", 600)) pantallaActual = MENU_PRINCIPAL;
            break;

        case MENU_TIENDA:
            DrawText("Estás en la TIENDA", 100, 100, 30, DARKGREEN);
            if (btnsMenu("Volver", 600)) pantallaActual = MENU_PRINCIPAL;
            break;

        case SALIR:
            CloseWindow();
            return 0;
    }

    EndDrawing();
}

    UnloadTexture(fondoTexture); // Libera la textura del fondo

    return 0;
}