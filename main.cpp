#include <stdio.h>
#include "mainMenu.h"
#include "menuNiveles.h"
#include "menuPausa.h"
#include "niveles.h"

#define NUM_NIVELES 3
#define NUM_COLORS 5
#define SAVE_FILE "savegame.dat"

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

typedef struct
{
    const char *name;
    int price;
    bool unlocked;
    const char *imagePath;
    Texture2D fullTexture;
    int frames;
    const char *previewPath;
    Texture2D previewTexture;
} ColorOption;

typedef struct
{
    int coins;
    int selectedSkin;
    bool unlockedSkins[NUM_COLORS];
    bool unlockedLevels[NUM_NIVELES];
    int highScores[NUM_NIVELES];
} SaveData;

bool DrawBuyButton(const char *text, int price, bool unlocked, int posX, int posY, int *coins);
void UpdateMusic(Music &gameMusic, bool &musicInitialized, int &currentMusicLevel, int level);
void GuardarProgreso(int coins, int selectedSkin, ColorOption colors[], bool nivelesDesbloqueados[], int highScores[]);
void CargarProgreso(int *coins, int *selectedSkin, ColorOption colors[], bool nivelesDesbloqueados[], int highScores[]);

int main()
{
    const int screenWidth = 1280;
    const int screenHeight = 720;
    Vector2 posInicialJugador = {50.0f, 550.0f};

    InitWindow(screenWidth, screenHeight, "Math Bros");

    if (!ChangeDirectory(GetApplicationDirectory()))
    {
        TraceLog(LOG_ERROR, "No se pudo abrir la carpeta del ejecutable: %s", GetApplicationDirectory());
        CloseWindow();
        return 1;
    }

    Music gameMusic;
    bool musicInitialized = false;
    int currentMusicLevel = 0;
    InitAudioDevice();
    gameMusic = LoadMusicStream("src/sound/main.mp3");
    musicInitialized = true;
    SetMusicVolume(gameMusic, 0.1f);
    PlayMusicStream(gameMusic);

    Image icon = LoadImage("src/images/icon.png");
    SetWindowIcon(icon);
    UnloadImage(icon);

    Texture2D fondosNiveles[NUM_NIVELES];
    const char *rutasFondos[NUM_NIVELES] = {
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

    int coinsCollected = 0;
    int selectedColorIndex = 0;
    bool nivelesDesbloqueados[NUM_NIVELES] = {true, false, false};
    int highScores[NUM_NIVELES] = {0, 0, 0};

    ColorOption colors[NUM_COLORS] = {
        {"Default", 0, true, "src/images/zorrodefault.png", {0}, 4, "src/images/default.png", {0}},
        {"Rojo", 3, false, "src/images/zorrorojo.png", {0}, 4, "src/images/red.png", {0}},
        {"Azul", 8, false, "src/images/zorroazul.png", {0}, 4, "src/images/azul.png", {0}},
        {"Verde", 10, false, "src/images/zorroverde.png", {0}, 4, "src/images/verde.png", {0}},
        {"Morado", 15, false, "src/images/zorromorado.png", {0}, 4, "src/images/morado.png", {0}}};

    CargarProgreso(&coinsCollected, &selectedColorIndex, colors, nivelesDesbloqueados, highScores);

    for (int i = 0; i < NUM_COLORS; i++)
    {
        Image imgFull = LoadImage(colors[i].imagePath);
        colors[i].fullTexture = LoadTextureFromImage(imgFull);
        UnloadImage(imgFull);

        Image imgPreview = LoadImage(colors[i].previewPath);
        ImageResize(&imgPreview, 150, 150);
        colors[i].previewTexture = LoadTextureFromImage(imgPreview);
        UnloadImage(imgPreview);
    }

    SetExitKey(0);

    Image fondo = LoadImage("src/images/op1.jpg");
    ImageResize(&fondo, screenWidth, screenHeight);
    Texture2D fondoTexture = LoadTextureFromImage(fondo);
    UnloadImage(fondo);

    Image shop = LoadImage("src/images/shop.jpg");
    ImageResize(&shop, screenWidth, screenHeight);
    Texture2D shop_texture = LoadTextureFromImage(shop);
    UnloadImage(shop);

    SetTargetFPS(60);

    while (!WindowShouldClose() && pantallaActual != SALIR)
    {
        switch (pantallaActual)
        {
        case MENU_PRINCIPAL:
        {
            UpdateMusic(gameMusic, musicInitialized, currentMusicLevel, 0);
            BeginDrawing();
            ClearBackground(RAYWHITE);
            fondoMenu(fondoTexture);
            menuTitulo("MATH BROS");

            DrawRectangle(10, 15, 220, 45, Fade(BLACK, 0.6f));
            DrawRectangleLines(10, 15, 220, 45, GOLD);
            DrawText(TextFormat("Monedas: %d", coinsCollected), 25, 27, 24, YELLOW);

            int imgWidth = colors[selectedColorIndex].previewTexture.width;
            int imgHeight = colors[selectedColorIndex].previewTexture.height;
            float scale = 1.2f;
            int frameWidth = (int)(imgWidth * scale) + 20;
            int frameHeight = (int)(imgHeight * scale) + 20;
            int frameX = 1280 - frameWidth - 20;
            int frameY = 720 - frameHeight - 60;

            const char *text = "Personaje actual";
            int textSize = 24;
            int textWidth = MeasureText(text, textSize);
            int textX = frameX + (frameWidth - textWidth) / 2;
            int textY = frameY - 30;
            DrawText(text, textX + 2, textY + 2, textSize, BLACK);
            DrawText(text, textX, textY, textSize, YELLOW);

            int imgX = frameX + (frameWidth - (int)(imgWidth * scale)) / 2;
            int imgY = frameY + (frameHeight - (int)(imgHeight * scale)) / 2;
            DrawTextureEx(colors[selectedColorIndex].previewTexture, (Vector2){(float)imgX, (float)imgY}, 0.0f, scale, WHITE);

            if (btnsMenu("Jugar", 250))
                pantallaActual = MENU_NIVELES;
            if (btnsMenu("Tienda", 365))
                pantallaActual = MENU_TIENDA;
            if (btnsMenu("Salir", 480))
                pantallaActual = SALIR;

            EndDrawing();
            break;
        }

        case MENU_NIVELES:
        {
            UpdateMusic(gameMusic, musicInitialized, currentMusicLevel, 0);
            BeginDrawing();
            ClearBackground(RAYWHITE);
            fondoMenu(fondoTexture);
            menuTitulo("Niveles");

            if (btnsNiveles("Nivel 1", 200, 240, nivelesDesbloqueados[0], highScores[0]))
                pantallaActual = MENU_JUEGO;

            if (btnsNiveles("Nivel 2", 530, 240, nivelesDesbloqueados[1], highScores[1]))
                pantallaActual = MENU_JUEGO2;

            if (btnsNiveles("Nivel 3", 860, 240, nivelesDesbloqueados[2], highScores[2]))
                pantallaActual = MENU_JUEGO3;

            DrawText("Presiona [SUPR / DEL] para reiniciar el progreso guardado", 380, 420, 18, LIGHTGRAY);
            if (IsKeyPressed(KEY_DELETE))
            {
                coinsCollected = 0;
                selectedColorIndex = 0;
                for (int i = 1; i < NUM_COLORS; i++) colors[i].unlocked = false;
                for (int i = 0; i < NUM_NIVELES; i++) { nivelesDesbloqueados[i] = (i == 0); highScores[i] = 0; }
                GuardarProgreso(coinsCollected, selectedColorIndex, colors, nivelesDesbloqueados, highScores);
            }

            if (btnsMenu("Volver", 530))
                pantallaActual = MENU_PRINCIPAL;

            EndDrawing();
            break;
        }

        case MENU_JUEGO:
        {
            UpdateMusic(gameMusic, musicInitialized, currentMusicLevel, 1);
            GameStatus status = nivel1(posInicialJugador, fondosNiveles[0], &coinsCollected, &highScores[0],
                                       colors[selectedColorIndex].fullTexture, colors[selectedColorIndex].frames, &gameMusic);
            if (status == GAME_WON)
            {
                nivelesDesbloqueados[1] = true;
                GuardarProgreso(coinsCollected, selectedColorIndex, colors, nivelesDesbloqueados, highScores);
                pantallaActual = MENU_NIVELES;
            }
            else if (status == GAME_OVER)
                pantallaActual = MENU_NIVELES;
            else if (status == GAME_EXIT_MAIN_MENU)
                pantallaActual = MENU_PRINCIPAL;
            else if (status == GAME_EXIT_APP)
                pantallaActual = SALIR;
            break;
        }

        case MENU_JUEGO2:
        {
            UpdateMusic(gameMusic, musicInitialized, currentMusicLevel, 2);
            GameStatus status = nivel2(posInicialJugador, fondosNiveles[1], &coinsCollected, &highScores[1],
                                       colors[selectedColorIndex].fullTexture, colors[selectedColorIndex].frames, &gameMusic);
            if (status == GAME_WON)
            {
                nivelesDesbloqueados[2] = true;
                GuardarProgreso(coinsCollected, selectedColorIndex, colors, nivelesDesbloqueados, highScores);
                pantallaActual = MENU_NIVELES;
            }
            else if (status == GAME_OVER)
                pantallaActual = MENU_NIVELES;
            else if (status == GAME_EXIT_MAIN_MENU)
                pantallaActual = MENU_PRINCIPAL;
            else if (status == GAME_EXIT_APP)
                pantallaActual = SALIR;
            break;
        }

        case MENU_JUEGO3:
        {
            UpdateMusic(gameMusic, musicInitialized, currentMusicLevel, 3);
            GameStatus status = nivel3(posInicialJugador, fondosNiveles[2], &coinsCollected, &highScores[2],
                                       colors[selectedColorIndex].fullTexture, colors[selectedColorIndex].frames, &gameMusic);
            if (status == GAME_WON)
            {
                GuardarProgreso(coinsCollected, selectedColorIndex, colors, nivelesDesbloqueados, highScores);
                pantallaActual = MENU_NIVELES;
            }
            else if (status == GAME_OVER)
                pantallaActual = MENU_NIVELES;
            else if (status == GAME_EXIT_MAIN_MENU)
                pantallaActual = MENU_PRINCIPAL;
            else if (status == GAME_EXIT_APP)
                pantallaActual = SALIR;
            break;
        }

        case MENU_TIENDA:
        {
            UpdateMusic(gameMusic, musicInitialized, currentMusicLevel, 0);
            BeginDrawing();
            ClearBackground(RAYWHITE);
            fondoMenu(shop_texture);

            DrawRectangle(10, 15, 220, 45, Fade(BLACK, 0.6f));
            DrawRectangleLines(10, 15, 220, 45, GOLD);
            DrawText(TextFormat("Monedas: %d", coinsCollected), 25, 27, 24, YELLOW);

            menuTitulo("Tienda");

            const char *helpText = "¡Compra y equipa nuevos personajes! (Guardado automatico)";
            int helpTextSize = 24;
            int helpTextWidth = MeasureText(helpText, helpTextSize);
            int helpTextX = (1280 - helpTextWidth) / 2;
            int helpTextY = 160;
            DrawText(helpText, helpTextX + 2, helpTextY + 2, helpTextSize, BLACK);
            DrawText(helpText, helpTextX, helpTextY, helpTextSize, WHITE);

            int btnWidth = 180;
            int btnHeight = 45;
            int imageSize = 130;
            int spacing = 60;

            int totalWidth = NUM_COLORS * btnWidth + (NUM_COLORS - 1) * spacing;
            int startX = (1280 - totalWidth) / 2;
            int btnY = 400;

            for (int i = 0; i < NUM_COLORS; i++)
            {
                int btnX = startX + i * (btnWidth + spacing);
                int imageX = btnX + (btnWidth - imageSize) / 2;
                int imageY = btnY - imageSize - 30;

                if (i == selectedColorIndex)
                {
                    DrawRectangleLinesEx((Rectangle){(float)imageX - 5, (float)imageY - 5, (float)imageSize + 10, (float)imageSize + 10}, 3.0f, GOLD);
                }

                DrawTextureEx(colors[i].previewTexture, (Vector2){(float)imageX, (float)imageY},
                              0.0f, (float)imageSize / colors[i].previewTexture.width, WHITE);

                int nameWidth = MeasureText(colors[i].name, 18);
                DrawText(colors[i].name, btnX + (btnWidth - nameWidth) / 2, imageY - 25, 18, WHITE);

                if (DrawBuyButton(colors[i].name, colors[i].price, colors[i].unlocked, btnX, btnY, &coinsCollected))
                {
                    colors[i].unlocked = true;
                    GuardarProgreso(coinsCollected, selectedColorIndex, colors, nivelesDesbloqueados, highScores);
                }

                if (colors[i].unlocked)
                {
                    int equipY = btnY + btnHeight + 15;
                    Rectangle equipBtn = {(float)btnX, (float)equipY, (float)btnWidth, (float)btnHeight};
                    bool equipMouseOver = CheckCollisionPointRec(GetMousePosition(), equipBtn);

                    Color equipColor;
                    if (i == selectedColorIndex)
                        equipColor = Fade(GOLD, 0.85f);
                    else if (equipMouseOver)
                        equipColor = Fade(GREEN, 0.85f);
                    else
                        equipColor = Fade(DARKGREEN, 0.75f);

                    DrawRectangleRec(equipBtn, equipColor);
                    DrawRectangleLinesEx(equipBtn, 2.0f, (i == selectedColorIndex) ? GOLD : DARKGREEN);

                    const char *equipText = (i == selectedColorIndex) ? "EQUIPADO" : "EQUIPAR";
                    int equipTextWidth = MeasureText(equipText, 16);
                    DrawText(equipText, btnX + (btnWidth - equipTextWidth) / 2, equipY + (btnHeight - 16) / 2, 16, WHITE);

                    if (equipMouseOver && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                    {
                        selectedColorIndex = i;
                        GuardarProgreso(coinsCollected, selectedColorIndex, colors, nivelesDesbloqueados, highScores);
                    }
                }
            }

            if (btnsMenu("Volver", 580))
                pantallaActual = MENU_PRINCIPAL;

            EndDrawing();
            break;
        }

        default:
            break;
        }
    }

    GuardarProgreso(coinsCollected, selectedColorIndex, colors, nivelesDesbloqueados, highScores);

    UnloadTexture(fondoTexture);
    UnloadTexture(shop_texture);
    for (int i = 0; i < NUM_NIVELES; i++)
    {
        UnloadTexture(fondosNiveles[i]);
    }
    for (int i = 0; i < NUM_COLORS; i++)
    {
        UnloadTexture(colors[i].fullTexture);
        UnloadTexture(colors[i].previewTexture);
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

bool DrawBuyButton(const char *text, int price, bool unlocked, int posX, int posY, int *coins)
{
    int btnWidth = 180;
    int btnHeight = 45;
    Rectangle btn = {(float)posX, (float)posY, (float)btnWidth, (float)btnHeight};
    bool mouseOver = CheckCollisionPointRec(GetMousePosition(), btn);
    Color btnColor = unlocked ? Fade(GREEN, 0.7f) : (mouseOver ? Fade(DARKBLUE, 0.85f) : Fade(BLUE, 0.75f));

    DrawRectangleRec(btn, btnColor);
    DrawRectangleLinesEx(btn, 3.0f, DARKBLUE);

    const char *displayText = unlocked ? "Desbloqueado" : TextFormat("%s - %d Monedas", text, price);
    int textWidth = MeasureText(displayText, 15);
    DrawText(displayText, posX + (btnWidth - textWidth) / 2, posY + (btnHeight - 15) / 2, 15, WHITE);

    if (mouseOver && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !unlocked && *coins >= price)
    {
        *coins -= price;
        return true;
    }
    return false;
}

void UpdateMusic(Music &gameMusic, bool &musicInitialized, int &currentMusicLevel, int level)
{
    if (!musicInitialized)
        return;

    if (level != currentMusicLevel)
    {
        StopMusicStream(gameMusic);
        UnloadMusicStream(gameMusic);

        switch (level)
        {
        case 0:
            gameMusic = LoadMusicStream("src/sound/main.mp3");
            break;
        case 1:
            gameMusic = LoadMusicStream("src/sound/level1.mp3");
            break;
        case 2:
            gameMusic = LoadMusicStream("src/sound/level2.mp3");
            break;
        case 3:
            gameMusic = LoadMusicStream("src/sound/level3.mp3");
            break;
        default:
            gameMusic = LoadMusicStream("src/sound/main.mp3");
            break;
        }
        SetMusicVolume(gameMusic, 0.1f);
        currentMusicLevel = level;
        PlayMusicStream(gameMusic);
    }
    UpdateMusicStream(gameMusic);
}

void GuardarProgreso(int coins, int selectedSkin, ColorOption colors[], bool nivelesDesbloqueados[], int highScores[])
{
    SaveData data = {0};
    data.coins = coins;
    data.selectedSkin = selectedSkin;
    for (int i = 0; i < NUM_COLORS; i++)
        data.unlockedSkins[i] = colors[i].unlocked;
    for (int i = 0; i < NUM_NIVELES; i++)
    {
        data.unlockedLevels[i] = nivelesDesbloqueados[i];
        data.highScores[i] = highScores[i];
    }

    FILE *file = fopen(SAVE_FILE, "wb");
    if (file != NULL)
    {
        fwrite(&data, sizeof(SaveData), 1, file);
        fclose(file);
    }
}

void CargarProgreso(int *coins, int *selectedSkin, ColorOption colors[], bool nivelesDesbloqueados[], int highScores[])
{
    FILE *file = fopen(SAVE_FILE, "rb");
    if (file != NULL)
    {
        SaveData data = {0};
        if (fread(&data, sizeof(SaveData), 1, file) == 1)
        {
            *coins = data.coins;
            *selectedSkin = (data.selectedSkin >= 0 && data.selectedSkin < NUM_COLORS) ? data.selectedSkin : 0;
            for (int i = 0; i < NUM_COLORS; i++)
                colors[i].unlocked = data.unlockedSkins[i];
            colors[0].unlocked = true;
            for (int i = 0; i < NUM_NIVELES; i++)
            {
                nivelesDesbloqueados[i] = data.unlockedLevels[i];
                highScores[i] = data.highScores[i];
            }
            nivelesDesbloqueados[0] = true;
        }
        fclose(file);
    }
}