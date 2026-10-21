@echo off
setlocal
cd /d "%~dp0"
set "filename=%~1"
if "%filename%"=="" set "filename=main.cpp"
rem Usa g++ del PATH o, si no esta, el w64devkit del instalador oficial de raylib (C:\raylib)
set "RAYLIB_FLAGS="
where g++ >nul 2>nul
if errorlevel 1 (
  if exist "C:\raylib\w64devkit\bin\g++.exe" (
    set "PATH=C:\raylib\w64devkit\bin;%PATH%"
  ) else (
    echo No se encontro g++. Instala raylib para Windows desde https://raysan5.itch.io/raylib
    echo o agrega MinGW-w64 al PATH.
    pause
    exit /b 1
  )
)
if exist "C:\raylib\raylib\src\raylib.h" set "RAYLIB_FLAGS=-IC:\raylib\raylib\src -LC:\raylib\raylib\src"

if /i "%filename%"=="main.cpp" (
    g++ -std=c++17 -O2 -static -static-libgcc -static-libstdc++ -mwindows main.cpp -o main.exe %RAYLIB_FLAGS% -lraylib -lopengl32 -lgdi32 -lwinmm && main.exe
) else (
    g++ "%filename%" -o out.exe && out.exe
)
