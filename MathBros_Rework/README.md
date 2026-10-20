# Math Bros Rework

Renovacion independiente de Math Bros. El proyecto original permanece en la carpeta superior; esta version utiliza una copia de los recursos en `src/`.

## Requisitos
- Windows 64 bits.
- MinGW-w64 (`g++`) en `PATH`.
- raylib para MinGW-w64 instalada y enlazable con `-lraylib`.

## Compilar y ejecutar
Abre una terminal en esta carpeta y ejecuta:

```bat
compile.bat
run.bat
```

`run.bat` inicia el juego con esta carpeta como directorio de trabajo para que raylib encuentre `src/images/` y `src/sound/`.

## Controles
- Mover: A/D o flechas izquierda/derecha.
- Saltar y doble salto: Espacio, W o flecha arriba.
- Dash: Shift izquierdo o K.
- Responder una opción: clic izquierdo o teclas 1–4.
- Pausar: Esc.
- Reiniciar tras perder: R.
- Borrar el progreso guardado desde el menú de niveles: Supr.

## Cambios incluidos
Tres niveles largos con plataformas móviles, elevadores, plataformas frágiles, trampolines, bloques de recompensa, checkpoints, portal de salida y jefe final; animación procedural, partículas, dash, progresión, tienda y guardado automático en `savegame.dat`.
