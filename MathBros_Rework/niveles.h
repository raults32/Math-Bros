#ifndef NIVELES_H
#define NIVELES_H

#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "mainMenu.h"
#include "menuPausa.h"
#include "personaje.h"

#define MAX_ENVIRONMENT_ELEMENTS 24
#define MAX_COINS 10
#define MAX_PARTICLES 120
#define MAX_ENEMIES 7
#define MAX_MYSTERY_BLOCKS 4
#define MAX_CHECKPOINTS 2
#define MAX_FLOATING_TEXTS 12
#define MAX_TRAIL 8

typedef enum
{
    PLAT_NORMAL = 0,
    PLAT_MOVING_H,
    PLAT_MOVING_V,
    PLAT_BOUNCE,
    PLAT_CRUMBLE
} PlatformType;

typedef struct GhostFrame
{
    Vector2 position;
    int frame;
    bool facingRight;
    float alpha;
    Color tint;
    bool active;
} GhostFrame;

typedef struct Player
{
    Vector2 position;
    float velX;
    float speedY;
    bool canJump;
    int jumpsAvailable;
    int lives;
    bool hasShield;
    float invulnerableTimer;
    float dashTimer;
    float dashCooldown;
    float scaleX;
    float scaleY;
    float rotation;
    float idleTimer;
    float trailSpawnTimer;
} Player;

typedef struct EnvElement
{
    Rectangle rect;
    int blocking;
    PlatformType type;
    float moveSpeed;
    float minVal;
    float maxVal;
    float timer;
    bool falling;
    bool active;
    Vector2 startPos;
} EnvElement;

typedef struct MysteryBlock
{
    Rectangle rect;
    bool used;
    int rewardType;
    float bounceOffset;
    bool active;
} MysteryBlock;

typedef struct Checkpoint
{
    Vector2 position;
    bool activated;
    bool active;
} Checkpoint;

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
    int hp;
    int maxHp;
    bool isBoss;
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

void DrawTiledPlatform(Texture2D texture, Rectangle platform, Color tint);
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

    if (dificultad == 1)
    {
        // Facil (primaria baja): sumas y restas hasta 20, tablas sencillas
        int tipo = GetRandomValue(0, 2);
        if (tipo == 0)
        {
            a = GetRandomValue(1, 10);
            b = GetRandomValue(1, 10);
            respuesta = a + b;
            snprintf(q.text, sizeof(q.text), "¿Cuanto es %d + %d?", a, b);
        }
        else if (tipo == 1)
        {
            a = GetRandomValue(5, 20);
            b = GetRandomValue(1, a);
            respuesta = a - b;
            snprintf(q.text, sizeof(q.text), "¿Cuanto es %d - %d?", a, b);
        }
        else
        {
            int tablas[4] = {2, 3, 5, 10};
            a = tablas[GetRandomValue(0, 3)];
            b = GetRandomValue(1, 5);
            respuesta = a * b;
            snprintf(q.text, sizeof(q.text), "¿Cuanto es %d x %d?", a, b);
        }
    }
    else if (dificultad == 2)
    {
        // Medio (primaria alta): dos cifras, tablas completas y divisiones exactas
        int tipo = GetRandomValue(0, 3);
        if (tipo == 0)
        {
            a = GetRandomValue(10, 60);
            b = GetRandomValue(5, 39);
            respuesta = a + b;
            snprintf(q.text, sizeof(q.text), "¿Cuanto es %d + %d?", a, b);
        }
        else if (tipo == 1)
        {
            a = GetRandomValue(20, 99);
            b = GetRandomValue(5, a - 5);
            respuesta = a - b;
            snprintf(q.text, sizeof(q.text), "¿Cuanto es %d - %d?", a, b);
        }
        else if (tipo == 2)
        {
            a = GetRandomValue(2, 9);
            b = GetRandomValue(2, 9);
            respuesta = a * b;
            snprintf(q.text, sizeof(q.text), "¿Cuanto es %d x %d?", a, b);
        }
        else
        {
            b = GetRandomValue(2, 9);
            respuesta = GetRandomValue(2, 10);
            a = b * respuesta;
            snprintf(q.text, sizeof(q.text), "¿Cuanto es %d / %d?", a, b);
        }
    }
    else
    {
        // Dificil (preparatoria): ecuaciones, potencias, raices, porcentajes y jerarquia con negativos
        int tipo = GetRandomValue(0, 4);
        if (tipo == 0)
        {
            a = GetRandomValue(2, 9);
            respuesta = GetRandomValue(-9, 12);
            if (respuesta == 0) respuesta = 5;
            b = GetRandomValue(-20, 20);
            if (b == 0) b = 7;
            c = a * respuesta + b;
            snprintf(q.text, sizeof(q.text), "Si %dx %c %d = %d, ¿x = ?", a, b < 0 ? '-' : '+', b < 0 ? -b : b, c);
        }
        else if (tipo == 1)
        {
            if (GetRandomValue(0, 1) == 0)
            {
                a = GetRandomValue(11, 15);
                b = 2;
            }
            else
            {
                a = GetRandomValue(2, 3);
                b = (a == 2) ? GetRandomValue(5, 8) : GetRandomValue(3, 4);
            }
            respuesta = 1;
            for (int i = 0; i < b; i++) respuesta *= a;
            snprintf(q.text, sizeof(q.text), "¿Cuanto es %d^%d?", a, b);
        }
        else if (tipo == 2)
        {
            respuesta = GetRandomValue(11, 25);
            snprintf(q.text, sizeof(q.text), "¿Raiz cuadrada de %d?", respuesta * respuesta);
        }
        else if (tipo == 3)
        {
            int porcentajes[5] = {10, 15, 20, 25, 75};
            a = porcentajes[GetRandomValue(0, 4)];
            b = GetRandomValue(2, 20) * 20;
            respuesta = a * b / 100;
            snprintf(q.text, sizeof(q.text), "¿Cuanto es el %d%% de %d?", a, b);
        }
        else
        {
            a = GetRandomValue(-10, 15);
            b = GetRandomValue(3, 9);
            c = GetRandomValue(-6, 6);
            if (c == 0) c = -4;
            respuesta = a - b * c;
            snprintf(q.text, sizeof(q.text), "¿Cuanto es %d - %d x (%d)?", a, b, c);
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
                // Opciones incorrectas cercanas a la respuesta, proporcionales a su tamano
                int rango = abs(respuesta) / 5;
                if (rango < 3) rango = 3;
                if (rango > 12) rango = 12;
                int offset = GetRandomValue(-rango, rango);
                if (offset == 0)
                    offset = 1;
                distractor = respuesta + offset;
                if (distractor < 0 && dificultad < 3)
                    distractor = respuesta + GetRandomValue(1, rango);
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
    const char *nombreNivel,
    Vector2 posInicial,
    Vector2 posPortalMeta,
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
    Enemy enemigosIniciales[MAX_ENEMIES],
    MysteryBlock bloquesIniciales[MAX_MYSTERY_BLOCKS],
    Checkpoint checkpointsIniciales[MAX_CHECKPOINTS])
{
    const int screenWidth = 1280;
    const int screenHeight = 720;

    Texture2D coinTex = LoadTextureSmart("src/images/coin.png", 0, 0);
    Texture2D platformTex = LoadTextureSmart(rutaPlat, 0, 0);
    Texture2D dirtTex = LoadTextureSmart(rutaDirt, 0, 0);
    Sound coinSound = LoadSound("src/sound/coin.mp3");

    EnvElement envElements[MAX_ENVIRONMENT_ELEMENTS];
    Coin coins[MAX_COINS];
    Enemy enemies[MAX_ENEMIES];
    MysteryBlock mysteryBlocks[MAX_MYSTERY_BLOCKS];
    Checkpoint checkpoints[MAX_CHECKPOINTS];
    Particle particles[MAX_PARTICLES] = {0};
    FloatingText floatingTexts[MAX_FLOATING_TEXTS] = {0};
    GhostFrame ghostTrail[MAX_TRAIL] = {0};

    memcpy(envElements, elementosIniciales, sizeof(EnvElement) * MAX_ENVIRONMENT_ELEMENTS);
    memcpy(coins, monedasIniciales, sizeof(Coin) * MAX_COINS);
    memcpy(enemies, enemigosIniciales, sizeof(Enemy) * MAX_ENEMIES);
    memcpy(mysteryBlocks, bloquesIniciales, sizeof(MysteryBlock) * MAX_MYSTERY_BLOCKS);
    memcpy(checkpoints, checkpointsIniciales, sizeof(Checkpoint) * MAX_CHECKPOINTS);

    for (int i = 0; i < MAX_ENVIRONMENT_ELEMENTS; i++)
    {
        envElements[i].startPos = (Vector2){envElements[i].rect.x, envElements[i].rect.y};
        envElements[i].active = (envElements[i].rect.width > 0);
    }

    Vector2 respawnPos = posInicial;

    Player player = {0};
    player.position = posInicial;
    player.velX = 0.0f;
    player.speedY = 0.0f;
    player.canJump = false;
    player.jumpsAvailable = 2;
    player.lives = 3;
    player.hasShield = false;
    player.invulnerableTimer = 0.0f;
    player.dashTimer = 0.0f;
    player.dashCooldown = 0.0f;
    player.scaleX = 1.0f;
    player.scaleY = 1.0f;
    player.rotation = 0.0f;

    int currentFrame = 0;
    float frameWidth = (float)jugadorTex.width / totalFrames;
    float frameHeight = (float)jugadorTex.height;
    float frameCounter = 0.0f;
    bool facingRight = true;
    bool wasOnGround = true;

    bool showingQuestion = false;
    Question currentQuestionData = {0};
    bool answeredCorrectly = false;
    float feedbackTimer = 0.0f;
    const float FEEDBACK_DURATION = 1.4f;

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
    int coinsAnsweredInLevel = 0;
    const int MIN_COINS_FOR_PORTAL = 6;

    Camera2D camera = {0};
    camera.target = player.position;
    camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 1.45f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    float minCameraX = -200 + screenWidth / 2.0f;
    float maxCameraX = posPortalMeta.x + 400 - screenWidth / 2.0f;

    PlayerConfig cfg = ObtenerConfiguracionJugador();
    const float GRAVITY = cfg.gravity;
    const float PLAYER_JUMP_SPD = cfg.jumpVelocity;
    const float PLAYER_MAX_HOR_SPD = cfg.maxHorizontalSpeed;
    const float PLAYER_ACCEL = 1400.0f;
    const float PLAYER_FRICTION = 1200.0f;

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();
        if (deltaTime > 0.05f) deltaTime = 0.05f;
        UpdateMusicStream(*gameMusic);

        if (IsKeyPressed(KEY_ESCAPE) && !showingQuestion && !levelCompleted && !gameOver)
        {
            juegoPausado = !juegoPausado;
        }

        if (screenShakeTimer > 0.0f && !juegoPausado)
            screenShakeTimer -= deltaTime;

        if (!juegoPausado && !showingQuestion && feedbackTimer <= 0.0f && !gameOver && !levelCompleted)
        {
            if (player.invulnerableTimer > 0.0f)
                player.invulnerableTimer -= deltaTime;
            if (player.dashCooldown > 0.0f)
                player.dashCooldown -= deltaTime;
            if (player.dashTimer > 0.0f)
                player.dashTimer -= deltaTime;

            player.scaleX += (1.0f - player.scaleX) * 12.0f * deltaTime;
            player.scaleY += (1.0f - player.scaleY) * 12.0f * deltaTime;

            for (int i = 2; i < MAX_ENVIRONMENT_ELEMENTS; i++)
            {
                if (!envElements[i].blocking && !envElements[i].falling)
                    continue;

                if (envElements[i].type == PLAT_MOVING_H)
                {
                    envElements[i].rect.x += envElements[i].moveSpeed * deltaTime;
                    if (envElements[i].rect.x < envElements[i].minVal)
                    {
                        envElements[i].rect.x = envElements[i].minVal;
                        envElements[i].moveSpeed *= -1.0f;
                    }
                    else if (envElements[i].rect.x > envElements[i].maxVal)
                    {
                        envElements[i].rect.x = envElements[i].maxVal;
                        envElements[i].moveSpeed *= -1.0f;
                    }
                }
                else if (envElements[i].type == PLAT_MOVING_V)
                {
                    envElements[i].rect.y += envElements[i].moveSpeed * deltaTime;
                    if (envElements[i].rect.y < envElements[i].minVal)
                    {
                        envElements[i].rect.y = envElements[i].minVal;
                        envElements[i].moveSpeed *= -1.0f;
                    }
                    else if (envElements[i].rect.y > envElements[i].maxVal)
                    {
                        envElements[i].rect.y = envElements[i].maxVal;
                        envElements[i].moveSpeed *= -1.0f;
                    }
                }
                else if (envElements[i].type == PLAT_CRUMBLE)
                {
                    if (envElements[i].falling)
                    {
                        envElements[i].timer += deltaTime;
                        if (envElements[i].timer >= 0.65f && envElements[i].active)
                        {
                            envElements[i].active = false;
                            SpawnParticles(particles, (Vector2){envElements[i].rect.x + envElements[i].rect.width / 2, envElements[i].rect.y}, ORANGE, 18);
                        }
                        else if (envElements[i].timer >= 3.8f)
                        {
                            envElements[i].active = true;
                            envElements[i].falling = false;
                            envElements[i].timer = 0.0f;
                        }
                    }
                }
            }

            for (int i = 0; i < MAX_MYSTERY_BLOCKS; i++)
            {
                if (mysteryBlocks[i].bounceOffset > 0.0f)
                {
                    mysteryBlocks[i].bounceOffset -= 60.0f * deltaTime;
                    if (mysteryBlocks[i].bounceOffset < 0.0f)
                        mysteryBlocks[i].bounceOffset = 0.0f;
                }
            }

            if ((IsKeyPressed(KEY_LEFT_SHIFT) || IsKeyPressed(KEY_K)) && player.dashCooldown <= 0.0f)
            {
                player.dashTimer = 0.20f;
                player.dashCooldown = 1.3f;
                player.scaleX = 1.35f;
                player.scaleY = 0.72f;
                SpawnParticles(particles, player.position, ORANGE, 18);
            }

            if (player.dashTimer > 0.0f)
            {
                player.velX = (facingRight ? 1.0f : -1.0f) * (PLAYER_MAX_HOR_SPD * 2.5f);
                player.speedY = 0.0f;
            }
            else
            {
                bool moveLeft = IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A);
                bool moveRight = IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D);

                if (moveLeft)
                {
                    if (player.velX > 60.0f && player.canJump)
                        SpawnParticles(particles, player.position, LIGHTGRAY, 2);
                    player.velX -= PLAYER_ACCEL * deltaTime;
                    if (player.velX < -PLAYER_MAX_HOR_SPD)
                        player.velX = -PLAYER_MAX_HOR_SPD;
                    facingRight = false;
                }
                else if (moveRight)
                {
                    if (player.velX < -60.0f && player.canJump)
                        SpawnParticles(particles, player.position, LIGHTGRAY, 2);
                    player.velX += PLAYER_ACCEL * deltaTime;
                    if (player.velX > PLAYER_MAX_HOR_SPD)
                        player.velX = PLAYER_MAX_HOR_SPD;
                    facingRight = true;
                }
                else
                {
                    if (player.velX > 0.0f)
                    {
                        player.velX -= PLAYER_FRICTION * deltaTime;
                        if (player.velX < 0.0f) player.velX = 0.0f;
                    }
                    else if (player.velX < 0.0f)
                    {
                        player.velX += PLAYER_FRICTION * deltaTime;
                        if (player.velX > 0.0f) player.velX = 0.0f;
                    }
                }
            }

            player.position.x += player.velX * deltaTime;

            if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) && player.jumpsAvailable > 0)
            {
                player.speedY = -PLAYER_JUMP_SPD;
                player.canJump = false;
                player.jumpsAvailable--;
                player.scaleX = 0.76f;
                player.scaleY = 1.28f;
                SpawnParticles(particles, player.position, (player.jumpsAvailable == 0) ? SKYBLUE : RAYWHITE, 14);
            }

            Rectangle playerRect = {player.position.x - frameWidth * 0.35f, player.position.y - frameHeight, frameWidth * 0.7f, frameHeight};
            for (int i = 0; i < MAX_MYSTERY_BLOCKS; i++)
            {
                if (!mysteryBlocks[i].active) continue;
                if (CheckCollisionRecs(playerRect, mysteryBlocks[i].rect) && player.speedY < 0.0f)
                {
                    player.speedY = 60.0f;
                    mysteryBlocks[i].bounceOffset = 12.0f;
                    if (!mysteryBlocks[i].used)
                    {
                        mysteryBlocks[i].used = true;
                        PlayCoinSound(coinSound);
                        SpawnParticles(particles, (Vector2){mysteryBlocks[i].rect.x + 25, mysteryBlocks[i].rect.y}, GOLD, 25);
                        if (mysteryBlocks[i].rewardType == 0)
                        {
                            (*coinsCollected) += 3;
                            score += 150;
                            SpawnFloatingText(floatingTexts, player.position, "¡BLOQUE [?]: +3 MONEDAS!", GOLD);
                        }
                        else if (mysteryBlocks[i].rewardType == 1)
                        {
                            player.hasShield = true;
                            score += 100;
                            SpawnFloatingText(floatingTexts, player.position, "¡ESCUDO ESTRELLA!", SKYBLUE);
                        }
                        else
                        {
                            player.lives++;
                            score += 100;
                            SpawnFloatingText(floatingTexts, player.position, "¡1-UP VIDA EXTRA!", LIME);
                        }
                    }
                }
            }

            int hitObstacle = 0;
            float platformDeltaX = 0.0f;
            float platformDeltaY = 0.0f;

            for (int i = 0; i < MAX_ENVIRONMENT_ELEMENTS; i++)
            {
                EnvElement *element = &envElements[i];
                if (!element->blocking || !element->active)
                    continue;

                Vector2 *p = &player.position;
                float margin = frameWidth * 0.25f;

                if (element->rect.x - margin <= p->x &&
                    element->rect.x + element->rect.width + margin >= p->x &&
                    element->rect.y >= p->y - 8.0f &&
                    element->rect.y <= p->y + player.speedY * deltaTime + 10.0f &&
                    player.speedY >= 0.0f)
                {
                    if (element->type == PLAT_BOUNCE)
                    {
                        player.speedY = -PLAYER_JUMP_SPD * 1.55f;
                        player.jumpsAvailable = 2;
                        player.scaleX = 0.68f;
                        player.scaleY = 1.40f;
                        SpawnParticles(particles, p[0], LIME, 22);
                        SpawnFloatingText(floatingTexts, p[0], "¡SUPER RESORTE!", LIME);
                        hitObstacle = 0;
                        break;
                    }

                    hitObstacle = 1;
                    player.speedY = 0.0f;
                    p->y = element->rect.y;

                    if (element->type == PLAT_MOVING_H)
                        platformDeltaX = element->moveSpeed * deltaTime;
                    else if (element->type == PLAT_MOVING_V)
                        platformDeltaY = element->moveSpeed * deltaTime;
                    else if (element->type == PLAT_CRUMBLE)
                        element->falling = true;
                }
            }

            if (!hitObstacle)
            {
                player.position.y += player.speedY * deltaTime;
                player.speedY += GRAVITY * deltaTime;
                player.canJump = false;
            }
            else
            {
                if (!wasOnGround)
                {
                    player.scaleX = 1.28f;
                    player.scaleY = 0.74f;
                    SpawnParticles(particles, player.position, LIGHTGRAY, 8);
                }
                player.canJump = true;
                player.jumpsAvailable = 2;
                player.position.x += platformDeltaX;
                player.position.y += platformDeltaY;
            }
            wasOnGround = (hitObstacle != 0);

            for (int i = 0; i < MAX_CHECKPOINTS; i++)
            {
                if (checkpoints[i].active && !checkpoints[i].activated)
                {
                    Rectangle cpRect = {checkpoints[i].position.x - 20, checkpoints[i].position.y - 70, 50, 70};
                    if (CheckCollisionRecs(playerRect, cpRect))
                    {
                        checkpoints[i].activated = true;
                        respawnPos = checkpoints[i].position;
                        score += 100;
                        SpawnParticles(particles, checkpoints[i].position, LIME, 28);
                        SpawnFloatingText(floatingTexts, checkpoints[i].position, "¡CHECKPOINT!", GREEN);
                        PlayCoinSound(coinSound);
                    }
                }
            }

            Rectangle portalRect = {posPortalMeta.x - 35, posPortalMeta.y - 95, 70, 95};
            if (CheckCollisionRecs(playerRect, portalRect))
            {
                if (coinsAnsweredInLevel >= MIN_COINS_FOR_PORTAL)
                {
                    levelCompleted = true;
                    score += player.lives * 150;
                    if (score > *highScoreNivel)
                        *highScoreNivel = score;
                }
                else
                {
                    SpawnFloatingText(floatingTexts, posPortalMeta, TextFormat("¡Faltan %d monedas!", MIN_COINS_FOR_PORTAL - coinsAnsweredInLevel), RED);
                }
            }

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
                        if (player.speedY > 0.0f && player.position.y - player.speedY * deltaTime <= enemyRect.y + 18.0f)
                        {
                            enemies[i].hp--;
                            player.speedY = -PLAYER_JUMP_SPD * 0.90f;
                            player.jumpsAvailable = 2;
                            player.scaleX = 1.3f;
                            player.scaleY = 0.7f;
                            PlayCoinSound(coinSound);

                            if (enemies[i].hp <= 0)
                            {
                                enemies[i].active = false;
                                int pts = enemies[i].isBoss ? 300 : 60;
                                score += pts;
                                SpawnParticles(particles, enemies[i].position, PURPLE, 32);
                                SpawnFloatingText(floatingTexts, enemies[i].position, enemies[i].isBoss ? "¡JEFE DERROTADO +300!" : "¡APLASTADO +60!", LIME);
                            }
                            else
                            {
                                enemies[i].speed *= 1.25f;
                                SpawnParticles(particles, enemies[i].position, ORANGE, 20);
                                SpawnFloatingText(floatingTexts, enemies[i].position, TextFormat("¡GOLPE AL JEFE! (%d HP)", enemies[i].hp), ORANGE);
                            }
                        }
                        else if (player.invulnerableTimer <= 0.0f)
                        {
                            player.invulnerableTimer = 2.0f;
                            player.speedY = -230.0f;
                            player.velX = (player.position.x < enemies[i].position.x) ? -250.0f : 250.0f;
                            screenShakeTimer = 0.35f;

                            if (player.hasShield)
                            {
                                player.hasShield = false;
                                SpawnParticles(particles, player.position, SKYBLUE, 25);
                                SpawnFloatingText(floatingTexts, player.position, "¡ESCUDO ROTO!", SKYBLUE);
                            }
                            else
                            {
                                player.lives--;
                                streak = 0;
                                SpawnParticles(particles, player.position, RED, 22);
                                SpawnFloatingText(floatingTexts, player.position, "-1 VIDA", RED);
                                if (player.lives <= 0)
                                    gameOver = true;
                            }
                        }
                    }
                }
            }

            if (player.position.y > screenHeight + 180)
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
                    player.position = respawnPos;
                    player.velX = 0.0f;
                    player.speedY = 0.0f;
                    player.jumpsAvailable = 2;
                    player.invulnerableTimer = 1.5f;
                    SpawnFloatingText(floatingTexts, player.position, "¡REAPARICION EN CHECKPOINT!", ORANGE);
                }
            }

            for (int i = 0; i < MAX_COINS; i++)
            {
                if (coins[i].active && CheckCollisionRecs(playerRect, coins[i].rect))
                {
                    coins[i].active = false;
                    showingQuestion = true;
                    questionTimer = MAX_QUESTION_TIME;
                    currentQuestionData = GenerarPreguntaAleatoria(numeroNivel);
                    SpawnParticles(particles, coins[i].position, GOLD, 24);

                    answeredCorrectly = false;
                    feedbackTimer = 0.0f;
                    currentAttempts = 0;
                    questionFailed = false;
                }
            }

            float speedRatio = fabsf(player.velX) / PLAYER_MAX_HOR_SPD;
            if (!player.canJump)
            {
                currentFrame = (player.speedY < 0.0f) ? 1 : 2;
                float targetRot = (facingRight ? 1.0f : -1.0f) * (player.speedY * 0.035f);
                player.rotation += (targetRot - player.rotation) * 10.0f * deltaTime;
            }
            else if (fabsf(player.velX) > 18.0f)
            {
                float dynamicFrameTime = 0.16f - (speedRatio * 0.07f);
                frameCounter += deltaTime;
                if (frameCounter >= dynamicFrameTime)
                {
                    currentFrame = 1 + ((currentFrame) % (totalFrames - 1));
                    frameCounter = 0.0f;
                }
                float targetRot = (player.velX / PLAYER_MAX_HOR_SPD) * 7.5f;
                player.rotation += (targetRot - player.rotation) * 12.0f * deltaTime;
            }
            else
            {
                currentFrame = 0;
                player.idleTimer += deltaTime * 4.5f;
                player.rotation += (0.0f - player.rotation) * 12.0f * deltaTime;
            }

            if (player.dashTimer > 0.0f || fabsf(player.speedY) > PLAYER_JUMP_SPD * 1.15f)
            {
                player.trailSpawnTimer += deltaTime;
                if (player.trailSpawnTimer >= 0.03f)
                {
                    player.trailSpawnTimer = 0.0f;
                    for (int t = 0; t < MAX_TRAIL; t++)
                    {
                        if (!ghostTrail[t].active)
                        {
                            ghostTrail[t].active = true;
                            ghostTrail[t].position = player.position;
                            ghostTrail[t].frame = currentFrame;
                            ghostTrail[t].facingRight = facingRight;
                            ghostTrail[t].alpha = 0.55f;
                            ghostTrail[t].tint = (player.dashTimer > 0.0f) ? ORANGE : SKYBLUE;
                            break;
                        }
                    }
                }
            }
        }

        if (!juegoPausado)
        {
            for (int t = 0; t < MAX_TRAIL; t++)
            {
                if (ghostTrail[t].active)
                {
                    ghostTrail[t].alpha -= deltaTime * 2.4f;
                    if (ghostTrail[t].alpha <= 0.0f)
                        ghostTrail[t].active = false;
                }
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
            memcpy(mysteryBlocks, bloquesIniciales, sizeof(MysteryBlock) * MAX_MYSTERY_BLOCKS);
            memcpy(checkpoints, checkpointsIniciales, sizeof(Checkpoint) * MAX_CHECKPOINTS);
            for (int i = 0; i < MAX_ENVIRONMENT_ELEMENTS; i++)
                envElements[i].active = (envElements[i].rect.width > 0);

            respawnPos = posInicial;
            player.position = posInicial;
            player.velX = 0.0f;
            player.speedY = 0.0f;
            player.canJump = false;
            player.jumpsAvailable = 2;
            player.lives = 3;
            player.hasShield = false;
            player.invulnerableTimer = 0.0f;
            score = 0;
            streak = 0;
            coinsAnsweredInLevel = 0;
            gameOver = false;
            showingQuestion = false;
            questionFailed = false;
        }

        if (!gameOver && !levelCompleted)
        {
            float targetX = fmaxf(minCameraX, fminf(maxCameraX, player.position.x + (facingRight ? 70.0f : -70.0f)));
            float targetY = fminf(560.0f, fmaxf(120.0f, player.position.y));
            camera.target.x += (targetX - camera.target.x) * 7.5f * deltaTime;
            camera.target.y += (targetY - camera.target.y) * 5.5f * deltaTime;
            camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 1.45f};
            if (screenShakeTimer > 0.0f)
            {
                camera.offset.x += (float)GetRandomValue(-9, 9);
                camera.offset.y += (float)GetRandomValue(-9, 9);
            }
        }

        // ================= DIBUJO DEL ESCENARIO CON FONDO GARANTIZADO =================
        ComenzarDibujo();
        ClearBackground(BLACK);

        // Dibuja el fondo del nivel + parallax procedural (Especialmente el Castillo en Nivel 3)
        DrawMarioBackground(fondo, numeroNivel, camera.target.x);

        BeginMode2D(camera);

        for (int i = 1; i < MAX_ENVIRONMENT_ELEMENTS; i++)
        {
            if (!envElements[i].active || envElements[i].rect.width <= 0)
                continue;

            Rectangle drawRect = envElements[i].rect;
            if (envElements[i].type == PLAT_CRUMBLE && envElements[i].falling)
            {
                drawRect.x += (float)GetRandomValue(-3, 3);
                drawRect.y += (float)GetRandomValue(-2, 2);
            }

            if (!envElements[i].blocking)
            {
                DrawTiledPlatform(dirtTex, drawRect, WHITE);
            }
            else
            {
                Color platTint = WHITE;
                if (envElements[i].type == PLAT_BOUNCE) platTint = LIME;
                else if (envElements[i].type == PLAT_CRUMBLE) platTint = ORANGE;
                else if (envElements[i].type == PLAT_MOVING_V) platTint = SKYBLUE;

                DrawTiledPlatform(dirtTex, drawRect, platTint);
                DrawTiledPlatform(platformTex, drawRect, platTint);

                if (envElements[i].type == PLAT_BOUNCE)
                {
                    DrawRectangleLinesEx(drawRect, 3.0f, GREEN);
                    DrawText("^^^", (int)(drawRect.x + drawRect.width / 2 - 18), (int)(drawRect.y + 8), 20, DARKGREEN);
                }

                DrawHalloweenPlatformDecor(drawRect, i);
            }
        }

        // Bloques Misteriosos [?] con remaches estilo Mario Bros
        for (int i = 0; i < MAX_MYSTERY_BLOCKS; i++)
        {
            if (!mysteryBlocks[i].active) continue;
            Rectangle bRect = mysteryBlocks[i].rect;
            bRect.y -= mysteryBlocks[i].bounceOffset;
            Color boxColor = mysteryBlocks[i].used ? (Color){130, 120, 110, 255} : MARIO_YELLOW;
            DrawRectangleRec((Rectangle){bRect.x + 3, bRect.y + 4, bRect.width, bRect.height}, Fade(BLACK, 0.45f));
            DrawRectangleRec(bRect, boxColor);
            DrawRectangleLinesEx(bRect, 3.5f, BLACK);
            DrawCircle((int)bRect.x + 6, (int)bRect.y + 6, 2, BLACK);
            DrawCircle((int)(bRect.x + bRect.width - 6), (int)bRect.y + 6, 2, BLACK);
            DrawCircle((int)bRect.x + 6, (int)(bRect.y + bRect.height - 6), 2, BLACK);
            DrawCircle((int)(bRect.x + bRect.width - 6), (int)(bRect.y + bRect.height - 6), 2, BLACK);
            const char *sym = mysteryBlocks[i].used ? "-" : "?";
            DrawText(sym, (int)(bRect.x + 17), (int)(bRect.y + 9), 32, mysteryBlocks[i].used ? DARKGRAY : MARIO_BRICK);
        }

        // Checkpoints estilo asta de bandera de Mario
        for (int i = 0; i < MAX_CHECKPOINTS; i++)
        {
            if (!checkpoints[i].active) continue;
            Vector2 cp = checkpoints[i].position;
            DrawRectangle((int)cp.x - 1, (int)cp.y - 78, 8, 78, BLACK);
            DrawRectangle((int)cp.x, (int)cp.y - 76, 6, 76, LIGHTGRAY);
            DrawCircle((int)cp.x + 3, (int)cp.y - 80, 8, GOLD);
            Color flagCol = checkpoints[i].activated ? MARIO_GREEN : MARIO_RED;
            DrawTriangle((Vector2){cp.x + 6, cp.y - 75}, (Vector2){cp.x + 6, cp.y - 38}, (Vector2){cp.x + 52, cp.y - 56}, flagCol);
            DrawText(checkpoints[i].activated ? "¡GUARDADO!" : "CHECKPOINT", (int)cp.x - 28, (int)cp.y - 102, 15, flagCol);
        }

        // Portal Dimensional / Meta
        float portalPulse = sinf((float)GetTime() * 5.0f) * 6.0f;
        bool portalAbierto = (coinsAnsweredInLevel >= MIN_COINS_FOR_PORTAL);
        Color portalCol = portalAbierto ? MARIO_PURPLE : DARKGRAY;
        DrawCircle((int)posPortalMeta.x, (int)posPortalMeta.y - 50, 48 + portalPulse, Fade(portalCol, 0.50f));
        DrawCircle((int)posPortalMeta.x, (int)posPortalMeta.y - 50, 35, Fade(portalAbierto ? VIOLET : GRAY, 0.90f));
        DrawCircleLines((int)posPortalMeta.x, (int)posPortalMeta.y - 50, 48 + portalPulse, MARIO_YELLOW);
        const char *portalTxt = portalAbierto ? "★ ¡META ABIERTA! ★" : TextFormat("META (%d/%d Monedas)", coinsAnsweredInLevel, MIN_COINS_FOR_PORTAL);
        DrawText(portalTxt, (int)posPortalMeta.x - MeasureText(portalTxt, 18) / 2, (int)posPortalMeta.y - 118, 18, portalAbierto ? MARIO_YELLOW : LIGHTGRAY);

        // Enemigos y Jefe Guardián
        float bounce = fabsf(sinf((float)GetTime() * 8.0f)) * 5.0f;
        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            if (enemies[i].active)
            {
                Rectangle eRect = {enemies[i].position.x, enemies[i].position.y - enemies[i].height - bounce, enemies[i].width, enemies[i].height + bounce};
                Color eColor = enemies[i].isBoss ? MARIO_PURPLE : MARIO_RED;
                DrawRectangleRounded(eRect, 0.3f, 6, eColor);
                DrawRectangleLinesEx(eRect, 3.0f, enemies[i].isBoss ? MARIO_YELLOW : BLACK);

                int eyeOffset = (enemies[i].speed > 0) ? (int)(enemies[i].width * 0.5f) : 8;
                DrawCircle((int)eRect.x + eyeOffset, (int)eRect.y + 14, enemies[i].isBoss ? 8 : 5, WHITE);
                DrawCircle((int)eRect.x + eyeOffset + (enemies[i].isBoss ? 18 : 12), (int)eRect.y + 14, enemies[i].isBoss ? 8 : 5, WHITE);
                DrawCircle((int)eRect.x + eyeOffset, (int)eRect.y + 14, 3, BLACK);
                DrawCircle((int)eRect.x + eyeOffset + (enemies[i].isBoss ? 18 : 12), (int)eRect.y + 14, 3, BLACK);

                if (enemies[i].isBoss)
                {
#if MODO_HALLOWEEN
                    DrawWitchHat((Vector2){eRect.x + eRect.width / 2.0f, eRect.y + 2}, eRect.width * 0.9f);
#endif
                    DrawText("REY GUARDIAN", (int)eRect.x - 20, (int)eRect.y - 38, 17, MARIO_YELLOW);
                    DrawRectangle((int)eRect.x - 10, (int)eRect.y - 18, 92, 12, BLACK);
                    DrawRectangle((int)eRect.x - 8, (int)eRect.y - 16, (int)(88.0f * ((float)enemies[i].hp / enemies[i].maxHp)), 8, MARIO_RED);
                }
            }
        }

        // Monedas
        float floatOffset = sinf((float)GetTime() * 4.5f) * 5.0f;
        for (int i = 0; i < MAX_COINS; i++)
        {
            if (coins[i].active)
            {
                if (coinTex.id != 0)
                    DrawTexture(coinTex, (int)coins[i].position.x, (int)(coins[i].position.y + floatOffset), WHITE);
                else
                    DrawCircle((int)coins[i].position.x + 15, (int)(coins[i].position.y + 15 + floatOffset), 15, MARIO_YELLOW);
            }
        }

        UpdateAndDrawParticles(particles, juegoPausado ? 0.0f : deltaTime);
        UpdateAndDrawFloatingTexts(floatingTexts, juegoPausado ? 0.0f : deltaTime);

        for (int t = 0; t < MAX_TRAIL; t++)
        {
            if (ghostTrail[t].active)
            {
                Rectangle gSrc = {frameWidth * ghostTrail[t].frame, 0, ghostTrail[t].facingRight ? frameWidth : -frameWidth, frameHeight};
                Rectangle gDst = {ghostTrail[t].position.x, ghostTrail[t].position.y, frameWidth, frameHeight};
                Vector2 gOrigin = {frameWidth / 2.0f, frameHeight};
                DrawTexturePro(jugadorTex, gSrc, gDst, gOrigin, 0.0f, Fade(ghostTrail[t].tint, ghostTrail[t].alpha));
            }
        }

        bool visible = (player.invulnerableTimer <= 0.0f) || (((int)(player.invulnerableTimer * 12.0f) % 2) == 0);
        if (visible)
        {
            float breathY = (currentFrame == 0 && player.canJump) ? sinf(player.idleTimer) * 0.04f : 0.0f;
            float finalScaleX = player.scaleX - breathY * 0.5f;
            float finalScaleY = player.scaleY + breathY;

            Rectangle sourceRec = {frameWidth * currentFrame, 0, facingRight ? frameWidth : -frameWidth, frameHeight};
            Rectangle destRec = {
                player.position.x,
                player.position.y,
                frameWidth * finalScaleX,
                frameHeight * finalScaleY};
            Vector2 origin = {(frameWidth * finalScaleX) / 2.0f, frameHeight * finalScaleY};

            DrawTexturePro(jugadorTex, sourceRec, destRec, origin, player.rotation, (player.dashTimer > 0.0f) ? MARIO_YELLOW : WHITE);

            if (player.hasShield)
            {
                DrawCircleLines((int)player.position.x, (int)(player.position.y - frameHeight / 2), frameHeight * 0.65f, SKYBLUE);
                DrawCircle((int)player.position.x, (int)(player.position.y - frameHeight / 2), frameHeight * 0.65f, Fade(SKYBLUE, 0.25f));
            }
        }

        EndMode2D();

        // ================= HUD COLORIDO ESTILO MARIO BROS =================
        DrawMarioPanel((Rectangle){12, 12, 375, 92}, (Color){25, 55, 130, 235}, MARIO_YELLOW);
        DrawText(TextFormat("MUNDO 1-%d: %s", numeroNivel, nombreNivel), 30, 22, 18, MARIO_YELLOW);
        DrawText(TextFormat("Vidas: %d   |   Racha: x%d   |   Escudo: %s", player.lives, streak, player.hasShield ? "SI" : "NO"),
                 30, 48, 17, (player.lives == 1) ? MARIO_RED : LIME);
        const char *dashStatus = (player.dashCooldown <= 0.0f) ? "LISTO (SHIFT)" : TextFormat("%.1fs", player.dashCooldown);
        DrawText(TextFormat("Salto x2: ESPACIO  |  Dash: %s", dashStatus), 30, 72, 15, SKYBLUE);

        // Barra central de recorrido hacia el castillo/meta
        float avance = fmaxf(0.0f, fminf(1.0f, player.position.x / posPortalMeta.x));
        DrawMarioPanel((Rectangle){425, 14, 430, 56}, (Color){35, 35, 55, 235}, WHITE);
        DrawRectangle(445, 42, 390, 16, BLACK);
        DrawRectangle(447, 44, (int)(386 * avance), 12, MARIO_GREEN);
        DrawCircle(447 + (int)(386 * avance), 50, 9, MARIO_YELLOW);
        DrawText(TextFormat("META: %d%%  (%d/%d Monedas)", (int)(avance * 100), coinsAnsweredInLevel, MIN_COINS_FOR_PORTAL), 505, 22, 17, WHITE);

        // Panel derecho de Monedas y Puntos
        DrawMarioPanel((Rectangle){screenWidth - 262, 12, 248, 82}, (Color){165, 35, 35, 235}, MARIO_YELLOW);
        DrawCircle(screenWidth - 232, 36, 11, MARIO_YELLOW);
        DrawText(TextFormat("MONEDAS: %d", *coinsCollected), screenWidth - 212, 26, 21, MARIO_YELLOW);
        DrawText(TextFormat("PUNTOS: %d", score), screenWidth - 238, 56, 21, WHITE);

        // ================= VENTANA DE PREGUNTA ESTILO BLOQUE [?] DE MARIO =================
        if (showingQuestion)
        {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.72f));

            Rectangle qBox = {(screenWidth - 760) / 2.0f, 65, 760, 560};
            DrawMarioPanel(qBox, (Color){35, 75, 165, 250}, MARIO_YELLOW);

            const char *headerQ = "★ ¡DESAFIO MATEMATICO! ★";
            DrawText(headerQ, (screenWidth - MeasureText(headerQ, 24)) / 2, 85, 24, MARIO_YELLOW);

            int textWidth = MeasureText(currentQuestionData.text, 42);
            DrawText(currentQuestionData.text, (screenWidth - textWidth) / 2 + 3, 125 + 3, 42, BLACK);
            DrawText(currentQuestionData.text, (screenWidth - textWidth) / 2, 125, 42, WHITE);

            float ratio = fmaxf(0.0f, questionTimer / MAX_QUESTION_TIME);
            DrawRectangle((screenWidth - 460) / 2, 182, 460, 22, BLACK);
            DrawRectangle((screenWidth - 456) / 2, 184, (int)(452 * ratio), 18, (ratio > 0.35f) ? MARIO_GREEN : MARIO_RED);

            char attemptInfo[90];
            sprintf(attemptInfo, "Intentos: %d  |  Tiempo: %.1fs  |  Usa Clic o Teclas [1 - 4]", MAX_ATTEMPTS - currentAttempts, questionTimer);
            int infoWidth = MeasureText(attemptInfo, 18);
            DrawText(attemptInfo, (screenWidth - infoWidth) / 2, 212, 18, MARIO_YELLOW);

            int btnWidth = 420;
            int btnHeight = 58;
            int btnX = (screenWidth - btnWidth) / 2;
            int btnY = 250;
            int spacing = 18;

            int teclaSeleccionada = -1;
            if (feedbackTimer <= 0.0f)
            {
                if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1)) teclaSeleccionada = 0;
                if (IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_KP_2)) teclaSeleccionada = 1;
                if (IsKeyPressed(KEY_THREE) || IsKeyPressed(KEY_KP_3)) teclaSeleccionada = 2;
                if (IsKeyPressed(KEY_FOUR) || IsKeyPressed(KEY_KP_4)) teclaSeleccionada = 3;
            }

            Color optColors[4] = {MARIO_RED, MARIO_BLUE, MARIO_GREEN, MARIO_PURPLE};
            Color optHover[4] = {MARIO_ORANGE, SKYBLUE, (Color){70, 225, 85, 255}, VIOLET};

            for (int i = 0; i < 4; i++)
            {
                Rectangle btn = {(float)btnX, (float)(btnY + i * (btnHeight + spacing)), (float)btnWidth, (float)btnHeight};
                const char *optLabel = TextFormat("[%d]    %s", i + 1, currentQuestionData.options[i]);

                bool clickedOpt = DrawMarioButton(btn, optLabel, optColors[i], optHover[i], 26);

                if ((clickedOpt && feedbackTimer <= 0.0f) || teclaSeleccionada == i)
                {
                    answeredCorrectly = (i == currentQuestionData.correctAnswer);

                    if (answeredCorrectly)
                    {
                        streak++;
                        coinsAnsweredInLevel++;
                        int bonusMonedas = (streak >= 3) ? 2 : 1;
                        (*coinsCollected) += bonusMonedas;
                        int ptsGanados = 100 * streak + (int)(questionTimer * 10);
                        score += ptsGanados;

                        SpawnFloatingText(floatingTexts, player.position, TextFormat("+%d PTS!", ptsGanados), MARIO_YELLOW);
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
            const char *msg = NULL;
            Color msgCol = WHITE;
            if (answeredCorrectly)
            {
                msg = (streak >= 3) ? "¡CORRECTO! ¡RACHA BONUS (+2 Monedas)!" : "¡CORRECTO! (+1 Moneda)";
                msgCol = MARIO_GREEN;
            }
            else if (questionFailed)
            {
                msg = "¡Fallaste la pregunta y perdiste 1 vida!";
                msgCol = MARIO_ORANGE;
            }
            else
            {
                msg = TextFormat("Incorrecto. Intento %d de %d", currentAttempts, MAX_ATTEMPTS);
                msgCol = MARIO_RED;
            }

            int mw = MeasureText(msg, 32);
            DrawRectangleRounded((Rectangle){(screenWidth - mw - 40) / 2.0f, 555, (float)mw + 40, 50}, 0.4f, 6, Fade(BLACK, 0.80f));
            DrawText(msg, (screenWidth - mw) / 2, 565, 32, msgCol);
        }

        if (levelCompleted)
        {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.78f));
            DrawMarioPanel((Rectangle){(screenWidth - 640) / 2.0f, 170, 640, 360}, (Color){30, 110, 55, 250}, MARIO_YELLOW);

            const char *winTitle = "¡MUNDO COMPLETADO!";
            DrawText(winTitle, (screenWidth - MeasureText(winTitle, 44)) / 2 + 3, 213, 44, BLACK);
            DrawText(winTitle, (screenWidth - MeasureText(winTitle, 44)) / 2, 210, 44, MARIO_YELLOW);

            const char *scoreTxt = TextFormat("Puntaje Final: %d pts  (Record: %d pts)", score, *highScoreNivel);
            DrawText(scoreTxt, (screenWidth - MeasureText(scoreTxt, 26)) / 2, 295, 26, WHITE);

            Rectangle btnCont = {(screenWidth - 280) / 2.0f, 390, 280, 68};
            if (DrawMarioButton(btnCont, "Continuar", MARIO_ORANGE, MARIO_YELLOW, 28))
            {
                UnloadTexture(coinTex);
                UnloadTexture(platformTex);
                UnloadTexture(dirtTex);
                UnloadSound(coinSound);
                TerminarDibujo();
                return GAME_WON;
            }
        }

        if (gameOver)
        {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.82f));
            DrawMarioPanel((Rectangle){(screenWidth - 640) / 2.0f, 180, 640, 340}, (Color){120, 25, 30, 250}, MARIO_YELLOW);

            const char *loseTitle = "¡GAME OVER!";
            DrawText(loseTitle, (screenWidth - MeasureText(loseTitle, 52)) / 2 + 3, 223, 52, BLACK);
            DrawText(loseTitle, (screenWidth - MeasureText(loseTitle, 52)) / 2, 220, 52, MARIO_RED);

            const char *retryTxt = "Presiona [R] para Reintentar o [ESC] para Salir";
            DrawText(retryTxt, (screenWidth - MeasureText(retryTxt, 22)) / 2, 320, 22, WHITE);

            Rectangle btnBack = {(screenWidth - 280) / 2.0f, 390, 280, 64};
            if (IsKeyPressed(KEY_ESCAPE) || DrawMarioButton(btnBack, "Volver a Niveles", MARIO_BLUE, SKYBLUE, 24))
            {
                UnloadTexture(coinTex);
                UnloadTexture(platformTex);
                UnloadTexture(dirtTex);
                UnloadSound(coinSound);
                TerminarDibujo();
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
                memcpy(mysteryBlocks, bloquesIniciales, sizeof(MysteryBlock) * MAX_MYSTERY_BLOCKS);
                memcpy(checkpoints, checkpointsIniciales, sizeof(Checkpoint) * MAX_CHECKPOINTS);
                for (int i = 0; i < MAX_ENVIRONMENT_ELEMENTS; i++)
                    envElements[i].active = (envElements[i].rect.width > 0);
                respawnPos = posInicial;
                player.position = posInicial;
                player.velX = 0.0f;
                player.speedY = 0.0f;
                player.jumpsAvailable = 2;
                player.lives = 3;
                player.hasShield = false;
                score = 0;
                streak = 0;
                coinsAnsweredInLevel = 0;
                juegoPausado = false;
            }
            else if (op == PAUSA_MENU_PRINCIPAL)
            {
                UnloadTexture(coinTex);
                UnloadTexture(platformTex);
                UnloadTexture(dirtTex);
                UnloadSound(coinSound);
                TerminarDibujo();
                return GAME_EXIT_MAIN_MENU;
            }
            else if (op == PAUSA_SALIR)
            {
                UnloadTexture(coinTex);
                UnloadTexture(platformTex);
                UnloadTexture(dirtTex);
                UnloadSound(coinSound);
                TerminarDibujo();
                return GAME_EXIT_APP;
            }
        }

        TerminarDibujo();
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
        {{0, 0, 1280, 720}, 0, PLAT_NORMAL, 0, 0, 0},
        {{-600, 650, 5200, 450}, 0, PLAT_NORMAL, 0, 0, 0},
        {{-600, 600, 5200, 50}, 1, PLAT_NORMAL, 0, 0, 0},
        {{220, 500, 180, 45}, 1, PLAT_NORMAL, 0, 0, 0},
        {{470, 410, 180, 45}, 1, PLAT_NORMAL, 0, 0, 0},
        {{730, 520, 130, 35}, 1, PLAT_BOUNCE, 0, 0, 0},
        {{920, 260, 220, 45}, 1, PLAT_NORMAL, 0, 0, 0},
        {{1220, 350, 200, 45}, 1, PLAT_MOVING_H, 80.0f, 1180.0f, 1450.0f},
        {{1560, 470, 220, 45}, 1, PLAT_NORMAL, 0, 0, 0},
        {{1850, 380, 180, 45}, 1, PLAT_NORMAL, 0, 0, 0},
        {{2120, 520, 130, 35}, 1, PLAT_BOUNCE, 0, 0, 0},
        {{2320, 240, 220, 45}, 1, PLAT_NORMAL, 0, 0, 0},
        {{2620, 340, 200, 45}, 1, PLAT_MOVING_H, 95.0f, 2580.0f, 2850.0f},
        {{2950, 250, 200, 45}, 1, PLAT_NORMAL, 0, 0, 0},
        {{3250, 420, 220, 45}, 1, PLAT_NORMAL, 0, 0, 0},
        {{3550, 320, 220, 45}, 1, PLAT_NORMAL, 0, 0, 0},
        {{3850, 480, 320, 50}, 1, PLAT_NORMAL, 0, 0, 0}};

    Coin coins[MAX_COINS] = {
        {{280, 440}, true, {280, 440, 30, 30}},
        {{530, 350}, true, {530, 350, 30, 30}},
        {{990, 200}, true, {990, 200, 30, 30}},
        {{1300, 290}, true, {1300, 290, 30, 30}},
        {{1630, 410}, true, {1630, 410, 30, 30}},
        {{1910, 320}, true, {1910, 320, 30, 30}},
        {{2400, 180}, true, {2400, 180, 30, 30}},
        {{2700, 280}, true, {2700, 280, 30, 30}},
        {{3020, 190}, true, {3020, 190, 30, 30}},
        {{3620, 260}, true, {3620, 260, 30, 30}}};

    Enemy enemies[MAX_ENEMIES] = {
        {{380, 600}, 36, 30, 90.0f, 200.0f, 700.0f, 1, 1, false, true},
        {{950, 260}, 34, 28, 70.0f, 925.0f, 1100.0f, 1, 1, false, true},
        {{1400, 600}, 36, 30, 110.0f, 1150.0f, 1800.0f, 1, 1, false, true},
        {{2350, 240}, 34, 28, 75.0f, 2325.0f, 2500.0f, 1, 1, false, true},
        {{2800, 600}, 36, 30, 120.0f, 2500.0f, 3300.0f, 1, 1, false, true},
        {0}, {0}};

    MysteryBlock blocks[MAX_MYSTERY_BLOCKS] = {
        {{535, 260, 50, 50}, false, 1, 0.0f, true},
        {{1640, 320, 50, 50}, false, 0, 0.0f, true},
        {{3330, 270, 50, 50}, false, 2, 0.0f, true},
        {0}};

    Checkpoint cps[MAX_CHECKPOINTS] = {
        {{1920, 380}, false, true},
        {{3350, 420}, false, true}};

    return ejecutarNivel(1, "Pradera de Resortes", posInicial, (Vector2){4020, 480}, fondo, coinsCollected, highScore,
                         jugadorTexture, totalFrames, gameMusic, "src/images/platform1.png", "src/images/dirt.png",
                         envElements, coins, enemies, blocks, cps);
}

GameStatus nivel2(Vector2 posInicial, Texture2D fondo, int *coinsCollected, int *highScore, Texture2D jugadorTexture, int totalFrames, Music *gameMusic)
{
    EnvElement envElements[MAX_ENVIRONMENT_ELEMENTS] = {
        {{0, 0, 1280, 720}, 0, PLAT_NORMAL, 0, 0, 0},
        {{-400, 650, 900, 450}, 0, PLAT_NORMAL, 0, 0, 0},
        {{-400, 600, 900, 50}, 1, PLAT_NORMAL, 0, 0, 0},
        {{550, 520, 160, 40}, 1, PLAT_CRUMBLE, 0, 0, 0},
        {{780, 450, 180, 45}, 1, PLAT_MOVING_V, 85.0f, 240.0f, 520.0f},
        {{1040, 280, 220, 45}, 1, PLAT_NORMAL, 0, 0, 0},
        {{1330, 320, 160, 40}, 1, PLAT_CRUMBLE, 0, 0, 0},
        {{1560, 420, 220, 45}, 1, PLAT_MOVING_H, 105.0f, 1520.0f, 1820.0f},
        {{1900, 550, 500, 50}, 1, PLAT_NORMAL, 0, 0, 0},
        {{2120, 470, 130, 35}, 1, PLAT_BOUNCE, 0, 0, 0},
        {{2460, 250, 200, 45}, 1, PLAT_NORMAL, 0, 0, 0},
        {{2730, 320, 160, 40}, 1, PLAT_CRUMBLE, 0, 0, 0},
        {{2960, 420, 180, 45}, 1, PLAT_MOVING_V, 95.0f, 180.0f, 480.0f},
        {{3220, 220, 220, 45}, 1, PLAT_NORMAL, 0, 0, 0},
        {{3500, 350, 170, 40}, 1, PLAT_CRUMBLE, 0, 0, 0},
        {{3750, 500, 450, 50}, 1, PLAT_NORMAL, 0, 0, 0}};

    Coin coins[MAX_COINS] = {
        {{600, 450}, true, {600, 450, 30, 30}},
        {{850, 300}, true, {850, 300, 30, 30}},
        {{1120, 220}, true, {1120, 220, 30, 30}},
        {{1380, 250}, true, {1380, 250, 30, 30}},
        {{1660, 350}, true, {1660, 350, 30, 30}},
        {{2170, 220}, true, {2170, 220, 30, 30}},
        {{2530, 190}, true, {2530, 190, 30, 30}},
        {{2790, 250}, true, {2790, 250, 30, 30}},
        {{3300, 160}, true, {3300, 160, 30, 30}},
        {{3560, 280}, true, {3560, 280, 30, 30}}};

    Enemy enemies[MAX_ENEMIES] = {
        {{180, 600}, 36, 30, 100.0f, 80.0f, 440.0f, 1, 1, false, true},
        {{1060, 280}, 34, 28, 80.0f, 1045.0f, 1220.0f, 1, 1, false, true},
        {{1950, 550}, 36, 30, 115.0f, 1910.0f, 2350.0f, 1, 1, false, true},
        {{2480, 250}, 34, 28, 85.0f, 2465.0f, 2620.0f, 1, 1, false, true},
        {{3250, 220}, 34, 28, 90.0f, 3225.0f, 3400.0f, 1, 1, false, true},
        {0}, {0}};

    MysteryBlock blocks[MAX_MYSTERY_BLOCKS] = {
        {{280, 440, 50, 50}, false, 1, 0.0f, true},
        {{1980, 400, 50, 50}, false, 2, 0.0f, true},
        {{3310, 80, 50, 50}, false, 0, 0.0f, true},
        {0}};

    Checkpoint cps[MAX_CHECKPOINTS] = {
        {{1950, 550}, false, true},
        {{3280, 220}, false, true}};

    return ejecutarNivel(2, "Canon de Elevadores", posInicial, (Vector2){4050, 500}, fondo, coinsCollected, highScore,
                         jugadorTexture, totalFrames, gameMusic, "src/images/platform2.png", "src/images/dirt2.png",
                         envElements, coins, enemies, blocks, cps);
}

GameStatus nivel3(Vector2 posInicial, Texture2D fondo, int *coinsCollected, int *highScore, Texture2D jugadorTexture, int totalFrames, Music *gameMusic)
{
    EnvElement envElements[MAX_ENVIRONMENT_ELEMENTS] = {
        {{0, 0, 1280, 720}, 0, PLAT_NORMAL, 0, 0, 0},
        {{-400, 650, 750, 450}, 0, PLAT_NORMAL, 0, 0, 0},
        {{-400, 600, 750, 50}, 1, PLAT_NORMAL, 0, 0, 0},
        {{420, 500, 130, 35}, 1, PLAT_CRUMBLE, 0, 0, 0},
        {{630, 420, 130, 35}, 1, PLAT_BOUNCE, 0, 0, 0},
        {{850, 200, 180, 40}, 1, PLAT_MOVING_H, 120.0f, 820.0f, 1120.0f},
        {{1200, 320, 140, 35}, 1, PLAT_CRUMBLE, 0, 0, 0},
        {{1430, 420, 160, 40}, 1, PLAT_MOVING_V, 110.0f, 180.0f, 480.0f},
        {{1680, 520, 420, 50}, 1, PLAT_NORMAL, 0, 0, 0},
        {{2160, 440, 140, 35}, 1, PLAT_CRUMBLE, 0, 0, 0},
        {{2380, 360, 140, 35}, 1, PLAT_CRUMBLE, 0, 0, 0},
        {{2600, 480, 130, 35}, 1, PLAT_BOUNCE, 0, 0, 0},
        {{2820, 220, 220, 45}, 1, PLAT_MOVING_H, 130.0f, 2780.0f, 3080.0f},
        {{3180, 350, 150, 40}, 1, PLAT_CRUMBLE, 0, 0, 0},
        {{3420, 550, 820, 55}, 1, PLAT_NORMAL, 0, 0, 0}};

    Coin coins[MAX_COINS] = {
        {{460, 430}, true, {460, 430, 30, 30}},
        {{680, 240}, true, {680, 240, 30, 30}},
        {{950, 130}, true, {950, 130, 30, 30}},
        {{1250, 250}, true, {1250, 250, 30, 30}},
        {{1480, 220}, true, {1480, 220, 30, 30}},
        {{1880, 450}, true, {1880, 450, 30, 30}},
        {{2210, 370}, true, {2210, 370, 30, 30}},
        {{2430, 290}, true, {2430, 290, 30, 30}},
        {{2900, 150}, true, {2900, 150, 30, 30}},
        {{3680, 480}, true, {3680, 480, 30, 30}}};

    Enemy enemies[MAX_ENEMIES] = {
        {{140, 600}, 36, 30, 115.0f, 40.0f, 310.0f, 1, 1, false, true},
        {{1720, 520}, 36, 30, 125.0f, 1690.0f, 2040.0f, 1, 1, false, true},
        {{3750, 550}, 68, 58, 135.0f, 3450.0f, 4020.0f, 3, 3, true, true},
        {0}, {0}, {0}, {0}};

    MysteryBlock blocks[MAX_MYSTERY_BLOCKS] = {
        {{180, 440, 50, 50}, false, 1, 0.0f, true},
        {{1820, 360, 50, 50}, false, 2, 0.0f, true},
        {{3520, 380, 50, 50}, false, 1, 0.0f, true},
        {{3820, 380, 50, 50}, false, 0, 0.0f, true}};

    Checkpoint cps[MAX_CHECKPOINTS] = {
        {{1730, 520}, false, true},
        {{3460, 550}, false, true}};

    return ejecutarNivel(3, "Fortaleza del Guardian", posInicial, (Vector2){4150, 550}, fondo, coinsCollected, highScore,
                         jugadorTexture, totalFrames, gameMusic, "src/images/platform3.png", "src/images/dirt3.png",
                         envElements, coins, enemies, blocks, cps);
}

void DrawTiledPlatform(Texture2D texture, Rectangle platform, Color tint)
{
    if (texture.id == 0)
    {
        DrawRectangleRec(platform, (tint.r == 255 && tint.g == 255 && tint.b == 255) ? MARIO_BRICK : tint);
        DrawRectangleLinesEx(platform, 2.5f, BLACK);
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

            DrawTexturePro(texture, sourceRect, destRect, (Vector2){0, 0}, 0.0f, tint);
        }
    }
}

void PlayCoinSound(Sound coinSound)
{
    if (IsAudioDeviceReady())
    {
        SetSoundVolume(coinSound, 0.22f);
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
            particles[i].velocity = (Vector2){(float)GetRandomValue(-140, 140), (float)GetRandomValue(-200, -30)};
            particles[i].alpha = 1.0f;
            particles[i].size = (float)GetRandomValue(4, 9);
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
            texts[i].position = (Vector2){pos.x - 40, pos.y - 55};
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
            texts[i].position.y -= 38.0f * deltaTime;
            texts[i].alpha -= deltaTime * 0.85f;
            if (texts[i].alpha <= 0.0f)
            {
                texts[i].active = false;
            }
            else
            {
                DrawText(texts[i].text, (int)texts[i].position.x + 2, (int)texts[i].position.y + 2, 20, Fade(BLACK, texts[i].alpha));
                DrawText(texts[i].text, (int)texts[i].position.x, (int)texts[i].position.y, 20, Fade(texts[i].color, texts[i].alpha));
            }
        }
    }
}

#endif