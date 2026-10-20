#ifndef NIVELES_H
#define NIVELES_H

#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "menuPausa.h"

#define MAX_ENVIRONMENT_ELEMENTS 9
#define MAX_COINS 6
#define MAX_PARTICLES 80
#define MAX_ENEMIES 3
#define MAX_FLOATING_TEXTS 10

typedef struct Player
{
    Vector2 position;
    float speed;
    bool canJump;
    int jumpsAvailable;
    float coyoteTimer;
    int lives;
    float invulnerableTimer;
    float dashTimer;
    float dashCooldown;
} Player;

typedef struct EnvElement
{
    Rectangle rect;
    int blocking;
    Color color;
    float moveSpeed;
    float minX;
    float maxX;
} EnvElement;

typedef struct Coin
{
    Vector2 position;
    bool active;
    Rectangle rect;
} Coin;

typedef struct Enemy
{
    Vector2 position;
    float width;
    float height;
    float speed;
    float minX;
    float maxX;
    bool active;
} Enemy;

typedef struct Question
{
    char text[64];
    char options[4][32];
    int correctAnswer;
} Question;

typedef struct Particle
{
    Vector2 position;
    Vector2 velocity;
    float alpha;
    float size;
    Color color;
    bool active;
} Particle;

typedef struct FloatingText
{
    Vector2 position;
    char text[40];
    float alpha;
    Color color;
    bool active;
} FloatingText;

typedef enum
{
    GAME_CONTINUE,
    GAME_PAUSE,
    GAME_OVER,
    GAME_WON,
    GAME_EXIT_MAIN_MENU,
    GAME_EXIT_APP
} GameStatus;

void DrawTiledPlatform(Texture2D texture, Rectangle platform);
void PlayCoinSound(Sound coinSound);
void SpawnParticles(Particle particles[], Vector2 pos, Color color, int count);
void UpdateAndDrawParticles(Particle particles[], float deltaTime);
void SpawnFloatingText(FloatingText texts[], Vector2 pos, const char *msg, Color color);
void UpdateAndDrawFloatingTexts(FloatingText texts[], float deltaTime);
Question GenerarPreguntaAleatoria(int dificultad);

Question GenerarPreguntaAleatoria(int dificultad)
{
    Question q = {0};
    int a = 0, b = 0, c = 0, respuesta = 0;
    int tipo = GetRandomValue(0, 2);

    if (dificultad == 1)
    {
        if (tipo == 0)
        {
            a = GetRandomValue(2, 9);
            b = GetRandomValue(2, 9);
            respuesta = a * b;
            snprintf(q.text, sizeof(q.text), "¿Cuanto es %d x %d?", a, b);
        }
        else if (tipo == 1)
        {
            a = GetRandomValue(15, 60);
            b = GetRandomValue(12, 50);
            respuesta = a + b;
            snprintf(q.text, sizeof(q.text), "¿Cuanto es %d + %d?", a, b);
        }
        else
        {
            a = GetRandomValue(35, 95);
            b = GetRandomValue(10, a - 5);
            respuesta = a - b;
            snprintf(q.text, sizeof(q.text), "¿Cuanto es %d - %d?", a, b);
        }
    }
    else if (dificultad == 2)
    {
        if (tipo == 0)
        {
            b = GetRandomValue(4, 9);
            respuesta = GetRandomValue(4, 12);
            a = b * respuesta;
            snprintf(q.text, sizeof(q.text), "¿Cuanto es %d / %d?", a, b);
        }
        else if (tipo == 1)
        {
            a = GetRandomValue(110, 350);
            b = GetRandomValue(45, 180);
            respuesta = a + b;
            snprintf(q.text, sizeof(q.text), "¿Cuanto es %d + %d?", a, b);
        }
        else
        {
            a = GetRandomValue(7, 14);
            b = GetRandomValue(6, 11);
            respuesta = a * b;
            snprintf(q.text, sizeof(q.text), "¿Cuanto es %d x %d?", a, b);
        }
    }
    else
    {
        if (tipo == 0)
        {
            a = GetRandomValue(6, 12);
            b = GetRandomValue(4, 9);
            c = GetRandomValue(10, 40);
            respuesta = (a * b) + c;
            snprintf(q.text, sizeof(q.text), "¿Cuanto es (%d x %d) + %d?", a, b, c);
        }
        else if (tipo == 1)
        {
            a = GetRandomValue(8, 15);
            b = GetRandomValue(5, 10);
            c = GetRandomValue(5, 25);
            respuesta = (a * b) - c;
            snprintf(q.text, sizeof(q.text), "¿Cuanto es (%d x %d) - %d?", a, b, c);
        }
        else
        {
            a = GetRandomValue(250, 600);
            b = GetRandomValue(100, 220);
            c = GetRandomValue(15, 80);
            respuesta = a - b + c;
            snprintf(q.text, sizeof(q.text), "¿Cuanto es %d - %d + %d?", a, b, c);
        }
    }

    q.correctAnswer = GetRandomValue(0, 3);
    int valoresUsados[4] = {0};

    for (int i = 0; i < 4; i++)
    {
        if (i == q.correctAnswer)
        {
            valoresUsados[i] = respuesta;
        }
        else
        {
            int distractor = 0;
            bool repetido = false;
            do
            {
                repetido = false;
                int offset = GetRandomValue(-12, 12);
                if (offset == 0)
                    offset = 3;
                distractor = respuesta + offset;
                if (distractor < 0)
                    distractor = respuesta + GetRandomValue(2, 10);
                if (distractor == respuesta)
                    repetido = true;
                for (int j = 0; j < i; j++)
                {
                    if (valoresUsados[j] == distractor)
                        repetido = true;
                }
            } while (repetido);
            valoresUsados[i] = distractor;
        }
        snprintf(q.options[i], sizeof(q.options[i]), "%d", valoresUsados[i]);
    }

    return q;
}

GameStatus ejecutarNivel(
    int numeroNivel,
    Vector2 posInicial,
    Texture2D fondo,
    int *coinsCollected,
    int *highScoreNivel,
    Texture2D jugadorTex,
    int totalFrames,
    Music *gameMusic,
    const char *rutaPlat,
    const char *rutaDirt,
    EnvElement elementosIniciales[MAX_ENVIRONMENT_ELEMENTS],
    Coin monedasIniciales[MAX_COINS],
    Enemy enemigosIniciales[MAX_ENEMIES])
{
    const int screenWidth = 1280;
    const int screenHeight = 720;

    Texture2D coinTex = LoadTexture("src/images/coin.png");
    Texture2D platformTex = LoadTexture(rutaPlat);
    Texture2D dirtTex = LoadTexture(rutaDirt);
    Sound coinSound = LoadSound("src/sound/coin.mp3");

    EnvElement envElements[MAX_ENVIRONMENT_ELEMENTS];
    Coin coins[MAX_COINS];
    Enemy enemies[MAX_ENEMIES];
    Particle particles[MAX_PARTICLES] = {0};
    FloatingText floatingTexts[MAX_FLOATING_TEXTS] = {0};

    memcpy(envElements, elementosIniciales, sizeof(EnvElement) * MAX_ENVIRONMENT_ELEMENTS);
    memcpy(coins, monedasIniciales, sizeof(Coin) * MAX_COINS);
    memcpy(enemies, enemigosIniciales, sizeof(Enemy) * MAX_ENEMIES);

    Player player = {0};
    player.position = posInicial;
    player.speed = 0;
    player.canJump = false;
    player.jumpsAvailable = 2;
    player.coyoteTimer = 0.0f;
    player.lives = 3;
    player.invulnerableTimer = 0.0f;
    player.dashTimer = 0.0f;
    player.dashCooldown = 0.0f;

    int currentFrame = 0;
    float frameWidth = (float)jugadorTex.width / totalFrames;
    float frameHeight = (float)jugadorTex.height;
    float frameTime = 0.15f;
    float frameCounter = 0.0f;
    bool facingRight = true;

    bool showingQuestion = false;
    Question currentQuestionData = {0};
    bool answeredCorrectly = false;
    float feedbackTimer = 0.0f;
    const float FEEDBACK_DURATION = 1.5f;

    float questionTimer = 0.0f;
    const float MAX_QUESTION_TIME = 14.0f;
    float screenShakeTimer = 0.0f;

    bool gameOver = false;
    bool levelCompleted = false;
    bool juegoPausado = false;

    int currentAttempts = 0;
    const int MAX_ATTEMPTS = 2;
    bool questionFailed = false;

    int score = 0;
    int streak = 0;

    Camera2D camera = {0};
    camera.target = player.position;
    camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 1.5f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    float minCameraX = -1000 + screenWidth / 2.0f;
    float maxCameraX = 2000 - screenWidth / 2.0f;

    const float GRAVITY = 520.0f;
    const float PLAYER_JUMP_SPD = 335.0f;
    const float PLAYER_HOR_SPD = 240.0f;

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();
        UpdateMusicStream(*gameMusic);

        if (IsKeyPressed(KEY_ESCAPE) && !showingQuestion && !levelCompleted && !gameOver)
        {
            juegoPausado = !juegoPausado;
        }

        if (screenShakeTimer > 0.0f && !juegoPausado)
            screenShakeTimer -= deltaTime;

        if (!levelCompleted && !showingQuestion && feedbackTimer <= 0.0f && !gameOver)
        {
            int activeCoins = 0;
            for (int i = 0; i < MAX_COINS; i++)
            {
                if (coins[i].active)
                    activeCoins++;
            }
            if (activeCoins == 0)
            {
                levelCompleted = true;
                score += player.lives * 100;
                if (score > *highScoreNivel)
                    *highScoreNivel = score;
            }
        }

        if (!juegoPausado && !showingQuestion && feedbackTimer <= 0.0f && !gameOver && !levelCompleted)
        {
            if (player.invulnerableTimer > 0.0f)
                player.invulnerableTimer -= deltaTime;
            if (player.dashCooldown > 0.0f)
                player.dashCooldown -= deltaTime;
            if (player.dashTimer > 0.0f)
                player.dashTimer -= deltaTime;

            // Plataformas móviles
            for (int i = 3; i < MAX_ENVIRONMENT_ELEMENTS; i++)
            {
                if (envElements[i].moveSpeed != 0.0f)
                {
                    envElements[i].rect.x += envElements[i].moveSpeed * deltaTime;
                    if (envElements[i].rect.x < envElements[i].minX)
                    {
                        envElements[i].rect.x = envElements[i].minX;
                        envElements[i].moveSpeed *= -1.0f;
                    }
                    else if (envElements[i].rect.x > envElements[i].maxX)
                    {
                        envElements[i].rect.x = envElements[i].maxX;
                        envElements[i].moveSpeed *= -1.0f;
                    }
                }
            }

            // Activar Dash con Left Shift o K
            if ((IsKeyPressed(KEY_LEFT_SHIFT) || IsKeyPressed(KEY_K)) && player.dashCooldown <= 0.0f)
            {
                player.dashTimer = 0.18f;
                player.dashCooldown = 1.4f;
                SpawnParticles(particles, player.position, ORANGE, 15);
            }

            float currentHorSpeed = (player.dashTimer > 0.0f) ? PLAYER_HOR_SPD * 2.6f : PLAYER_HOR_SPD;

            if (player.dashTimer > 0.0f)
            {
                player.position.x += (facingRight ? 1.0f : -1.0f) * currentHorSpeed * deltaTime;
                SpawnParticles(particles, (Vector2){player.position.x, player.position.y - 25}, GOLD, 2);
            }
            else
            {
                if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
                {
                    player.position.x -= currentHorSpeed * deltaTime;
                    facingRight = false;
                }
                if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
                {
                    player.position.x += currentHorSpeed * deltaTime;
                    facingRight = true;
                }
            }

            bool jumpPressed = IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP);
            bool canUseGroundJump = player.canJump || player.coyoteTimer > 0.0f;
            bool canUseDoubleJump = player.jumpsAvailable == 1;
            if (jumpPressed && player.jumpsAvailable > 0 && (canUseGroundJump || canUseDoubleJump))
            {
                player.speed = -PLAYER_JUMP_SPD;
                player.canJump = false;
                player.coyoteTimer = 0.0f;
                player.jumpsAvailable--;
                SpawnParticles(particles, player.position, (player.jumpsAvailable == 0) ? SKYBLUE : LIGHTGRAY, 12);
            }

            int hitObstacle = 0;
            float platformDeltaX = 0.0f;

            for (int i = 0; i < MAX_ENVIRONMENT_ELEMENTS; i++)
            {
                EnvElement *element = &envElements[i];
                Vector2 *p = &player.position;

                if (element->blocking &&
                    element->rect.x <= p->x &&
                    element->rect.x + element->rect.width >= p->x &&
                    element->rect.y >= p->y &&
                    element->rect.y <= p->y + player.speed * deltaTime &&
                    player.speed >= 0.0f)
                {
                    hitObstacle = 1;
                    player.speed = 0.0f;
                    p->y = element->rect.y;
                    platformDeltaX = element->moveSpeed * deltaTime;
                }
            }

            if (!hitObstacle)
            {
                player.position.y += player.speed * deltaTime;
                player.speed += GRAVITY * deltaTime;
                player.canJump = false;
                if (player.coyoteTimer > 0.0f)
                {
                    player.coyoteTimer = fmaxf(0.0f, player.coyoteTimer - deltaTime);
                    if (player.coyoteTimer == 0.0f && player.jumpsAvailable == 2)
                        player.jumpsAvailable = 1;
                }
            }
            else
            {
                player.canJump = true;
                player.jumpsAvailable = 2;
                player.coyoteTimer = 0.12f;
                player.position.x += platformDeltaX;
            }

            // Actualizar y checar colisiones con Enemigos Patrulleros
            Rectangle playerRect = {player.position.x - frameWidth / 2, player.position.y - frameHeight, frameWidth, frameHeight};
            for (int i = 0; i < MAX_ENEMIES; i++)
            {
                if (enemies[i].active)
                {
                    enemies[i].position.x += enemies[i].speed * deltaTime;
                    if (enemies[i].position.x < enemies[i].minX)
                    {
                        enemies[i].position.x = enemies[i].minX;
                        enemies[i].speed *= -1.0f;
                    }
                    else if (enemies[i].position.x > enemies[i].maxX)
                    {
                        enemies[i].position.x = enemies[i].maxX;
                        enemies[i].speed *= -1.0f;
                    }

                    Rectangle enemyRect = {enemies[i].position.x, enemies[i].position.y - enemies[i].height, enemies[i].width, enemies[i].height};
                    if (CheckCollisionRecs(playerRect, enemyRect))
                    {
                        // Si el jugador cae sobre el enemigo, lo aplasta
                        if (player.speed > 0.0f && player.position.y - player.speed * deltaTime <= enemyRect.y + 14.0f)
                        {
                            enemies[i].active = false;
                            player.speed = -PLAYER_JUMP_SPD * 0.85f;
                            player.jumpsAvailable = 1;
                            player.coyoteTimer = 0.0f;
                            score += 50;
                            SpawnParticles(particles, enemies[i].position, PURPLE, 25);
                            SpawnFloatingText(floatingTexts, enemies[i].position, "¡APLASTADO +50!", LIME);
                            PlayCoinSound(coinSound);
                        }
                        else if (player.invulnerableTimer <= 0.0f)
                        {
                            player.lives--;
                            streak = 0;
                            player.invulnerableTimer = 2.0f;
                            player.speed = -220.0f;
                            screenShakeTimer = 0.35f;
                            SpawnParticles(particles, player.position, RED, 20);
                            SpawnFloatingText(floatingTexts, player.position, "-1 VIDA", RED);
                            if (player.lives <= 0)
                                gameOver = true;
                        }
                    }
                }
            }

            // Caída al vacío
            if (player.position.y > screenHeight + 120)
            {
                player.lives--;
                streak = 0;
                screenShakeTimer = 0.35f;
                if (player.lives <= 0)
                {
                    gameOver = true;
                }
                else
                {
                    player.position = posInicial;
                    player.speed = 0;
                    player.jumpsAvailable = 2;
                    player.coyoteTimer = 0.0f;
                    player.invulnerableTimer = 1.5f;
                    SpawnFloatingText(floatingTexts, player.position, "¡CUIDADO! -1 VIDA", RED);
                }
            }

            // Colisión con monedas (Genera pregunta matemática dinámica)
            for (int i = 0; i < MAX_COINS; i++)
            {
                if (coins[i].active && CheckCollisionRecs(playerRect, coins[i].rect))
                {
                    coins[i].active = false;
                    showingQuestion = true;
                    questionTimer = MAX_QUESTION_TIME;
                    currentQuestionData = GenerarPreguntaAleatoria(numeroNivel);
                    SpawnParticles(particles, coins[i].position, GOLD, 22);

                    answeredCorrectly = false;
                    feedbackTimer = 0.0f;
                    currentAttempts = 0;
                    questionFailed = false;
                }
            }

            bool isMoving = (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A) || IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D));
            if (isMoving)
            {
                if (currentFrame == 0)
                {
                    currentFrame = 1;
                    frameCounter = 0;
                }
                else
                {
                    frameCounter += deltaTime;
                    if (frameCounter >= frameTime)
                    {
                        currentFrame = 1 + ((currentFrame - 1 + 1) % 3);
                        frameCounter = 0;
                    }
                }
            }
            else
            {
                currentFrame = 0;
            }
        }

        if (!juegoPausado && showingQuestion && feedbackTimer <= 0.0f)
        {
            questionTimer -= deltaTime;
            if (questionTimer <= 0.0f)
            {
                questionFailed = true;
                showingQuestion = false;
                streak = 0;
                player.lives--;
                screenShakeTimer = 0.35f;
                if (player.lives <= 0)
                    gameOver = true;
                feedbackTimer = FEEDBACK_DURATION;
            }
        }

        if (!juegoPausado && feedbackTimer > 0.0f)
        {
            feedbackTimer -= deltaTime;
            if (feedbackTimer <= 0.0f && !gameOver)
            {
                if (answeredCorrectly || questionFailed)
                {
                    showingQuestion = false;
                }
            }
        }

        if (gameOver && IsKeyPressed(KEY_R))
        {
            memcpy(envElements, elementosIniciales, sizeof(EnvElement) * MAX_ENVIRONMENT_ELEMENTS);
            memcpy(coins, monedasIniciales, sizeof(Coin) * MAX_COINS);
            memcpy(enemies, enemigosIniciales, sizeof(Enemy) * MAX_ENEMIES);

            player.position = posInicial;
            player.speed = 0;
            player.canJump = false;
            player.jumpsAvailable = 2;
            player.coyoteTimer = 0.0f;
            player.lives = 3;
            player.invulnerableTimer = 0.0f;
            score = 0;
            streak = 0;
            gameOver = false;
            showingQuestion = false;
            questionFailed = false;
        }

        if (!gameOver && !levelCompleted)
        {
            camera.target = player.position;
            camera.target.x = fmaxf(minCameraX, fminf(maxCameraX, camera.target.x));
            camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 1.5f};
            if (screenShakeTimer > 0.0f)
            {
                camera.offset.x += (float)GetRandomValue(-8, 8);
                camera.offset.y += (float)GetRandomValue(-8, 8);
            }
        }

        // ================= DIBUJO =================
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(fondo, 0, 0, WHITE);

        BeginMode2D(camera);

        DrawTiledPlatform(dirtTex, envElements[2].rect);
        for (int i = 2; i < MAX_ENVIRONMENT_ELEMENTS; i++)
        {
            if (envElements[i].blocking)
                DrawTiledPlatform(dirtTex, envElements[i].rect);
        }
        for (int i = 1; i < MAX_ENVIRONMENT_ELEMENTS; i++)
        {
            if (envElements[i].blocking)
                DrawTiledPlatform(platformTex, envElements[i].rect);
        }

        // Dibujar Enemigos Patrulleros procedurales
        float bounce = fabsf(sinf((float)GetTime() * 8.0f)) * 5.0f;
        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            if (enemies[i].active)
            {
                Rectangle eRect = {enemies[i].position.x, enemies[i].position.y - enemies[i].height - bounce, enemies[i].width, enemies[i].height + bounce};
                DrawRectangleRec(eRect, MAROON);
                DrawRectangleLinesEx(eRect, 2.5f, BLACK);
                // Ojos del enemigo
                int eyeOffset = (enemies[i].speed > 0) ? 18 : 6;
                DrawCircle((int)eRect.x + eyeOffset, (int)eRect.y + 12, 5, WHITE);
                DrawCircle((int)eRect.x + eyeOffset + 12, (int)eRect.y + 12, 5, WHITE);
                DrawCircle((int)eRect.x + eyeOffset, (int)eRect.y + 12, 2, BLACK);
                DrawCircle((int)eRect.x + eyeOffset + 12, (int)eRect.y + 12, 2, BLACK);
            }
        }

        // Dibujar monedas con flotación
        float floatOffset = sinf((float)GetTime() * 4.5f) * 4.0f;
        for (int i = 0; i < MAX_COINS; i++)
        {
            if (coins[i].active)
            {
                if (coinTex.id != 0)
                    DrawTexture(coinTex, (int)coins[i].position.x, (int)(coins[i].position.y + floatOffset), WHITE);
                else
                    DrawCircle((int)coins[i].position.x + 15, (int)(coins[i].position.y + 15 + floatOffset), 15, YELLOW);
            }
        }

        UpdateAndDrawParticles(particles, juegoPausado ? 0.0f : deltaTime);
        UpdateAndDrawFloatingTexts(floatingTexts, juegoPausado ? 0.0f : deltaTime);

        // Dibujar jugador (parpadea si es invulnerable)
        bool visible = (player.invulnerableTimer <= 0.0f) || (((int)(player.invulnerableTimer * 12.0f) % 2) == 0);
        if (visible)
        {
            Vector2 drawPos = {player.position.x - frameWidth / 2, player.position.y - frameHeight};
            Rectangle sourceRec = {frameWidth * currentFrame, 0, facingRight ? frameWidth : -frameWidth, frameHeight};
            DrawTextureRec(jugadorTex, sourceRec, drawPos, (player.dashTimer > 0.0f) ? GOLD : WHITE);
        }

        EndMode2D();

        // ================= HUD SUPERIOR =================
        DrawRectangle(10, 10, 345, 85, Fade(BLACK, 0.60f));
        DrawRectangleLines(10, 10, 345, 85, GOLD);
        DrawText(TextFormat("NIVEL %d  |  ESC: Pausa", numeroNivel), 22, 16, 20, WHITE);
        DrawText(TextFormat("Vidas: %d   |   Racha: x%d", player.lives, streak), 22, 42, 20, (player.lives == 1) ? RED : GREEN);
        const char *dashStatus = (player.dashCooldown <= 0.0f) ? "LISTO (SHIFT)" : TextFormat("%.1fs", player.dashCooldown);
        DrawText(TextFormat("Doble Salto: ESPACIO | Dash: %s", dashStatus), 22, 68, 15, SKYBLUE);

        DrawRectangle(screenWidth - 250, 10, 240, 70, Fade(BLACK, 0.60f));
        DrawRectangleLines(screenWidth - 250, 10, 240, 70, GOLD);
        DrawText(TextFormat("Monedas: %d", *coinsCollected), screenWidth - 235, 18, 22, YELLOW);
        DrawText(TextFormat("Puntos: %d", score), screenWidth - 235, 46, 22, ORANGE);

        // ================= INTERFAZ DE PREGUNTA =================
        if (showingQuestion)
        {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.72f));

            int textWidth = MeasureText(currentQuestionData.text, 40);
            DrawText(currentQuestionData.text, (screenWidth - textWidth) / 2, 85, 40, WHITE);

            float ratio = fmaxf(0.0f, questionTimer / MAX_QUESTION_TIME);
            DrawRectangle((screenWidth - 400) / 2, 140, 400, 18, DARKGRAY);
            DrawRectangle((screenWidth - 400) / 2, 140, (int)(400 * ratio), 18, (ratio > 0.35f) ? LIME : RED);
            DrawRectangleLines((screenWidth - 400) / 2, 140, 400, 18, WHITE);

            char attemptInfo[80];
            sprintf(attemptInfo, "Intentos: %d  |  Tiempo: %.1fs  |  Responde con Clic o Teclas [1-4]", MAX_ATTEMPTS - currentAttempts, questionTimer);
            int infoWidth = MeasureText(attemptInfo, 18);
            DrawText(attemptInfo, (screenWidth - infoWidth) / 2, 168, 18, YELLOW);

            int btnWidth = 340;
            int btnHeight = 52;
            int btnX = (screenWidth - btnWidth) / 2;
            int btnY = 210;
            int spacing = 18;

            int teclaSeleccionada = -1;
            if (feedbackTimer <= 0.0f)
            {
                if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1)) teclaSeleccionada = 0;
                if (IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_KP_2)) teclaSeleccionada = 1;
                if (IsKeyPressed(KEY_THREE) || IsKeyPressed(KEY_KP_3)) teclaSeleccionada = 2;
                if (IsKeyPressed(KEY_FOUR) || IsKeyPressed(KEY_KP_4)) teclaSeleccionada = 3;
            }

            for (int i = 0; i < 4; i++)
            {
                Rectangle btn = {(float)btnX, (float)(btnY + i * (btnHeight + spacing)), (float)btnWidth, (float)btnHeight};
                bool mouseOver = CheckCollisionPointRec(GetMousePosition(), btn) && (feedbackTimer <= 0.0f);
                Color btnColor = mouseOver ? Fade(SKYBLUE, 0.9f) : Fade(BLUE, 0.75f);

                DrawRectangleRec(btn, btnColor);
                DrawRectangleLinesEx(btn, 4.0f, mouseOver ? GOLD : DARKBLUE);

                const char *optLabel = TextFormat("[%d]   %s", i + 1, currentQuestionData.options[i]);
                int optWidth = MeasureText(optLabel, 22);
                DrawText(optLabel, btnX + (btnWidth - optWidth) / 2, (int)btn.y + (btnHeight - 22) / 2, 22, WHITE);

                if ((mouseOver && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) || teclaSeleccionada == i)
                {
                    answeredCorrectly = (i == currentQuestionData.correctAnswer);

                    if (answeredCorrectly)
                    {
                        streak++;
                        int bonusMonedas = (streak >= 3) ? 2 : 1;
                        (*coinsCollected) += bonusMonedas;
                        int ptsGanados = 100 * streak + (int)(questionTimer * 10);
                        score += ptsGanados;

                        SpawnFloatingText(floatingTexts, player.position, TextFormat("+%d PTS!", ptsGanados), GOLD);
                        currentAttempts = 0;
                        questionFailed = false;
                        showingQuestion = false;
                        feedbackTimer = FEEDBACK_DURATION;
                        PlayCoinSound(coinSound);
                    }
                    else
                    {
                        currentAttempts++;
                        streak = 0;
                        screenShakeTimer = 0.25f;
                        if (currentAttempts >= MAX_ATTEMPTS)
                        {
                            questionFailed = true;
                            showingQuestion = false;
                            player.lives--;
                            if (player.lives <= 0)
                                gameOver = true;
                            feedbackTimer = FEEDBACK_DURATION;
                        }
                        else
                        {
                            feedbackTimer = 1.0f;
                        }
                    }
                }
            }
        }

        if (feedbackTimer > 0.0f && !gameOver)
        {
            if (answeredCorrectly)
            {
                const char *msg = (streak >= 3) ? "¡CORRECTO! ¡RACHA BONUS (+2 Monedas)!" : "¡Correcto! (+1 Moneda)";
                DrawText(msg, (screenWidth - MeasureText(msg, 32)) / 2, 520, 32, GREEN);
            }
            else if (questionFailed)
            {
                const char *msg = "¡Fallaste la pregunta y perdiste 1 vida!";
                DrawText(msg, (screenWidth - MeasureText(msg, 32)) / 2, 520, 32, ORANGE);
            }
            else
            {
                char attemptMsg[100];
                sprintf(attemptMsg, "Incorrecto. Intento %d de %d", currentAttempts, MAX_ATTEMPTS);
                DrawText(attemptMsg, (screenWidth - MeasureText(attemptMsg, 30)) / 2, 520, 30, RED);
            }
        }

        if (levelCompleted)
        {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.78f));
            DrawText("¡HAS COMPLETADO EL NIVEL!", (screenWidth - MeasureText("¡HAS COMPLETADO EL NIVEL!", 42)) / 2, 230, 42, GREEN);
            DrawText(TextFormat("Puntaje Final: %d pts (Record: %d pts)", score, *highScoreNivel),
                     (screenWidth - MeasureText(TextFormat("Puntaje Final: %d pts (Record: %d pts)", score, *highScoreNivel), 28)) / 2, 300, 28, GOLD);

            int btnWidth = 220;
            int btnHeight = 55;
            int btnX = (screenWidth - btnWidth) / 2;
            int btnY = 390;
            Rectangle btn = {(float)btnX, (float)btnY, (float)btnWidth, (float)btnHeight};
            bool mouseOver = CheckCollisionPointRec(GetMousePosition(), btn);
            Color btnColor = mouseOver ? Fade(GREEN, 0.9f) : Fade(DARKGREEN, 0.8f);

            DrawRectangleRec(btn, btnColor);
            DrawRectangleLinesEx(btn, 4.0f, GOLD);
            int textWidth = MeasureText("Continuar", 24);
            DrawText("Continuar", btnX + (btnWidth - textWidth) / 2, btnY + (btnHeight - 24) / 2, 24, WHITE);

            if (mouseOver && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                UnloadTexture(coinTex);
                UnloadTexture(platformTex);
                UnloadTexture(dirtTex);
                UnloadSound(coinSound);
                EndDrawing();
                return GAME_WON;
            }
        }

        if (gameOver)
        {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.80f));
            DrawText("¡TE QUEDASTE SIN VIDAS!", (screenWidth - MeasureText("¡TE QUEDASTE SIN VIDAS!", 42)) / 2, 260, 42, RED);
            DrawText("Presiona R para reintentar o ESC para volver al menu", (screenWidth - MeasureText("Presiona R para reintentar o ESC para volver al menu", 24)) / 2, 330, 24, WHITE);

            if (IsKeyPressed(KEY_ESCAPE))
            {
                UnloadTexture(coinTex);
                UnloadTexture(platformTex);
                UnloadTexture(dirtTex);
                UnloadSound(coinSound);
                EndDrawing();
                return GAME_OVER;
            }
        }

        if (juegoPausado)
        {
            PausaOpcion op = menuPausa();
            if (op == PAUSA_CONTINUAR)
            {
                juegoPausado = false;
            }
            else if (op == PAUSA_REINICIAR)
            {
                memcpy(envElements, elementosIniciales, sizeof(EnvElement) * MAX_ENVIRONMENT_ELEMENTS);
                memcpy(coins, monedasIniciales, sizeof(Coin) * MAX_COINS);
                memcpy(enemies, enemigosIniciales, sizeof(Enemy) * MAX_ENEMIES);
                player.position = posInicial;
                player.speed = 0;
                player.canJump = false;
                player.jumpsAvailable = 2;
                player.coyoteTimer = 0.0f;
                player.lives = 3;
                player.invulnerableTimer = 0.0f;
                score = 0;
                streak = 0;
                juegoPausado = false;
            }
            else if (op == PAUSA_MENU_PRINCIPAL)
            {
                UnloadTexture(coinTex);
                UnloadTexture(platformTex);
                UnloadTexture(dirtTex);
                UnloadSound(coinSound);
                EndDrawing();
                return GAME_EXIT_MAIN_MENU;
            }
            else if (op == PAUSA_SALIR)
            {
                UnloadTexture(coinTex);
                UnloadTexture(platformTex);
                UnloadTexture(dirtTex);
                UnloadSound(coinSound);
                EndDrawing();
                return GAME_EXIT_APP;
            }
        }

        EndDrawing();
    }

    UnloadTexture(platformTex);
    UnloadTexture(coinTex);
    UnloadTexture(dirtTex);
    UnloadSound(coinSound);
    return GAME_EXIT_APP;
}

GameStatus nivel1(Vector2 posInicial, Texture2D fondo, int *coinsCollected, int *highScore, Texture2D jugadorTexture, int totalFrames, Music *gameMusic)
{
    EnvElement envElements[MAX_ENVIRONMENT_ELEMENTS] = {
        {{0, 0, 1280, 720}, 0, BLANK, 0, 0, 0},
        {{-1500, 600, 5000, 50}, 1, BLANK, 0, 0, 0},
        {{-1000, 650, 3500, 500}, 0, BLANK, 0, 0, 0},
        {{100, 500, 200, 50}, 1, BLANK, 0, 0, 0},
        {{280, 410, 200, 50}, 1, BLANK, 0, 0, 0},
        {{460, 320, 200, 50}, 1, BLANK, 0, 0, 0},
        {{640, 230, 200, 50}, 1, BLANK, 0, 0, 0},
        {{820, 140, 200, 50}, 1, BLANK, 0, 0, 0},
        {{1000, 50, 200, 50}, 1, BLANK, 0, 0, 0}};

    Coin coins[MAX_COINS] = {
        {{165, 440}, true, {165, 440, 30, 30}},
        {{345, 350}, true, {345, 350, 30, 30}},
        {{525, 260}, true, {525, 260, 30, 30}},
        {{705, 170}, true, {705, 170, 30, 30}},
        {{885, 80}, true, {885, 80, 30, 30}},
        {{1065, -10}, true, {1065, -10, 30, 30}}};

    Enemy enemies[MAX_ENEMIES] = {
        {{320, 600}, 36, 30, 90.0f, 180.0f, 750.0f, true},
        {{480, 320}, 34, 28, 65.0f, 465.0f, 620.0f, true},
        {{0, 0}, 34, 28, 0.0f, 0.0f, 0.0f, false}};

    return ejecutarNivel(1, posInicial, fondo, coinsCollected, highScore, jugadorTexture, totalFrames, gameMusic,
                         "src/images/platform1.png", "src/images/dirt.png", envElements, coins, enemies);
}

GameStatus nivel2(Vector2 posInicial, Texture2D fondo, int *coinsCollected, int *highScore, Texture2D jugadorTexture, int totalFrames, Music *gameMusic)
{
    EnvElement envElements[MAX_ENVIRONMENT_ELEMENTS] = {
        {{0, 0, 1280, 720}, 0, BLANK, 0, 0, 0},
        {{-1500, 600, 5000, 50}, 1, BLANK, 0, 0, 0},
        {{-1000, 650, 3500, 500}, 0, BLANK, 0, 0, 0},
        {{150, 500, 200, 50}, 1, BLANK, 0, 0, 0},
        {{450, 400, 200, 50}, 1, BLANK, 75.0f, 380.0f, 560.0f},
        {{750, 300, 200, 50}, 1, BLANK, 0, 0, 0},
        {{350, 250, 200, 50}, 1, BLANK, 90.0f, 260.0f, 460.0f},
        {{650, 150, 200, 50}, 1, BLANK, 0, 0, 0},
        {{950, 120, 200, 50}, 1, BLANK, 0, 0, 0}};

    Coin coins[MAX_COINS] = {
        {{210, 440}, true, {210, 440, 30, 30}},
        {{510, 340}, true, {510, 340, 30, 30}},
        {{810, 240}, true, {810, 240, 30, 30}},
        {{410, 190}, true, {410, 190, 30, 30}},
        {{710, 90}, true, {710, 80, 30, 30}},
        {{1010, 60}, true, {1010, 60, 30, 30}}};

    Enemy enemies[MAX_ENEMIES] = {
        {{250, 600}, 36, 30, 115.0f, 150.0f, 900.0f, true},
        {{760, 300}, 34, 28, 75.0f, 755.0f, 910.0f, true},
        {{660, 150}, 34, 28, 75.0f, 655.0f, 810.0f, true}};

    return ejecutarNivel(2, posInicial, fondo, coinsCollected, highScore, jugadorTexture, totalFrames, gameMusic,
                         "src/images/platform2.png", "src/images/dirt2.png", envElements, coins, enemies);
}

GameStatus nivel3(Vector2 posInicial, Texture2D fondo, int *coinsCollected, int *highScore, Texture2D jugadorTexture, int totalFrames, Music *gameMusic)
{
    EnvElement envElements[MAX_ENVIRONMENT_ELEMENTS] = {
        {{0, 0, 1280, 720}, 0, BLANK, 0, 0, 0},
        {{-1500, 600, 5000, 50}, 1, BLANK, 0, 0, 0},
        {{-1000, 650, 3500, 500}, 0, BLANK, 0, 0, 0},
        {{150, 500, 95, 30}, 1, BLANK, 0, 0, 0},
        {{440, 420, 95, 30}, 1, BLANK, 85.0f, 360.0f, 520.0f},
        {{250, 340, 95, 30}, 1, BLANK, 0, 0, 0},
        {{420, 260, 95, 30}, 1, BLANK, 100.0f, 350.0f, 510.0f},
        {{180, 180, 95, 30}, 1, BLANK, 0, 0, 0},
        {{380, 100, 95, 30}, 1, BLANK, 0, 0, 0}};

    Coin coins[MAX_COINS] = {
        {{160, 440}, true, {165, 440, 30, 30}},
        {{450, 360}, true, {465, 360, 30, 30}},
        {{255, 280}, true, {255, 280, 30, 30}},
        {{425, 200}, true, {425, 200, 30, 30}},
        {{195, 120}, true, {195, 120, 30, 30}},
        {{400, 40}, true, {400, 40, 30, 30}}};

    Enemy enemies[MAX_ENEMIES] = {
        {{200, 600}, 36, 30, 130.0f, 120.0f, 650.0f, true},
        {{500, 600}, 36, 30, -140.0f, 120.0f, 650.0f, true},
        {{255, 340}, 32, 26, 55.0f, 250.0f, 310.0f, true}};

    return ejecutarNivel(3, posInicial, fondo, coinsCollected, highScore, jugadorTexture, totalFrames, gameMusic,
                         "src/images/platform3.png", "src/images/dirt3.png", envElements, coins, enemies);
}

void DrawTiledPlatform(Texture2D texture, Rectangle platform)
{
    if (texture.id == 0)
    {
        DrawRectangleRec(platform, GRAY);
        return;
    }

    float tileWidth = (float)texture.width;
    float tileHeight = (float)texture.height;

    for (float x = platform.x; x < platform.x + platform.width; x += tileWidth)
    {
        for (float y = platform.y; y < platform.y + platform.height; y += tileHeight)
        {
            float currentTileWidth = fminf(tileWidth, platform.x + platform.width - x);
            float currentTileHeight = fminf(tileHeight, platform.y + platform.height - y);

            Rectangle sourceRect = {0, 0, currentTileWidth, currentTileHeight};
            Rectangle destRect = {x, y, currentTileWidth, currentTileHeight};

            DrawTexturePro(texture, sourceRect, destRect, (Vector2){0, 0}, 0.0f, WHITE);
        }
    }
}

void PlayCoinSound(Sound coinSound)
{
    if (IsAudioDeviceReady())
    {
        SetSoundVolume(coinSound, 0.2f);
        PlaySound(coinSound);
    }
}

void SpawnParticles(Particle particles[], Vector2 pos, Color color, int count)
{
    int spawned = 0;
    for (int i = 0; i < MAX_PARTICLES && spawned < count; i++)
    {
        if (!particles[i].active)
        {
            particles[i].active = true;
            particles[i].position = pos;
            particles[i].velocity = (Vector2){(float)GetRandomValue(-130, 130), (float)GetRandomValue(-190, -30)};
            particles[i].alpha = 1.0f;
            particles[i].size = (float)GetRandomValue(4, 8);
            particles[i].color = color;
            spawned++;
        }
    }
}

void UpdateAndDrawParticles(Particle particles[], float deltaTime)
{
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (particles[i].active)
        {
            particles[i].position.x += particles[i].velocity.x * deltaTime;
            particles[i].position.y += particles[i].velocity.y * deltaTime;
            particles[i].alpha -= deltaTime * 1.8f;
            if (particles[i].alpha <= 0.0f)
            {
                particles[i].active = false;
            }
            else
            {
                DrawRectangle((int)particles[i].position.x, (int)particles[i].position.y,
                              (int)particles[i].size, (int)particles[i].size,
                              Fade(particles[i].color, particles[i].alpha));
            }
        }
    }
}

void SpawnFloatingText(FloatingText texts[], Vector2 pos, const char *msg, Color color)
{
    for (int i = 0; i < MAX_FLOATING_TEXTS; i++)
    {
        if (!texts[i].active)
        {
            texts[i].active = true;
            texts[i].position = (Vector2){pos.x - 30, pos.y - 50};
            strncpy(texts[i].text, msg, sizeof(texts[i].text) - 1);
            texts[i].alpha = 1.0f;
            texts[i].color = color;
            break;
        }
    }
}

void UpdateAndDrawFloatingTexts(FloatingText texts[], float deltaTime)
{
    for (int i = 0; i < MAX_FLOATING_TEXTS; i++)
    {
        if (texts[i].active)
        {
            texts[i].position.y -= 35.0f * deltaTime;
            texts[i].alpha -= deltaTime * 0.9f;
            if (texts[i].alpha <= 0.0f)
            {
                texts[i].active = false;
            }
            else
            {
                DrawText(texts[i].text, (int)texts[i].position.x + 1, (int)texts[i].position.y + 1, 20, Fade(BLACK, texts[i].alpha));
                DrawText(texts[i].text, (int)texts[i].position.x, (int)texts[i].position.y, 20, Fade(texts[i].color, texts[i].alpha));
            }
        }
    }
}

#endif
