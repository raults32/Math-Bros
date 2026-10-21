# Math Bros Rework

Renovacion independiente de Math Bros. El proyecto original permanece en la carpeta superior; esta version utiliza una copia de los recursos en `src/`.

## Requisitos
- Windows 64 bits.
- MinGW-w64 (`g++`) en `PATH` y raylib enlazable con `-lraylib`, **o** el instalador oficial de raylib para Windows (https://raysan5.itch.io/raylib) en `C:\raylib`: `compile.bat` lo detecta automáticamente.

## Jugar en Windows sin compilar
La carpeta incluye `main.exe` ya compilado (64 bits, raylib 6.2). Haz doble clic en `run.bat`.

## Compilar y ejecutar
Abre una terminal en esta carpeta y ejecuta:

```bat
compile.bat
run.bat
```

`run.bat` inicia el juego con esta carpeta como directorio de trabajo para que raylib encuentre `src/images/` y `src/sound/`.

## macOS
Requisitos: herramientas de línea de comandos de Xcode (`xcode-select --install`) y raylib vía Homebrew (`brew install raylib`).

```sh
./compile.sh
./run.sh
```

`run.sh` compila automáticamente si todavía no existe el ejecutable `main`. La raylib de Homebrew no incluye soporte JPG, por eso el juego usa las versiones `.png` de los fondos cuando un `.jpg` no se puede cargar.

## Dificultad de las preguntas
- Mundo 1-1 (Facil, primaria): sumas y restas hasta 20 y tablas del 2, 3, 5 y 10.
- Mundo 1-2 (Medio, primaria): sumas y restas de dos cifras, tablas completas y divisiones exactas.
- Mundo 1-3 (Dificil, preparatoria): ecuaciones de primer grado, potencias, raices cuadradas, porcentajes y jerarquia de operaciones con negativos.

## Halloween
El juego incluye decoraciones de Halloween dibujadas por código (`halloween.h`): cielo nocturno con luna, murciélagos, telarañas, arañas, fantasmas en el menú, calabazas y lápidas sobre las plataformas, y un sombrero de bruja para el jefe. Para quitarlas, cambia `#define MODO_HALLOWEEN 1` a `0` en `halloween.h` y vuelve a compilar.

## Controles
- Mover: A/D o flechas izquierda/derecha.
- Saltar y doble salto: Espacio, W o flecha arriba.
- Dash: Shift izquierdo o K.
- Responder una opción: clic izquierdo o teclas 1–4.
- Pausar: Esc.
- Reiniciar tras perder: R.
- Borrar el progreso guardado desde el menú de niveles: Supr.

## Cambios incluidos
Tres niveles largos con plataformas móviles, elevadores, plataformas frágiles, trampolines, bloques de recompensa, checkpoints, portal de salida y jefe final; animación procedural, partículas, dash, progresión, tienda y guardado automático en `savegame.dat`. La interfaz ahora usa tarjetas, botones y fondos animados con una estética de plataformas colorida; los niveles tienen un fondo procedural alternativo para evitar pantallas vacías si falta una textura.
