# Life Tracker RPG - Group 3 (Two-File Version, Simple Syntax)

A high-quality, interactive Life Tracker Game written in pure C using **Raylib**.
Turn your real-life daily habits, fitness, study, and creative goals into quests, level up your character, build daily streaks, and spend hard-earned gold on real-life rewards!

## Group Members
- **Mamoon**
- **Sajid**
- **Khadija**
*Course: Programming Fundamentals Lab (Miss Ramsha Jutt) - FAST NUCES*

---

## About This Version

This is the **Game 3.0 project** (same two-file architecture, same screens, same
GUI interface, same features, same save files) rewritten with **only plain,
basic C syntax** - the easy style used in the Group 3 Project files. Every
gameplay value, every button position, every text, every file format and every
rule is exactly the same as Game 3.0. Only the way the code is written changed:

- **No ternary operators** (`a ? b : c`) - every one became a normal
  `if / else` with a small color or value variable.
- **No shorthand operators** - no `+=`, `-=`, `++` or `--` as statements;
  everything is written in full, like `gold = gold + gain;`,
  `accCount = accCount + 1;`, `msgTimer = msgTimer - 1;`
  (the standard `i++` inside a `for` loop line is kept, because that is part
  of the basic `for` loop itself).
- **No `do while` loops and no `continue` statements** - only simple `for`
  and `while` loops with plain `if / else` decisions.
- **No `static` variables hidden inside functions** - the previous-screen
  tracker (`prevScreen`) is a normal global variable at the top.
- **Full variable names** - `frameTime`, `panelX`, `rowY`, `statValue`,
  `accountIndex`, `dateInfo`, `mouseFraction`, `buttonColor`, and so on.
- **Same small library set** - `logic.c` uses only `stdio.h`, `stdlib.h`,
  `string.h`, `time.h`; `gui.c` adds only `raylib.h` and `math.h`.

**Verified:** both versions were tested through 6 full scripted play-throughs
(signup, quests, completion, shop, developer mode, re-login streaks, invalid
logins, validation messages) and produced byte-for-byte identical save files,
accounts and adventure logs as the original Game 3.0, with zero compiler
warnings under `gcc -Wall -Wextra`.

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
   - Visual widgets: clean buttons with hover colors, text input fields with blinking cursors, draggable sliders, and progress bars.

3. **`logic.h` (Shared Definitions & Prototypes):**
   - Defines shared constants, the `Quest` data structure, arrays, and function declarations connecting the logic and GUI modules.

---

## Beginner-Friendly Syntax

Following standard Programming Fundamentals conventions, the code strictly avoids obscure, hard-to-explain C features and uses repetitive, predictable, and clean programming patterns:

- **Simple Loops**: Uses only standard `for` loops and `while` loops (no `do while`, no `goto`).
- **Clear Conditionals**: All logic uses plain `if`, `else if`, and `else` statements (no ternary operators `? :` anywhere).
- **Full Statements**: No shorthand operators (`+=`, `-=`, `++`, `--`) as statements - every change is written out in full.
- **Array Indexing**: Uses standard array syntax (`array[i]`) instead of pointer arithmetic.
- **Standard I/O**: File operations use straightforward `fopen()`, `fclose()`, `fgets()`, `fscanf()`, and `fprintf()`.
- **Descriptive Comments**: Every function is labeled with a clear explanatory comment (e.g. `// below is for ...`).

---

## How to Run on Windows

### Option 1: One-Click Compile & Run
Double-click **`run.bat`**. It will automatically compile `gui.c` and `logic.c` using GCC and Raylib (`C:\raylib\raylib`), then launch the game.

### Option 2: Manual Compilation
Open Command Prompt in this folder and run:
```bash
gcc gui.c logic.c -o LifeTracker.exe -IC:\raylib\raylib\include -LC:\raylib\raylib\lib -lraylib -lopengl32 -lgdi32 -lwinmm -Wall
```

Then start the game with **`LifeTracker.exe`**.

---

## Files in this Project

1. `gui.c` - Raylib graphics, interface, animations and the main window loop.
2. `logic.c` - Pure game logic, rules and file persistence (standard C only).
3. `logic.h` - Shared constants, the Quest structure and function declarations.
4. `run.bat` - One-click compile and launch script for Windows.
5. `KSM-321.png` - Custom group image and window icon for Group 3.
6. `accounts.txt` - Account credentials stored in clean readable format (`Name: Sajid` / `PIN: 1234`).
7. `Logs/Sajid.txt` - Sample adventure log; every player gets their own file in `Logs/`.

---

## Key Features

- **Interactive Hero Avatar:**
  - Bobs up and down and blinks naturally.
  - Eyes follow the mouse cursor in real-time.
  - Equips distinct class hats: Scholar cap with gold tassel, Athlete red headband, or Artist beret.
  - Click the avatar anytime to trigger motivational tips and animations.
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
