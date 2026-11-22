# Life Tracker RPG - Group 3

A high-quality, interactive Life Tracker Game written in pure C using **Raylib**.
Turn your real-life daily habits, fitness, study, and creative goals into quests, level up your character, build daily streaks, and spend hard-earned gold on real-life rewards!

## Group Members
- **Mamoon**
- **Sajid**
- **Khadija**
*Course: Programming Fundamentals Lab (Miss Ramsha Jutt) - FAST NUCES*

---

## Project Structure & Architecture

To reduce complexity and make the code easy to understand, explain in a viva, and maintain, the project is divided into **two main files**:

1. **`logic.c` (Game Logic & Persistence):**
   - Handles all core game data, calculations, rules, and file operations.
   - Manages user accounts (`accounts.txt`), registration, and login authentication.
   - Tracks player stats (Intellect, Stamina, Discipline, Social, Creativity), levels, XP, streaks, and gold.
   - Manages quest creation, quest completion, quest deletion, and rewards.
   - Handles real-life reward shop purchases and writes human-readable adventure logs to `Logs/<HeroName>.txt`.
   - **Zero GUI dependencies**: Written in 100% standard C (`stdio.h`, `stdlib.h`, `string.h`, `time.h`).

2. **`gui.c` (User Interface & Graphics):**
   - Handles all Raylib graphics, window rendering, and the main event loop.
   - Interactive animated avatar that bobs, blinks, tracks mouse movement, bounces, and speaks via speech bubbles.
   - Smooth sliding drawer popups for Quest Creation, Developer Mode, and Adventure Logs.
   - Visual widgets: custom buttons with hover glow, text input fields with blinking cursors, draggable sliders, progress bars, and celebration confetti.

3. **`logic.h` (Shared Definitions & Prototypes):**
   - Defines shared constants, the `Quest` data structure, arrays, and function declarations connecting the logic and GUI modules.

---

## Beginner-Friendly & Optimized Syntax

Following standard Programming Fundamentals conventions, the code strictly avoids obscure, hard-to-explain C features and uses repetitive, predictable, and clean programming patterns:

- **Simple Loops**: Uses only standard `for` loops and `while` loops.
- **Clear Conditionals**: All logic uses plain `if`, `else if`, and `else` statements (no cryptic ternary operators `? :`).
- **Array Indexing**: Uses standard array syntax (`array[i]`) instead of pointer arithmetic (`*(p + i)`).
- **Standard I/O**: File operations use straightforward `fopen()`, `fclose()`, `fgets()`, and `fprintf()`.
- **Descriptive Comments**: Every function is labeled with a clear explanatory comment (e.g. `// below is for ...`).

---

## How to Run on Windows

### Option 1: Quick Launch (Pre-compiled)
Simply double-click **`LifeTracker.exe`** to start playing immediately.

### Option 2: One-Click Compile & Run
Double-click **`run.bat`**. It will automatically compile `gui.c` and `logic.c` using GCC and Raylib (`C:\raylib\raylib`), then launch the game.

### Option 3: Manual Compilation
Open Command Prompt in this folder and run:
```bash
gcc gui.c logic.c -o LifeTracker.exe -IC:\raylib\raylib\include -LC:\raylib\raylib\lib -lraylib -lopengl32 -lgdi32 -lwinmm -Wall
```

---

## Key Features

- **Interactive Hero Avatar:**
  - Bobs up and down and blinks naturally.
  - Eyes follow the mouse cursor in real-time.
  - Equips distinct class hats: Scholar cap with gold tassel, Athlete red headband, or Artist beret.
  - Click the avatar anytime to trigger motivational tips, confetti bursts, and animations.
- **Weekly Quest Planner:**
  - Organize quests for every day of the week (Monday through Sunday).
  - 5 Life Attributes: Intellect, Stamina, Discipline, Social, Creativity.
  - 3 Difficulty tiers (Easy, Normal, Hard) rewarding proportional Gold and XP.
- **Daily Streaks & Leveling:**
  - Login daily to increase your consecutive streak.
  - Level up as you accumulate XP from completed real-life tasks.
- **Real-Life Reward Shop:**
  - Redeem earned gold for real-life treats, 1-hour guilt-free gaming sessions, or movie nights.
- **Adventure Logs:**
  - Automatically records timestamped activities in individual player log files under `Logs/`.
- **Developer Control Panel (Passcode: `KSM-321`):**
  - Group 3 authenticated control panel with stat manipulation, sliders, and batch quest actions.
