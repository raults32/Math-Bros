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
 
    const float GRAVITY = 500; // Gravedad que afecta al jugador
    const float PLAYER_JUMP_SPD = 320.0f; // Velocidad de salto del jugador
    const float PLAYER_HOR_SPD = 225.0f;  // Velocidad horizontal del jugador

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        // Movimiento horizontal
        if (IsKeyDown(KEY_LEFT)) player.position.x -= PLAYER_HOR_SPD * deltaTime;
        if (IsKeyDown(KEY_RIGHT)) player.position.x += PLAYER_HOR_SPD * deltaTime;

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

        // Dibujar
        BeginDrawing();
        ClearBackground(RAYWHITE); // Limpia la pantalla

        // Fondo estático
        DrawTexture(fondo, 0, 0, WHITE); // Aquí debe estar tu fondo de nivel

        BeginMode2D(camera);

        // Elementos del nivel que SÍ se mueven con la cámara
        for (int i = 0; i < MAX_ENVIRONMENT_ELEMENTS; i++)
        {
            DrawRectangleRec(envElements[i].rect, envElements[i].color);
        }
        DrawRectangleRec((Rectangle){player.position.x - 20, player.position.y - 40, 40, 40}, RED);

        EndMode2D();

        // Texto de interfaz
        DrawText("Presiona ESC para salir al menú", 10, 10, 20, DARKGRAY);
        EndDrawing();
    }
    *posJugador = player.position;
    return GAME_CONTINUE;
}
