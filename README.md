# Math Bros

Juego de plataformas hecho en C++ con raylib.

## Ejecutar el juego

Haz doble clic en `run.bat`. Este abre `main.exe` desde la carpeta del proyecto para que pueda encontrar las imágenes y los sonidos de `src/`.

## Compilar desde el código fuente

Se necesita MinGW-w64 con `g++` y raylib instalada. Lo más fácil es usar el instalador oficial de raylib para Windows (https://raysan5.itch.io/raylib): `compile.bat` detecta automáticamente `C:\raylib` y su compilador w64devkit aunque `g++` no esté en el PATH. Después ejecuta:

```bat
compile.bat main.cpp
```

Los recursos del juego están en `src/images/` y `src/sound/`.

## macOS

Requisitos: herramientas de línea de comandos de Xcode (`xcode-select --install`) y raylib vía [Homebrew](https://brew.sh):

```sh
brew install raylib
```

Compilar y ejecutar desde la carpeta del proyecto:

```sh
./compile.sh
./run.sh
```

`run.sh` compila automáticamente si todavía no existe el ejecutable `main`. raylib se enlaza de forma estática, así que el binario no necesita Homebrew para ejecutarse.
