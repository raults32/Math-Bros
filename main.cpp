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
    Texture2D imageTexture;   // Miniatura para la tienda
    Texture2D fullTexture;    // Textura original para animación en el juego
    int frames;               // Número de frames de animación
    const char *previewPath;  // Ruta de la imagen ilustrativa
    Texture2D previewTexture; // Imagen ilustrativa para la tienda
} ColorOption;

// Función para dibujar botones de compra

bool DrawBuyButton(const char *text, int price, bool unlocked, int posX, int posY, int *coins);
void UpdateMusic(Music &gameMusic, bool &musicInitialized, int &currentMusicLevel, int level);

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
    Texture2D shop_texture;
    InitWindow(screenWidth, screenHeight, "Math Bros"); // Inicializa la ventana con un tamaño de 1280x720 y el título "Math Bros"

    // add music
    Music gameMusic;
    bool musicInitialized = false;
    int currentMusicLevel = 0;
    InitAudioDevice(); 
    gameMusic = LoadMusicStream("src/sound/main.mp3");
    musicInitialized = true;
    SetMusicVolume(gameMusic, 0.5f); 
    PlayMusicStream(gameMusic);

    //icono
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
        {"Default", 0, true, "src/images/zorrodefault.png", {0}, {0}, 4, "src/images/default.png", {0}},
        {"Rojo", 3, false, "src/images/zorrorojo.png", {0}, {0}, 4, "src/images/red.png", {0}},
        {"Azul", 8, false, "src/images/zorroazul.png", {0}, {0}, 4, "src/images/azul.png", {0}},
        {"Verde", 10, false, "src/images/zorroverde.png", {0}, {0}, 4, "src/images/verde.png", {0}},
        {"Morado", 15, false, "src/images/zorromorado.png", {0}, {0}, 4, "src/images/morado.png", {0}}};

    int selectedColorIndex = 0; // Índice del color actualmente seleccionado

    // Cargar las imágenes de referencia de los personajes
    for (int i = 0; i < NUM_COLORS; i++)
    {
        // Textura original para el juego
        Image imgFull = LoadImage(colors[i].imagePath);
        colors[i].fullTexture = LoadTextureFromImage(imgFull);
        UnloadImage(imgFull);

        // Imagen ilustrativa para la tienda
        Image imgPreview = LoadImage(colors[i].previewPath);
        ImageResize(&imgPreview, 150, 150);
        colors[i].previewTexture = LoadTextureFromImage(imgPreview);
        UnloadImage(imgPreview);
    }

    SetExitKey(0); // Desactiva la tecla de salida (ESC) para evitar cerrar la ventana

    Image fondo = LoadImage("src/images/op1.jpg"); // Carga la imagen del fondo
    ImageResize(&fondo, screenWidth, screenHeight);      // Redimensiona la imagen del fondo al tamaño de la ventana
    fondoTexture = LoadTextureFromImage(fondo);          // Convierte la imagen a textura
    UnloadImage(fondo);                                  // Libera la imagen de memoria

    Image shop = LoadImage("src/images/shop.jpg"); // Carga la imagen del fondo
    ImageResize(&shop, screenWidth, screenHeight);      // Redimensiona la imagen del fondo al tamaño de la ventana
    shop_texture = LoadTextureFromImage(shop);          // Convierte la imagen a textura
    UnloadImage(shop);                                  // Libera la imagen de memoria

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
            ClearBackground(RAYWHITE);
            fondoMenu(fondoTexture);
            menuTitulo("MATH BROS");
            UpdateMusic(gameMusic, musicInitialized, currentMusicLevel, 0);
            // Mostrar monedas en el menú principal
            DrawText(TextFormat("Monedas: %d", coinsCollected), 10, 40, 20, YELLOW);

            // Mostrar personaje equipado actual en una esquina
            // Fondo y marco
            int imgWidth = colors[selectedColorIndex].previewTexture.width;
            int imgHeight = colors[selectedColorIndex].previewTexture.height;
            float scale = 1.2f;                            // Escala para hacer la imagen más grande
            int frameWidth = (int)(imgWidth * scale) + 20; // Margen de 10 píxeles a cada lado
            int frameHeight = (int)(imgHeight * scale) + 20;
            int frameX = 1280 - frameWidth - 20; // 20 píxeles desde el borde derecho
            int frameY = 720 - frameHeight - 60; // 60 píxeles desde el borde inferior

            // Texto "Personaje actual"
            const char *text = "Personaje actual";
            int textSize = 24;
            int textWidth = MeasureText(text, textSize);
            int textX = frameX + (frameWidth - textWidth) / 2;
            int textY = frameY - 30;
            DrawText(text, textX + 2, textY + 2, textSize, BLACK); // Sombra
            DrawText(text, textX, textY, textSize, YELLOW);        // Texto principal

            // Imagen del personaje
            int imgX = frameX + (frameWidth - (int)(imgWidth * scale)) / 2;
            int imgY = frameY + (frameHeight - (int)(imgHeight * scale)) / 2;
            DrawTextureEx(colors[selectedColorIndex].previewTexture,
                          (Vector2){(float)imgX, (float)imgY}, 0.0f, scale, WHITE);

            if (btnsMenu("Jugar", 250))
                pantallaActual = MENU_NIVELES;
            if (btnsMenu("Tienda", 370))
                pantallaActual = MENU_TIENDA;
            if (btnsMenu("Salir", 490))
                pantallaActual = SALIR;
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
            UpdateMusic(gameMusic, musicInitialized, currentMusicLevel, 0);
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
            UpdateMusic(gameMusic, musicInitialized, currentMusicLevel, 1);
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
                UpdateMusic(gameMusic, musicInitialized, currentMusicLevel, 1);
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
            UpdateMusic(gameMusic, musicInitialized, currentMusicLevel, 2);
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
                UpdateMusic(gameMusic, musicInitialized, currentMusicLevel, 2);
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
            UpdateMusic(gameMusic, musicInitialized, currentMusicLevel, 3);
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
                UpdateMusic(gameMusic, musicInitialized, currentMusicLevel, 3);
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
            fondoMenu(shop_texture);
            DrawText(TextFormat("Monedas: %d", coinsCollected), 10, 40, 20, YELLOW);
            menuTitulo("Tienda");
            UpdateMusic(gameMusic, musicInitialized, currentMusicLevel, 0);
            // Mensaje de ayuda centrado
            const char *helpText = "Compra y equipa nuevos personajes!";
            int helpTextSize = 24;
            int helpTextWidth = MeasureText(helpText, helpTextSize);
            int helpTextX = (1280 - helpTextWidth) / 2;
            int helpTextY = 150;
            DrawText(helpText, helpTextX + 2, helpTextY + 2, helpTextSize, BLACK); // Sombra
            DrawText(helpText, helpTextX, helpTextY, helpTextSize, WHITE);         // Texto principal

            int btnWidth = 180; // Reducido para mejor ajuste
            int btnHeight = 45;
            int imageSize = 130; // Reducido ligeramente
            int spacing = 60;    // Espaciado optimizado

            int totalWidth = NUM_COLORS * btnWidth + (NUM_COLORS - 1) * spacing;
            int startX = (1280 - totalWidth) / 2;
            int btnY = 400; // Subido un poco

            for (int i = 0; i < NUM_COLORS; i++)
            {
                int btnX = startX + i * (btnWidth + spacing);

                // Marco decorativo alrededor de la imagen
                int imageX = btnX + (btnWidth - imageSize) / 2;
                int imageY = btnY - imageSize - 30;

                // Marco dorado si está equipado
                if (i == selectedColorIndex)
                {
                    DrawRectangleLinesEx((Rectangle){(float)imageX - 5, (float)imageY - 5, (float)imageSize + 10, (float)imageSize + 10},
                                         3.0f, GOLD);
                }

                DrawTextureEx(colors[i].previewTexture, (Vector2){(float)imageX, (float)imageY},
                              0.0f, (float)imageSize / colors[i].previewTexture.width, WHITE);

                // Nombre del personaje arriba de la imagen
                int nameWidth = MeasureText(colors[i].name, 16);
                DrawText(colors[i].name, btnX + (btnWidth - nameWidth) / 2,
                         imageY - 25, 16, WHITE);

                // Botón de compra mejorado
                if (DrawBuyButton(colors[i].name, colors[i].price, colors[i].unlocked,
                                  btnX, btnY, &coinsCollected))
                {
                    colors[i].unlocked = true;
                }

                // Botón de equipar mejorado
                if (colors[i].unlocked)
                {
                    int equipY = btnY + btnHeight + 15;
                    Rectangle equipBtn = {(float)btnX, (float)equipY, (float)btnWidth, (float)btnHeight};
                    bool equipMouseOver = CheckCollisionPointRec(GetMousePosition(), equipBtn);

                    Color equipColor;
                    if (i == selectedColorIndex)
                        equipColor = Fade(GOLD, 0.8f); // Dorado para equipado
                    else if (equipMouseOver)
                        equipColor = Fade(GREEN, 0.8f);
                    else
                        equipColor = Fade(DARKGREEN, 0.7f);

                    DrawRectangleRec(equipBtn, equipColor);
                    DrawRectangleLinesEx(equipBtn, 2.0f,
                                         (i == selectedColorIndex) ? GOLD : DARKGREEN);

                    const char *equipText = (i == selectedColorIndex) ? "EQUIPADO" : "EQUIPAR";
                    int equipTextWidth = MeasureText(equipText, 14);
                    DrawText(equipText, btnX + (btnWidth - equipTextWidth) / 2,
                             equipY + (btnHeight - 14) / 2, 14, WHITE);

                    if (equipMouseOver && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                    {
                        selectedColorIndex = i;
                    }
                }
            }

            if (btnsMenu("Volver", 580))
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
            if (musicInitialized)
            {
                StopMusicStream(gameMusic);
                UnloadMusicStream(gameMusic);
                CloseAudioDevice();
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

bool DrawBuyButton(const char *text, int price, bool unlocked, int posX, int posY, int *coins)
{
    int btnWidth = 200; // CAMBIADO: ahora coincide con el ancho del menú
    int btnHeight = 50;
    int btnX = posX;
    Rectangle btn = {(float)btnX, (float)posY, (float)btnWidth, (float)btnHeight};
    bool mouseOver = CheckCollisionPointRec(GetMousePosition(), btn);
    Color btnColor = unlocked ? Fade(GREEN, 0.7f) : (mouseOver ? Fade(DARKBLUE, 0.8f) : Fade(BLUE, 0.7f));

    DrawRectangleRec(btn, btnColor);
    DrawRectangleLinesEx(btn, 5.0f, DARKBLUE); // Borde más delgado

    const char *displayText = unlocked ? "Desbloqueado" : TextFormat("%s - %d Monedas", text, price);
    int textWidth = MeasureText(displayText, 16); // Texto más pequeño para que quepa
    DrawText(displayText, btnX + (btnWidth - textWidth) / 2, posY + (btnHeight - 16) / 2, 16, WHITE);

    // Permitir compra si no está desbloqueado y hay suficientes monedas
    if (mouseOver && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !unlocked && *coins >= price)
    {
        *coins -= price;
        return true;
    }
    return false;
}

void UpdateMusic(Music &gameMusic, bool &musicInitialized, int &currentMusicLevel, int level)
{
    if (!musicInitialized) return;
    
    if (level != currentMusicLevel)
    {
        StopMusicStream(gameMusic);
        UnloadMusicStream(gameMusic);
        
        switch(level)
        {
            case 0: // Main menu
                gameMusic = LoadMusicStream("src/sound/main.mp3");
                break;
            case 1: // Level 1
                gameMusic = LoadMusicStream("src/sound/level1.mp3");
                break;
            case 2: // Level 2
                gameMusic = LoadMusicStream("src/sound/level2.mp3");
                break;
            case 3: // Level 3
                gameMusic = LoadMusicStream("src/sound/level3.mp3");
                break;
            default:
                gameMusic = LoadMusicStream("src/sound/main.mp3");
        }
        currentMusicLevel = level;
        PlayMusicStream(gameMusic);
    }
    UpdateMusicStream(gameMusic);
}

