@echo off
set filename=%1

if "%filename%"=="main.cpp" (
    g++ main.cpp -o main.exe -lraylib -lopengl32 -lgdi32 -lwinmm && main.exe
) else (
    g++ %filename% -o out.exe && out.exe
)
