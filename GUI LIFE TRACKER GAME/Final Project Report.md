# PROJECT REPORT
# LIFE RPG – LEVEL UP YOUR REAL LIFE
### A Personal Productivity Game in C with Raylib and Console Versions

**Submitted By:**
- Mamoon 26k-0003
- Sajid 26k-0013
- Khadija 26k-0027

**Course:** Programming Fundamentals Lab
**Programming Language:** C  
**Project Type:** Raylib graphical game with a separate console version  
**Department:** Artificial Intelligence  
**Institution:** FAST – NUCES 
**Semester 2026 Fall

---

# 1. Introduction

LIFE RPG – Level Up Your Real Life is a personal productivity game written in C. The project includes a Raylib graphical interface and a separate console implementation. Both versions use quests, player statistics, levels, gold, streaks, and rewards to make habit tracking more engaging.

The application allows users to create personal accounts, select a character class, create weekly quests, complete tasks, earn experience points (XP), collect gold, and improve their personal statistics.

The project is based on the concept of gamification, which means applying game elements to non-game activities. Instead of simply maintaining a list of daily tasks, users can treat their personal goals as missions in a role-playing game.

The system also includes account management, file handling, daily streaks, rewards, player profiles, and developer mode.

# 2. Project Objectives

The main objectives of this project are:

1. To develop a personal productivity system using the C programming language.
2. To encourage users to complete their daily and weekly tasks.
3. To implement a reward system using experience points and virtual gold.
4. To provide account creation and login functionality.
5. To store and retrieve player information using file handling.
6. To track player progress through levels and statistics.
7. To implement weekly quests and daily activity streaks.
8. To demonstrate the practical use of arrays, functions, loops, conditional statements, strings, and file handling.

# 3. Project Overview

LIFE RPG provides a game-like environment where users can manage their personal development activities.

The Raylib version opens at a graphical login screen. Players can sign in with their KSM ID or create a new account. The KSM ID is shown in the game header after login and is used to sign in again. The separate console version starts with a text menu for login, account creation, and exit.

Each player can select one of three character classes:

- Scholar
- Athlete
- Artist

After entering the player hub, users can access their profile, manage quests, purchase rewards, and access developer tools.

# 4. Main Features of the Project

## 4.1 Account Management

The program supports multiple user accounts.

Features include:
- Creating a new account.
- Generating a unique KSM ID.
- Saving account information.
- Displaying registered players.
- Logging into an existing account.
- Saving player progress when logging out.

The program supports up to 500 accounts.

## 4.2 Character Classes

Users can choose one of three character classes when creating an account.

| Class | Special Ability |
|---|---|
| Scholar | Bonus XP for Intellectual quests |
| Athlete | Bonus XP for Stamina quests |
| Artist | Bonus XP for Creativity quests |

The selected class provides a 20% XP bonus when the player completes a quest associated with that class.

## 4.3 Player Profile and Statistics

Each player has a profile containing personal progress information.

The profile displays:
- Player name and KSM ID.
- Selected character class.
- Current level.
- Experience points (XP).
- Available gold.
- Daily streak.
- Daily fortune.
- Player title.
- Personal statistics.
- Discipline score.

The system maintains five main statistics:

| Statistic | Description |
|---|---|
| Intellect | Represents learning and mental development |
| Stamina | Represents physical activity and fitness |
| Discipline | Represents consistency and self-control |
| Social Skills | Represents social development |
| Creativity | Represents creative activities |

Each statistic starts at 10 and can increase through quest completion, up to a maximum of 100.

## 4.4 Quest and Habit Management

The quest system is one of the main components of the project.

Users can:
- View the weekly quest board.
- Create new quests.
- Assign quests to specific days.
- Select a statistic for each quest.
- Choose the difficulty level.
- Complete quests.
- View completed and incomplete quests.

The program supports up to 50 quests per player. The Raylib quest screen displays up to 12 quests at a time.

### Raylib Activity Choices

The graphical quest form offers four predefined activities for each statistic. Players choose a day, statistic, activity, amount, and difficulty. Running accepts a decimal distance in kilometers; the other activities accept whole-number amounts. The activity and amount are saved in the quest name, so the existing quest, reward, and leveling rules stay the same.

| Statistic | Activity choices and amount units |
|---|---|
| Intellect | Reading (pages), Study (minutes), Coding (minutes), Problems solved (problems) |
| Stamina | Running (km), Push-ups (reps), Pull-ups (reps), Squats (reps) |
| Discipline | Meditation (minutes), Journaling (entries), Focus session (minutes), Early wake-up (days) |
| Social Skills | Conversations (conversations), Calls or meetups (meetups), Acts of help (acts), Group activities (activities) |
| Creativity | Drawing (minutes), Writing (words), Music practice (minutes), Crafts made (projects) |

The graphical interface also includes a dashboard, quest board, player profile, activity log, reward shop, and developer page. A simple portrait avatar and the player KSM ID appear in the interface. The GUI uses Arial from the standard Windows Fonts folder when available and falls back to Raylib’s default font when Arial is not present.

### Quest Difficulty and Rewards

| Difficulty | Experience Points |
|---|---:|
| Easy | 10 XP |
| Medium | 20 XP |
| Hard | 40 XP |

Users can filter quests according to their associated statistics or display only incomplete quests.

## 4.5 Experience Points and Level System

Players earn experience points by completing quests.

The experience required to reach the next level depends on the player's current level.

The formula is:

**Required XP = Current Level × 100**

For example:

| Current Level | Required XP |
|---|---:|
| 1 | 100 XP |
| 2 | 200 XP |
| 3 | 300 XP |
| 4 | 400 XP |

When the player earns enough XP, the program automatically increases their level.

The system also provides a 20% class bonus and a daily inspiration bonus when applicable.

## 4.6 Gold and Reward Shop

Players earn virtual gold by completing quests.

The amount of gold earned is normally half of the XP awarded. A special daily event can double the gold earned.

The reward shop provides three virtual rewards:

| Reward | Price |
|---|---:|
| 1 Hour Free Time | 50 Gold |
| Favorite Snack | 100 Gold |
| Day Off / Sleep In | 250 Gold |

Players can purchase a reward if they have sufficient gold.

## 4.7 Daily Streak System

The daily streak system encourages users to maintain consistency.

The program checks the number of consecutive days on which the player has maintained their streak.

- The streak increases when the next consecutive day is detected.
- The streak resets if a day is missed.
- A bonus of 5 gold per streak day is awarded when completing a quest if the streak is greater than one.

## 4.8 Daily Fortune System

Every time a player logs in, the program randomly selects one of three daily events.

| Daily Fortune | Effect |
|---|---|
| Double Gold Day | Doubles gold earnings |
| Inspiration Strike | Provides an additional 5 XP |
| Balanced Energy | Standard progress |

This feature adds variety to the gaming experience.

## 4.9 Developer Mode

The program includes a developer mode protected by a PIN.

The developer PIN is:

`KSM-321`

Developer mode provides two options:

1. Add 500 XP and 250 Gold.
2. Max out all five player statistics to 100.

This feature is mainly intended for testing and demonstration purposes.

# 5. Technologies and Programming Concepts Used

The project was developed in C. The graphical version uses Raylib for drawing, text, input, and the game window; the console version uses standard C input and output.

The following programming concepts and libraries were used:

## 5.1 Header Files

| Header File | Purpose |
|---|---|
| stdio.h | Input and output operations |
| stdlib.h | Random number generation and general utilities |
| string.h | String manipulation and comparison |
| time.h | Date, time, and daily streak calculations |
| ctype.h | Character handling utilities in the console source |
| raylib.h | Window, drawing, font, and mouse/keyboard input for the GUI |

## 5.2 Programming Concepts

**Arrays:** Used to store account information, quests, statistics, and activity logs.

**Functions:** Used to divide the program into smaller modules and make the code easier to understand and maintain.

**Structures:** The project uses parallel arrays instead of structures to store player and quest information.

**Conditional Statements:** Used for login verification, menu selection, quest validation, rewards, and level progression.

**Loops:** Used to display menus, process quests, search accounts, and calculate progress.

**Strings:** Used to manage player names, quest names, account IDs, and activity messages.

**File Handling:** Used to save and load account information and player progress.

**Random Number Generation:** Used to generate KSM IDs and daily fortunes.

**Time Functions:** Used to calculate daily streaks and detect weekly resets.

# 6. File Handling and Data Storage

One of the important features of this project is permanent data storage.

The program uses text files to store account information and individual player progress.

### accounts.txt

This file stores the registered account IDs and player names.

Example:

`KSM-1234, Mamoon`

### Player Files

Each player has a separate file named according to their KSM ID.

For example:

`KSM-1234.txt`

The player file stores:

- Player ID and name.
- Current level and XP.
- Gold balance.
- Character class.
- Login time.
- Daily streak information.
- Player statistics.
- Discipline score.
- Weekly quests.
- Activity history.

The program uses functions such as `fopen()`, `fprintf()`, `fgets()`, and `fclose()` to manage these files.

# 7. Program Architecture and Main Functions

The program is divided into different functional modules.

| Function | Responsibility |
|---|---|
| main() | Starts the program and displays the main menu |
| createAccount() | Creates a new player account |
| loginPlayer() | Authenticates an existing player |
| playerHub() | Displays the player's main dashboard |
| viewProfile() | Displays player information and statistics |
| printTitle() | Determines the player's title |
| createQuest() | Creates a new quest |
| printQuestBoard() | Displays and filters weekly quests |
| completeQuest() | Completes quests and awards rewards |
| questMenu() | Controls the quest management menu |
| addXp() | Calculates XP, gold, and level progression |
| rewardShop() | Allows players to purchase virtual rewards |
| refreshDailyStreak() | Updates the daily streak |
| developerMode() | Provides developer testing options |
| savePlayer() | Saves player information |
| loadPlayer() | Loads saved player information |
| saveAccounts() | Saves registered accounts |
| loadAccounts() | Loads registered accounts |
| addLog() | Stores activity messages |
| pressEnter() | Pauses the console version until Enter is pressed |

The Raylib entry point is `main()` in `life_rpg_raylib.c`. It loads the account list, draws the selected screen each frame, routes keyboard input, and saves player data on exit. Drawing helpers such as `DrawPanel()`, `Button()`, `SelectButton()`, and `DrawUI()` keep common interface elements in one place. `run.bat` builds this source with GCC and Raylib, then starts `LifeRPG.exe`.

# 8. Contribution of Team Members

The project was divided into different modules so that each contributor could work on a specific part of the system.

## 8.1 Mamoon – Main Program and Helper Functions

Responsibilities:

- Developed the main program structure.
- Implemented the main menu.
- Integrated the different modules.
- Developed helper functions such as `trimString()` and `pressEnter()`.
- Implemented the daily streak refresh logic.
- Managed the overall program flow.

## 8.2 Khadija – File Handling, Profile and Quest Management

Responsibilities:

- Implemented account and player data storage.
- Developed file loading and saving functions.
- Implemented the player profile and title system.
- Developed quest creation and quest board functionality.
- Implemented quest completion and progress tracking.
- Developed the reward shop.

## 8.3 Sajid – XP System, Developer Mode and Player Hub

Responsibilities:

- Developed the experience point and level progression system.
- Implemented class-based XP bonuses.
- Developed the developer mode.
- Implemented the player hub.
- Developed the login and account creation modules.
- Integrated player navigation and reward-related functionality.

**Note:** These responsibilities are based on the module comments and function organization in the submitted source code. They should be adjusted if the actual work distribution was different.

# 9. Program Workflow

The Raylib workflow is:

1. Run `run.bat` to build and start the graphical game.
2. The login screen loads account names from `accounts.txt`.
3. Create an account and select a character class, or choose an account and enter its KSM ID.
4. Use the sidebar to open the dashboard, quests, profile, activity log, reward shop, or developer page.
5. On the quest screen, choose a day and statistic, select one of that statistic’s four activities, enter an amount, and choose a difficulty.
6. Complete a quest to receive the same XP, gold, and statistic updates as before.
7. Save or log out; player files retain the account’s progress.

The separate console version continues to use a text menu and free-form quest names.

# 10. Testing and Expected Results

The following test cases can be used to evaluate the program.

| Test Case | Expected Result |
|---|---|
| Create a new account | Account is created with a KSM ID |
| Login using correct KSM ID | Player enters the player hub |
| Login using incorrect KSM ID | Error message is displayed |
| Create a new quest | Quest is added to the quest board |
| Complete a valid quest | XP, gold, and statistics are updated |
| Complete an already completed quest | Already completed message appears |
| Complete an invalid quest | Invalid choice message appears |
| Earn sufficient XP | Player levels up |
| Purchase a reward with sufficient gold | Reward is purchased |
| Purchase a reward without sufficient gold | Not enough Gold message appears |
| Use incorrect developer PIN | Access is denied |
| Save and log in again | Previously saved progress is loaded |

# 11. Challenges Faced During Development

During development, several programming challenges had to be considered:

1. **Data Management:** Managing multiple players and their individual progress using arrays required careful organization.

2. **File Handling:** Saving and retrieving player information required a consistent file format.

3. **XP and Level Calculation:** The system needed to calculate experience points and automatically handle level progression.

4. **Quest Management:** Each quest required separate information, including its name, day, statistic, reward, and completion status.

5. **Time Management:** Daily streaks and weekly resets required the use of time-related functions.

6. **Module Integration:** Combining different contributors' modules into one working program required consistent global variables and function declarations.

# 12. Limitations and Future Improvements

Although the project provides several useful features, there are opportunities for improvement.

### Current Limitations

- The Raylib version targets Windows and expects a compatible GCC and Raylib installation. `run.bat` supports the standard install path and a custom `RAYLIB_DIR`.
- Player data is stored in text files.
- The KSM ID is used as a password without additional encryption.
- Input validation can be improved.
- The current program uses fixed-size arrays.
- The older console source does not reliably load quest names that contain spaces.

### Future Improvements

The project can be enhanced by adding:

1. A portable font and clearer support for additional operating systems.
2. Stronger account authentication and password protection.
3. More character classes and special abilities.
4. Achievement badges and trophies.
5. Monthly and yearly progress reports.
6. More detailed activity statistics.
7. A leaderboard for comparing player progress.
8. Database integration for improved data management.
9. More advanced quest categories and customizable rewards.

# 13. Learning Outcomes

Through this project, the team gained practical experience in:

- Writing modular programs in C.
- Using arrays and strings.
- Developing and calling functions.
- Implementing conditional statements and loops.
- Working with file handling.
- Managing multiple user accounts.
- Using time and random number functions.
- Debugging and integrating different modules.
- Applying programming concepts to a practical real-world problem.

# 14. Conclusion

LIFE RPG – Level Up Your Real Life is a personal productivity game developed in C, with a Raylib graphical version and a separate console version.

The project combines everyday task management with role-playing game elements such as character classes, quests, experience points, gold, levels, statistics, daily streaks, and rewards.

It demonstrates the practical implementation of fundamental programming concepts, including arrays, functions, strings, loops, conditional statements, and file handling.

The project also provided an opportunity for teamwork, modular programming, problem-solving, and software integration.

The Raylib version adds a dashboard, a clear Arial-based interface, a KSM ID header, a simple avatar, and 20 predefined activity choices with amount units. The existing quest rewards, XP progression, and player data format remain in use. Overall, LIFE RPG demonstrates how programming can support consistency, productivity, and personal development.

---

# 15. References

1. C Programming Language – Standard Library Documentation.
2. Programming Fundamentals Course Materials.
3. C Standard Library Reference – File Handling and String Functions.
4. Project Source Code – LIFE RPG: Level Up Your Real Life.
