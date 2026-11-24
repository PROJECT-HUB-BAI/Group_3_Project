@echo off
setlocal EnableExtensions
cd /d "%~dp0"

title Life RPG - Build and Run

set "SOURCE=life_rpg_raylib.c"
set "OUTPUT=LifeRPG.exe"
set "GCC="
set "RAY_INC="
set "RAY_LIB="

 echo.
echo ==========================================
echo          LIFE RPG - RAYLIB
echo ==========================================
echo.

if not exist "%SOURCE%" (
    echo [ERROR] Game source not found: %SOURCE%
    echo Keep run.bat in the same folder as the game source.
    goto :fail
)

rem Prefer the compiler bundled with the raylib Windows package.
if exist "C:\raylib\w64devkit\bin\gcc.exe" set "GCC=C:\raylib\w64devkit\bin\gcc.exe"
if not defined GCC (
    for %%G in (gcc.exe) do set "GCC=%%~$PATH:G"
)
if not defined GCC (
    echo [ERROR] GCC was not found on PATH or in C:\raylib\w64devkit\bin.
    echo Install a MinGW-w64 toolchain or the raylib Windows package.
    goto :fail
)
echo [OK] Compiler: %GCC%

rem RAYLIB_DIR can point to the raylib root or its src/include/lib folder.
if defined RAYLIB_DIR (
    if exist "%RAYLIB_DIR%\raylib.h" set "RAY_INC=%RAYLIB_DIR%"
    if exist "%RAYLIB_DIR%\raylib\raylib.h" set "RAY_INC=%RAYLIB_DIR%\raylib"
    if exist "%RAYLIB_DIR%\src\raylib.h" set "RAY_INC=%RAYLIB_DIR%\src"
    if exist "%RAYLIB_DIR%\include\raylib.h" set "RAY_INC=%RAYLIB_DIR%\include"
    if exist "%RAYLIB_DIR%\raylib\src\raylib.h" set "RAY_INC=%RAYLIB_DIR%\raylib\src"
)
if not defined RAY_INC if exist "C:\raylib\raylib\raylib.h" set "RAY_INC=C:\raylib\raylib"
if not defined RAY_INC if exist "C:\raylib\raylib\src\raylib.h" set "RAY_INC=C:\raylib\raylib\src"
if not defined RAY_INC if exist "C:\raylib\src\raylib.h" set "RAY_INC=C:\raylib\src"
if not defined RAY_INC if exist "C:\raylib\include\raylib.h" set "RAY_INC=C:\raylib\include"
if not defined RAY_INC (
    echo [ERROR] raylib.h was not found.
    echo Set RAYLIB_DIR to your raylib installation folder and retry.
    echo Example: set RAYLIB_DIR=C:\raylib\raylib
    goto :fail
)
echo [OK] Raylib headers: %RAY_INC%

rem Find the matching MinGW static library beside headers or in common lib folders.
if exist "%RAY_INC%\libraylib.a" set "RAY_LIB=%RAY_INC%"
if not defined RAY_LIB if exist "%RAY_INC%\..\..\lib\libraylib.a" set "RAY_LIB=%RAY_INC%\..\..\lib"
if not defined RAY_LIB if exist "%RAY_INC%\..\lib\libraylib.a" set "RAY_LIB=%RAY_INC%\..\lib"
if not defined RAY_LIB if exist "%RAY_INC%\lib\libraylib.a" set "RAY_LIB=%RAY_INC%\lib"
if defined RAYLIB_DIR if not defined RAY_LIB if exist "%RAYLIB_DIR%\lib\libraylib.a" set "RAY_LIB=%RAYLIB_DIR%\lib"
if defined RAYLIB_DIR if not defined RAY_LIB if exist "%RAYLIB_DIR%\raylib\lib\libraylib.a" set "RAY_LIB=%RAYLIB_DIR%\raylib\lib"
if not defined RAY_LIB (
    echo [ERROR] libraylib.a was not found for the selected Raylib headers.
    echo Install the MinGW-compatible Raylib package, or set RAYLIB_DIR correctly.
    goto :fail
)
echo [OK] Raylib library: %RAY_LIB%

echo.
echo [BUILD] Compiling %SOURCE%...
"%GCC%" "%SOURCE%" -std=c99 -O2 -Wall -Wextra -o "%OUTPUT%" -I"%RAY_INC%" -L"%RAY_LIB%" -lraylib -lopengl32 -lgdi32 -lwinmm
if errorlevel 1 (
    echo.
    echo [ERROR] Build failed. Review the compiler output above.
    goto :fail
)

echo.
echo [OK] Build complete: %OUTPUT%

rem Copy an optional Raylib runtime DLL next to the executable.
if exist "%RAY_LIB%\raylib.dll" copy /Y "%RAY_LIB%\raylib.dll" "." >nul
if exist "%RAY_INC%\..\bin\raylib.dll" copy /Y "%RAY_INC%\..\bin\raylib.dll" "." >nul

echo [RUN] Starting the game...
"%OUTPUT%"
set "GAME_EXIT=%ERRORLEVEL%"
echo.
echo Game closed with exit code %GAME_EXIT%.
pause
endlocal & exit /b %GAME_EXIT%

:fail
echo.
pause
endlocal & exit /b 1



