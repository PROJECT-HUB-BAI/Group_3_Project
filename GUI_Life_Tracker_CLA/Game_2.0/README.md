# Life Tracker RPG - Group 3

A high-quality, interactive Life Tracker Game written in pure C using **Raylib**.
Turn your real-life daily habits, fitness, study, and creative goals into quests, level up your character, build daily streaks, and spend hard-earned gold on real-life rewards!

## Group Members
- **Mamoon**
- **Sajid**
- **Khadija**
*Course: Programming Fundamentals Lab (Miss Ramsha Jutt) - FAST NUCES*

---

## This Version: SIMPLE SYNTAX

Same game, same look, same logic, same files - only the **code style** was made
beginner-friendly, so it is easy to read, explain in a viva, and modify.
Every "rare" or advanced C feature was replaced with plain, everyday syntax.
The gameplay, screens, colors, layout, saves and logs are exactly the same as
the original v3. What was simplified:

1. **No ternary operators** (`a ? b : c`) - every one became a normal
   `if / else` with a small color/value variable (e.g. `DrawButton`, the day
   tabs, the avatar's suit color, the XP bar clamp, the quest-maker safe
   values).
2. **No variadic function** - `LogNote()` used `stdarg` / `vsnprintf` (hard to
   explain). Now it simply takes one finished sentence, and the call sites
   build it with raylib's `TextFormat()` that is already used everywhere else:
   `LogNote(TextFormat("Logged in on %s", days[TodayIndex()]));`
3. **No `strftime`** - the log timestamp is made with a plain `sprintf` of the
   `struct tm` fields: `sprintf(dateBuf, "%02d-%02d-%d %02d:%02d", ...)`.
4. **No `sscanf` scanset** (`"%39[^;];%d;..."`) - loading a saved quest line is
   done with two tiny loop helpers, `ReadField()` (copies text up to the `;`
   sign) and `ReadNumber()` (same but returns `atoi()`).
5. **No pointer arithmetic when reading accounts** - `char *p = line + 5;
   while (*p == ' ') p++;` became a small helper `AfterLabel()` that uses plain
   index numbers.
6. **No pointer variables over arrays** - confetti particles use plain
   `bits[i].x` indexing instead of `Particle *p`, and the dead-particle swap
   is written out step by step.
7. **No "static" variables hidden inside functions** - the slider drag flag
   (`sliderDragging`) and the star positions (`starX`, `starY`, `starsReady`)
   are normal globals at the top, easier to see and explain.
8. **Step-by-step validation** - `HandleTyping()` checks each rule with its own
   simple `if` line instead of one long combined condition.

If you want the fastest-running version instead, use the **Optimized** zip -
both behave identically.

The other files (`run.bat`, `KSM-321.png`, `accounts.txt`, `Logs/`) are exactly
the same as the original v3 zip. Compile once with `run.bat` below to get a
fresh `LifeTracker.exe` built from this source.

---

## Files in this Project

1. `main.c` - Clean, spacious, beginner-friendly single-file C code. Every major function is preceded by lowercase explanatory comments (e.g. `// below is for updating the record`).
2. `run.bat` - One-click compile and launch script for Windows using gcc and Raylib (`C:\raylib\raylib`).
3. `KSM-321.png` - Custom group image and window icon for Group 3.
4. `accounts.txt` - Account credentials stored in clean readable format:
   ```
   Name: Sajid
   PIN: 1234
   ```
5. `Logs/` - Directory storing individual activity logs for each player (e.g. `Logs/Sajid.txt`) with human-readable sentences like `Quest added on Monday`, `Quest completed on Monday (+50 gold)`, etc.
6. `README.md` - Documentation and project guide.

---

## How to Run on Windows (Quick & Easy)

1. Ensure Raylib is installed at `C:\raylib\raylib` (standard FAST w64devkit installation).
2. Simply double-click **`run.bat`**.
3. It will compile `main.c` and launch `LifeTracker.exe` immediately!

Manual compilation via command prompt:
```bash
gcc main.c -o LifeTracker.exe -IC:\raylib\raylib\include -LC:\raylib\raylib\lib -lraylib -lopengl32 -lgdi32 -lwinmm -Wall
```

---

## Key Features & Interactivity

- **Interactive Animated Avatar:**
  - Naturally bobs and blinks.
  - Eye pupils follow mouse movement across the screen.
  - Clickable avatar reacts with a bounce, confetti burst, and speech bubbles offering tips, motivation, and wisdom.
  - Equips distinct class gear: Scholar graduation cap, Athlete headband, or Artist beret.
- **Left-Sliding Popups:**
  - Modern sliding drawer panels animate smoothly from the left side with easing effects for **Quest Creation**, **Developer Mode**, and **Activity Logs**.
- **6-Step Guided Quest Creator:**
  - Set quest name, day of the week, life attribute, specific activity, amount, and difficulty multiplier slider (Easy, Normal, Hard).
- **Life Attributes & Progression:**
  - 5 core attributes: *Intellect, Stamina, Discipline, Social, Creativity*.
  - Leveling system with celebratory particle confetti explosions on quest completion and level ups.
- **Real-Life Reward Shop:**
  - Spend earned gold on real-world guilt-free rewards (e.g. Treats, 1-hour gaming, Movie night).
- **Developer Mode:**
  - Access via the **DEV** button in the header bar.
  - Developer Passcode: **`KSM-321`**.
  - Displays authenticated `KSM-321.png` badge.
  - Sliders to manipulate Gold (0-9999), Level (1-99), Day Streak (1-365).
  - Quick boost buttons (+5 each attribute).
  - Power buttons: "COMPLETE ALL TODAY", "WIPE ALL QUESTS", "RESET STREAK".
