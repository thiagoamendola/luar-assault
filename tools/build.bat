@echo off
setlocal EnableDelayedExpansion

echo ============================================================
echo  Luar Assault - Build
echo ============================================================
echo.

:: Resolve project root (one level above this script's folder)
set "SCRIPT_DIR=%~dp0"
pushd "%SCRIPT_DIR%.."
set "PROJECT_DIR=%CD%"
popd

echo [INFO] Building in "%PROJECT_DIR%"
echo.

cd /d "%PROJECT_DIR%"

make
if !errorlevel! neq 0 (
    echo.
    echo [FAILED] Build failed. Review the errors above.
    pause
    exit /b 1
)

echo.
echo ============================================================
echo  Build successful!
echo ============================================================
pause
