#ifndef PERSONAJE_H
#define PERSONAJE_H

#include <raylib.h>

typedef struct PlayerConfig
{
    Vector2 startPosition;
    float maxHorizontalSpeed;
    float jumpVelocity;
    float gravity;
} PlayerConfig;

static inline PlayerConfig ObtenerConfiguracionJugador(void)
{
    PlayerConfig cfg = {{50.0f, 550.0f}, 265.0f, 350.0f, 560.0f};
    return cfg;
}

#endif
