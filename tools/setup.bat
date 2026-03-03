@echo off
setlocal EnableDelayedExpansion

echo ============================================================
echo  Luar Assault - Environment Setup
echo ============================================================
echo.

:: Resolve the project root (one level above this script's folder)
set "SCRIPT_DIR=%~dp0"
set "PROJECT_DIR=%SCRIPT_DIR%.."
pushd "%PROJECT_DIR%\.."
set "PARENT_DIR=%CD%"
popd

echo [INFO] Project root : %PROJECT_DIR%
echo [INFO] Clone target : %PARENT_DIR%
echo.

:: ------------------------------------------------------------
:: 1. Check / install Git
:: ------------------------------------------------------------
echo [STEP 1/4] Checking for Git...
git --version >nul 2>&1
if !errorlevel! neq 0 (
    echo [INFO] Git not found. Attempting to install via winget...
    winget install --id Git.Git -e --source winget
    if !errorlevel! neq 0 (
        echo [ERROR] Automatic Git installation failed.
        echo         Please install Git manually from https://git-scm.com/downloads
        echo         and re-run this script.
        goto :error
    )
    :: Refresh PATH so git is available in this session
    for /f "tokens=*" %%i in ('where git 2^>nul') do set "GIT_EXE=%%i"
    if not defined GIT_EXE (
        echo [WARN] Git was installed but is not yet available in this session's PATH.
        echo        Please open a new terminal and re-run this script.
        goto :error
    )
    echo [OK] Git installed successfully.
) else (
    for /f "tokens=*" %%v in ('git --version') do echo [OK] Found %%v
)
echo.

:: ------------------------------------------------------------
:: 2. Clone Butano
:: ------------------------------------------------------------
echo [STEP 2/5] Cloning Butano...
if exist "%PARENT_DIR%\butano" (
    echo [SKIP] Butano folder already exists at "%PARENT_DIR%\butano". Skipping clone.
) else (
    git clone https://github.com/GValiente/butano.git "%PARENT_DIR%\butano"
    if !errorlevel! neq 0 (
        echo [ERROR] Failed to clone Butano. Make sure git is installed and you have internet access.
        goto :error
    )
    echo [OK] Butano cloned successfully.
)
echo.

:: ------------------------------------------------------------
:: 3. Check devkitARM
:: ------------------------------------------------------------
echo [STEP 3/5] Checking for devkitARM...
if defined DEVKITARM (
    echo [OK] devkitARM found at "%DEVKITARM%".
) else if exist "C:\devkitPro\devkitARM" (
    echo [OK] devkitARM found at default path "C:\devkitPro\devkitARM".
) else (
    echo [ERROR] devkitARM was not detected.
    echo.
    echo         Please install it manually:
    echo           1. Go to https://devkitpro.org/wiki/Getting_Started
    echo           2. Download and run the devkitPro installer for Windows.
    echo           3. Select the "GBA Development" component.
    echo           4. After installation, re-run this script.
    goto :error
)
echo.

:: ------------------------------------------------------------
:: 4. Install Python packages
:: ------------------------------------------------------------
echo [STEP 4/5] Installing Python packages...

python --version >nul 2>&1
if !errorlevel! neq 0 (
    echo [ERROR] Python is not found in PATH.
    echo         Please install Python from https://www.python.org/ and make sure to add it to PATH.
    goto :error
)

python -m pip install pillow
if !errorlevel! neq 0 (
    echo [ERROR] Failed to install Pillow.
    goto :error
)

python -m pip install termcolor
if !errorlevel! neq 0 (
    echo [ERROR] Failed to install termcolor.
    goto :error
)

echo [OK] Python packages installed successfully.
echo.

:: ------------------------------------------------------------
:: 5. Clone gba-free-fonts
:: ------------------------------------------------------------
echo [STEP 5/5] Cloning gba-free-fonts...
if exist "%PARENT_DIR%\gba-free-fonts" (
    echo [SKIP] gba-free-fonts folder already exists at "%PARENT_DIR%\gba-free-fonts". Skipping clone.
) else (
    git clone https://github.com/laqieer/gba-free-fonts.git "%PARENT_DIR%\gba-free-fonts"
    if !errorlevel! neq 0 (
        echo [ERROR] Failed to clone gba-free-fonts. Make sure git is installed and you have internet access.
        goto :error
    )
    echo [OK] gba-free-fonts cloned successfully.
)
echo.

:: ------------------------------------------------------------
:: Done
:: ------------------------------------------------------------
echo ============================================================
echo  Setup complete! All dependencies are in place.
echo  You can now run: make
echo ============================================================
pause
goto :eof

:error
echo.
echo [FAILED] Setup did not complete successfully. Review the errors above.
pause
exit /b 1
