
#include <raylib.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ================= CONSTANTS ================= */

#define MAX_ACCOUNTS 500
#define MAX_QUESTS 50
#define MAX_LOGS 200
#define NAME_LEN 64
#define LOG_LEN 128

#define SCREEN_WIDTH 1100
#define SCREEN_HEIGHT 750

/* ================= COLORS ================= */

#define BG ((Color){18, 25, 37, 255})
#define PANEL ((Color){29, 39, 53, 255})
#define GREEN_COLOR ((Color){110, 210, 175, 255})
#define GOLD_COLOR ((Color){255, 205, 100, 255})

/* ================= GAME DATA ================= */

/* Accounts */

char accountIds[MAX_ACCOUNTS][16];
char accountNames[MAX_ACCOUNTS][64];

int accountCount = 0;

/* Current player */

char curId[16] = "";
char curName[64] = "";

int curLevel = 1;
int curXp = 0;
int curGold = 0;

int curClass = 0;

int curStats[5] = {10, 10, 10, 10, 10};

int curDiscipline = 0;

int dailyStreak = 1;
long lastStreakDay = -1;

long lastLoginTime = 0;

char dailyFortune[128] = "Normal Day";

/* Quests */

int questCount = 0;

char questName[MAX_QUESTS][64];

int questDay[MAX_QUESTS];
int questStat[MAX_QUESTS];
int questReward[MAX_QUESTS];
int questDone[MAX_QUESTS];

/* Activity */

int logCount = 0;

char logText[MAX_LOGS][LOG_LEN];

/* Labels */

const char *days[7] = {
    "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday", "Sunday"
};

const char *statNames[5] = {
    "Intellect",
    "Stamina",
    "Discipline",
    "Social Skills",
    "Creativity"
};

const char *classNames[3] = {
    "Scholar",
    "Athlete",
    "Artist"
};

/* ================= GUI VARIABLES ================= */

int screen = 0;
/*
    0 = Login
    1 = Create Account
    2 = Game
*/

/* ================= GAME ANIMATIONS ================= */

#define MAX_PARTICLES 70

typedef struct
{
    float x, y;
    float speed;
    float radius;
    float alpha;
} Particle;

Particle particles[MAX_PARTICLES];

float gameTime = 0.0f;
float animatedXP = 0.0f;
float transitionAlpha = 0.0f;
float buttonScale = 1.0f;
int page = 0;
/*
    0 = Dashboard
    1 = Quests
    2 = Profile
    3 = Activity
    4 = Reward Shop
    5 = Developer Mode
*/

int selectedAccount = -1;
int selectedQuest = -1;

int selectedStat = 0;
int selectedDay = 0;
int selectedDifficulty = 0;

int selectedClass = 0;

int activeField = 0;

int saveMessageTimer = 0;

char nameInput[64] = "";
char pinInput[16] = "";
char questInput[64] = "";
char developerPin[16] = "";

int developerUnlocked = 0;

/* ================= HELPER FUNCTIONS ================= */

void trimString(char *str)
{
    int len = strlen(str);

    while (len > 0 &&
          (str[len - 1] == '\n' ||
           str[len - 1] == '\r'))
    {
        str[len - 1] = '\0';
        len--;
    }
}

void addLog(const char *msg)
{
    if (logCount < MAX_LOGS)
    {
        snprintf(logText[logCount], LOG_LEN, "%s", msg);
        logCount++;
    }
    else
    {
        for (int i = 0; i < MAX_LOGS - 1; i++)
        {
            strcpy(logText[i], logText[i + 1]);
        }

        snprintf(logText[MAX_LOGS - 1], LOG_LEN, "%s", msg);
    }
}

/* Text input */

void readText(char *buffer, int maxLength)
{
    int key = GetCharPressed();

    while (key > 0)
    {
        if (key >= 32 && key <= 125 &&
            strlen(buffer) < maxLength - 1)
        {
            int len = strlen(buffer);

            buffer[len] = (char)key;
            buffer[len + 1] = '\0';
        }

        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE))
    {
        int len = strlen(buffer);

        if (len > 0)
        {
            buffer[len - 1] = '\0';
        }
    }
}

/* Button */

int Button(int x, int y, int width, int height, const char *text)
{
    Rectangle rect = {
        (float)x,
        (float)y,
        (float)width,
        (float)height
    };

    Vector2 mouse = GetMousePosition();

    int hover = CheckCollisionPointRec(mouse, rect);

    float scale = hover ? 1.04f : 1.0f;
    float newWidth = width * scale;
    float newHeight = height * scale;

    Rectangle animatedRect = {
        x - (newWidth - width) / 2.0f,
        y - (newHeight - height) / 2.0f,
        newWidth,
        newHeight
    };

    Color buttonColor = hover ?
        (Color){45, 110, 100, 255} :
        (Color){29, 43, 58, 255};

    DrawRectangleRounded(
        animatedRect,
        0.25f,
        10,
        buttonColor
    );

    DrawRectangleRoundedLines(
        animatedRect,
        0.25f,
        10,
        hover ? GOLD_COLOR : GREEN_COLOR
    );

    if (hover)
    {
        DrawRectangleRoundedLines(
            (Rectangle){
                animatedRect.x - 2,
                animatedRect.y - 2,
                animatedRect.width + 4,
                animatedRect.height + 4
            },
            0.25f,
            10,
            (Color){110, 210, 175, 90}
        );
    }

    int fontSize = 17;
    int textWidth = MeasureText(text, fontSize);

    DrawText(
        text,
        animatedRect.x +
        (animatedRect.width - textWidth) / 2.0f,
        animatedRect.y +
        (animatedRect.height - fontSize) / 2.0f,
        fontSize,
        RAYWHITE
    );

    return hover &&
           IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

/* Input box */

void InputBox(int x, int y, int width,
              const char *label,
              char *buffer,
              int active)
{
    DrawText(label, x, y - 27, 17, LIGHTGRAY);

    DrawRectangle(
        x, y, width, 42,
        PANEL
    );

    DrawRectangleLines(
        x, y, width, 42,
        active ? GREEN_COLOR : GRAY
    );

    DrawText(
        buffer,
        x + 10, y + 11,
        18, RAYWHITE
    );
}

/* Attribute bar */

void DrawStatBar(int x, int y,
                 const char *name, int value)
{
    if (value < 0) value = 0;
    if (value > 100) value = 100;

    DrawText(name, x, y, 17, RAYWHITE);

    DrawRectangle(
        x + 180, y + 2,
        250, 16, PANEL
    );

    DrawRectangle(
        x + 180, y + 2,
        value * 250 / 100,
        16, GREEN_COLOR
    );

    DrawText(
        TextFormat("%d/100", value),
        x + 445, y,
        16, LIGHTGRAY
    );
}

/* ================= SAVING ================= */

void saveAccounts(void)
{
    FILE *f = fopen("accounts.txt", "w");

    if (f == NULL) return;

    for (int i = 0; i < accountCount; i++)
    {
        fprintf(
            f, "%s, %s\n",
            accountIds[i],
            accountNames[i]
        );
    }

    fclose(f);
}

void loadAccounts(void)
{
    FILE *f = fopen("accounts.txt", "r");

    if (f == NULL) return;

    char line[256];

    accountCount = 0;

    while (fgets(line, sizeof(line), f) &&
           accountCount < MAX_ACCOUNTS)
    {
        trimString(line);

        char *comma = strchr(line, ',');

        if (comma != NULL)
        {
            *comma = '\0';

            char *id = line;
            char *name = comma + 1;

            while (*name == ' ') name++;

            snprintf(
                accountIds[accountCount],
                sizeof(accountIds[accountCount]),
                "%s", id
            );

            snprintf(
                accountNames[accountCount],
                sizeof(accountNames[accountCount]),
                "%s", name
            );

            accountCount++;
        }
    }

    fclose(f);
}

void refreshDailyStreak(void)
{
    long now = time(NULL);
    long currentDay = now / 86400L;

    if (lastStreakDay < 0)
    {
        lastStreakDay = currentDay;
        dailyStreak = 1;
        return;
    }

    if (currentDay == lastStreakDay)
    {
        return;
    }

    if (currentDay == lastStreakDay + 1)
    {
        dailyStreak++;
        addLog("Daily streak increased.");
    }
    else
    {
        dailyStreak = 1;
        addLog("Your streak reset. Keep building momentum!");
    }

    lastStreakDay = currentDay;
}

void savePlayer(void)
{
    char filename[32];

    snprintf(filename, sizeof(filename), "%s.txt", curId);

    FILE *f = fopen(filename, "w");

    if (f == NULL) return;

    fprintf(f, "NAME: %s\n", curName);
    fprintf(f, "LEVEL: %d\n", curLevel);
    fprintf(f, "XP: %d\n", curXp);
    fprintf(f, "GOLD: %d\n", curGold);
    fprintf(f, "CLASS: %d\n", curClass);
    fprintf(f, "LAST_LOGIN: %ld\n", lastLoginTime);
    fprintf(f, "DAILY_STREAK: %d\n", dailyStreak);
    fprintf(f, "LAST_STREAK_DAY: %ld\n", lastStreakDay);

    for (int i = 0; i < 5; i++)
    {
        fprintf(f, "STAT%d: %d\n", i, curStats[i]);
    }

    fprintf(f, "DISCIPLINE: %d\n", curDiscipline);

    fprintf(f, "[QUESTS]\n");

    for (int i = 0; i < questCount; i++)
    {
        fprintf(
            f, "QUEST|%d|%d|%d|%d|%s\n",
            questDay[i],
            questStat[i],
            questReward[i],
            questDone[i],
            questName[i]
        );
    }

    fprintf(f, "[ACTIVITY]\n");

    for (int i = 0; i < logCount; i++)
    {
        fprintf(f, "%s\n", logText[i]);
    }

    fclose(f);
}

void loadPlayer(const char *id)
{
    char filename[32];

    snprintf(filename, sizeof(filename), "%s.txt", id);

    strcpy(curId, id);

    curLevel = 1;
    curXp = 0;
    curGold = 0;
    curClass = 0;
    curDiscipline = 0;

    lastLoginTime = time(NULL);

    questCount = 0;
    logCount = 0;

    for (int i = 0; i < 5; i++)
    {
        curStats[i] = 10;
    }

    FILE *f = fopen(filename, "r");

    if (f != NULL)
    {
        char line[256];
        int section = 0;

        while (fgets(line, sizeof(line), f))
        {
            trimString(line);

            if (strcmp(line, "[QUESTS]") == 0)
            {
                section = 1;
                continue;
            }

            if (strcmp(line, "[ACTIVITY]") == 0)
            {
                section = 2;
                continue;
            }

            if (section == 0)
            {
                if (strncmp(line, "NAME: ", 6) == 0)
                    snprintf(curName, sizeof(curName), "%s", line + 6);

                sscanf(line, "LEVEL: %d", &curLevel);
                sscanf(line, "XP: %d", &curXp);
                sscanf(line, "GOLD: %d", &curGold);
                sscanf(line, "CLASS: %d", &curClass);
                sscanf(line, "LAST_LOGIN: %ld", &lastLoginTime);
                sscanf(line, "DAILY_STREAK: %d", &dailyStreak);
                sscanf(line, "LAST_STREAK_DAY: %ld", &lastStreakDay);

                for (int i = 0; i < 5; i++)
                {
                    char key[16];

                    sprintf(key, "STAT%d: ", i);

                    if (strncmp(line, key, strlen(key)) == 0)
                    {
                        curStats[i] = atoi(line + strlen(key));
                    }
                }

                sscanf(line, "DISCIPLINE: %d", &curDiscipline);
            }

            else if (section == 1 &&
                     questCount < MAX_QUESTS)
            {
                int d, s, r, done;
                char qn[64];

                if (sscanf(
                    line,
                    "QUEST|%d|%d|%d|%d|%63[^\n]",
                    &d, &s, &r, &done, qn
                ) == 5)
                {
                    questDay[questCount] = d;
                    questStat[questCount] = s;
                    questReward[questCount] = r;
                    questDone[questCount] = done;

                    strcpy(questName[questCount], qn);

                    questCount++;
                }
            }

            else if (section == 2 &&
                     logCount < MAX_LOGS &&
                     strlen(line) > 0)
            {
                strcpy(logText[logCount], line);
                logCount++;
            }
        }

        fclose(f);
    }

    if (curClass < 0 || curClass > 2)
        curClass = 0;

    if (curLevel < 1)
        curLevel = 1;

    if (dailyStreak < 1)
        dailyStreak = 1;

    if (curXp < 0)
        curXp = 0;

    refreshDailyStreak();

    for (int i = 0; i < 5; i++)
    {
        if (curStats[i] < 0) curStats[i] = 0;
        if (curStats[i] > 100) curStats[i] = 100;
    }

    /* Daily fortune */

    int fortuneRoll = rand() % 3;

    if (fortuneRoll == 0)
    {
        strcpy(dailyFortune, "Double Gold Day!");
    }
    else if (fortuneRoll == 1)
    {
        strcpy(dailyFortune, "Inspiration Strike!");
    }
    else
    {
        strcpy(dailyFortune, "Balanced Energy");
    }

    /* Weekly reset */

    long currentTime = time(NULL);

    if (currentTime - lastLoginTime > 604800)
    {
        for (int i = 0; i < questCount; i++)
        {
            questDone[i] = 0;
        }

        addLog("Weekly quest reset triggered.");
    }

    lastLoginTime = currentTime;
}

/* ================= GAME ACTIONS ================= */

void addXp(int amount, int statIndex)
{
    if (statIndex < 0 || statIndex > 4)
        return;

    /* Class bonus */

    if ((curClass == 0 && statIndex == 0) ||
        (curClass == 1 && statIndex == 1) ||
        (curClass == 2 && statIndex == 4))
    {
        amount += amount / 5;
        addLog("Class XP bonus activated.");
    }

    /* Daily event bonus */

    if (strcmp(dailyFortune, "Inspiration Strike!") == 0)
    {
        amount += 5;
    }

    curXp += amount;

    int goldEarned = amount / 2;

    if (strcmp(dailyFortune, "Double Gold Day!") == 0)
    {
        goldEarned *= 2;
    }

    curGold += goldEarned;

    int requiredXP = curLevel * 100;

    while (curXp >= requiredXP)
    {
        curXp -= requiredXP;
        curLevel++;

        addLog("Level up!");

        requiredXP = curLevel * 100;
    }
}

void completeQuest(int index)
{
    if (index < 0 || index >= questCount)
        return;

    if (questDone[index])
        return;

    questDone[index] = 1;

    curDiscipline++;

    curStats[questStat[index]] += 1;

    if (curStats[questStat[index]] > 100)
        curStats[questStat[index]] = 100;

    int streakBonus = dailyStreak > 1 ? dailyStreak * 5 : 0;

    if (streakBonus > 0)
    {
        curGold += streakBonus;
        addLog(TextFormat("Streak bonus! +%d gold.", streakBonus));
    }

    addXp(questReward[index], questStat[index]);

    addLog("Completed a quest!");

    savePlayer();
}

void createNewAccount(void)
{
    if (accountCount >= MAX_ACCOUNTS)
        return;

    if (strlen(nameInput) == 0)
        return;

    int r;
    char newId[16];
    int exists;

    do
    {
        r = 1000 + rand() % 9000;

        sprintf(newId, "KSM-%04d", r);

        exists = 0;

        for (int i = 0; i < accountCount; i++)
        {
            if (strcmp(accountIds[i], newId) == 0)
            {
                exists = 1;
                break;
            }
        }

    } while (exists);

    strcpy(accountIds[accountCount], newId);
    strcpy(accountNames[accountCount], nameInput);

    accountCount++;

    saveAccounts();

    loadPlayer(newId);

    strcpy(curName, nameInput);

    curClass = selectedClass;

    addLog("Welcome to Life RPG!");

    savePlayer();

    nameInput[0] = '\0';

    screen = 2;
    page = 0;
}

int loginAccount(void)
{
    if (selectedAccount < 0 ||
        selectedAccount >= accountCount)
        return 0;

    if (strcmp(pinInput, accountIds[selectedAccount]) == 0)
    {
        loadPlayer(accountIds[selectedAccount]);

        strcpy(curName, accountNames[selectedAccount]);

        addLog("Player logged in.");

        savePlayer();

        pinInput[0] = '\0';

        screen = 2;
        page = 0;

        return 1;
    }

    return 0;
}

/* ================= TITLE ================= */

const char *getTitle(void)
{
    int highest = 0;

    for (int i = 1; i < 5; i++)
    {
        if (curStats[i] > curStats[highest])
            highest = i;
    }

    if (curLevel < 3)
        return "Novice Adventurer";

    if (highest == 0)
        return "Grand Scholar";

    if (highest == 1)
        return "Iron Athlete";

    if (highest == 2)
        return "Master of Discipline";

    if (highest == 3)
        return "Charismatic Diplomat";

    return "Visionary Creator";
}

/* ================= LOGIN SCREEN ================= */

void drawLoginScreen(void)
{
    DrawText("LIFE RPG", 390, 65, 48, GREEN_COLOR);

    DrawText(
        "Turn your daily goals into quests",
        385, 120, 19, LIGHTGRAY
    );

    DrawText(
        "LOGIN TO YOUR ACCOUNT",
        390, 190, 23, RAYWHITE
    );

    DrawText(
        "Registered Players:",
        390, 235, 17, LIGHTGRAY
    );

    if (accountCount == 0)
    {
        DrawText(
            "No accounts found. Create an account first.",
            390, 275, 17, GOLD_COLOR
        );
    }

    for (int i = 0; i < accountCount && i < 8; i++)
    {
        int y = 275 + i * 43;

        if (Button(
            390, y, 320, 36,
            accountNames[i]
        ))
        {
            selectedAccount = i;
        }

        if (selectedAccount == i)
        {
            DrawRectangleLines(
                385, y - 3, 330, 42,
                GREEN_COLOR
            );
        }
    }

    if (selectedAccount >= 0 &&
        selectedAccount < accountCount)
    {
        InputBox(
            390, 635, 320,
            "Enter your KSM ID",
            pinInput, 1
        );

        if (Button(390, 690, 150, 40, "Login"))
        {
            if (!loginAccount())
            {
                addLog("Incorrect login attempt.");
            }
        }
    }

    if (Button(735, 275, 200, 45, "Create Account"))
    {
        screen = 1;
        nameInput[0] = '\0';
    }
}

/* ================= CREATE ACCOUNT SCREEN ================= */

void drawCreateScreen(void)
{
    DrawText(
        "CREATE NEW ACCOUNT",
        370, 90, 32, GREEN_COLOR
    );

    InputBox(
        370, 190, 360,
        "Player Name",
        nameInput, 1
    );

    DrawText(
        "Choose Your Character Class:",
        370, 285, 20, RAYWHITE
    );

    const char *classes[3] = {
        "Scholar",
        "Athlete",
        "Artist"
    };

    for (int i = 0; i < 3; i++)
    {
        int x = 370 + i * 125;

        if (Button(x, 330, 115, 45, classes[i]))
        {
            selectedClass = i;
        }

        if (selectedClass == i)
        {
            DrawRectangleLines(
                x - 2, 328, 119, 49,
                GREEN_COLOR
            );
        }
    }

    const char *descriptions[3] = {
        "Bonus XP for Intellect",
        "Bonus XP for Stamina",
        "Bonus XP for Creativity"
    };

    DrawText(
        descriptions[selectedClass],
        370, 405, 17, GOLD_COLOR
    );

    if (Button(370, 475, 165, 45, "Create Account"))
    {
        createNewAccount();
    }

    if (Button(550, 475, 165, 45, "Back to Login"))
    {
        screen = 0;
    }
}

/* ================= DASHBOARD ================= */

void drawPlayerCharacter(int x, int y)
{
    float bounce = sinf(gameTime * 3.0f) * 5.0f;
    float glow = 0.5f + 0.5f * sinf(gameTime * 2.0f);

    DrawCircle(
        x,
        y + 20,
        (int)(90 + glow * 8.0f),
        (Color){70, 200, 160, 18}
    );

    DrawCircleLines(
        x,
        y + 20,
        (int)(75 + glow * 8.0f),
        (Color){100, 220, 180, 100}
    );

    DrawEllipse(
        x,
        y + 100,
        45,
        10,
        (Color){0, 0, 0, 100}
    );

    DrawRectangleRounded(
        (Rectangle){
            x - 32,
            y + 5 + bounce,
            64,
            70
        },
        0.3f,
        8,
        (Color){55, 105, 150, 255}
    );

    DrawCircle(
        x,
        (int)(y - 18 + bounce),
        27,
        (Color){235, 190, 145, 255}
    );

    DrawCircle(
        x,
        (int)(y - 29 + bounce),
        27,
        (Color){45, 35, 35, 255}
    );

    DrawRectangle(
        x - 25,
        (int)(y - 20 + bounce),
        50,
        14,
        (Color){235, 190, 145, 255}
    );

    DrawCircle(
        x - 9,
        (int)(y - 17 + bounce),
        3,
        DARKGRAY
    );

    DrawCircle(
        x + 9,
        (int)(y - 17 + bounce),
        3,
        DARKGRAY
    );

    DrawCircle(
        x + 48,
        (int)(y - 55 + bounce),
        19,
        GOLD_COLOR
    );

    DrawText(
        TextFormat("%d", curLevel),
        (int)(x + 40),
        (int)(y - 64 + bounce),
        17,
        BLACK
    );
}

void DrawGameCard(
    int x,
    int y,
    int width,
    int height,
    const char *title)
{
    Rectangle card = {
        (float)x,
        (float)y,
        (float)width,
        (float)height
    };

    DrawRectangleRounded(
        card,
        0.08f,
        12,
        (Color){25, 36, 52, 245}
    );

    DrawRectangleRoundedLines(
        card,
        0.08f,
        12,
        (Color){70, 130, 120, 180}
    );

    DrawText(
        title,
        x + 20,
        y + 15,
        19,
        GREEN_COLOR
    );
}

void drawDashboard(void)
{
    DrawText(
        TextFormat("WELCOME BACK, %s", curName),
        35, 175, 27, RAYWHITE
    );

    DrawText(
        "Your journey continues...",
        38, 210, 17, LIGHTGRAY
    );

    DrawGameCard(35, 250, 480, 390, "PLAYER CHARACTER");
    drawPlayerCharacter(145, 420);

    DrawText(
        TextFormat("LEVEL %d", curLevel),
        240, 330, 27, GOLD_COLOR
    );

    DrawText(
        classNames[curClass],
        240, 375, 20, GREEN_COLOR
    );

    DrawText(
        getTitle(),
        240, 415, 17, LIGHTGRAY
    );

    DrawText(
        TextFormat("%d / %d XP", curXp, curLevel * 100),
        240, 465, 17, RAYWHITE
    );

    float targetXP =
        (float)curXp / (curLevel * 100);

    animatedXP +=
        (targetXP - animatedXP) * GetFrameTime() * 5.0f;

    if (animatedXP < 0.0f) animatedXP = 0.0f;
    if (animatedXP > 1.0f) animatedXP = 1.0f;

    DrawRectangle(240, 495, 230, 15, DARKGRAY);
    DrawRectangle(
        240, 495,
        (int)(230 * animatedXP),
        15,
        GREEN_COLOR
    );

    DrawText(
        TextFormat("%d GOLD", curGold),
        240, 535, 21, GOLD_COLOR
    );

    DrawGameCard(545, 250, 520, 250, "YOUR ATTRIBUTES");

    for (int i = 0; i < 5; i++)
    {
        DrawStatBar(
            570,
            295 + i * 38,
            statNames[i],
            curStats[i]
        );
    }

    DrawGameCard(545, 520, 520, 120, "QUEST PROGRESS");

    int completed = 0;

    for (int i = 0; i < questCount; i++)
    {
        if (questDone[i])
            completed++;
    }

    DrawText(
        TextFormat("Total: %d", questCount),
        570, 565, 18, RAYWHITE
    );

    DrawText(
        TextFormat("Completed: %d", completed),
        730, 565, 18, GREEN_COLOR
    );

    DrawText(
        TextFormat("Streak: %d days", dailyStreak),
        570, 600, 16, GOLD_COLOR
    );

    DrawText(
        TextFormat("Gold: %d", curGold),
        570, 630, 16, GOLD_COLOR
    );
}

/* ================= QUEST SCREEN ================= */

void drawQuestScreen(void)
{
    DrawText(
        "WEEKLY QUEST BOARD",
        40, 175, 27, GREEN_COLOR
    );

    DrawText(
        "Enter a quest name:",
        40, 230, 18, RAYWHITE
    );

    InputBox(
        40, 265, 360,
        "Quest Name",
        questInput, 1
    );

    DrawText(
        TextFormat("Attribute: %s (press 1-5)", statNames[selectedStat]),
        40, 335, 17, LIGHTGRAY
    );

    DrawText(
        TextFormat("Day: %s (press 0-6)", days[selectedDay]),
        40, 365, 17, LIGHTGRAY
    );

    DrawText(
        "Difficulty:",
        40, 405, 17, RAYWHITE
    );

    const char *difficulty[3] = {
        "Easy +10 XP",
        "Medium +20 XP",
        "Hard +40 XP"
    };

    for (int i = 0; i < 3; i++)
    {
        if (Button(
            40 + i * 140, 435, 130, 38,
            difficulty[i]
        ))
        {
            selectedDifficulty = i;
        }

        if (selectedDifficulty == i)
        {
            DrawRectangleLines(
                38 + i * 140, 433,
                134, 42, GREEN_COLOR
            );
        }
    }

    DrawText(
        "Keyboard: 1-5 selects attribute, 0-6 selects day",
        40, 490, 15, LIGHTGRAY
    );

    if (Button(40, 525, 160, 42, "Add Quest"))
    {
        if (strlen(questInput) > 0 &&
            questCount < MAX_QUESTS)
        {
            strcpy(
                questName[questCount],
                questInput
            );

            questDay[questCount] = selectedDay;
            questStat[questCount] = selectedStat;

            int rewards[3] = {10, 20, 40};

            questReward[questCount] =
                rewards[selectedDifficulty];

            questDone[questCount] = 0;

            questCount++;

            questInput[0] = '\0';

            addLog("Created a new quest.");

            savePlayer();
        }
    }

    if (Button(220, 525, 190, 42, "Complete Selected"))
    {
        completeQuest(selectedQuest);
    }

    DrawText(
        "YOUR QUESTS",
        500, 175, 22, GREEN_COLOR
    );

    for (int i = 0; i < questCount && i < 10; i++)
    {
        int y = 220 + i * 42;

        Rectangle row = {
            500, (float)y, 550, 36
        };

        int hover = CheckCollisionPointRec(
            GetMousePosition(), row
        );

        DrawRectangleRec(
            row,
            selectedQuest == i ?
            (Color){65, 85, 100, 255} :
            (hover ? (Color){45, 55, 70, 255} : PANEL)
        );

        if (CheckCollisionPointRec(
                GetMousePosition(), row) &&
            IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            selectedQuest = i;
        }

        DrawText(
            TextFormat(
                "%d. %s | +%d XP | %s",
                i + 1,
                questName[i],
                questReward[i],
                questDone[i] ? "DONE" : "OPEN"
            ),
            510, y + 9, 15,
            questDone[i] ? GREEN_COLOR : RAYWHITE
        );
    }

    if (questCount == 0)
    {
        DrawText(
            "No quests yet.",
            500, 230, 18, LIGHTGRAY
        );
    }

    if (questCount > 10)
    {
        DrawText(
            "Showing first 10 quests.",
            500, 660, 15, GRAY
        );
    }
}

/* ================= PROFILE SCREEN ================= */

void drawProfile(void)
{
    DrawText(
        "PLAYER PROFILE",
        40, 175, 28, GREEN_COLOR
    );

    DrawText(
        TextFormat("Name: %s", curName),
        40, 235, 20, RAYWHITE
    );

    DrawText(
        TextFormat("KSM ID: %s", curId),
        40, 275, 20, RAYWHITE
    );

    DrawText(
        TextFormat("Class: %s", classNames[curClass]),
        40, 315, 20, RAYWHITE
    );

    DrawText(
        TextFormat("Title: %s", getTitle()),
        40, 355, 20, GOLD_COLOR
    );

    DrawText(
        TextFormat("Level: %d", curLevel),
        40, 395, 20, RAYWHITE
    );

    DrawText(
        TextFormat("XP: %d / %d", curXp, curLevel * 100),
        40, 435, 20, RAYWHITE
    );

    DrawText(
        TextFormat("Gold: %d", curGold),
        40, 475, 20, GOLD_COLOR
    );

    DrawText(
        TextFormat("Discipline Score: %d", curDiscipline),
        40, 515, 20, RAYWHITE
    );

    DrawText(
        "ATTRIBUTE SCORES",
        600, 190, 23, GREEN_COLOR
    );

    for (int i = 0; i < 5; i++)
    {
        DrawStatBar(
            600, 245 + i * 55,
            statNames[i], curStats[i]
        );
    }
}

/* ================= ACTIVITY SCREEN ================= */

void drawActivity(void)
{
    DrawText(
        "ACTIVITY LOG",
        40, 175, 28, GREEN_COLOR
    );

    if (logCount == 0)
    {
        DrawText(
            "No activity recorded.",
            45, 230, 18, LIGHTGRAY
        );
    }

    for (int i = 0; i < logCount && i < 18; i++)
    {
        DrawText(
            TextFormat(
                "%02d. %s",
                i + 1,
                logText[i]
            ),
            45, 225 + i * 27,
            17, RAYWHITE
        );
    }
}

/* ================= REWARD SHOP ================= */

void drawRewardShop(void)
{
    const char *items[3] = {
        "1 Hour Free Time",
        "Favorite Snack",
        "Day Off / Sleep In"
    };

    int prices[3] = {50, 100, 250};

    DrawText(
        "REWARD SHOP",
        40, 175, 28, GREEN_COLOR
    );

    DrawText(
        TextFormat("Your Gold: %d", curGold),
        40, 230, 21, GOLD_COLOR
    );

    for (int i = 0; i < 3; i++)
    {
        int y = 300 + i * 100;

        DrawRectangle(
            40, y, 650, 75,
            PANEL
        );

        DrawText(
            items[i],
            60, y + 25,
            19, RAYWHITE
        );

        DrawText(
            TextFormat("%d Gold", prices[i]),
            400, y + 25,
            18, GOLD_COLOR
        );

        if (Button(550, y + 17, 120, 40, "Purchase"))
        {
            if (curGold >= prices[i])
            {
                curGold -= prices[i];

                addLog("Purchased a reward from the shop.");

                savePlayer();
            }
        }
    }
}

/* ================= DEVELOPER MODE ================= */

void drawDeveloper(void)
{
    DrawText(
        "DEVELOPER MODE",
        40, 175, 28, GREEN_COLOR
    );

    if (!developerUnlocked)
    {
        InputBox(
            40, 260, 300,
            "Developer PIN",
            developerPin, 1
        );

        if (Button(40, 330, 150, 42, "Unlock"))
        {
            if (strcmp(developerPin, "KSM-321") == 0)
            {
                developerUnlocked = 1;
                developerPin[0] = '\0';
            }
        }

        return;
    }

    DrawText(
        "Developer tools unlocked.",
        40, 250, 20, GOLD_COLOR
    );

    if (Button(40, 310, 280, 45, "Add 500 XP and 250 Gold"))
    {
        addXp(500, 0);

        curGold += 250;

        addLog("Developer reward activated.");

        savePlayer();
    }

    if (Button(40, 390, 280, 45, "Max Out All Stats"))
    {
        for (int i = 0; i < 5; i++)
        {
            curStats[i] = 100;
        }

        addLog("Developer max stats activated.");

        savePlayer();
    }

    if (Button(40, 470, 280, 45, "Lock Developer Mode"))
    {
        developerUnlocked = 0;
    }
}

/* ================= GAME NAVIGATION ================= */

void drawNavigation(void)
{
    DrawText(
        "LIFE RPG",
        35, 22, 34, GREEN_COLOR
    );

    DrawText(
        "Turn your daily goals into quests",
        38, 62, 17, LIGHTGRAY
    );

    if (Button(35, 105, 145, 42, "Dashboard"))
        page = 0;

    if (Button(190, 105, 145, 42, "Quests"))
        page = 1;

    if (Button(345, 105, 145, 42, "Profile"))
        page = 2;

    if (Button(500, 105, 145, 42, "Activity"))
        page = 3;

    if (Button(655, 105, 145, 42, "Reward Shop"))
        page = 4;

    if (Button(810, 105, 145, 42, "Developer"))
        page = 5;

    if (Button(970, 105, 95, 42, "Save"))
    {
        savePlayer();
        saveMessageTimer = 120;
    }
}

/* ================= MAIN GAME ================= */

void initializeParticles(void)
{
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        particles[i].x = (float)(rand() % SCREEN_WIDTH);
        particles[i].y = (float)(rand() % SCREEN_HEIGHT);
        particles[i].speed = (float)(15 + rand() % 35);
        particles[i].radius = (float)(1 + rand() % 3);
        particles[i].alpha = 0.3f + (rand() % 70) / 100.0f;
    }
}

void updateParticles(void)
{
    float delta = GetFrameTime();
    gameTime += delta;

    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        particles[i].y -= particles[i].speed * delta;
        particles[i].x += sinf(gameTime + i) * 12.0f * delta;

        if (particles[i].y < -10.0f)
        {
            particles[i].y = SCREEN_HEIGHT + 10.0f;
            particles[i].x = (float)(rand() % SCREEN_WIDTH);
        }
    }
}

void drawAnimatedBackground(void)
{
    ClearBackground((Color){12, 18, 32, 255});

    DrawCircleGradient(
        (Vector2){
            200 + sinf(gameTime * 0.5f) * 30.0f,
            150
        },
        280,
        (Color){30, 90, 85, 45},
        (Color){12, 18, 32, 0}
    );

    DrawCircleGradient(
        (Vector2){
            900,
            550 + cosf(gameTime * 0.4f) * 30.0f
        },
        300,
        (Color){50, 65, 130, 35},
        (Color){12, 18, 32, 0}
    );

    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        unsigned char alpha =
            (unsigned char)(particles[i].alpha * 180.0f);

        DrawCircleV(
            (Vector2){particles[i].x, particles[i].y},
            particles[i].radius,
            (Color){110, 220, 190, alpha}
        );
    }

    for (int x = 0; x < SCREEN_WIDTH; x += 55)
    {
        DrawLine(
            x, 0, x, SCREEN_HEIGHT,
            (Color){100, 150, 160, 12}
        );
    }

    for (int y = 0; y < SCREEN_HEIGHT; y += 55)
    {
        DrawLine(
            0, y, SCREEN_WIDTH, y,
            (Color){100, 150, 160, 12}
        );
    }
}

void drawGame(void)
{
    drawNavigation();

    if (page == 0)
        drawDashboard();

    else if (page == 1)
        drawQuestScreen();

    else if (page == 2)
        drawProfile();

    else if (page == 3)
        drawActivity();

    else if (page == 4)
        drawRewardShop();

    else if (page == 5)
        drawDeveloper();

    if (saveMessageTimer > 0)
    {
        DrawText(
            "Game saved!",
            920, 675, 18, GREEN_COLOR
        );

        saveMessageTimer--;
    }

    DrawText(
        "ESC: Logout",
        35, 690, 15, GRAY
    );

    if (IsKeyPressed(KEY_ESCAPE))
    {
        savePlayer();

        screen = 0;

        selectedAccount = -1;

        page = 0;

        developerUnlocked = 0;
        developerPin[0] = '\0';
    }
}

/* ================= MAIN FUNCTION ================= */

int main(void)
{
    srand((unsigned int)time(NULL));

    InitWindow(
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        "Life RPG - Personal Growth Game"
    );

    SetTargetFPS(60);
    initializeParticles();

    loadAccounts();

    while (!WindowShouldClose())
    {
        if (screen == 1)
        {
            readText(nameInput, NAME_LEN);
        }

        else if (screen == 0 && selectedAccount >= 0)
        {
            readText(pinInput, 16);
        }

        else if (screen == 2)
        {
            if (page == 1)
            {
                readText(questInput, 64);

                if (IsKeyPressed(KEY_ONE)) selectedStat = 0;
                if (IsKeyPressed(KEY_TWO)) selectedStat = 1;
                if (IsKeyPressed(KEY_THREE)) selectedStat = 2;
                if (IsKeyPressed(KEY_FOUR)) selectedStat = 3;
                if (IsKeyPressed(KEY_FIVE)) selectedStat = 4;

                if (IsKeyPressed(KEY_ZERO)) selectedDay = 0;
                if (IsKeyPressed(KEY_SIX)) selectedDay = 6;
            }

            else if (page == 5 && !developerUnlocked)
            {
                readText(developerPin, 16);
            }
        }

        updateParticles();

        BeginDrawing();

        drawAnimatedBackground();

        if (screen == 0)
        {
            drawLoginScreen();
        }

        else if (screen == 1)
        {
            drawCreateScreen();
        }

        else if (screen == 2)
        {
            drawGame();
        }

        EndDrawing();
    }

    if (strlen(curId) > 0)
    {
        savePlayer();
    }

    saveAccounts();

    CloseWindow();

    return 0;
}
