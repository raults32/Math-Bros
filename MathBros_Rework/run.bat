@echo off
cd /d "%~dp0"
if not exist main.exe (
  echo No existe main.exe. Ejecuta compile.bat primero.
  pause
  exit /b 1
)
start "Math Bros Rework" /D "%~dp0" "%~dp0main.exe"
