# Life RPG 

## Run the Raylib game

Double-click `run.bat`. It builds `life_rpg_raylib.c` with GCC and Raylib, then launches the game. If Raylib is installed outside the standard `C:\raylib` folder, set `RAYLIB_DIR` to the Raylib folder before running the batch file.

The graphical quest form has four predefined activities for each of the five statistics. Enter an amount in the displayed unit: kilometers for running, repetitions for strength activities, and the shown count or duration for the other activities. Arial is loaded from Windows when available; the game falls back to Raylib’s default font if it cannot find Arial.

## Source files

- `life_rpg_raylib.c` is the graphical game started by `run.bat`.
- `GUI_LIFE_RPG_Game.c` is the separate console implementation. Its existing behavior is retained.
- `Final Project Report.md` describes both versions and the current graphical activities.

Account and player save files are stored beside the game.
