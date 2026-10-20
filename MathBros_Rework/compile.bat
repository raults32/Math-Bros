@echo off
setlocal
cd /d "%~dp0"
where g++ >nul 2>nul
if errorlevel 1 (
  echo No se encontro g++ de MinGW-w64 en PATH.
  exit /b 1
)
g++ -std=c++17 -O2 -Wall -Wno-missing-braces main.cpp -o main.exe -static-libgcc -static-libstdc++ -lraylib -lopengl32 -lgdi32 -lwinmm
if errorlevel 1 (
  echo Fallo la compilacion. Revisa que raylib y MinGW-w64 esten instalados.
  exit /b 1
)
echo Compilacion completada: main.exe
