#ifndef NIVELES_H
#define NIVELES_H

#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
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
            a = GetRandomValue(15, 65);
            b = GetRandomValue(12, 55);
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
            a = GetRandomValue(120, 380);
            b = GetRandomValue(45, 190);
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
                    offset = 4;
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

    Texture2D coinTex = LoadTexture("src/images/coin.png");
    Texture2D platformTex = LoadTexture(rutaPlat);
    Texture2D dirtTex = LoadTexture(rutaDirt);
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
                            SpawnFloatingText(floatingTexts, player.position, "¡CAJA: +3 MONEDAS!", GOLD);
                        }
                        else if (mysteryBlocks[i].rewardType == 1)
                        {
                            player.hasShield = true;
                            score += 100;
                            SpawnFloatingText(floatingTexts, player.position, "¡ESCUDO PROTECTOR!", SKYBLUE);
                        }
                        else
                        {
                            player.lives++;
                            score += 100;
                            SpawnFloatingText(floatingTexts, player.position, "¡+1 VIDA EXTRA!", LIME);
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
                        SpawnFloatingText(floatingTexts, p[0], "¡SUPER SALTO!", LIME);
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
                        SpawnFloatingText(floatingTexts, checkpoints[i].position, "¡CHECKPOINT GUARDADO!", GREEN);
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

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(fondo, 0, 0, WHITE);

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
            }
        }

        for (int i = 0; i < MAX_MYSTERY_BLOCKS; i++)
        {
            if (!mysteryBlocks[i].active) continue;
            Rectangle bRect = mysteryBlocks[i].rect;
            bRect.y -= mysteryBlocks[i].bounceOffset;
            Color boxColor = mysteryBlocks[i].used ? GRAY : GOLD;
            DrawRectangleRec(bRect, boxColor);
            DrawRectangleLinesEx(bRect, 3.5f, mysteryBlocks[i].used ? DARKGRAY : ORANGE);
            const char *sym = mysteryBlocks[i].used ? "-" : "?";
            DrawText(sym, (int)(bRect.x + 17), (int)(bRect.y + 8), 32, mysteryBlocks[i].used ? LIGHTGRAY : MAROON);
        }

        for (int i = 0; i < MAX_CHECKPOINTS; i++)
        {
            if (!checkpoints[i].active) continue;
            Vector2 cp = checkpoints[i].position;
            DrawRectangle((int)cp.x, (int)cp.y - 75, 6, 75, LIGHTGRAY);
            Color flagCol = checkpoints[i].activated ? LIME : RED;
            DrawTriangle((Vector2){cp.x + 6, cp.y - 75}, (Vector2){cp.x + 6, cp.y - 40}, (Vector2){cp.x + 48, cp.y - 57}, flagCol);
            DrawText(checkpoints[i].activated ? "GUARDADO" : "CHECKPOINT", (int)cp.x - 25, (int)cp.y - 95, 14, flagCol);
        }

        float portalPulse = sinf((float)GetTime() * 5.0f) * 6.0f;
        bool portalAbierto = (coinsAnsweredInLevel >= MIN_COINS_FOR_PORTAL);
        Color portalCol = portalAbierto ? PURPLE : DARKGRAY;
        DrawCircle((int)posPortalMeta.x, (int)posPortalMeta.y - 50, 46 + portalPulse, Fade(portalCol, 0.45f));
        DrawCircle((int)posPortalMeta.x, (int)posPortalMeta.y - 50, 34, Fade(portalAbierto ? VIOLET : GRAY, 0.85f));
        DrawCircleLines((int)posPortalMeta.x, (int)posPortalMeta.y - 50, 46 + portalPulse, GOLD);
        const char *portalTxt = portalAbierto ? "¡META ABIERTA!" : TextFormat("META (%d/%d Monedas)", coinsAnsweredInLevel, MIN_COINS_FOR_PORTAL);
        DrawText(portalTxt, (int)posPortalMeta.x - MeasureText(portalTxt, 18) / 2, (int)posPortalMeta.y - 115, 18, portalAbierto ? GOLD : LIGHTGRAY);

        float bounce = fabsf(sinf((float)GetTime() * 8.0f)) * 5.0f;
        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            if (enemies[i].active)
            {
                Rectangle eRect = {enemies[i].position.x, enemies[i].position.y - enemies[i].height - bounce, enemies[i].width, enemies[i].height + bounce};
                Color eColor = enemies[i].isBoss ? DARKPURPLE : MAROON;
                DrawRectangleRec(eRect, eColor);
                DrawRectangleLinesEx(eRect, 3.0f, enemies[i].isBoss ? GOLD : BLACK);

                int eyeOffset = (enemies[i].speed > 0) ? (int)(enemies[i].width * 0.5f) : 8;
                DrawCircle((int)eRect.x + eyeOffset, (int)eRect.y + 14, enemies[i].isBoss ? 8 : 5, WHITE);
                DrawCircle((int)eRect.x + eyeOffset + (enemies[i].isBoss ? 18 : 12), (int)eRect.y + 14, enemies[i].isBoss ? 8 : 5, WHITE);
                DrawCircle((int)eRect.x + eyeOffset, (int)eRect.y + 14, 3, RED);
                DrawCircle((int)eRect.x + eyeOffset + (enemies[i].isBoss ? 18 : 12), (int)eRect.y + 14, 3, RED);

                if (enemies[i].isBoss)
                {
                    DrawText("GUARDIAN MATEMATICO", (int)eRect.x - 35, (int)eRect.y - 38, 16, GOLD);
                    DrawRectangle((int)eRect.x - 10, (int)eRect.y - 18, 90, 10, DARKGRAY);
                    DrawRectangle((int)eRect.x - 10, (int)eRect.y - 18, (int)(90.0f * ((float)enemies[i].hp / enemies[i].maxHp)), 10, RED);
                }
            }
        }

        float floatOffset = sinf((float)GetTime() * 4.5f) * 5.0f;
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

            DrawTexturePro(jugadorTex, sourceRec, destRec, origin, player.rotation, (player.dashTimer > 0.0f) ? GOLD : WHITE);

            if (player.hasShield)
            {
                DrawCircleLines((int)player.position.x, (int)(player.position.y - frameHeight / 2), frameHeight * 0.65f, SKYBLUE);
                DrawCircle((int)player.position.x, (int)(player.position.y - frameHeight / 2), frameHeight * 0.65f, Fade(SKYBLUE, 0.22f));
            }
        }

        EndMode2D();

        DrawRectangle(10, 10, 365, 92, Fade(BLACK, 0.65f));
        DrawRectangleLines(10, 10, 365, 92, GOLD);
        DrawText(TextFormat("NIVEL %d: %s", numeroNivel, nombreNivel), 22, 16, 18, GOLD);
        DrawText(TextFormat("Vidas: %d  |  Racha: x%d  |  Escudo: %s", player.lives, streak, player.hasShield ? "SI" : "NO"),
                 22, 42, 18, (player.lives == 1) ? RED : GREEN);
        const char *dashStatus = (player.dashCooldown <= 0.0f) ? "LISTO (SHIFT)" : TextFormat("%.1fs", player.dashCooldown);
        DrawText(TextFormat("Salto x2: ESPACIO  |  Dash: %s", dashStatus), 22, 68, 16, SKYBLUE);

        float avance = fmaxf(0.0f, fminf(1.0f, player.position.x / posPortalMeta.x));
        DrawRectangle(430, 16, 420, 26, Fade(BLACK, 0.65f));
        DrawRectangle(434, 20, (int)(412 * avance), 18, PURPLE);
        DrawRectangleLines(430, 16, 420, 26, GOLD);
        DrawText(TextFormat("Progreso a la Meta: %d%% (%d/%d Monedas)", (int)(avance * 100), coinsAnsweredInLevel, MIN_COINS_FOR_PORTAL), 465, 21, 16, WHITE);

        DrawRectangle(screenWidth - 250, 10, 240, 75, Fade(BLACK, 0.65f));
        DrawRectangleLines(screenWidth - 250, 10, 240, 75, GOLD);
        DrawText(TextFormat("Monedas: %d", *coinsCollected), screenWidth - 235, 18, 22, YELLOW);
        DrawText(TextFormat("Puntos: %d", score), screenWidth - 235, 48, 22, ORANGE);

        if (showingQuestion)
        {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.74f));

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
                        coinsAnsweredInLevel++;
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
        DrawRectangleRec(platform, tint);
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