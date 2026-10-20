@echo off
cd /d "%~dp0"
set filename=%1

if "%filename%"=="main.cpp" (
    g++ -std=c++17 -O2 -static -static-libgcc -static-libstdc++ -mwindows main.cpp -o main.exe -lraylib -lopengl32 -lgdi32 -lwinmm && main.exe
) else (
    g++ %filename% -o out.exe && out.exe
)
