#include <stdio.h>
#include "mainMenu.h"
#include "menuNiveles.h"
#include "menuPausa.h"
#include "niveles.h"

#define NUM_NIVELES 3
#define NUM_COLORS 5

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

// Estructura para manejar los colores desbloqueados
typedef struct
{
    const char *name;
    int price;
    bool unlocked;
    const char *imagePath;
    Texture2D imageTexture; // Miniatura para la tienda
    Texture2D fullTexture;  // Textura original para animación en el juego
    int frames;             // Número de frames de animación
} ColorOption;

// Función para dibujar botones de compra
bool DrawBuyButton(const char *text, int price, bool unlocked, int posY, int *coins)
{
    int btnWidth = 300;
    int btnHeight = 50;
    int btnX = (1280 - btnWidth) / 2; // Centrado en pantalla (1280 es el ancho de la ventana)
    Rectangle btn = {(float)btnX, (float)posY, (float)btnWidth, (float)btnHeight};
    bool mouseOver = CheckCollisionPointRec(GetMousePosition(), btn);
    Color btnColor = unlocked ? Fade(GREEN, 0.7f) : (mouseOver ? Fade(DARKBLUE, 0.8f) : Fade(BLUE, 0.7f));

    DrawRectangleRec(btn, btnColor);
    DrawRectangleLinesEx(btn, 6.0f, DARKBLUE);
    const char *displayText = unlocked ? "Desbloqueado" : TextFormat("%s - %d monedas", text, price);
    int textWidth = MeasureText(displayText, 20);
    DrawText(displayText, btnX + (btnWidth - textWidth) / 2, posY + (btnHeight - 20) / 2, 20, WHITE);

    // Permitir compra si no está desbloqueado y hay suficientes monedas
    if (mouseOver && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !unlocked && *coins >= price)
    {
        *coins -= price; // Restar monedas
        return true;     // Indicar que se compró
    }
    return false;
}

int main()
{
    //**********************************************************************************
    //** Inicialización de la ventana y variables **//
    //**********************************************************************************
    const int screenWidth = 1280; // Ancho de la ventana X
    const int screenHeight = 720; // Alto de la ventana Y

    int posinicialX = 50;
    int posinicialY = 550;

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
    int nivelActual = 0;    // 0 = Nivel 1, 1 = Nivel 2, etc.
    int coinsCollected = 0; // Contador de monedas recolectadas

    // Colores disponibles para comprar, con rutas de imágenes de referencia
    ColorOption colors[NUM_COLORS] = {
        {"Default", 0, true, "src/images/zorrodefault.png", {0}, {0}, 4},
        {"Rojo", 1, false, "src/images/zorrorojo.png", {0}, {0}, 4},
        {"Azul", 10, false, "src/images/zorroazul.png", {0}, {0}, 4},
        {"Verde", 15, false, "src/images/zorroverde.png", {0}, {0}, 4},
        {"Morado", 15, false, "src/images/zorromorado.png", {0}, {0}, 4}};

    int selectedColorIndex = 0; // Índice del color actualmente seleccionado

    // Cargar las imágenes de referencia de los personajes
    for (int i = 0; i < NUM_COLORS; i++)
    {
        // Miniatura para la tienda
        Image imgMini = LoadImage(colors[i].imagePath);
        ImageResize(&imgMini, 50, 50);
        colors[i].imageTexture = LoadTextureFromImage(imgMini);
        UnloadImage(imgMini);

        // Textura original para el juego
        Image imgFull = LoadImage(colors[i].imagePath);
        colors[i].fullTexture = LoadTextureFromImage(imgFull);
        UnloadImage(imgFull);
    }

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
        {
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
        }

        //**********************************************************************************
        //** Menu de Niveles **//
        //**********************************************************************************
        case MENU_NIVELES:
        {
            ClearBackground(RAYWHITE); // Limpia el fondo de la pantalla con color blanco
            fondoMenu(fondoTexture);
            menuTitulo("Niveles");
            if (btnsNiveles("Nivel 1", 200, 250))
            {
                nivelActual = 0;                                                     // Cambia el nivel actual a 1
                posicionJugador = (Vector2){(float)posinicialX, (float)posinicialY}; // Reinicia la posición del jugador
                pantallaActual = MENU_JUEGO;
                menuPausa();
            }

            if (btnsNiveles("Nivel 2", 540, 250))
            {
                nivelActual = 1;                                                     // Cambia el nivel actual a 2
                posicionJugador = (Vector2){(float)posinicialX, (float)posinicialY}; // Reinicia la posición del jugador
                pantallaActual = MENU_JUEGO2;
                menuPausa();
            }

            if (btnsNiveles("Nivel 3", 840, 250))
            {
                nivelActual = 2;                                                     // Cambia el nivel actual a 3
                posicionJugador = (Vector2){(float)posinicialX, (float)posinicialY}; // Reinicia la posición del jugador
                pantallaActual = MENU_JUEGO3;
                menuPausa();
            }

            if (btnsMenu("Volver", 550))
                pantallaActual = MENU_PRINCIPAL;
            break;
        }

        //**********************************************************************************
        //** Nivel 1 **//
        //**********************************************************************************
        case MENU_JUEGO:
        {
            static bool juegoPausado = false;

            if (juegoPausado)
            {
                if (IsKeyPressed(KEY_ESCAPE))
                {
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
                GameStatus status = nivel1(&posicionJugador, fondosNiveles[nivelActual], &coinsCollected, colors[selectedColorIndex].fullTexture, colors[selectedColorIndex].frames);
                
                if (IsKeyPressed(KEY_ESCAPE)) // Permitir pausa en cualquier momento
                {
                    juegoPausado = true;
                }
                else if (status == GAME_PAUSE) // Pausa desde el nivel (si está implementada)
                {
                    juegoPausado = true;
                }
                else if (status == GAME_OVER)
                {
                    pantallaActual = MENU_NIVELES; // Volver al menú de niveles tras perder
                    juegoPausado = false;
                }
                else if (status == GAME_WON)
                {
                    pantallaActual = MENU_NIVELES; // Volver al menú de niveles tras ganar
                    juegoPausado = false;
                }
            }
            break;
        }

        //**********************************************************************************
        //** Nivel 2 **//
        //**********************************************************************************
        case MENU_JUEGO2:
        {
            static bool juegoPausado = false;

            if (juegoPausado)
            {
                if (IsKeyPressed(KEY_ESCAPE))
                {
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
                GameStatus status = nivel2(&posicionJugador, fondosNiveles[nivelActual], &coinsCollected, colors[selectedColorIndex].fullTexture, colors[selectedColorIndex].frames);

                if (IsKeyPressed(KEY_ESCAPE)) // Permitir pausa en cualquier momento
                {
                    juegoPausado = true;
                }
                else if (status == GAME_PAUSE) // Pausa desde el nivel (si está implementada)
                {
                    juegoPausado = true;
                }
                else if (status == GAME_OVER)
                {
                    pantallaActual = MENU_NIVELES; // Volver al menú de niveles tras perder
                    juegoPausado = false;
                }
                else if (status == GAME_WON)
                {
                    pantallaActual = MENU_NIVELES; // Volver al menú de niveles tras ganar
                    juegoPausado = false;
                }
            }
            break;
        }

        //**********************************************************************************
        //** Nivel 3 **//
        //**********************************************************************************
        case MENU_JUEGO3:
        {
            static bool juegoPausado = false;

            if (juegoPausado)
            {
                if (IsKeyPressed(KEY_ESCAPE))
                {
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
                GameStatus status = nivel3(&posicionJugador, fondosNiveles[nivelActual], &coinsCollected, colors[selectedColorIndex].fullTexture, colors[selectedColorIndex].frames);

                if (status == GAME_PAUSE)
                {
                    juegoPausado = true;
                }
                else if (status == GAME_OVER)
                {
                    pantallaActual = MENU_NIVELES; // Volver al menú de niveles tras perder
                    juegoPausado = false;
                }
                else if (status == GAME_WON)
                {
                    pantallaActual = MENU_NIVELES; // Volver al menú de niveles tras ganar
                    juegoPausado = false;
                }
            }
            break;
        }

        //**********************************************************************************
        //** Menu de Tienda **//
        //**********************************************************************************
        case MENU_TIENDA:
        {
            fondoMenu(fondoTexture);
            menuTitulo("Tienda");
            DrawText(TextFormat("Monedas: %d", coinsCollected), 10, 50, 20, WHITE); // Mostrar monedas recolectadas

            // Botones para comprar colores con imágenes de referencia
            int spacing = 70;
            for (int i = 0; i < NUM_COLORS; i++)
            {
                int posY = 200 + i * spacing;
                // Dibujar la imagen de referencia a la izquierda del botón
                int imageX = (1280 - 300) / 2 - 70; // A la izquierda del botón (300 es el ancho del botón)
                int imageY = posY;
                DrawTexture(colors[i].imageTexture, imageX, imageY, WHITE);

                if (DrawBuyButton(colors[i].name, colors[i].price, colors[i].unlocked, posY, &coinsCollected))
                {
                    colors[i].unlocked = true; // Marcar como desbloqueado
                }

                // Si está desbloqueado, permitir seleccionar
                if (colors[i].unlocked)
                {
                    Rectangle selector = {(float)(imageX + 60), (float)(posY + 10), 20, 20};
                    DrawRectangleRec(selector, (i == selectedColorIndex) ? GREEN : DARKGRAY);
                    if (CheckCollisionPointRec(GetMousePosition(), selector) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                    {
                        selectedColorIndex = i;
                    }
                }
            }

            if (btnsMenu("Volver", 600))
                pantallaActual = MENU_PRINCIPAL;
            break;
        }

        case SALIR:
        {
            UnloadTexture(fondoTexture); // Libera la textura del fondo
            for (int i = 0; i < NUM_NIVELES; i++)
            {
                UnloadTexture(fondosNiveles[i]);
            } // Libera las texturas de los niveles

            // Liberar las imágenes de los colores
            for (int i = 0; i < NUM_COLORS; i++)
            {
                UnloadTexture(colors[i].imageTexture);
            }

            CloseWindow();
            return 0;
        }
        }

        EndDrawing();
    }
    // Libera los recursos utilizados
    UnloadTexture(fondoTexture); // Libera la textura del fondo
    for (int i = 0; i < NUM_NIVELES; i++)
    {
        UnloadTexture(fondosNiveles[i]);
    } // Libera las texturas de los niveles

    // Liberar las imágenes de los colores
    for (int i = 0; i < NUM_COLORS; i++)
    {
        UnloadTexture(colors[i].imageTexture);
    }

    CloseWindow(); // Cierra la ventana

    return 0;
}