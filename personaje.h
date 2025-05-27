#include <raylib.h> // Incluye la librería raylib para gráficos y entrada/salida

//**********************************************************************************
//** Inicializacion de la ventana y variables **//
//**********************************************************************************

Vector2 playerPosition = { 100, 520 }; // Posicion inicial del jugador
Vector2 playerSpeed = { 5.0f, 5.0f }; // Velocidad del jugador

int movimientosPersonaje()
{

if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) playerPosition.x += playerSpeed.x; // Mueve el jugador a la derecha
if (IsKeyDown(KEY_DOWN)  || IsKeyDown(KEY_S)) playerPosition.y += playerSpeed.y; // Mueve el jugador hacia abajo
if (IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A)) playerPosition.x -= playerSpeed.x; // Mueve el jugador a la izquierda
if (IsKeyDown(KEY_UP)    || IsKeyDown(KEY_W)) playerPosition.y -= playerSpeed.y; //Mueve el jugador hacia arriba
if (IsKeyDown(KEY_SPACE)) playerPosition.y -= playerSpeed.y; // Mueve el jugador hacia arriba al presionar espacio

return 0;
}

int designPersonaje()
{
    // Dibuja el personaje en la posición actual
    DrawCircle(playerPosition.x, playerPosition.y, 20, RED); // Dibuja un círculo rojo en la posición del jugador
    return 0;
}