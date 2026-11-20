@echo off
rem ==================================================================
rem  Life Tracker RPG - Group 3 (Mamoon, Sajid, Khadija)
rem  One-click compile and run for Windows
rem ==================================================================
set RAYLIB=C:\raylib\raylib

gcc main.c -o LifeTracker.exe -I%RAYLIB%\include -L%RAYLIB%\lib -lraylib -lopengl32 -lgdi32 -lwinmm -Wall

if exist LifeTracker.exe (
    LifeTracker.exe
) else (
    echo Compile failed. Check if raylib is at %RAYLIB%
    pause
)
