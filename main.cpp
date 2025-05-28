#include <stdio.h>
#include "mainMenu.h"
#include "menuNiveles.h"
#include "menuPausa.h"
#include "niveles.h"

#define NUM_NIVELES 3

// Cambio de pantalla
enum Pantalla
{
    MENU_PRINCIPAL,
    MENU_NIVELES,
    MENU_JUEGO,
    MENU_JUEGO2,
    MENU_JUEGO3,
    MENU_TIENDA,
    SALIR
};
Pantalla pantallaActual = MENU_PRINCIPAL;

int main()
{
    //**********************************************************************************
    //** Inicializacion de la ventana y variables **//
    //**********************************************************************************
    const int screenWidth = 1280; // Ancho de la ventana X
    const int screenHeight = 720; // Alto de la ventana Y

    Texture2D fondoTexture;
    InitWindow(screenWidth, screenHeight, "Math Bros"); // Inicializa la ventana con un tamaño de 1280x720 y el título "Math Bros"

    Image icon = LoadImage("src/images/icon.png"); // Carga la imagen del icono
    SetWindowIcon(icon);                           // Establece el icono de la ventana
    UnloadImage(icon);                             // Libera la imagen del icono de la memoria

    Texture2D fondosNiveles[NUM_NIVELES];
    char rutasFondos[NUM_NIVELES][50] = {
        "src/images/nivel1.png",
        "src/images/nivel2.png",
        "src/images/nivel3.jpg"};

    for (int i = 0; i < NUM_NIVELES; i++)
    {
        Image img = LoadImage(rutasFondos[i]);
        ImageResize(&img, screenWidth, screenHeight);
        fondosNiveles[i] = LoadTextureFromImage(img);
        UnloadImage(img);
    }
    int nivelActual = 0; // 0 = Nivel 1, 1 = Nivel 2, etc.

    SetExitKey(0); // Desactiva la tecla de salida (ESC) para evitar cerrar la ventana

    Image fondo = LoadImage("src/images/fondotest.png"); // Carga la imagen del fondo
    ImageResize(&fondo, screenWidth, screenHeight);      // Redimensiona la imagen del fondo al tamaño de la ventana
    fondoTexture = LoadTextureFromImage(fondo);          // Convierte la imagen a textura
    UnloadImage(fondo);                                  // Libera la imagen de memoria

    SetTargetFPS(60); // Establece la tasa de fotogramas por segundo

    Vector2 posicionJugador = {400, 280}; // Posición inicial del jugador

    while (!WindowShouldClose())
    {
        BeginDrawing();

        switch (pantallaActual)
        {
        //**********************************************************************************
        //** Menu Principal **//
        //**********************************************************************************
        case MENU_PRINCIPAL:
            ClearBackground(RAYWHITE); // Limpia el fondo de la pantalla con color blanco
            fondoMenu(fondoTexture);
            menuTitulo("MATH BROS");

            if (btnsMenu("Jugar", 250))
                pantallaActual = MENU_NIVELES;
            if (btnsMenu("Tienda", 370))
                pantallaActual = MENU_TIENDA;
            if (btnsMenu("Salir", 490))
                pantallaActual = SALIR;
            ClearBackground(RAYWHITE);

            break;
            //**********************************************************************************
            //** Menu de Niveles **//
            //**********************************************************************************

        case MENU_NIVELES:
            ClearBackground(RAYWHITE); // Limpia el fondo de la pantalla con color blanco
            fondoMenu(fondoTexture);
            menuTitulo("Niveles");
            if (btnsNiveles("Nivel 1", 200, 250))
            {
                nivelActual = 0;                       // Cambia el nivel actual a 1
                posicionJugador = (Vector2){400, 280}; // Reinicia la posición del jugador
                pantallaActual = MENU_JUEGO;
                menuPausa();
            }

            if (btnsNiveles("Nivel 2", 540, 250))
            {
                nivelActual = 1;                       // Cambia el nivel actual a 2
                posicionJugador = (Vector2){400, 280}; // Reinicia la posición del jugador
                pantallaActual = MENU_JUEGO;
                menuPausa();
            }

            if (btnsNiveles("Nivel 3", 840, 250))
            {
                nivelActual = 2;                       // Cambia el nivel actual a 3
                posicionJugador = (Vector2){400, 280}; // Reinicia la posición del jugador
                pantallaActual = MENU_JUEGO;
                menuPausa();
            }

            if (btnsMenu("Volver", 550))
                pantallaActual = MENU_PRINCIPAL;
            break;
        case MENU_JUEGO:
        {
            static bool juegoPausado = false;

            if (juegoPausado)
            {

                if (IsKeyPressed(KEY_ESCAPE))
                {
                    // Si ya está en pausa y se presiona ESC, se reanuda
                    juegoPausado = false;
                }
                else
                {
                    PausaOpcion opcion = menuPausa();
                    switch (opcion)
                    {
                    case PAUSA_CONTINUAR:
                        juegoPausado = false;
                        break;
                    case PAUSA_MENU_PRINCIPAL:
                        pantallaActual = MENU_PRINCIPAL;
                        juegoPausado = false;
                        break;
                    case PAUSA_SALIR:
                        pantallaActual = SALIR;
                        break;
                    default:
                        break;
                    }
                }
            }
            else
            {
                DrawTexture(fondosNiveles[nivelActual], 0, 0, WHITE);
                GameStatus status = runGame(&posicionJugador, fondosNiveles[nivelActual]);

                if (status == GAME_PAUSE)
                {
                    juegoPausado = true;
                }
            }
            break;
        }
        //**********************************************************************************
        //** Menu de Tienda **//
        //**********************************************************************************
        case MENU_TIENDA:
            fondoMenu(fondoTexture);
            menuTitulo("Tienda");
            if (btnsMenu("Volver", 600))
                pantallaActual = MENU_PRINCIPAL;
            break;

        case SALIR:
            UnloadTexture(fondoTexture); // Libera la textura del fondo
            for (int i = 0; i < NUM_NIVELES; i++)
            {
                UnloadTexture(fondosNiveles[i]);
            } // Libera las texturas de los niveles

            CloseWindow();
            return 0;
        }

        EndDrawing();
    }
    // Libera los recursos utilizados
    UnloadTexture(fondoTexture); // Libera la textura del fondo
    for (int i = 0; i < NUM_NIVELES; i++)
    {
        UnloadTexture(fondosNiveles[i]);
    } // Libera las texturas de los niveles

    CloseWindow(); // Cierra la ventana

    return 0;
}