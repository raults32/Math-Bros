#include <raylib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#define MAX_ENVIRONMENT_ELEMENTS 9
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
GameStatus nivel1(Vector2 *posJugador, Texture2D fondo, int *coinsCollected, Texture2D jugadorTexture, int totalFrames, Music *gameMusic, bool *musicInitialized, int *currentMusicLevel);
GameStatus nivel2(Vector2 *posJugador, Texture2D fondo, int *coinsCollected, Texture2D jugadorTexture, int totalFrames, Music *gameMusic, bool *musicInitialized, int *currentMusicLevel);
GameStatus nivel3(Vector2 *posJugador, Texture2D fondo, int *coinsCollected, Texture2D jugadorTexture, int totalFrames, Music *gameMusic, bool *musicInitialized, int *currentMusicLevel);

void DrawTiledPlatform(Texture2D texture, Rectangle platform);
void PlayCoinSound(Sound coinSound);

GameStatus nivel1(Vector2 *posJugador, Texture2D fondo, int *coinsCollected, Texture2D jugadorTexture, int totalFrames, Music *gameMusic, bool *musicInitialized, int *currentMusicLevel)
{
    Player player = {0};

    Texture2D jugadorTex = jugadorTexture;
    Texture2D coinTex = LoadTexture("src/images/coin.png");
    Texture2D platformTex = LoadTexture("src/images/platform1.png");
    Texture2D dirtTex = LoadTexture("src/images/dirt.png");

    Sound coinSound = LoadSound("src/sound/coin.mp3"); 

    // VERIFICAR SI LAS TEXTURAS SE CARGARON CORRECTAMENTE
    if (platformTex.id == 0)
    {
        printf("Error: No se pudo cargar platform1.png\n");
        // Usar un color por defecto si no se carga la textura
    }

    if (coinTex.id == 0)
    {
        printf("Error: No se pudo cargar coin.png\n");
    }

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

        {{0, 0, 1280, 720}, 0, BLANK},       // Fondo completo (no bloqueante)
                                             //   X      Y   ancho  alto blocking  color
        {{-1500, 600, 5000, 50}, 1, BLANK},  // Suelo
        {{-1000, 650, 3500, 500}, 0, BLANK}, // Plataforma visual de tierra (no bloqueante)

        {{100, 500, 200, 50}, 1, BLANK},  // Plataforma 1 - centro
        {{280, 410, 200, 50}, 1, BLANK},  // Plataforma 2 - derecha
        {{460, 320, 200, 50}, 1, BLANK},  // Plataforma 3 - derecha
        {{640, 230, 200, 50}, 1, BLANK},  // Plataforma 4 - derecha
        {{820, 140, 200, 50}, 1, BLANK},  // Plataforma 5 - derecha
        {{1000, 50, 200, 50}, 1, BLANK}}; // Plataforma 6 - derecha alta

    Coin coins[MAX_COINS] = {
        // pos    X     Y   Activo   X   Y  ancho  largo
        {{165, 440}, true, {165, 440, 30, 30}},    // moneda 1
        {{345, 350}, true, {345, 350, 30, 30}},    // moneda 2
        {{525, 260}, true, {525, 260, 30, 30}},    // moneda 3
        {{705, 170}, true, {705, 170, 30, 30}},    // moneda 4
        {{885, 80}, true, {885, 80, 30, 30}},      // moneda 5
        {{1065, -10}, true, {1065, -10, 30, 30}}}; // moneda 6

    Question questions[MAX_QUESTIONS] = {
        // Pregunta               Opciones       Posicion de respuesta correcta
        {"¿Cuánto es 7 x 7?", {"32", "42", "49", "45"}, 2},
        {"¿Cuánto es 8 x 6?", {"42", "48", "54", "56"}, 1},
        {"¿Cuánto es 5 x 10", {"35", "40", "45", "50"}, 3},
        {"¿Cuánto es 45 - 19?", {"26", "24", "28", "30"}, 0},
        {"¿Cuánto es 9 x 4?", {"32", "36", "40", "45"}, 1},
        {"¿Cuánto es 6 x 9?", {"69", "54", "40", "96"}, 1},
        {"¿Cuánto es 23 + 48?", {"69", "70", "71", "72"}, 2},
        {"¿Cuánto es 7 x 3?", {"7", "14", "28", "21"}, 3},
        {"¿Cuánto es 64 - 29?", {"33", "34", "35", "36"}, 2},
        {"¿Cuánto es 9 x 3?", {"18", "15", "27", "20"}, 2}};

    bool showingQuestion = false;
    int currentQuestion = -1;
    bool answeredCorrectly = false;
    float feedbackTimer = 0.0f;
    const float FEEDBACK_DURATION = 2.0f;
    int lastCoinCollected = -1;
    bool gameOver = false;
    bool levelCompleted = false;
    bool questionsAnswered[MAX_QUESTIONS] = {false}; // Rastrear preguntas respondidas correctamente
    bool coinQuestionAnswered = false;

    // SISTEMA DE INTENTOS
    int currentAttempts = 0;     // Intentos actuales para la pregunta
    const int MAX_ATTEMPTS = 2;  // Máximo 2 intentos por pregunta
    bool questionFailed = false; // Si falló la pregunta después de 2 intentos

    int screenWidth = 1280;
    int screenHeight = 720;

    Camera2D camera = {0};
    camera.target = player.position;
    camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 1.5f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    float minCameraX = -1000 + screenWidth / 2.0f; // Left edge of ground + half screen width
    float maxCameraX = 2000 - screenWidth / 2.0f;  // Right edge of ground - half screen width

    const float GRAVITY = 500;
    const float PLAYER_JUMP_SPD = 320.0f;
    const float PLAYER_HOR_SPD = 225.0f;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();
        UpdateMusicStream(*gameMusic);

        // Verificar si se han recolectado todas las monedas
        if (!levelCompleted && coinQuestionAnswered)
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
        // Condicion del estado del juego
        if (!showingQuestion && feedbackTimer <= 0.0f && !gameOver && !levelCompleted)
        {
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

            if (IsKeyPressed(KEY_SPACE) && player.canJump)
            {
                player.speed = -PLAYER_JUMP_SPD;
                player.canJump = false;
            }

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

            if (player.position.y > screenHeight + 100)
            {
                gameOver = true;
                feedbackTimer = FEEDBACK_DURATION;
            }

            Rectangle playerRect = {player.position.x - frameWidth / 2, player.position.y - frameHeight, frameWidth, frameHeight};
            for (int i = 0; i < MAX_COINS; i++)
            {
                if (coins[i].active && CheckCollisionRecs(playerRect, coins[i].rect))
                {
                    coins[i].active = false;
                    (*coinsCollected)++;
                    showingQuestion = true;

                    do
                    {
                        currentQuestion = GetRandomValue(0, MAX_QUESTIONS - 1);
                    } while (questionsAnswered[currentQuestion]);

                    lastCoinCollected = i;
                    answeredCorrectly = false;
                    feedbackTimer = 0.0f;
                    currentAttempts = 0;
                    questionFailed = false;
                    coinQuestionAnswered = false;
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

        // Maneja el tiempo de espera tras responder una pregunta antes de continuar el juego.
        if (feedbackTimer > 0.0f)
        {
            feedbackTimer -= deltaTime;
            if (feedbackTimer <= 0.0f && !gameOver)
            {
                if (answeredCorrectly || coinQuestionAnswered)
                {
                    showingQuestion = false;
                    lastCoinCollected = -1;
                    coinQuestionAnswered = true;
                }
                else if (questionFailed)
                {
                    showingQuestion = false;
                    lastCoinCollected = -1;
                    (*coinsCollected)--;
                    if (*coinsCollected < 0)
                        *coinsCollected = 0;
                    coinQuestionAnswered = false;
                }
            }
        }

        if (IsKeyPressed(KEY_ESCAPE) && !showingQuestion && !levelCompleted)
        {
            *posJugador = player.position;
            UnloadTexture(coinTex);
            UnloadTexture(platformTex); // No olvides liberar la textura
            UnloadSound(coinSound); // Unload sound
            UnloadTexture(dirtTex);
            return GAME_PAUSE;
        }

        // REINICIAR EL NIVEL CUANDO CAE
        if (gameOver && IsKeyPressed(KEY_R))
        {
            player.position = *posJugador;
            player.speed = 0;
            player.canJump = false;
            gameOver = false;
            for (int i = 0; i < MAX_COINS; i++)
                coins[i].active = true;
            for (int i = 0; i < MAX_QUESTIONS; i++)
                questionsAnswered[i] = false;
            *coinsCollected -= MAX_COINS;
            if (*coinsCollected < 0)
                *coinsCollected = 0;
            currentAttempts = 0;
            questionFailed = false;
            coinQuestionAnswered = false;
        }
        if (gameOver && feedbackTimer <= 0.0f)
        {
            *posJugador = player.position;
            UnloadTexture(coinTex);
            UnloadTexture(platformTex); // No olvides liberar la textura
            UnloadSound(coinSound); // Unload sound
            UnloadTexture(dirtTex);
            return GAME_OVER;
        }

        if (!gameOver && !levelCompleted)
        {
            camera.target = player.position;
            // Clamp camera target to level boundaries
            camera.target.x = fmaxf(minCameraX, fminf(maxCameraX, camera.target.x));
            camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 1.5f};
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(fondo, 0, 0, WHITE);

        BeginMode2D(camera);
        // Dibujar plataforma visual de tierra (index 1)
        DrawTiledPlatform(dirtTex, envElements[2].rect);

        // Luego tierra extra
        for (int i = 2; i < MAX_ENVIRONMENT_ELEMENTS; i++)
        {
            if (envElements[i].blocking)
            {
                DrawTiledPlatform(dirtTex, envElements[i].rect);
            }
        }

        // Luego plataforma de pasto encima de las de tierra
        for (int i = 1; i < MAX_ENVIRONMENT_ELEMENTS; i++)
        {
            if (envElements[i].blocking)
            {
                DrawTiledPlatform(platformTex, envElements[i].rect);
            }
        }

        // DIBUJAR MONEDAS
        for (int i = 0; i < MAX_COINS; i++)
        {
            if (coins[i].active)
            {
                if (coinTex.id != 0)
                {
                    DrawTexture(coinTex, coins[i].position.x, coins[i].position.y, WHITE);
                }
                else
                {
                    // Si no se carga la textura de moneda, dibujar círculo amarillo
                    DrawCircle(coins[i].position.x + 15, coins[i].position.y + 15, 15, YELLOW);
                }
            }
        }

        // DIBUJAR JUGADOR
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

        // Muestra la pregunta actual y sus opciones si esta activa, y
        // gestiona las respuestas del jugador.

        if (showingQuestion && currentQuestion >= 0)
        {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(DARKGRAY, 0.5f));
            int textWidth = MeasureText(questions[currentQuestion].text, 40);
            DrawText(questions[currentQuestion].text, (screenWidth - textWidth) / 2, 100, 40, WHITE);

            char attemptInfo[50];
            sprintf(attemptInfo, "Intentos restantes: %d", MAX_ATTEMPTS - currentAttempts);
            int infoWidth = MeasureText(attemptInfo, 20);
            DrawText(attemptInfo, (screenWidth - infoWidth) / 2, 150, 20, YELLOW);

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
                        coinQuestionAnswered = true;
                        currentAttempts = 0;
                        questionFailed = false;
                        showingQuestion = false;
                        feedbackTimer = FEEDBACK_DURATION;
                        PlayCoinSound(coinSound);
                    }
                    else
                    {
                        currentAttempts++;
                        if (currentAttempts >= MAX_ATTEMPTS)
                        {
                            questionFailed = true;
                            showingQuestion = false;
                            feedbackTimer = FEEDBACK_DURATION;
                        }
                        else
                        {
                            // Solo muestra el feedback y mantiene la pregunta activa
                            feedbackTimer = FEEDBACK_DURATION;
                        }
                    }
                }
            }
        }

        if (feedbackTimer > 0.0f && !gameOver)
        {
            if (answeredCorrectly)
            {
                DrawText("¡Correcto!", (screenWidth - MeasureText("¡Correcto!", 30)) / 2, 500, 30, GREEN);
            }
            else if (questionFailed)
            {
                DrawText("¡Oh no, perdiste la moneda!", (screenWidth - MeasureText("¡Oh no, perdiste la moneda!", 30)) / 2, 500, 30, ORANGE);
            }
            else
            {
                char attemptMsg[100];
                sprintf(attemptMsg, "Incorrecto. Intento %d de %d", currentAttempts, MAX_ATTEMPTS);
                DrawText(attemptMsg, (screenWidth - MeasureText(attemptMsg, 30)) / 2, 500, 30, RED);
            }
        }

        if (levelCompleted)
        {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(DARKGRAY, 0.5f));
            DrawText("¡Has completado el nivel!", (screenWidth - MeasureText("¡Has completado el nivel!", 40)) / 2, 300, 40, GREEN);
            DrawText("¡Felicidades!", (screenWidth - MeasureText("¡Felicidades!", 30)) / 2, 350, 30, WHITE);

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
                UnloadTexture(platformTex); // No olvides liberar la textura
                UnloadSound(coinSound); // Unload sound
                UnloadTexture(dirtTex);
                return GAME_WON;
            }
        }

        if (gameOver)
        {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(DARKGRAY, 0.5f));
            DrawText("¡Perdiste!", (screenWidth - MeasureText("¡Perdiste!", 40)) / 2, 300, 40, RED);
            DrawText("Presiona R para reiniciar", (screenWidth - MeasureText("Presiona R para reiniciar", 20)) / 2, 350, 20, WHITE);
        }

        DrawText("Presiona ESC para salir al menú", 10, 10, 20, DARKGRAY);
        DrawText(TextFormat("Monedas: %d", *coinsCollected), screenWidth - 150, 10, 20, YELLOW);
        EndDrawing();
    }

    *posJugador = player.position;
    UnloadTexture(platformTex);
    UnloadTexture(coinTex);
    UnloadTexture(dirtTex);
    return GAME_CONTINUE;
}

GameStatus nivel2(Vector2 *posJugador, Texture2D fondo, int *coinsCollected, Texture2D jugadorTexture, int totalFrames, Music *gameMusic, bool *musicInitialized, int *currentMusicLevel)
{
    Player player = {0};

    Texture2D jugadorTex = jugadorTexture;
    Texture2D coinTex = LoadTexture("src/images/coin.png");
    Texture2D platformTex = LoadTexture("src/images/platform2.png");
    Texture2D dirtTex = LoadTexture("src/images/dirt2.png");
    Sound coinSound = LoadSound("src/sound/coin.mp3"); 


    // VERIFICAR SI LAS TEXTURAS SE CARGARON CORRECTAMENTE
    if (platformTex.id == 0)
    {
        printf("Error: No se pudo cargar platform1.png\n");
        // Usar un color por defecto si no se carga la textura
    }

    if (coinTex.id == 0)
    {
        printf("Error: No se pudo cargar coin.png\n");
    }

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
    
        {{0, 0, 1280, 720}, 0, BLANK}, // Fondo completo (no bloqueante)
     //   X      Y   ancho  alto blocking  color
        {{-1500, 600, 5000, 50}, 1, BLANK},  // Suelo
        {{-1000, 650, 3500, 500}, 0, BLANK}, // Plataforma visual de tierra (no bloqueante)

        {{150, 500, 200, 50}, 1, BLANK},  // Plataforma 1 - BLANK para que no se vea el rectángulo
        {{450, 400, 200, 50}, 1, BLANK},  // Plataforma 2 - centro
        {{750, 300, 200, 50}, 1, BLANK},  // Plataforma 3 - derecha
        {{350, 250, 200, 50}, 1, BLANK},  // Plataforma 4 - centro-izquierda
        {{650, 150, 200, 50}, 1, BLANK},  // Plataforma 5 - centro-derecha
        {{950, 120, 200, 50}, 1, BLANK}}; // Plataforma 6 - derecha alta

    Coin coins[MAX_COINS] = {
        // pos    X     Y   Activo   X   Y  ancho  largo
        {{210, 440}, true, {210, 440, 30, 30}},  // Plataforma 1
        {{510, 340}, true, {510, 340, 30, 30}},  // Plataforma 2
        {{810, 240}, true, {810, 240, 30, 30}},  // Plataforma 3
        {{410, 190}, true, {410, 190, 30, 30}},  // Plataforma 4
        {{710, 90}, true, {710, 80, 30, 30}},    // Plataforma 5
        {{1010, 60}, true, {1010, 60, 30, 30}}}; // Plataforma 6

    Question questions[MAX_QUESTIONS] = {
        // Pregunta               Opciones       Posicion de respuesta correcta
        {"¿Cuánto es 140 + 23?", {"163", "124", "160", "180"}, 0},
        {"¿Cuánto es 312 + 50?", {"560", "480", "362", "563"}, 2},
        {"¿Cuánto es 56 ÷ 7?", {"7", "8", "6", "9"}, 1},
        {"¿Cuánto es 45 - 19?", {"26", "24", "28", "30"}, 0},
        {"¿Cuánto es 9 x 4?", {"32", "36", "40", "45"}, 1},
        {"¿Cuánto es 72 ÷ 9?", {"7", "8", "9", "10"}, 1},
        {"¿Cuánto es 23 + 48?", {"69", "70", "71", "72"}, 2},
        {"¿Cuánto es 7 x 8?", {"54", "56", "58", "60"}, 1},
        {"¿Cuánto es 64 - 30?", {"33", "34", "35", "36"}, 1},
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
    bool coinQuestionAnswered = false;

    // SISTEMA DE INTENTOS
    int currentAttempts = 0;     // Intentos actuales para la pregunta
    const int MAX_ATTEMPTS = 2;  // Máximo 2 intentos por pregunta
    bool questionFailed = false; // Si falló la pregunta después de 2 intentos

    int screenWidth = 1280;
    int screenHeight = 720;

    Camera2D camera = {0};
    camera.target = player.position;
    camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 1.5f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    float minCameraX = -1000 + screenWidth / 2.0f; // Left edge of ground + half screen width
    float maxCameraX = 2000 - screenWidth / 2.0f;  // Right edge of ground - half screen width

    const float GRAVITY = 500;
    const float PLAYER_JUMP_SPD = 320.0f;
    const float PLAYER_HOR_SPD = 225.0f;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();
        UpdateMusicStream(*gameMusic);

        // Verificar si se han recolectado todas las monedas
        if (!levelCompleted && coinQuestionAnswered)
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
        // Condicion del estado del juego
        if (!showingQuestion && feedbackTimer <= 0.0f && !gameOver && !levelCompleted)
        {
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

            if (IsKeyPressed(KEY_SPACE) && player.canJump)
            {
                player.speed = -PLAYER_JUMP_SPD;
                player.canJump = false;
            }

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

            if (player.position.y > screenHeight + 100)
            {
                gameOver = true;
                feedbackTimer = FEEDBACK_DURATION;
            }

            Rectangle playerRect = {player.position.x - frameWidth / 2, player.position.y - frameHeight, frameWidth, frameHeight};
            for (int i = 0; i < MAX_COINS; i++)
            {
                if (coins[i].active && CheckCollisionRecs(playerRect, coins[i].rect))
                {
                    coins[i].active = false;
                    (*coinsCollected)++;
                    showingQuestion = true;

                    do
                    {
                        currentQuestion = GetRandomValue(0, MAX_QUESTIONS - 1);
                    } while (questionsAnswered[currentQuestion]);

                    lastCoinCollected = i;
                    answeredCorrectly = false;
                    feedbackTimer = 0.0f;
                    currentAttempts = 0;
                    questionFailed = false;
                    coinQuestionAnswered = false;
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

        // Maneja el tiempo de espera tras responder una pregunta antes de continuar el juego.
        if (feedbackTimer > 0.0f)
        {
            feedbackTimer -= deltaTime;
            if (feedbackTimer <= 0.0f && !gameOver)
            {
                if (answeredCorrectly || coinQuestionAnswered)
                {
                    showingQuestion = false;
                    lastCoinCollected = -1;
                    coinQuestionAnswered = true;
                }
                else if (questionFailed)
                {
                    showingQuestion = false;
                    lastCoinCollected = -1;
                    (*coinsCollected)--;
                    if (*coinsCollected < 0)
                        *coinsCollected = 0;
                    coinQuestionAnswered = false;
                }
            }
        }

        if (IsKeyPressed(KEY_ESCAPE) && !showingQuestion && !levelCompleted)
        {
            *posJugador = player.position;
            UnloadTexture(coinTex);
            UnloadTexture(platformTex); // No olvides liberar la textura
            UnloadSound(coinSound); // Unload sound
            UnloadTexture(dirtTex);
            return GAME_PAUSE;
        }

        // REINICIAR EL NIVEL CUANDO CAE
        if (gameOver && IsKeyPressed(KEY_R))
        {
            player.position = *posJugador;
            player.speed = 0;
            player.canJump = false;
            gameOver = false;
            for (int i = 0; i < MAX_COINS; i++)
                coins[i].active = true;
            for (int i = 0; i < MAX_QUESTIONS; i++)
                questionsAnswered[i] = false;
            *coinsCollected -= MAX_COINS;
            if (*coinsCollected < 0)
                *coinsCollected = 0;
            currentAttempts = 0;
            questionFailed = false;
            coinQuestionAnswered = false;
        }
        if (gameOver && feedbackTimer <= 0.0f)
        {
            *posJugador = player.position;
            UnloadTexture(coinTex);
            UnloadTexture(platformTex); // No olvides liberar la textura
            UnloadSound(coinSound); // Unload sound
            UnloadTexture(dirtTex);
            return GAME_OVER;
        }

         if (!gameOver && !levelCompleted)
        {
            camera.target = player.position;
            // Clamp camera target to level boundaries
            camera.target.x = fmaxf(minCameraX, fminf(maxCameraX, camera.target.x));
            camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 1.5f};
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(fondo, 0, 0, WHITE);

        BeginMode2D(camera);
        // Dibujar plataforma visual de tierra (index 1)
        DrawTiledPlatform(dirtTex, envElements[2].rect);

        // Luego tierra extra
        for (int i = 2; i < MAX_ENVIRONMENT_ELEMENTS; i++)
        {
            if (envElements[i].blocking)
            {
                DrawTiledPlatform(dirtTex, envElements[i].rect);
            }
        }

        // Luego plataforma de pasto encima de las de tierra
        for (int i = 1; i < MAX_ENVIRONMENT_ELEMENTS; i++)
        {
            if (envElements[i].blocking)
            {
                DrawTiledPlatform(platformTex, envElements[i].rect);
            }
        }

        // DIBUJAR MONEDAS
        for (int i = 0; i < MAX_COINS; i++)
        {
            if (coins[i].active)
            {
                if (coinTex.id != 0)
                {
                    DrawTexture(coinTex, coins[i].position.x, coins[i].position.y, WHITE);
                }
                else
                {
                    // Si no se carga la textura de moneda, dibujar círculo amarillo
                    DrawCircle(coins[i].position.x + 15, coins[i].position.y + 15, 15, YELLOW);
                }
            }
        }

        // DIBUJAR JUGADOR
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

        // Muestra la pregunta actual y sus opciones si esta activa, y
        // gestiona las respuestas del jugador.

        if (showingQuestion && currentQuestion >= 0)
        {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(DARKGRAY, 0.5f));
            int textWidth = MeasureText(questions[currentQuestion].text, 40);
            DrawText(questions[currentQuestion].text, (screenWidth - textWidth) / 2, 100, 40, WHITE);

            char attemptInfo[50];
            sprintf(attemptInfo, "Intentos restantes: %d", MAX_ATTEMPTS - currentAttempts);
            int infoWidth = MeasureText(attemptInfo, 20);
            DrawText(attemptInfo, (screenWidth - infoWidth) / 2, 150, 20, YELLOW);

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
                        coinQuestionAnswered = true;
                        currentAttempts = 0;
                        questionFailed = false;
                        showingQuestion = false;
                        feedbackTimer = FEEDBACK_DURATION;
                        PlayCoinSound(coinSound);

                    }
                    else
                    {
                        currentAttempts++;
                        if (currentAttempts >= MAX_ATTEMPTS)
                        {
                            questionFailed = true;
                            showingQuestion = false;
                            feedbackTimer = FEEDBACK_DURATION;
                        }
                        else
                        {
                            // Solo muestra el feedback y mantiene la pregunta activa
                            feedbackTimer = FEEDBACK_DURATION;
                        }
                    }
                }
            }
        }

        if (feedbackTimer > 0.0f && !gameOver)
        {
            if (answeredCorrectly)
            {
                DrawText("¡Correcto!", (screenWidth - MeasureText("¡Correcto!", 30)) / 2, 500, 30, GREEN);
            }
            else if (questionFailed)
            {
                DrawText("¡Oh no, perdiste la moneda!", (screenWidth - MeasureText("¡Oh no, perdiste la moneda!", 30)) / 2, 500, 30, ORANGE);
            }
            else
            {
                char attemptMsg[100];
                sprintf(attemptMsg, "Incorrecto. Intento %d de %d", currentAttempts, MAX_ATTEMPTS);
                DrawText(attemptMsg, (screenWidth - MeasureText(attemptMsg, 30)) / 2, 500, 30, RED);
            }
        }

        if (levelCompleted)
        {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(DARKGRAY, 0.5f));
            DrawText("¡Has completado el nivel!", (screenWidth - MeasureText("¡Has completado el nivel!", 40)) / 2, 300, 40, GREEN);
            DrawText("¡Felicidades!", (screenWidth - MeasureText("¡Felicidades!", 30)) / 2, 350, 30, WHITE);

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
                UnloadTexture(platformTex); // No olvides liberar la textura
                UnloadSound(coinSound); // Unload sound
                UnloadTexture(dirtTex);
                return GAME_WON;
            }
        }

        if (gameOver)
        {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(DARKGRAY, 0.5f));
            DrawText("¡Perdiste!", (screenWidth - MeasureText("¡Perdiste!", 40)) / 2, 300, 40, RED);
            DrawText("Presiona R para reiniciar", (screenWidth - MeasureText("Presiona R para reiniciar", 20)) / 2, 350, 20, WHITE);
        }

        DrawText("Presiona ESC para salir al menú", 10, 10, 20, DARKGRAY);
        DrawText(TextFormat("Monedas: %d", *coinsCollected), screenWidth - 150, 10, 20, YELLOW);
        EndDrawing();
    }

    *posJugador = player.position;
    UnloadTexture(platformTex);
    UnloadTexture(coinTex);
    UnloadTexture(dirtTex);
    return GAME_CONTINUE;
}

GameStatus nivel3(Vector2 *posJugador, Texture2D fondo, int *coinsCollected, Texture2D jugadorTexture, int totalFrames, Music *gameMusic, bool *musicInitialized, int *currentMusicLevel)
{
    Player player = {0};

    Texture2D jugadorTex = jugadorTexture;
    Texture2D coinTex = LoadTexture("src/images/coin.png");
    Texture2D platformTex = LoadTexture("src/images/platform3.png");
    Texture2D dirtTex = LoadTexture("src/images/dirt3.png");
    Sound coinSound = LoadSound("src/sound/coin.mp3"); 

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
    
        {{0, 0, 1280, 720}, 0, BLANK}, // Fondo completo (no bloqueante)
     //   X      Y   ancho  alto blocking  color
        {{-1500, 600, 5000, 50}, 1, BLANK},  // Suelo
        {{-1000, 650, 3500, 500}, 0, BLANK}, // Plataforma visual de tierra (no bloqueante)

        {{150, 500, 80, 30}, 1, BLANK},    // Plataforma 1
        {{440, 420, 80, 30}, 1, BLANK},    // Plataforma 2
        {{250, 340, 80, 30}, 1, BLANK},     // Plataforma 3
        {{420, 260, 80, 30}, 1, BLANK},     // Plataforma 4
        {{180, 180, 80, 30}, 1, BLANK},    // Plataforma 5
        {{380, 100, 80, 30}, 1, BLANK}};   // Plataforma 6

    Coin coins[MAX_COINS] = {
 // pos    X     Y   Activo   X   Y  ancho  largo
        {{160, 440}, true, {165, 440, 30, 30}},  // Platforma 1 
        {{450, 360}, true, {465, 360, 30, 30}},  // Platforma 2 
        {{255, 280}, true, {255, 280, 30, 30}},  // Platforma 3 
        {{425, 200}, true, {425, 200, 30, 30}},  // Platforma 4 
        {{195, 120}, true, {195, 120, 30, 30}},  // Platforma 5
        {{400, 40}, true, {400, 40, 30, 30}}};   // Platforma 6 

    Question questions[MAX_QUESTIONS] = {
        // Pregunta               Opciones       Posicion de respuesta correcta
        {"¿Cuánto es 145 + 145?", {"413", "290", "323", "433"}, 1},
        {"¿Cuánto es 16 x 4?", {"56", "64", "48", "72"}, 1},
        {"¿Cuánto es 25 x 3?", {"25", "50", "75", "100"}, 2},
        {"¿Cuánto es 532 - 200?", {"254", "264", "332", "244"}, 2},
        {"¿Cuánto es 23 x 3?", {"66", "59", "69", "72"}, 2},
        {"¿Cuánto es 89 + 120?", {"209", "194", "180", "141"}, 0},
        {"¿Cuánto es 300 + 285?", {"622", "632", "585", "642"}, 2},
        {"¿Cuánto es 18 x 2?", {"30", "36", "25", "40"}, 1},
        {"¿Cuánto es 700 - 350?", {"350", "437", "447", "337"}, 0},
        {"¿Cuánto es 108 - 9 + 3?", {"90", "93", "102", "99"}, 2}};

    bool showingQuestion = false;
    int currentQuestion = -1;
    bool answeredCorrectly = false;
    float feedbackTimer = 0.0f;
    const float FEEDBACK_DURATION = 2.0f;
    int lastCoinCollected = -1;
    bool gameOver = false;
    bool levelCompleted = false;
    bool questionsAnswered[MAX_QUESTIONS] = {false}; // Rastrear preguntas respondidas correctamente
    bool coinQuestionAnswered = false;

    // SISTEMA DE INTENTOS
    int currentAttempts = 0;     // Intentos actuales para la pregunta
    const int MAX_ATTEMPTS = 2;  // Máximo 2 intentos por pregunta
    bool questionFailed = false; // Si falló la pregunta después de 2 intentos

    int screenWidth = 1280;
    int screenHeight = 720;

    Camera2D camera = {0};
    camera.target = player.position;
    camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 1.5f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    float minCameraX = -1000 + screenWidth / 2.0f; // Left edge of ground + half screen width
    float maxCameraX = 2000 - screenWidth / 2.0f;  // Right edge of ground - half screen width

    const float GRAVITY = 500;
    const float PLAYER_JUMP_SPD = 320.0f;
    const float PLAYER_HOR_SPD = 225.0f;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();
        UpdateMusicStream(*gameMusic);

        // Verificar si se han recolectado todas las monedas
        if (!levelCompleted && coinQuestionAnswered)
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
        // Condicion del estado del juego
        if (!showingQuestion && feedbackTimer <= 0.0f && !gameOver && !levelCompleted)
        {
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

            if (IsKeyPressed(KEY_SPACE) && player.canJump)
            {
                player.speed = -PLAYER_JUMP_SPD;
                player.canJump = false;
            }

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

            if (player.position.y > screenHeight + 100)
            {
                gameOver = true;
                feedbackTimer = FEEDBACK_DURATION;
            }

            Rectangle playerRect = {player.position.x - frameWidth / 2, player.position.y - frameHeight, frameWidth, frameHeight};
            for (int i = 0; i < MAX_COINS; i++)
            {
                if (coins[i].active && CheckCollisionRecs(playerRect, coins[i].rect))
                {
                    coins[i].active = false;
                    (*coinsCollected)++;
                    showingQuestion = true;

                    do
                    {
                        currentQuestion = GetRandomValue(0, MAX_QUESTIONS - 1);
                    } while (questionsAnswered[currentQuestion]);

                    lastCoinCollected = i;
                    answeredCorrectly = false;
                    feedbackTimer = 0.0f;
                    currentAttempts = 0;
                    questionFailed = false;
                    coinQuestionAnswered = false;
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

        // Maneja el tiempo de espera tras responder una pregunta antes de continuar el juego.
        if (feedbackTimer > 0.0f)
        {
            feedbackTimer -= deltaTime;
            if (feedbackTimer <= 0.0f && !gameOver)
            {
                if (answeredCorrectly || coinQuestionAnswered)
                {
                    showingQuestion = false;
                    lastCoinCollected = -1;
                    coinQuestionAnswered = true;
                }
                else if (questionFailed)
                {
                    showingQuestion = false;
                    lastCoinCollected = -1;
                    (*coinsCollected)--;
                    if (*coinsCollected < 0)
                        *coinsCollected = 0;
                    coinQuestionAnswered = false;
                }
            }
        }

        if (IsKeyPressed(KEY_ESCAPE) && !showingQuestion && !levelCompleted)
        {
            *posJugador = player.position;
            UnloadTexture(coinTex);
            UnloadTexture(platformTex); // No olvides liberar la textura
            UnloadSound(coinSound); // Unload sound
            UnloadTexture(dirtTex);
            return GAME_PAUSE;
        }

        // REINICIAR EL NIVEL CUANDO CAE
        if (gameOver && IsKeyPressed(KEY_R))
        {
            player.position = *posJugador;
            player.speed = 0;
            player.canJump = false;
            gameOver = false;
            for (int i = 0; i < MAX_COINS; i++)
                coins[i].active = true;
            for (int i = 0; i < MAX_QUESTIONS; i++)
                questionsAnswered[i] = false;
            *coinsCollected -= MAX_COINS;
            if (*coinsCollected < 0)
                *coinsCollected = 0;
            currentAttempts = 0;
            questionFailed = false;
            coinQuestionAnswered = false;
        }
        if (gameOver && feedbackTimer <= 0.0f)
        {
            *posJugador = player.position;
            UnloadTexture(coinTex);
            UnloadTexture(platformTex); // No olvides liberar la textura
            UnloadSound(coinSound); // Unload sound
            UnloadTexture(dirtTex);
            return GAME_OVER;
        }

        if (!gameOver && !levelCompleted)
        {
            camera.target = player.position;
            // Clamp camera target to level boundaries
            camera.target.x = fmaxf(minCameraX, fminf(maxCameraX, camera.target.x));
            camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 1.5f};
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(fondo, 0, 0, WHITE);

        BeginMode2D(camera);
        // Dibujar plataforma visual de tierra (index 1)
        DrawTiledPlatform(dirtTex, envElements[2].rect);

        // Luego tierra extra
        for (int i = 2; i < MAX_ENVIRONMENT_ELEMENTS; i++)
        {
            if (envElements[i].blocking)
            {
                DrawTiledPlatform(dirtTex, envElements[i].rect);
            }
        }

        // Luego plataforma de pasto encima de las de tierra
        for (int i = 1; i < MAX_ENVIRONMENT_ELEMENTS; i++)
        {
            if (envElements[i].blocking)
            {
                DrawTiledPlatform(platformTex, envElements[i].rect);
            }
        }

        // DIBUJAR MONEDAS
        for (int i = 0; i < MAX_COINS; i++)
        {
            if (coins[i].active)
            {
                if (coinTex.id != 0)
                {
                    DrawTexture(coinTex, coins[i].position.x, coins[i].position.y, WHITE);
                }
                else
                {
                    // Si no se carga la textura de moneda, dibujar círculo amarillo
                    DrawCircle(coins[i].position.x + 15, coins[i].position.y + 15, 15, YELLOW);
                }
            }
        }

        // DIBUJAR JUGADOR
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

        // Muestra la pregunta actual y sus opciones si esta activa, y
        // gestiona las respuestas del jugador.

        if (showingQuestion && currentQuestion >= 0)
        {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(DARKGRAY, 0.5f));
            int textWidth = MeasureText(questions[currentQuestion].text, 40);
            DrawText(questions[currentQuestion].text, (screenWidth - textWidth) / 2, 100, 40, WHITE);

            char attemptInfo[50];
            sprintf(attemptInfo, "Intentos restantes: %d", MAX_ATTEMPTS - currentAttempts);
            int infoWidth = MeasureText(attemptInfo, 20);
            DrawText(attemptInfo, (screenWidth - infoWidth) / 2, 150, 20, YELLOW);

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
                        coinQuestionAnswered = true;
                        currentAttempts = 0;
                        questionFailed = false;
                        showingQuestion = false;
                        feedbackTimer = FEEDBACK_DURATION;
                        PlayCoinSound(coinSound);
                    }
                    else
                    {
                        currentAttempts++;
                        if (currentAttempts >= MAX_ATTEMPTS)
                        {
                            questionFailed = true;
                            showingQuestion = false;
                            feedbackTimer = FEEDBACK_DURATION;
                        }
                        else
                        {
                            // Solo muestra el feedback y mantiene la pregunta activa
                            feedbackTimer = FEEDBACK_DURATION;
                        }
                    }
                }
            }
        }

        if (feedbackTimer > 0.0f && !gameOver)
        {
            if (answeredCorrectly)
            {
                DrawText("¡Correcto!", (screenWidth - MeasureText("¡Correcto!", 30)) / 2, 500, 30, GREEN);
            }
            else if (questionFailed)
            {
                DrawText("¡Oh no, perdiste la moneda!", (screenWidth - MeasureText("¡Oh no, perdiste la moneda!", 30)) / 2, 500, 30, ORANGE);
            }
            else
            {
                char attemptMsg[100];
                sprintf(attemptMsg, "Incorrecto. Intento %d de %d", currentAttempts, MAX_ATTEMPTS);
                DrawText(attemptMsg, (screenWidth - MeasureText(attemptMsg, 30)) / 2, 500, 30, RED);
            }
        }

        if (levelCompleted)
        {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(DARKGRAY, 0.5f));
            DrawText("¡Has completado el nivel!", (screenWidth - MeasureText("¡Has completado el nivel!", 40)) / 2, 300, 40, GREEN);
            DrawText("¡Felicidades!", (screenWidth - MeasureText("¡Felicidades!", 30)) / 2, 350, 30, WHITE);

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
                UnloadTexture(platformTex); // No olvides liberar la textura
                UnloadTexture(dirtTex);
                return GAME_WON;
            }
        }

        if (gameOver)
        {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(DARKGRAY, 0.5f));
            DrawText("¡Perdiste!", (screenWidth - MeasureText("¡Perdiste!", 40)) / 2, 300, 40, RED);
            DrawText("Presiona R para reiniciar", (screenWidth - MeasureText("Presiona R para reiniciar", 20)) / 2, 350, 20, WHITE);
        }

        DrawText("Presiona ESC para salir al menú", 10, 10, 20, DARKGRAY);
        DrawText(TextFormat("Monedas: %d", *coinsCollected), screenWidth - 150, 10, 20, YELLOW);
        EndDrawing();
    }

    *posJugador = player.position;
    UnloadTexture(platformTex);
    UnloadTexture(coinTex);
    UnloadSound(coinSound); // Unload sound
    UnloadTexture(dirtTex);
    return GAME_CONTINUE;
}

void DrawTiledPlatform(Texture2D texture, Rectangle platform)
{
    if (texture.id == 0)
    {
        DrawRectangleRec(platform, GRAY);
        return;
    }

    // Tamaño de cada bloque/tile
    float tileWidth = texture.width;
    float tileHeight = texture.height;

    // Dibujar tiles completos
    for (float x = platform.x; x < platform.x + platform.width; x += tileWidth)
    {
        for (float y = platform.y; y < platform.y + platform.height; y += tileHeight)
        {
            // Calcular el tamaño del tile actual (puede ser más pequeño en los bordes)
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
        PlaySound(coinSound);
        SetSoundVolume(coinSound, 0.1f);

    }
}



