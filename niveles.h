#include <raylib.h>

#define MAX_ENVIRONMENT_ELEMENTS 5

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

// Al inicio
typedef enum
{
    GAME_CONTINUE,
    GAME_PAUSE
} GameStatus;

// Y cambia la firma:
GameStatus runGame(Vector2 *posJugador, Texture2D fondo);

GameStatus runGame(Vector2 *posJugador, Texture2D fondo)
{
    Player player = {0};

    Texture2D jugadorTex = LoadTexture("src/images/spritezorro1.png"); // Carga la textura del jugador

    int totalFrames = 4; // Ajustado a 4 para coincidir con los 4 frames del sprite
    int currentFrame = 0;
    float frameWidth = jugadorTex.width / totalFrames;
    float frameHeight = jugadorTex.height;
    float frameTime = 0.17f; // Reducido de 0.1f a 0.05f para animación más fluida
    float frameCounter = 0.0f;
    bool facingRight = true;

    player.position = *posJugador;

    player.speed = 0;
    player.canJump = false;

    EnvElement envElements[MAX_ENVIRONMENT_ELEMENTS] = {
        {{0, 0, 1000, 400}, 0, BLANK},
        {{0, 400, 1000, 200}, 1, GRAY},
        {{300, 200, 400, 10}, 1, GRAY},
        {{250, 300, 100, 10}, 1, GRAY},
        {{650, 300, 100, 10}, 1, GRAY}};

    int screenWidth = 1280;
    int screenHeight = 720;

    Camera2D camera = {0};
    camera.target = player.position;
    camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    const float GRAVITY = 500;            // Gravedad que afecta al jugador
    const float PLAYER_JUMP_SPD = 320.0f; // Velocidad de salto del jugador
    const float PLAYER_HOR_SPD = 225.0f;  // Velocidad horizontal del jugador

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

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
            player.canJump = true;

        // Salir del juego y volver al menú con ESC
        if (IsKeyPressed(KEY_ESCAPE))
        {
            *posJugador = player.position;
            return GAME_PAUSE;
        }

        // Actualizar cámara
        camera.target = player.position;
        camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f};
        camera.zoom += ((float)GetMouseWheelMove() * 0.05f);
        if (camera.zoom > 3.0f)
            camera.zoom = 3.0f;
        else if (camera.zoom < 0.25f)
            camera.zoom = 0.25f;

        // Actualizar animación
        bool isMoving = IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A) || IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D);

        if (isMoving)
        {
            if (currentFrame == 0)
            {
                currentFrame = 1; // Fuerza a cambiar al primer frame de movimiento
                frameCounter = 0; // Reinicia el contador para que no se salte un frame
            }
            else
            {
                frameCounter += deltaTime;
                if (frameCounter >= frameTime)
                {
                    currentFrame = 1 + ((currentFrame - 1 + 1) % 3); // Cicla 1-3
                    frameCounter = 0;
                }
            }
        }
        else
        {
            currentFrame = 0; // Parado
        }

        // Dibujar
        BeginDrawing();
        ClearBackground(RAYWHITE);

      
        DrawTexture(fondo, 0, 0, WHITE); 

        BeginMode2D(camera);

      
        for (int i = 0; i < MAX_ENVIRONMENT_ELEMENTS; i++)
        {
            DrawRectangleRec(envElements[i].rect, envElements[i].color);
        }
       
        Vector2 drawPos = {player.position.x - frameWidth / 2, player.position.y - frameHeight};
        Rectangle sourceRec;

        if (currentFrame == 0) 
        {
           
            if (facingRight)
            {
                sourceRec = (Rectangle){frameWidth * currentFrame, 0, frameWidth, frameHeight};
            }
            else
            {
                sourceRec = (Rectangle){frameWidth * currentFrame, 0, frameWidth, frameHeight}; 
            }
        }
        else 
        {
            if (facingRight)
            {
                sourceRec = (Rectangle){frameWidth * currentFrame, 0, frameWidth, frameHeight};
            }
            else
            {
                sourceRec = (Rectangle){frameWidth * currentFrame, 0, -frameWidth, frameHeight};
            }
        }
        
        DrawTextureRec(jugadorTex, sourceRec, drawPos, WHITE);

        EndMode2D();

        // Texto de interfaz
        DrawText("Presiona ESC para salir al menú", 10, 10, 20, DARKGRAY);
        EndDrawing();
    }
    *posJugador = player.position;
    UnloadTexture(jugadorTex);

    return GAME_CONTINUE;
}