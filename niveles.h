#include <raylib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_ENVIRONMENT_ELEMENTS 8
#define MAX_COINS 6
#define MAX_QUESTIONS 10

typedef struct Player
{
    Vector2 position;
    float speed;
    bool canJump;
} Player;

typedef struct EnvElement
{
    Rectangle rect;
    int blocking;
    Color color;
} EnvElement;

typedef struct Coin
{
    Vector2 position;
    bool active;
    Rectangle rect;
} Coin;

typedef struct Question
{
    const char *text;
    const char *options[4];
    int correctAnswer;
} Question;

typedef enum
{
    GAME_CONTINUE,
    GAME_PAUSE,
    GAME_OVER,
    GAME_WON // Nuevo estado para indicar que se completó el nivel
} GameStatus;

// Prototipos de funciones
GameStatus nivel1(Vector2 *posJugador, Texture2D fondo, int *coinsCollected, Texture2D jugadorTexture, int totalFrames);
GameStatus nivel2(Vector2 *posJugador, Texture2D fondo, int *coinsCollected, Texture2D jugadorTexture, int totalFrames);
GameStatus nivel3(Vector2 *posJugador, Texture2D fondo, int *coinsCollected, Texture2D jugadorTexture, int totalFrames);

GameStatus nivel1(Vector2 *posJugador, Texture2D fondo, int *coinsCollected, Texture2D jugadorTexture, int totalFrames)
{
    Player player = {0};

    Texture2D jugadorTex = jugadorTexture;
    Texture2D coinTex = LoadTexture("src/images/coin.png");

    int currentFrame = 0;
    float frameWidth = jugadorTex.width / totalFrames;
    float frameHeight = jugadorTex.height;
    float frameTime = 0.17f;
    float frameCounter = 0.0f;
    bool facingRight = true;

    player.position = *posJugador;
    player.speed = 0;
    player.canJump = false;

    EnvElement envElements[MAX_ENVIRONMENT_ELEMENTS] = {
        //  X  Y  ancho  alto blocking  color
        {{0, 0, 1280, 720}, 0, BLANK},  // Fondo completo (no bloqueante)
        {{0, 600, 1280, 120}, 1, GRAY}, // Suelo

        {{200, 550, 200, 50}, 1, GRAY},  // Plataforma 1
        {{400, 450, 250, 50}, 1, GRAY},  // Plataforma 2
        {{650, 350, 200, 50}, 1, GRAY},  // Plataforma 3
        {{900, 250, 200, 50}, 1, GRAY},  // Plataforma 4
        {{300, 300, 150, 50}, 1, GRAY},  // Plataforma 5
        {{800, 150, 150, 50}, 1, GRAY}}; // Plataforma 6

    Coin coins[MAX_COINS] = {
        // pos    X     Y   Activo   X   Y  ancho  largo
        {{270, 505}, true, {270, 505, 54, 65}},   // Sobre Plataforma 1
        {{500, 430}, true, {500, 430, 30, 30}},   // Sobre Plataforma 2
        {{750, 330}, true, {750, 330, 30, 30}},   // Sobre Plataforma 3
        {{1000, 230}, true, {1000, 230, 30, 30}}, // Sobre Plataforma 4
        {{350, 280}, true, {350, 280, 30, 30}},   // Sobre Plataforma 5
        {{850, 180}, true, {850, 130, 30, 30}}};  // Sobre Plataforma 6

    Question questions[MAX_QUESTIONS] = {
        {"¿Cuánto es 15 + 27?", {"32", "42", "37", "45"}, 1},
        {"¿Cuánto es 8 x 6?", {"42", "48", "54", "56"}, 1},
        {"¿Cuánto es 56 ÷ 7?", {"7", "8", "6", "9"}, 1},
        {"¿Cuánto es 45 - 19?", {"26", "24", "28", "30"}, 0},
        {"¿Cuánto es 9 x 4?", {"32", "36", "40", "45"}, 1},
        {"¿Cuánto es 72 ÷ 9?", {"7", "8", "9", "10"}, 1},
        {"¿Cuánto es 23 + 48?", {"69", "70", "71", "72"}, 2},
        {"¿Cuánto es 7 x 8?", {"54", "56", "58", "60"}, 1},
        {"¿Cuánto es 64 - 29?", {"33", "34", "35", "36"}, 2},
        {"¿Cuánto es 81 ÷ 9?", {"7", "8", "9", "10"}, 2}};

    bool showingQuestion = false;
    int currentQuestion = -1;
    bool answeredCorrectly = false;
    float feedbackTimer = 0.0f;
    const float FEEDBACK_DURATION = 2.0f;
    int lastCoinCollected = -1;
    bool gameOver = false;
    bool levelCompleted = false;
    bool questionsAnswered[MAX_QUESTIONS] = {false}; // Rastrear preguntas respondidas correctamente

    int screenWidth = 1280;
    int screenHeight = 720;

    Camera2D camera = {0};
    camera.target = player.position;
    camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    const float GRAVITY = 500;
    const float PLAYER_JUMP_SPD = 320.0f;
    const float PLAYER_HOR_SPD = 225.0f;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        // Verificar si se han recolectado todas las monedas
        if (!levelCompleted)
        {
            int activeCoins = 0;
            for (int i = 0; i < MAX_COINS; i++)
            {
                if (coins[i].active)
                {
                    activeCoins++;
                }
            }
            if (activeCoins == 0)
            {
                levelCompleted = true;
            }
        }

        if (!showingQuestion && feedbackTimer <= 0.0f && !gameOver && !levelCompleted)
        {
            // Movimiento horizontal
            if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
            {
                player.position.x -= PLAYER_HOR_SPD * deltaTime;
                facingRight = false;
            }
            if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
            {
                player.position.x += PLAYER_HOR_SPD * deltaTime;
                facingRight = true;
            }

            // Salto
            if (IsKeyPressed(KEY_SPACE) && player.canJump)
            {
                player.speed = -PLAYER_JUMP_SPD;
                player.canJump = false;
            }

            // Colisiones con plataformas
            int hitObstacle = 0;
            for (int i = 0; i < MAX_ENVIRONMENT_ELEMENTS; i++)
            {
                EnvElement *element = &envElements[i];
                Vector2 *p = &player.position;

                if (element->blocking &&
                    element->rect.x <= p->x &&
                    element->rect.x + element->rect.width >= p->x &&
                    element->rect.y >= p->y &&
                    element->rect.y <= p->y + player.speed * deltaTime)
                {
                    hitObstacle = 1;
                    player.speed = 0.0f;
                    p->y = element->rect.y;
                }
            }

            if (!hitObstacle)
            {
                player.position.y += player.speed * deltaTime;
                player.speed += GRAVITY * deltaTime;
                player.canJump = false;
            }
            else
            {
                player.canJump = true;
            }

            // Detectar caída al vacío
            if (player.position.y > screenHeight + 100)
            {
                gameOver = true;
                feedbackTimer = FEEDBACK_DURATION;
            }

            // Colisiones con monedas
            Rectangle playerRect = {player.position.x - frameWidth / 2, player.position.y - frameHeight, frameWidth, frameHeight};
            for (int i = 0; i < MAX_COINS; i++)
            {
                if (coins[i].active && CheckCollisionRecs(playerRect, coins[i].rect))
                {
                    coins[i].active = false;
                    (*coinsCollected)++; // Incrementar contador de monedas
                    showingQuestion = true;
                    // Seleccionar una pregunta no respondida correctamente
                    do
                    {
                        currentQuestion = GetRandomValue(0, MAX_QUESTIONS - 1);
                    } while (questionsAnswered[currentQuestion]);
                    lastCoinCollected = i;
                    answeredCorrectly = false;
                    feedbackTimer = 0.0f;
                }
            }

            // Actualizar animación
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

        // Actualizar temporizador de retroalimentación
        if (feedbackTimer > 0.0f)
        {
            feedbackTimer -= deltaTime;
            if (feedbackTimer <= 0.0f && !answeredCorrectly && lastCoinCollected >= 0 && !gameOver)
            {
                coins[lastCoinCollected].active = true;
                showingQuestion = true;
                // Contar preguntas no respondidas correctamente
                int unansweredCount = 0;
                for (int q = 0; q < MAX_QUESTIONS; q++)
                {
                    if (!questionsAnswered[q])
                        unansweredCount++;
                }
                if (unansweredCount > 0)
                {
                    do
                    {
                        currentQuestion = GetRandomValue(0, MAX_QUESTIONS - 1);
                    } while (questionsAnswered[currentQuestion]);
                }
                else
                {
                    // Si todas las preguntas ya fueron respondidas correctamente, desactivar la moneda permanentemente
                    lastCoinCollected = -1; // Evitar que se reactive la moneda
                    showingQuestion = false;
                }
            }
        }

        // Salir o reiniciar
        if (IsKeyPressed(KEY_ESCAPE) && !showingQuestion && !levelCompleted)
        {
            *posJugador = player.position;
            
            UnloadTexture(coinTex);
            return GAME_PAUSE;
        }

        // SI CAE AL VACIO
        if (gameOver && IsKeyPressed(KEY_R))
        {
            player.position = *posJugador; // Reiniciar posición
            player.speed = 0;
            player.canJump = false;
            gameOver = false;
            for (int i = 0; i < MAX_COINS; i++)
                coins[i].active = true; // Reactivar todas las monedas
            for (int i = 0; i < MAX_QUESTIONS; i++)
                questionsAnswered[i] = false; // Reinicia el arreglo de preguntas
            *coinsCollected -= MAX_COINS;     // Restar las monedas recolectadas en este nivel
            if (*coinsCollected < 0)
                *coinsCollected = 0; // Asegurar que no baje de 0
        }
        if (gameOver && feedbackTimer <= 0.0f)
        {
            *posJugador = player.position;
            
            UnloadTexture(coinTex);
            return GAME_OVER;
        }

        // Actualizar cámara (no sigue al jugador si está en game over o nivel completado)
        if (!gameOver && !levelCompleted)
        {
            camera.target = player.position;
            camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f};
        }

        // Dibujar
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Dibujar fondo estático
        DrawTexture(fondo, 0, 0, WHITE);

        BeginMode2D(camera);

        for (int i = 0; i < MAX_ENVIRONMENT_ELEMENTS; i++)
        {
            DrawRectangleRec(envElements[i].rect, envElements[i].color);
        }

        for (int i = 0; i < MAX_COINS; i++)
        {
            if (coins[i].active)
            {
                DrawTexture(coinTex, coins[i].position.x, coins[i].position.y, WHITE);
            }
        }

        Vector2 drawPos = {player.position.x - frameWidth / 2, player.position.y - frameHeight};
        Rectangle sourceRec;
        if (currentFrame == 0)
        {
            sourceRec = (Rectangle){frameWidth * currentFrame, 0, frameWidth, frameHeight};
        }
        else
        {
            sourceRec = (Rectangle){frameWidth * currentFrame, 0, facingRight ? frameWidth : -frameWidth, frameHeight};
        }
        DrawTextureRec(jugadorTex, sourceRec, drawPos, WHITE);

        EndMode2D();

        // Mostrar pregunta si está activa
        if (showingQuestion && currentQuestion >= 0)
        {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(DARKGRAY, 0.5f));

            int textWidth = MeasureText(questions[currentQuestion].text, 40);
            DrawText(questions[currentQuestion].text, (screenWidth - textWidth) / 2, 100, 40, WHITE);

            int btnWidth = 300;
            int btnHeight = 50;
            int btnX = (screenWidth - btnWidth) / 2;
            int btnY = 200;
            int spacing = 20;

            for (int i = 0; i < 4; i++)
            {
                Rectangle btn = {(float)btnX, (float)(btnY + i * (btnHeight + spacing)), (float)btnWidth, (float)btnHeight};
                bool mouseOver = CheckCollisionPointRec(GetMousePosition(), btn);
                Color btnColor = mouseOver ? Fade(DARKBLUE, 0.8f) : Fade(BLUE, 0.7f);

                DrawRectangleRec(btn, btnColor);
                DrawRectangleLinesEx(btn, 6.0f, DARKBLUE);
                int optWidth = MeasureText(questions[currentQuestion].options[i], 20);
                DrawText(questions[currentQuestion].options[i], btnX + (btnWidth - optWidth) / 2, btn.y + (btnHeight - 20) / 2, 20, WHITE);

                if (mouseOver && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    answeredCorrectly = (i == questions[currentQuestion].correctAnswer);
                    if (answeredCorrectly)
                    {
                        questionsAnswered[currentQuestion] = true;
                    }
                    showingQuestion = false;
                    feedbackTimer = FEEDBACK_DURATION;
                }
            }
        }

        // Mostrar mensaje de retroalimentación
        if (feedbackTimer > 0.0f && !gameOver)
        {
            if (answeredCorrectly)
            {
                DrawText("¡Correcto!", (screenWidth - MeasureText("¡Correcto!", 30)) / 2, 500, 30, GREEN);
            }
            else
            {
                DrawText("Incorrecto, intenta de nuevo.", (screenWidth - MeasureText("Incorrecto, intenta de nuevo.", 30)) / 2, 500, 30, RED);
            }
        }

        // Mostrar mensaje de nivel completado con botón de continuar
        if (levelCompleted)
        {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(DARKGRAY, 0.5f));
            DrawText("¡Has completado el nivel!", (screenWidth - MeasureText("¡Has completado el nivel!", 40)) / 2, 300, 40, GREEN);
            DrawText("¡Felicidades!", (screenWidth - MeasureText("¡Felicidades!", 30)) / 2, 350, 30, WHITE);

            // Botón de Continuar
            int btnWidth = 200;
            int btnHeight = 50;
            int btnX = (screenWidth - btnWidth) / 2;
            int btnY = 420;
            Rectangle btn = {(float)btnX, (float)btnY, (float)btnWidth, (float)btnHeight};
            bool mouseOver = CheckCollisionPointRec(GetMousePosition(), btn);
            Color btnColor = mouseOver ? Fade(DARKBLUE, 0.8f) : Fade(BLUE, 0.7f);

            DrawRectangleRec(btn, btnColor);
            DrawRectangleLinesEx(btn, 6.0f, DARKBLUE);
            int textWidth = MeasureText("Continuar", 20);
            DrawText("Continuar", btnX + (btnWidth - textWidth) / 2, btnY + (btnHeight - 20) / 2, 20, WHITE);

            if (mouseOver && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                *posJugador = player.position;
                UnloadTexture(coinTex);
                return GAME_WON; // Regresar al menú de niveles
            }
        }

        // Mostrar mensaje de game over
        if (gameOver)
        {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(DARKGRAY, 0.5f));
            DrawText("¡Perdiste!", (screenWidth - MeasureText("¡Perdiste!", 40)) / 2, 300, 40, RED);
            DrawText("Presiona R para reiniciar", (screenWidth - MeasureText("Presiona R para reiniciar", 20)) / 2, 350, 20, WHITE);
        }

        DrawText("Presiona ESC para salir al menú", 10, 10, 20, DARKGRAY);
        DrawText(TextFormat("Monedas: %d", *coinsCollected), screenWidth - 150, 10, 20, DARKGRAY);
        EndDrawing();
    }

    *posJugador = player.position;
    
    UnloadTexture(coinTex);
    return GAME_CONTINUE;
}

GameStatus nivel2(Vector2 *posJugador, Texture2D fondo, int *coinsCollected, Texture2D jugadorTexture, int totalFrames)
{
    Player player = {0};
    Texture2D jugadorTex = jugadorTexture;
    Texture2D coinTex = LoadTexture("src/images/coin.png");

    int currentFrame = 0;
    float frameWidth = jugadorTex.width / totalFrames;
    float frameHeight = jugadorTex.height;
    float frameTime = 0.17f;
    float frameCounter = 0.0f;
    bool facingRight = true;
    bool questionsAnswered[MAX_QUESTIONS] = {false}; // Rastrear preguntas respondidas correctamente

    player.position = *posJugador;
    player.speed = 0;
    player.canJump = false;

    EnvElement envElements[MAX_ENVIRONMENT_ELEMENTS] = {
        {{0, 0, 1280, 720}, 0, BLANK},   // Fondo completo (no bloqueante)
        {{0, 600, 1280, 120}, 1, GRAY},  // Suelo
        {{150, 500, 200, 50}, 1, GRAY},  // Plataforma 1
        {{350, 400, 200, 50}, 1, GRAY},  // Plataforma 2
        {{550, 300, 200, 50}, 1, GRAY},  // Plataforma 3
        {{750, 200, 200, 50}, 1, GRAY},  // Plataforma 4
        {{200, 350, 150, 50}, 1, GRAY},  // Plataforma 5
        {{600, 150, 150, 50}, 1, GRAY}}; // Plataforma 6

    Coin coins[MAX_COINS] = {
        {{250, 480}, true, {250, 480, 30, 30}},  // Sobre Plataforma 1
        {{450, 380}, true, {450, 380, 30, 30}},  // Sobre Plataforma 2
        {{650, 280}, true, {650, 280, 30, 30}},  // Sobre Plataforma 3
        {{850, 180}, true, {850, 180, 30, 30}},  // Sobre Plataforma 4
        {{250, 330}, true, {250, 330, 30, 30}},  // Sobre Plataforma 5
        {{650, 130}, true, {650, 130, 30, 30}}}; // Sobre Plataforma 6

    Question questions[MAX_QUESTIONS] = {
        {"¿Cuánto es 15 + 27?", {"32", "42", "37", "45"}, 1},
        {"¿Cuánto es 8 x 6?", {"42", "48", "54", "56"}, 1},
        {"¿Cuánto es 56 ÷ 7?", {"7", "8", "6", "9"}, 1},
        {"¿Cuánto es 45 - 19?", {"26", "24", "28", "30"}, 0},
        {"¿Cuánto es 9 x 4?", {"32", "36", "40", "45"}, 1},
        {"¿Cuánto es 72 ÷ 9?", {"7", "8", "9", "10"}, 1},
        {"¿Cuánto es 23 + 48?", {"69", "70", "71", "72"}, 2},
        {"¿Cuánto es 7 x 8?", {"54", "56", "58", "60"}, 1},
        {"¿Cuánto es 64 - 29?", {"33", "34", "35", "36"}, 2},
        {"¿Cuánto es 81 ÷ 9?", {"7", "8", "9", "10"}, 2}};

    bool showingQuestion = false;
    int currentQuestion = -1;
    bool answeredCorrectly = false;
    float feedbackTimer = 0.0f;
    const float FEEDBACK_DURATION = 2.0f;
    int lastCoinCollected = -1;
    bool gameOver = false;
    bool levelCompleted = false;

    int screenWidth = 1280;
    int screenHeight = 720;

    Camera2D camera = {0};
    camera.target = player.position;
    camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    const float GRAVITY = 500;
    const float PLAYER_JUMP_SPD = 320.0f;
    const float PLAYER_HOR_SPD = 225.0f;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        // Verificar si se han recolectado todas las monedas
        if (!levelCompleted)
        {
            int activeCoins = 0;
            for (int i = 0; i < MAX_COINS; i++)
            {
                if (coins[i].active)
                {
                    activeCoins++;
                }
            }
            if (activeCoins == 0)
            {
                levelCompleted = true;
            }
        }

        if (!showingQuestion && feedbackTimer <= 0.0f && !gameOver && !levelCompleted)
        {
            // Movimiento horizontal
            if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
            {
                player.position.x -= PLAYER_HOR_SPD * deltaTime;
                facingRight = false;
            }
            if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
            {
                player.position.x += PLAYER_HOR_SPD * deltaTime;
                facingRight = true;
            }

            // Salto
            if (IsKeyPressed(KEY_SPACE) && player.canJump)
            {
                player.speed = -PLAYER_JUMP_SPD;
                player.canJump = false;
            }

            // Colisiones con plataformas
            int hitObstacle = 0;
            for (int i = 0; i < MAX_ENVIRONMENT_ELEMENTS; i++)
            {
                EnvElement *element = &envElements[i];
                Vector2 *p = &player.position;

                if (element->blocking &&
                    element->rect.x <= p->x &&
                    element->rect.x + element->rect.width >= p->x &&
                    element->rect.y >= p->y &&
                    element->rect.y <= p->y + player.speed * deltaTime)
                {
                    hitObstacle = 1;
                    player.speed = 0.0f;
                    p->y = element->rect.y;
                }
            }

            if (!hitObstacle)
            {
                player.position.y += player.speed * deltaTime;
                player.speed += GRAVITY * deltaTime;
                player.canJump = false;
            }
            else
            {
                player.canJump = true;
            }

            // Detectar caída al vacío
            if (player.position.y > screenHeight + 100)
            {
                gameOver = true;
                feedbackTimer = FEEDBACK_DURATION;
            }

            // Colisiones con monedas
            Rectangle playerRect = {player.position.x - frameWidth / 2, player.position.y - frameHeight, frameWidth, frameHeight};
            for (int i = 0; i < MAX_COINS; i++)
            {
                if (coins[i].active && CheckCollisionRecs(playerRect, coins[i].rect))
                {
                    coins[i].active = false;
                    (*coinsCollected)++; // Incrementar contador de monedas
                    showingQuestion = true;
                    do
                    {
                        currentQuestion = GetRandomValue(0, MAX_QUESTIONS - 1);
                    } while (questionsAnswered[currentQuestion]);
                    lastCoinCollected = i;
                    answeredCorrectly = false;
                    feedbackTimer = 0.0f;
                }
            }

            // Actualizar animación
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

        // Actualizar temporizador de retroalimentación
        if (feedbackTimer > 0.0f)
        {
            feedbackTimer -= deltaTime;
            if (feedbackTimer <= 0.0f && !answeredCorrectly && lastCoinCollected >= 0 && !gameOver)
            {
                coins[lastCoinCollected].active = true;
                showingQuestion = true;
                // Contar preguntas no respondidas correctamente
                int unansweredCount = 0;
                for (int q = 0; q < MAX_QUESTIONS; q++)
                {
                    if (!questionsAnswered[q])
                        unansweredCount++;
                }
                if (unansweredCount > 0)
                {
                    do
                    {
                        currentQuestion = GetRandomValue(0, MAX_QUESTIONS - 1);
                    } while (questionsAnswered[currentQuestion]);
                }
                else
                {
                    // Si todas las preguntas ya fueron respondidas correctamente, desactivar la moneda permanentemente
                    lastCoinCollected = -1; // Evitar que se reactive la moneda
                    showingQuestion = false;
                }
            }
        }

        // Salir o reiniciar
        if (IsKeyPressed(KEY_ESCAPE) && !showingQuestion && !levelCompleted)
        {
            *posJugador = player.position;
            
            UnloadTexture(coinTex);
            return GAME_PAUSE;
        }
        if (gameOver && IsKeyPressed(KEY_R))
        {
            player.position = *posJugador; // Reiniciar posición
            player.speed = 0;
            player.canJump = false;
            gameOver = false;
            for (int i = 0; i < MAX_COINS; i++)
                coins[i].active = true; // Reactivar todas las monedas
            for (int i = 0; i < MAX_QUESTIONS; i++)
                questionsAnswered[i] = false; // Reinicia el arreglo de preguntas
            *coinsCollected -= MAX_COINS;     // Restar las monedas recolectadas en este nivel
            if (*coinsCollected < 0)
                *coinsCollected = 0; // Asegurar que no baje de 0
        }
        if (gameOver && feedbackTimer <= 0.0f)
        {
            *posJugador = player.position;
            
            UnloadTexture(coinTex);
            return GAME_OVER;
        }

        // Actualizar cámara (no sigue al jugador si está en game over o nivel completado)
        if (!gameOver && !levelCompleted)
        {
            camera.target = player.position;
            camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f};
        }

        // Dibujar
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Dibujar fondo estático
        DrawTexture(fondo, 0, 0, WHITE);

        BeginMode2D(camera);

        for (int i = 0; i < MAX_ENVIRONMENT_ELEMENTS; i++)
        {
            DrawRectangleRec(envElements[i].rect, envElements[i].color);
        }

        for (int i = 0; i < MAX_COINS; i++)
        {
            if (coins[i].active)
            {
                DrawTexture(coinTex, coins[i].position.x, coins[i].position.y, WHITE);
            }
        }

        Vector2 drawPos = {player.position.x - frameWidth / 2, player.position.y - frameHeight};
        Rectangle sourceRec;
        if (currentFrame == 0)
        {
            sourceRec = (Rectangle){frameWidth * currentFrame, 0, frameWidth, frameHeight};
        }
        else
        {
            sourceRec = (Rectangle){frameWidth * currentFrame, 0, facingRight ? frameWidth : -frameWidth, frameHeight};
        }
        DrawTextureRec(jugadorTex, sourceRec, drawPos, WHITE);

        EndMode2D();

        // Mostrar pregunta si está activa
        if (showingQuestion && currentQuestion >= 0)
        {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(DARKGRAY, 0.5f));

            int textWidth = MeasureText(questions[currentQuestion].text, 40);
            DrawText(questions[currentQuestion].text, (screenWidth - textWidth) / 2, 100, 40, WHITE);

            int btnWidth = 300;
            int btnHeight = 50;
            int btnX = (screenWidth - btnWidth) / 2;
            int btnY = 200;
            int spacing = 20;

            for (int i = 0; i < 4; i++)
            {
                Rectangle btn = {(float)btnX, (float)(btnY + i * (btnHeight + spacing)), (float)btnWidth, (float)btnHeight};
                bool mouseOver = CheckCollisionPointRec(GetMousePosition(), btn);
                Color btnColor = mouseOver ? Fade(DARKBLUE, 0.8f) : Fade(BLUE, 0.7f);

                DrawRectangleRec(btn, btnColor);
                DrawRectangleLinesEx(btn, 6.0f, DARKBLUE);
                int optWidth = MeasureText(questions[currentQuestion].options[i], 20);
                DrawText(questions[currentQuestion].options[i], btnX + (btnWidth - optWidth) / 2, btn.y + (btnHeight - 20) / 2, 20, WHITE);

                if (mouseOver && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    answeredCorrectly = (i == questions[currentQuestion].correctAnswer);
                    if (answeredCorrectly)
                    {
                        questionsAnswered[currentQuestion] = true;
                    }
                    showingQuestion = false;
                    feedbackTimer = FEEDBACK_DURATION;
                }
            }
        }

        // Mostrar mensaje de retroalimentación
        if (feedbackTimer > 0.0f && !gameOver)
        {
            if (answeredCorrectly)
            {
                DrawText("¡Correcto!", (screenWidth - MeasureText("¡Correcto!", 30)) / 2, 500, 30, GREEN);
            }
            else
            {
                DrawText("Incorrecto, intenta de nuevo.", (screenWidth - MeasureText("Incorrecto, intenta de nuevo.", 30)) / 2, 500, 30, RED);
            }
        }

        // Mostrar mensaje de nivel completado con botón de continuar
        if (levelCompleted)
        {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(DARKGRAY, 0.5f));
            DrawText("¡Has completado el nivel!", (screenWidth - MeasureText("¡Has completado el nivel!", 40)) / 2, 300, 40, GREEN);
            DrawText("¡Felicidades!", (screenWidth - MeasureText("¡Felicidades!", 30)) / 2, 350, 30, WHITE);

            // Botón de Continuar
            int btnWidth = 200;
            int btnHeight = 50;
            int btnX = (screenWidth - btnWidth) / 2;
            int btnY = 420;
            Rectangle btn = {(float)btnX, (float)btnY, (float)btnWidth, (float)btnHeight};
            bool mouseOver = CheckCollisionPointRec(GetMousePosition(), btn);
            Color btnColor = mouseOver ? Fade(DARKBLUE, 0.8f) : Fade(BLUE, 0.7f);

            DrawRectangleRec(btn, btnColor);
            DrawRectangleLinesEx(btn, 6.0f, DARKBLUE);
            int textWidth = MeasureText("Continuar", 20);
            DrawText("Continuar", btnX + (btnWidth - textWidth) / 2, btnY + (btnHeight - 20) / 2, 20, WHITE);

            if (mouseOver && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                *posJugador = player.position;
                
                UnloadTexture(coinTex);
                return GAME_WON; // Regresar al menú de niveles
            }
        }

        // Mostrar mensaje de game over
        if (gameOver)
        {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(DARKGRAY, 0.5f));
            DrawText("¡Perdiste!", (screenWidth - MeasureText("¡Perdiste!", 40)) / 2, 300, 40, RED);
            DrawText("Presiona R para reiniciar", (screenWidth - MeasureText("Presiona R para reiniciar", 20)) / 2, 350, 20, WHITE);
        }

        DrawText("Presiona ESC para salir al menú", 10, 10, 20, DARKGRAY);
        DrawText(TextFormat("Monedas: %d", *coinsCollected), screenWidth - 150, 10, 20, DARKGRAY);
        EndDrawing();
    }

    *posJugador = player.position;
    
    UnloadTexture(coinTex);
    return GAME_CONTINUE;
}

GameStatus nivel3(Vector2 *posJugador, Texture2D fondo, int *coinsCollected, Texture2D jugadorTexture , int totalFrames)
{
    Player player = {0};

    Texture2D jugadorTex = jugadorTexture;
    Texture2D coinTex = LoadTexture("src/images/coin.png");

    int currentFrame = 0;
    float frameWidth = jugadorTex.width / totalFrames;
    float frameHeight = jugadorTex.height;
    float frameTime = 0.17f;
    float frameCounter = 0.0f;
    bool facingRight = true;
    bool questionsAnswered[MAX_QUESTIONS] = {false}; // Rastrear preguntas respondidas correctamente

    player.position = *posJugador;
    player.speed = 0;
    player.canJump = false;

    EnvElement envElements[MAX_ENVIRONMENT_ELEMENTS] = {
        {{0, 0, 1280, 720}, 0, BLANK},   // Fondo completo (no bloqueante)
        {{0, 600, 1280, 120}, 1, GRAY},  // Suelo
        {{100, 550, 200, 50}, 1, GRAY},  // Plataforma 1
        {{300, 450, 200, 50}, 1, GRAY},  // Plataforma 2
        {{500, 350, 200, 50}, 1, GRAY},  // Plataforma 3
        {{700, 250, 200, 50}, 1, GRAY},  // Plataforma 4
        {{250, 300, 150, 50}, 1, GRAY},  // Plataforma 5
        {{650, 150, 150, 50}, 1, GRAY}}; // Plataforma 6

    Coin coins[MAX_COINS] = {
        {{200, 530}, true, {200, 530, 30, 30}},  // Sobre Plataforma 1
        {{400, 430}, true, {400, 430, 30, 30}},  // Sobre Plataforma 2
        {{600, 330}, true, {600, 330, 30, 30}},  // Sobre Plataforma 3
        {{800, 230}, true, {800, 230, 30, 30}},  // Sobre Plataforma 4
        {{300, 280}, true, {300, 280, 30, 30}},  // Sobre Plataforma 5
        {{700, 130}, true, {700, 130, 30, 30}}}; // Sobre Plataforma 6

    Question questions[MAX_QUESTIONS] = {
        {"¿Cuánto es 15 + 27?", {"32", "42", "37", "45"}, 1},
        {"¿Cuánto es 8 x 6?", {"42", "48", "54", "56"}, 1},
        {"¿Cuánto es 56 ÷ 7?", {"7", "8", "6", "9"}, 1},
        {"¿Cuánto es 45 - 19?", {"26", "24", "28", "30"}, 0},
        {"¿Cuánto es 9 x 4?", {"32", "36", "40", "45"}, 1},
        {"¿Cuánto es 72 ÷ 9?", {"7", "8", "9", "10"}, 1},
        {"¿Cuánto es 23 + 48?", {"69", "70", "71", "72"}, 2},
        {"¿Cuánto es 7 x 8?", {"54", "56", "58", "60"}, 1},
        {"¿Cuánto es 64 - 29?", {"33", "34", "35", "36"}, 2},
        {"¿Cuánto es 81 ÷ 9?", {"7", "8", "9", "10"}, 2}};

    bool showingQuestion = false;
    int currentQuestion = -1;
    bool answeredCorrectly = false;
    float feedbackTimer = 0.0f;
    const float FEEDBACK_DURATION = 2.0f;
    int lastCoinCollected = -1;
    bool gameOver = false;
    bool levelCompleted = false;

    int screenWidth = 1280;
    int screenHeight = 720;

    Camera2D camera = {0};
    camera.target = player.position;
    camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    const float GRAVITY = 500;
    const float PLAYER_JUMP_SPD = 320.0f;
    const float PLAYER_HOR_SPD = 225.0f;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        // Verificar si se han recolectado todas las monedas
        if (!levelCompleted)
        {
            int activeCoins = 0;
            for (int i = 0; i < MAX_COINS; i++)
            {
                if (coins[i].active)
                {
                    activeCoins++;
                }
            }
            if (activeCoins == 0)
            {
                levelCompleted = true;
            }
        }

        if (!showingQuestion && feedbackTimer <= 0.0f && !gameOver && !levelCompleted)
        {
            // Movimiento horizontal
            if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
            {
                player.position.x -= PLAYER_HOR_SPD * deltaTime;
                facingRight = false;
            }
            if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
            {
                player.position.x += PLAYER_HOR_SPD * deltaTime;
                facingRight = true;
            }

            // Salto
            if (IsKeyPressed(KEY_SPACE) && player.canJump)
            {
                player.speed = -PLAYER_JUMP_SPD;
                player.canJump = false;
            }

            // Colisiones con plataformas
            int hitObstacle = 0;
            for (int i = 0; i < MAX_ENVIRONMENT_ELEMENTS; i++)
            {
                EnvElement *element = &envElements[i];
                Vector2 *p = &player.position;

                if (element->blocking &&
                    element->rect.x <= p->x &&
                    element->rect.x + element->rect.width >= p->x &&
                    element->rect.y >= p->y &&
                    element->rect.y <= p->y + player.speed * deltaTime)
                {
                    hitObstacle = 1;
                    player.speed = 0.0f;
                    p->y = element->rect.y;
                }
            }

            if (!hitObstacle)
            {
                player.position.y += player.speed * deltaTime;
                player.speed += GRAVITY * deltaTime;
                player.canJump = false;
            }
            else
            {
                player.canJump = true;
            }

            // Detectar caída al vacío
            if (player.position.y > screenHeight + 100)
            {
                gameOver = true;
                feedbackTimer = FEEDBACK_DURATION;
            }

            // Colisiones con monedas
            Rectangle playerRect = {player.position.x - frameWidth / 2, player.position.y - frameHeight, frameWidth, frameHeight};
            for (int i = 0; i < MAX_COINS; i++)
            {
                if (coins[i].active && CheckCollisionRecs(playerRect, coins[i].rect))
                {
                    coins[i].active = false;
                    (*coinsCollected)++; // Incrementar contador de monedas
                    showingQuestion = true;
                    // Seleccionar una pregunta no respondida correctamente
                    do
                    {
                        currentQuestion = GetRandomValue(0, MAX_QUESTIONS - 1);
                    } while (questionsAnswered[currentQuestion]);
                    lastCoinCollected = i;
                    answeredCorrectly = false;
                    feedbackTimer = 0.0f;
                }
            }

            // Actualizar animación
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

        // Actualizar temporizador de retroalimentación
        if (feedbackTimer > 0.0f)
        {
            feedbackTimer -= deltaTime;
            if (feedbackTimer <= 0.0f && !answeredCorrectly && lastCoinCollected >= 0 && !gameOver)
            {
                coins[lastCoinCollected].active = true;
                showingQuestion = true;
                // Contar preguntas no respondidas correctamente
                int unansweredCount = 0;
                for (int q = 0; q < MAX_QUESTIONS; q++)
                {
                    if (!questionsAnswered[q])
                        unansweredCount++;
                }
                if (unansweredCount > 0)
                {
                    do
                    {
                        currentQuestion = GetRandomValue(0, MAX_QUESTIONS - 1);
                    } while (questionsAnswered[currentQuestion]);
                }
                else
                {
                    // Si todas las preguntas ya fueron respondidas correctamente, desactivar la moneda permanentemente
                    lastCoinCollected = -1; // Evitar que se reactive la moneda
                    showingQuestion = false;
                }
            }
        }

        // Salir o reiniciar
        if (IsKeyPressed(KEY_ESCAPE) && !showingQuestion && !levelCompleted)
        {
            *posJugador = player.position;
            
            UnloadTexture(coinTex);
            return GAME_PAUSE;
        }
        if (gameOver && IsKeyPressed(KEY_R))
        {
            player.position = *posJugador; // Reiniciar posición
            player.speed = 0;
            player.canJump = false;
            gameOver = false;
            for (int i = 0; i < MAX_COINS; i++)
                coins[i].active = true; // Reactivar todas las monedas
            for (int i = 0; i < MAX_QUESTIONS; i++)
                questionsAnswered[i] = false; // Reinicia el arreglo de preguntas
            *coinsCollected -= MAX_COINS;     // Restar las monedas recolectadas en este nivel
            if (*coinsCollected < 0)
                *coinsCollected = 0; // Asegurar que no baje de 0
        }
        if (gameOver && feedbackTimer <= 0.0f)
        {
            *posJugador = player.position;
            
            UnloadTexture(coinTex);
            return GAME_OVER;
        }

        // Actualizar cámara (no sigue al jugador si está en game over o nivel completado)
        if (!gameOver && !levelCompleted)
        {
            camera.target = player.position;
            camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f};
        }

        // Dibujar
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Dibujar fondo estático
        DrawTexture(fondo, 0, 0, WHITE);

        BeginMode2D(camera);

        for (int i = 0; i < MAX_ENVIRONMENT_ELEMENTS; i++)
        {
            DrawRectangleRec(envElements[i].rect, envElements[i].color);
        }

        for (int i = 0; i < MAX_COINS; i++)
        {
            if (coins[i].active)
            {
                DrawTexture(coinTex, coins[i].position.x, coins[i].position.y, WHITE);
            }
        }

        Vector2 drawPos = {player.position.x - frameWidth / 2, player.position.y - frameHeight};
        Rectangle sourceRec;
        if (currentFrame == 0)
        {
            sourceRec = (Rectangle){frameWidth * currentFrame, 0, frameWidth, frameHeight};
        }
        else
        {
            sourceRec = (Rectangle){frameWidth * currentFrame, 0, facingRight ? frameWidth : -frameWidth, frameHeight};
        }
        DrawTextureRec(jugadorTex, sourceRec, drawPos, WHITE);

        EndMode2D();

        // Mostrar pregunta si está activa
        if (showingQuestion && currentQuestion >= 0)
        {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(DARKGRAY, 0.5f));

            int textWidth = MeasureText(questions[currentQuestion].text, 40);
            DrawText(questions[currentQuestion].text, (screenWidth - textWidth) / 2, 100, 40, WHITE);

            int btnWidth = 300;
            int btnHeight = 50;
            int btnX = (screenWidth - btnWidth) / 2;
            int btnY = 200;
            int spacing = 20;

            for (int i = 0; i < 4; i++)
            {
                Rectangle btn = {(float)btnX, (float)(btnY + i * (btnHeight + spacing)), (float)btnWidth, (float)btnHeight};
                bool mouseOver = CheckCollisionPointRec(GetMousePosition(), btn);
                Color btnColor = mouseOver ? Fade(DARKBLUE, 0.8f) : Fade(BLUE, 0.7f);

                DrawRectangleRec(btn, btnColor);
                DrawRectangleLinesEx(btn, 6.0f, DARKBLUE);
                int optWidth = MeasureText(questions[currentQuestion].options[i], 20);
                DrawText(questions[currentQuestion].options[i], btnX + (btnWidth - optWidth) / 2, btn.y + (btnHeight - 20) / 2, 20, WHITE);

                if (mouseOver && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    answeredCorrectly = (i == questions[currentQuestion].correctAnswer);
                    if (answeredCorrectly)
                    {
                        questionsAnswered[currentQuestion] = true;
                    }
                    showingQuestion = false;
                    feedbackTimer = FEEDBACK_DURATION;
                }
            }
        }

        // Mostrar mensaje de retroalimentación
        if (feedbackTimer > 0.0f && !gameOver)
        {
            if (answeredCorrectly)
            {
                DrawText("¡Correcto!", (screenWidth - MeasureText("¡Correcto!", 30)) / 2, 500, 30, GREEN);
            }
            else
            {
                DrawText("Incorrecto, intenta de nuevo.", (screenWidth - MeasureText("Incorrecto, intenta de nuevo.", 30)) / 2, 500, 30, RED);
            }
        }

        // Mostrar mensaje de nivel completado con botón de continuar
        if (levelCompleted)
        {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(DARKGRAY, 0.5f));
            DrawText("¡Has completado el nivel!", (screenWidth - MeasureText("¡Has completado el nivel!", 40)) / 2, 300, 40, GREEN);
            DrawText("¡Felicidades!", (screenWidth - MeasureText("¡Felicidades!", 30)) / 2, 350, 30, WHITE);

            // Botón de Continuar
            int btnWidth = 200;
            int btnHeight = 50;
            int btnX = (screenWidth - btnWidth) / 2;
            int btnY = 420;
            Rectangle btn = {(float)btnX, (float)btnY, (float)btnWidth, (float)btnHeight};
            bool mouseOver = CheckCollisionPointRec(GetMousePosition(), btn);
            Color btnColor = mouseOver ? Fade(DARKBLUE, 0.8f) : Fade(BLUE, 0.7f);

            DrawRectangleRec(btn, btnColor);
            DrawRectangleLinesEx(btn, 6.0f, DARKBLUE);
            int textWidth = MeasureText("Continuar", 20);
            DrawText("Continuar", btnX + (btnWidth - textWidth) / 2, btnY + (btnHeight - 20) / 2, 20, WHITE);

            if (mouseOver && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                *posJugador = player.position;
                
                UnloadTexture(coinTex);
                return GAME_WON; // Regresar al menú de niveles
            }
        }

        // Mostrar mensaje de game over
        if (gameOver)
        {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(DARKGRAY, 0.5f));
            DrawText("¡Perdiste!", (screenWidth - MeasureText("¡Perdiste!", 40)) / 2, 300, 40, RED);
            DrawText("Presiona R para reiniciar", (screenWidth - MeasureText("Presiona R para reiniciar", 20)) / 2, 350, 20, WHITE);
        }

        DrawText("Presiona ESC para salir al menú", 10, 10, 20, DARKGRAY);
        DrawText(TextFormat("Monedas: %d", *coinsCollected), screenWidth - 150, 10, 20, DARKGRAY);
        EndDrawing();
    }

    *posJugador = player.position;
    
    UnloadTexture(coinTex);
    return GAME_CONTINUE;
}