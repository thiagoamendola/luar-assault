@echo off
REM Open a new window and run make at project root
setlocal
cd /d %~dp0
if not exist Makefile (
  echo Makefile not found in %cd%
  pause
  exit /b 1
)
cmd /c "title GBA Build - %cd% && make && echo. && echo Build complete. && pause"
