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

if not "%PROJECT_DIR: =%"=="%PROJECT_DIR%" (
    echo [ERROR] The project path contains spaces, which the Butano build does not support:
    echo         "%PROJECT_DIR%"
    echo.
    echo         Move this project and its sibling dependencies to a path without spaces.
    pause
    exit /b 1
)

cd /d "%PROJECT_DIR%"

make
if !errorlevel! neq 0 (
    echo.
    echo [WARN] Incremental build failed. A clean rebuild may fix stale build files,
    echo        but it can take a while on older computers.
    echo.
    set "CLEAN_RETRY="
    set /p "CLEAN_RETRY=Run a clean rebuild now? [y/N]: "
    if /i "!CLEAN_RETRY!" neq "y" (
        echo.
        echo [FAILED] Build failed. Clean rebuild cancelled.
        pause
        exit /b 1
    )

    echo.
    echo [INFO] Cleaning previous build files...

    make clean
    if !errorlevel! neq 0 (
        echo.
        echo [FAILED] Could not clean the previous build files.
        pause
        exit /b 1 
    )

    make
    if !errorlevel! neq 0 (
        echo.
        echo [FAILED] Build failed after a clean retry. Review the errors above.
        pause
        exit /b 1
    )
)

echo.
echo ============================================================
echo  Build successful!
echo ============================================================
pause
