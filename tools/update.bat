@echo off
setlocal EnableDelayedExpansion

echo ============================================================
echo  Luar Assault - Update
echo ============================================================
echo.

:: Resolve project root (one level above this script's folder)
set "SCRIPT_DIR=%~dp0"
pushd "%SCRIPT_DIR%.."
set "PROJECT_DIR=%CD%"
popd

echo [INFO] Repository: "%PROJECT_DIR%"
echo.

cd /d "%PROJECT_DIR%"

:: ------------------------------------------------------------
:: 1. Check for local changes
:: ------------------------------------------------------------
git status --porcelain > nul 2>&1
for /f "tokens=*" %%l in ('git status --porcelain 2^>nul') do (
    set "HAS_CHANGES=1"
)

if defined HAS_CHANGES (
    echo [WARN] You have uncommitted local changes:
    echo.
    git status --short
    echo.
    set /p "CONFIRM=Local changes will be stashed before pulling. Continue? [y/N]: "
    if /i "!CONFIRM!" neq "y" (
        echo [INFO] Update cancelled.
        pause
        exit /b 0
    )

    echo.
    echo [INFO] Stashing local changes...
    git stash push -m "auto-stash before update"
    if !errorlevel! neq 0 (
        echo [ERROR] Failed to stash changes.
        pause
        exit /b 1
    )
    echo [OK] Changes stashed successfully.
    echo.
)

:: ------------------------------------------------------------
:: 2. Pull latest changes
:: ------------------------------------------------------------
echo [INFO] Pulling latest changes...
git pull --force
if !errorlevel! neq 0 (
    echo.
    echo [ERROR] git pull failed. Review the errors above.
    if defined HAS_CHANGES (
        echo [INFO] Restoring your stashed changes...
        git stash pop
    )
    pause
    exit /b 1
)

echo.
echo ============================================================
echo  Update complete!
if defined HAS_CHANGES (
    echo.
    echo  Your local changes were stashed. To restore them run:
    echo    git stash pop
)
echo ============================================================
pause
