#include <stdio.h>
#include <string>
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

    InitWindow(screenWidth, screenHeight, "Math Bros - Super Rework Edition");

    std::string applicationDirectory = GetApplicationDirectory();
    while (applicationDirectory.size() > 3 &&
           (applicationDirectory.back() == '\\' || applicationDirectory.back() == '/'))
    {
        applicationDirectory.pop_back();
    }

    // ChangeDirectory devuelve bool en raylib 5.x e int (0 = exito) en 6.x,
    // asi que se comprueba el directorio resultante en lugar del valor devuelto.
    if (applicationDirectory != GetWorkingDirectory())
    {
        ChangeDirectory(applicationDirectory.c_str());
    }
    if (applicationDirectory != GetWorkingDirectory())
    {
        TraceLog(LOG_ERROR, "No se pudo abrir la carpeta del ejecutable: %s", applicationDirectory.c_str());
        CloseWindow();
        return 1;
    }

    Music gameMusic = {0};
    bool musicInitialized = false;
    int currentMusicLevel = 0;
    InitAudioDevice();
    if (FileExists("src/sound/main.mp3"))
    {
        gameMusic = LoadMusicStream("src/sound/main.mp3");
        musicInitialized = true;
        SetMusicVolume(gameMusic, 0.12f);
        PlayMusicStream(gameMusic);
    }

    if (FileExists("src/images/icon.png"))
    {
        Image icon = LoadImage("src/images/icon.png");
        SetWindowIcon(icon);
        UnloadImage(icon);
    }

    // Carga inteligente de fondos (soporta .png y .jpg automáticamente)
    Texture2D fondosNiveles[NUM_NIVELES];
    const char *rutasFondos[NUM_NIVELES] = {
        "src/images/nivel1.png",
        "src/images/nivel2.png",
        "src/images/nivel3.jpg"};

    for (int i = 0; i < NUM_NIVELES; i++)
    {
        fondosNiveles[i] = LoadTextureSmart(rutasFondos[i], screenWidth, screenHeight);
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
        colors[i].fullTexture = LoadTextureSmart(colors[i].imagePath, 0, 0);
        colors[i].previewTexture = LoadTextureSmart(colors[i].previewPath, 140, 140);
    }

    SetExitKey(0);

    Texture2D fondoTexture = LoadTextureSmart("src/images/op1.jpg", screenWidth, screenHeight);
    Texture2D shop_texture = LoadTextureSmart("src/images/shop.jpg", screenWidth, screenHeight);

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
            DrawMarioBackground(fondoTexture, 0, 0.0f);
            menuTitulo("MATH BROS");

            // Contador de Monedas estilo Super Mario en la esquina superior izquierda
            DrawMarioPanel((Rectangle){20, 20, 250, 62}, (Color){30, 65, 145, 240}, MARIO_YELLOW);
            DrawCircle(52, 51, 14, BLACK);
            DrawCircle(52, 51, 12, MARIO_YELLOW);
            DrawText(TextFormat("MONEDAS: %d", coinsCollected), 76, 39, 24, MARIO_YELLOW);

            // Vitrina estilo Mario para el personaje equipado actual
            int frameWidth = 220;
            int frameHeight = 235;
            int frameX = 1280 - frameWidth - 35;
            int frameY = 720 - frameHeight - 65;

            DrawMarioPanel((Rectangle){(float)frameX, (float)frameY, (float)frameWidth, (float)frameHeight},
                           (Color){35, 75, 155, 235}, MARIO_YELLOW);

            const char *text = "PERSONAJE";
            int textSize = 22;
            int textWidth = MeasureText(text, textSize);
            DrawText(text, frameX + (frameWidth - textWidth) / 2 + 2, frameY + 16, textSize, BLACK);
            DrawText(text, frameX + (frameWidth - textWidth) / 2, frameY + 14, textSize, MARIO_YELLOW);

            float charBounce = sinf((float)GetTime() * 4.5f) * 5.0f;
            int imgW = colors[selectedColorIndex].previewTexture.width;
            int imgH = colors[selectedColorIndex].previewTexture.height;
            int imgX = frameX + (frameWidth - imgW) / 2;
            int imgY = frameY + 52 + (int)charBounce;
            DrawTexture(colors[selectedColorIndex].previewTexture, imgX, imgY, WHITE);

            int nameW = MeasureText(colors[selectedColorIndex].name, 20);
            DrawText(colors[selectedColorIndex].name, frameX + (frameWidth - nameW) / 2, frameY + frameHeight - 34, 20, WHITE);

            if (btnsMenu("Jugar", 235))
                pantallaActual = MENU_NIVELES;
            if (btnsMenu("Tienda", 345))
                pantallaActual = MENU_TIENDA;
            if (btnsMenu("Salir", 455))
                pantallaActual = SALIR;

            EndDrawing();
            break;
        }

        case MENU_NIVELES:
        {
            UpdateMusic(gameMusic, musicInitialized, currentMusicLevel, 0);
            BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawMarioBackground(fondoTexture, 0, 150.0f);
            menuTitulo("MUNDOS");

            if (btnsNiveles("MUNDO 1-1", "Facil - Primaria", 115, 205, nivelesDesbloqueados[0], highScores[0]))
                pantallaActual = MENU_JUEGO;

            if (btnsNiveles("MUNDO 1-2", "Medio - Primaria", 495, 205, nivelesDesbloqueados[1], highScores[1]))
                pantallaActual = MENU_JUEGO2;

            if (btnsNiveles("MUNDO 1-3", "Dificil - Preparatoria", 875, 205, nivelesDesbloqueados[2], highScores[2]))
                pantallaActual = MENU_JUEGO3;

            // Botón Mario para reiniciar el progreso guardado o tecla SUPR
            Rectangle resetBtn = {460, 435, 360, 48};
            if (DrawMarioButton(resetBtn, "Reiniciar Progreso [SUPR]", (Color){140, 45, 45, 255}, MARIO_RED, 20) || IsKeyPressed(KEY_DELETE))
            {
                coinsCollected = 0;
                selectedColorIndex = 0;
                for (int i = 1; i < NUM_COLORS; i++) colors[i].unlocked = false;
                for (int i = 0; i < NUM_NIVELES; i++) { nivelesDesbloqueados[i] = (i == 0); highScores[i] = 0; }
                GuardarProgreso(coinsCollected, selectedColorIndex, colors, nivelesDesbloqueados, highScores);
            }

            if (btnsMenu("Volver", 525))
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
            DrawMarioBackground(shop_texture, 4, 0.0f);

            DrawMarioPanel((Rectangle){20, 20, 250, 62}, (Color){30, 65, 145, 240}, MARIO_YELLOW);
            DrawCircle(52, 51, 14, BLACK);
            DrawCircle(52, 51, 12, MARIO_YELLOW);
            DrawText(TextFormat("MONEDAS: %d", coinsCollected), 76, 39, 24, MARIO_YELLOW);

            menuTitulo("TIENDA");

            const char *helpText = "★ ¡Desbloquea y equipa tus personajes favoritos! ★";
            int helpTextSize = 24;
            int helpTextWidth = MeasureText(helpText, helpTextSize);
            int helpTextX = (1280 - helpTextWidth) / 2;
            DrawText(helpText, helpTextX + 2, 148, helpTextSize, BLACK);
            DrawText(helpText, helpTextX, 146, helpTextSize, MARIO_YELLOW);

            int cardWidth = 210;
            int spacing = 32;
            int totalWidth = NUM_COLORS * cardWidth + (NUM_COLORS - 1) * spacing;
            int startX = (1280 - totalWidth) / 2;
            int cardY = 195;

            for (int i = 0; i < NUM_COLORS; i++)
            {
                int cardX = startX + i * (cardWidth + spacing);
                bool isEquipped = (i == selectedColorIndex);

                Color panelCol = isEquipped ? (Color){45, 115, 65, 245} : (colors[i].unlocked ? (Color){40, 75, 150, 240} : (Color){60, 60, 75, 240});
                DrawMarioPanel((Rectangle){(float)cardX, (float)cardY, (float)cardWidth, 330}, panelCol, isEquipped ? MARIO_YELLOW : WHITE);

                int nameWidth = MeasureText(colors[i].name, 22);
                DrawText(colors[i].name, cardX + (cardWidth - nameWidth) / 2 + 2, cardY + 20, 22, BLACK);
                DrawText(colors[i].name, cardX + (cardWidth - nameWidth) / 2, cardY + 18, 22, MARIO_YELLOW);

                float previewBounce = isEquipped ? sinf((float)GetTime() * 5.0f) * 5.0f : 0.0f;
                int imageSize = 125;
                int imageX = cardX + (cardWidth - imageSize) / 2;
                int imageY = cardY + 52 + (int)previewBounce;

                if (colors[i].previewTexture.id != 0)
                {
                    DrawTextureEx(colors[i].previewTexture, (Vector2){(float)imageX, (float)imageY},
                                  0.0f, (float)imageSize / colors[i].previewTexture.width, WHITE);
                }

                if (DrawBuyButton(colors[i].name, colors[i].price, colors[i].unlocked, cardX + 15, cardY + 195, &coinsCollected))
                {
                    colors[i].unlocked = true;
                    GuardarProgreso(coinsCollected, selectedColorIndex, colors, nivelesDesbloqueados, highScores);
                }

                if (colors[i].unlocked)
                {
                    Rectangle equipBtn = {(float)(cardX + 15), (float)(cardY + 260), (float)(cardWidth - 30), 48};
                    const char *equipText = isEquipped ? "★ EQUIPADO ★" : "EQUIPAR";
                    Color eqBase = isEquipped ? MARIO_ORANGE : MARIO_GREEN;
                    Color eqHover = isEquipped ? MARIO_YELLOW : (Color){65, 220, 85, 255};

                    if (DrawMarioButton(equipBtn, equipText, eqBase, eqHover, 18))
                    {
                        selectedColorIndex = i;
                        GuardarProgreso(coinsCollected, selectedColorIndex, colors, nivelesDesbloqueados, highScores);
                    }
                }
            }

            if (btnsMenu("Volver", 560))
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
    Rectangle btn = {(float)posX, (float)posY, 180.0f, 50.0f};
    const char *displayText = unlocked ? "DESBLOQUEADO" : TextFormat("%d MONEDAS", price);

    Color baseCol = unlocked ? (Color){45, 130, 65, 255} : ((*coins >= price) ? MARIO_BLUE : DARKGRAY);
    Color hoverCol = unlocked ? (Color){45, 130, 65, 255} : ((*coins >= price) ? SKYBLUE : GRAY);

    if (DrawMarioButton(btn, displayText, baseCol, hoverCol, 17))
    {
        if (!unlocked && *coins >= price)
        {
            *coins -= price;
            return true;
        }
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

        const char *musicPath = "src/sound/main.mp3";
        if (level == 1 && FileExists("src/sound/level1.mp3")) musicPath = "src/sound/level1.mp3";
        else if (level == 2 && FileExists("src/sound/level2.mp3")) musicPath = "src/sound/level2.mp3";
        else if (level == 3 && FileExists("src/sound/level3.mp3")) musicPath = "src/sound/level3.mp3";

        if (FileExists(musicPath))
        {
            gameMusic = LoadMusicStream(musicPath);
            SetMusicVolume(gameMusic, 0.12f);
            PlayMusicStream(gameMusic);
        }
        currentMusicLevel = level;
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
