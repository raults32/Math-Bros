#!/bin/sh
# Compila Math Bros en macOS. Requiere Xcode Command Line Tools y raylib (brew install raylib).
set -e
cd "$(dirname "$0")"

if ! command -v clang++ >/dev/null 2>&1; then
  echo "No se encontro clang++. Instala las herramientas con: xcode-select --install"
  exit 1
fi

RAYLIB_PREFIX="$(brew --prefix raylib 2>/dev/null || true)"
if [ -z "$RAYLIB_PREFIX" ] || [ ! -f "$RAYLIB_PREFIX/include/raylib.h" ]; then
  echo "No se encontro raylib. Instalala con: brew install raylib"
  exit 1
fi

clang++ -std=c++17 -O2 -Wall -Wno-missing-braces -Wno-deprecated-declarations main.cpp -o main \
  -I"$RAYLIB_PREFIX/include" "$RAYLIB_PREFIX/lib/libraylib.a" \
  -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -framework CoreAudio -framework QuartzCore

echo "Compilacion completada: main"
