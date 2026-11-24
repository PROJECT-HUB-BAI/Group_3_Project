/* ============================================================
   LIFE RPG - Raylib GUI version
   Polished dashboard UI, same gameplay as console version.
   ============================================================ */

#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ================= CONSTANTS ================= */

#define MAX_ACCOUNTS 500
#define MAX_QUESTS   50
#define MAX_LOGS     200
#define NAME_LEN     64
#define LOG_LEN      128

#define SCREEN_WIDTH  1100
#define SCREEN_HEIGHT 750
#define SIDEBAR_W     200
#define TOPBAR_H      80
#define ARIAL_FONT_PATH "C:/Windows/Fonts/arial.ttf"

/* ================= COLORS ================= */

const Color BG = { 14, 19, 30, 255 };
const Color PANEL = { 26, 34, 48, 255 };
const Color PANEL_LIGHT = { 38, 50, 68, 255 };
const Color BORDER = { 55, 70, 92, 255 };
const Color ACCENT = { 80, 200, 170, 255 };
const Color ACCENT_DARK = { 45, 130, 112, 255 };
const Color COLOR_GOLD = { 255, 200, 100, 255 };
const Color TEXT = { 235, 240, 250, 255 };
const Color TEXT_DIM = { 150, 160, 180, 255 };

/* ================= GAME DATA ================= */

char accountIds  [MAX_ACCOUNTS][16];
char accountNames[MAX_ACCOUNTS][64];
int  accountCount = 0;

char curId  [16] = "";
char curName[64] = "";

int curLevel = 1;
int curXp    = 0;
int curGold  = 0;
int curClass = 0;

int  curStats[5]     = {10, 10, 10, 10, 10};
int  curDiscipline   = 0;
int  dailyStreak     = 1;
long lastStreakDay   = -1;
long lastLoginTime   = 0;
char dailyFortune[128] = "Normal Day";

int questCount = 0;
char questName  [MAX_QUESTS][64];
int  questDay   [MAX_QUESTS];
int  questStat  [MAX_QUESTS];
int  questReward[MAX_QUESTS];
int  questDone  [MAX_QUESTS];

int  logCount = 0;
char logText[MAX_LOGS][LOG_LEN];

const char *days[7] = {
    "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
};
const char *statNames[5] = {
    "Intellect", "Stamina", "Discipline", "Social Skills", "Creativity"
};
const char *activityOptions[5][4] = {
    { "Reading", "Study", "Coding", "Problems solved" },
    { "Running", "Push-ups", "Pull-ups", "Squats" },
    { "Meditation", "Journaling", "Focus session", "Early wake-up" },
    { "Conversations", "Calls or meetups", "Acts of help", "Group activities" },
    { "Drawing", "Writing", "Music practice", "Crafts made" }
};
const char *activityUnits[5][4] = {
    { "pages", "minutes", "minutes", "problems" },
    { "km", "reps", "reps", "reps" },
    { "minutes", "entries", "minutes", "days" },
    { "conversations", "meetups", "acts", "activities" },
    { "minutes", "words", "minutes", "projects" }
};
const char *classNames[3] = { "Scholar", "Athlete", "Artist" };
const char *classDescs[3] = {
    "Bonus XP for Intellect quests",
    "Bonus XP for Stamina quests",
    "Bonus XP for Creativity quests"
};

/* ================= UI STATE ================= */

int screen = 0;   // 0 login, 1 create, 2 game
int page   = 0;   // 0 dashboard, 1 quests, 2 profile, 3 activity, 4 shop, 5 developer

int selectedAccount  = -1;
int selectedQuest    = -1;
int selectedStat     = 0;
int selectedDay      = 0;
int selectedDifficulty = 0;
int selectedActivity = 0;
int selectedClass    = 0;

int developerUnlocked = 0;
int saveMessageTimer  = 0;
Font uiFont;
int arialFontLoaded = 0;

char nameInput[64]     = "";
char pinInput[16]      = "";
char amountInput[16]   = "";
char developerPin[16]  = "";

/* ============================================================
   SMALL HELPERS
   ============================================================ */

Color MakeColor(unsigned char red, unsigned char green,
                unsigned char blue, unsigned char alpha)
{
    Color color;
    color.r = red;
    color.g = green;
    color.b = blue;
    color.a = alpha;
    return color;
}

Rectangle MakeRectangle(float x, float y, float width, float height)
{
    Rectangle rectangle;
    rectangle.x = x;
    rectangle.y = y;
    rectangle.width = width;
    rectangle.height = height;
    return rectangle;
}

Vector2 MakeVector2(float x, float y)
{
    Vector2 point;
    point.x = x;
    point.y = y;
    return point;
}

void DrawUI(const char *text, int x, int y, int size, Color color)
{
    Vector2 position = MakeVector2((float)x, (float)y);
    DrawTextEx(uiFont, text, position, (float)size, 1.0f, color);
}

int MeasureUI(const char *text, int size)
{
    Vector2 measured = MeasureTextEx(uiFont, text, (float)size, 1.0f);
    return (int)measured.x;
}

void trimString(char *str)
{
    int len = (int)strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r'))
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

void readText(char *buffer, int maxLength)
{
    int key = GetCharPressed();
    while (key > 0)
    {
        if (key >= 32 && key <= 125 && (int)strlen(buffer) < maxLength - 1)
        {
            int len = (int)strlen(buffer);
            buffer[len]     = (char)key;
            buffer[len + 1] = '\0';
        }
        key = GetCharPressed();
    }
    if (IsKeyPressed(KEY_BACKSPACE))
    {
        int len = (int)strlen(buffer);
        if (len > 0)
        {
            buffer[len - 1] = '\0';
        }
    }
}

void readAmount(char *buffer, int maxLength)
{
    int key = GetCharPressed();
    int hasDecimal = strchr(buffer, '.') != NULL;
    while (key > 0)
    {
        if (key >= '0' && key <= '9' && (int)strlen(buffer) < maxLength - 1)
        {
            int len = (int)strlen(buffer);
            buffer[len] = (char)key;
            buffer[len + 1] = '\0';
        }
        else if (key == '.' && !hasDecimal && (int)strlen(buffer) < maxLength - 1)
        {
            int len = (int)strlen(buffer);
            buffer[len] = '.';
            buffer[len + 1] = '\0';
            hasDecimal = 1;
        }
        key = GetCharPressed();
    }
    if (IsKeyPressed(KEY_BACKSPACE))
    {
        int len = (int)strlen(buffer);
        if (len > 0)
        {
            buffer[len - 1] = '\0';
        }
    }
}
/* ============================================================
   DRAWING HELPERS
   ============================================================ */

void DrawPanel(Rectangle r, const char *title)
{
    DrawRectangleRounded(r, 0.02f, 8, PANEL);
    DrawRectangleRoundedLines(r, 0.02f, 8, BORDER);
    if (title != NULL)
    {
        DrawUI(title, (int)r.x + 22, (int)r.y + 18, 15, ACCENT);
        DrawLine(
            (int)r.x + 22, (int)r.y + 42,
            (int)(r.x + r.width) - 22, (int)r.y + 42,
            BORDER
        );
    }
}

int Button(Rectangle r, const char *text)
{
    Vector2 m = GetMousePosition();
    int hover = CheckCollisionPointRec(m, r);

    Color bg = PANEL_LIGHT;
    Color border = BORDER;
    if (hover)
    {
        bg = ACCENT_DARK;
        border = ACCENT;
    }

    DrawRectangleRounded(r, 0.25f, 8, bg);
    DrawRectangleRoundedLines(r, 0.25f, 8, border);

    int fs = 15;
    int tw = MeasureUI(text, fs);
    DrawUI(
        text,
        (int)(r.x + (r.width - tw) / 2),
        (int)(r.y + (r.height - fs) / 2),
        fs, TEXT
    );

    return hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

int SelectButton(Rectangle r, const char *text, int selected)
{
    Vector2 m = GetMousePosition();
    int hover = CheckCollisionPointRec(m, r);

    Color bg, border, textCol;
    if (selected)
    {
        bg = ACCENT_DARK; border = ACCENT;     textCol = TEXT;
    }
    else if (hover)
    {
        bg = PANEL_LIGHT; border = ACCENT;     textCol = TEXT;
    }
    else
    {
        bg = PANEL;       border = BORDER;     textCol = TEXT_DIM;
    }

    DrawRectangleRounded(r, 0.25f, 8, bg);
    DrawRectangleRoundedLines(r, 0.25f, 8, border);

    int fs = 14;
    while (fs > 10 && MeasureUI(text, fs) > r.width - 12) fs--;
    int tw = MeasureUI(text, fs);
    DrawUI(
        text,
        (int)(r.x + (r.width - tw) / 2),
        (int)(r.y + (r.height - fs) / 2),
        fs, textCol
    );

    return hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

void InputBox(Rectangle r, const char *label, char *text, int active)
{
    if (label != NULL)
    {
        DrawUI(label, (int)r.x, (int)r.y - 22, 13, TEXT_DIM);
    }
    DrawRectangleRounded(r, 0.3f, 6, PANEL);
    Color border = BORDER;
    if (active)
    {
        border = ACCENT;
    }
    DrawRectangleRoundedLines(r, 0.3f, 6, border);
    DrawUI(text, (int)r.x + 12, (int)(r.y + (r.height - 16) / 2), 16, TEXT);
}

void DrawProgressBar(Rectangle r, float pct, Color fill)
{
    if (pct < 0.0f)
    {
        pct = 0.0f;
    }
    if (pct > 1.0f)
    {
        pct = 1.0f;
    }

    DrawRectangleRounded(r, 0.5f, 4, PANEL_LIGHT);
    if (pct > 0.0f)
    {
        Rectangle f = { r.x, r.y, r.width * pct, r.height };
        DrawRectangleRounded(f, 0.5f, 4, fill);
    }
}

void DrawStatBar(int x, int y, int w, const char *name, int value)
{
    if (value < 0)
    {
        value = 0;
    }
    if (value > 100)
    {
        value = 100;
    }

    DrawUI(name, x, y, 15, TEXT);
    DrawUI(TextFormat("%d", value), x + w - 28, y, 15, ACCENT);

    Rectangle bar = { (float)x, (float)(y + 22), (float)w, 10.0f };
    DrawRectangleRounded(bar, 0.5f, 4, PANEL_LIGHT);
    if (value > 0)
    {
        Rectangle fill = {
            (float)x, (float)(y + 22),
            (float)(w * value / 100.0f), 10.0f
        };
        DrawRectangleRounded(fill, 0.5f, 4, ACCENT);
    }
}

int NavButton(int y, const char *text, int active)
{
    Rectangle r = { 15, (float)y, 170, 42 };
    Vector2 m = GetMousePosition();
    int hover = CheckCollisionPointRec(m, r);

    Color bg = BLANK;
    if (active)
    {
        bg = ACCENT_DARK;
    }
    else if (hover)
    {
        bg = PANEL_LIGHT;
    }
    if (active || hover)
    {
        DrawRectangleRounded(r, 0.25f, 8, bg);
    }
    if (active)
    {
        DrawRectangle(0, y, 4, 42, ACCENT);
    }

    Color c = TEXT_DIM;
    if (active || hover)
    {
        c = TEXT;
    }
    DrawUI(text, 30, y + 13, 16, c);

    return hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

/* ============================================================
   SAVE / LOAD
   ============================================================ */

void refreshDailyStreak(void)
{
    long currentDay = time(NULL) / 86400L;

    if (lastStreakDay < 0)
    {
        dailyStreak  = 1;
        lastStreakDay = currentDay;
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
        addLog("Daily streak reset.");
    }
    lastStreakDay = currentDay;
}

void loadAccounts(void)
{
    FILE *f = fopen("accounts.txt", "r");
    if (f == NULL)
    {
        return;
    }

    char line[256];
    accountCount = 0;

    while (fgets(line, sizeof(line), f) && accountCount < MAX_ACCOUNTS)
    {
        trimString(line);
        char *comma = strchr(line, ',');
        if (comma != NULL)
        {
            *comma = '\0';
            char *id   = line;
            char *name = comma + 1;
            while (*name == ' ') name++;

            snprintf(accountIds[accountCount],
                     sizeof(accountIds[accountCount]), "%s", id);
            snprintf(accountNames[accountCount],
                     sizeof(accountNames[accountCount]), "%s", name);
            accountCount++;
        }
    }
    fclose(f);
}

void saveAccounts(void)
{
    FILE *f = fopen("accounts.txt", "w");
    if (f == NULL)
    {
        return;
    }
    for (int i = 0; i < accountCount; i++)
    {
        fprintf(f, "%s, %s\n", accountIds[i], accountNames[i]);
    }
    fclose(f);
}

void savePlayer(void)
{
    char filename[32];
    snprintf(filename, sizeof(filename), "%s.txt", curId);

    FILE *f = fopen(filename, "w");
    if (f == NULL)
    {
        return;
    }

    fprintf(f, "ID: %s\n", curId);
    fprintf(f, "NAME: %s\n", curName);
    fprintf(f, "LEVEL: %d\n", curLevel);
    fprintf(f, "XP: %d\n", curXp);
    fprintf(f, "GOLD: %d\n", curGold);
    fprintf(f, "CLASS: %d\n", curClass);
    fprintf(f, "LAST_LOGIN: %ld\n", lastLoginTime);
    fprintf(f, "DAILY_STREAK: %d\n", dailyStreak);
    fprintf(f, "LAST_STREAK_DAY: %ld\n", lastStreakDay);

    fprintf(f, "INTELLECTUAL: %d\n", curStats[0]);
    fprintf(f, "STAMINA: %d\n", curStats[1]);
    fprintf(f, "DISCIPLINE: %d\n", curStats[2]);
    fprintf(f, "SOCIAL: %d\n", curStats[3]);
    fprintf(f, "CREATIVITY: %d\n", curStats[4]);
    fprintf(f, "DISCIPLINE_SCORE: %d\n", curDiscipline);

    fprintf(f, "[QUESTS]\n");
    for (int i = 0; i < questCount; i++)
    {
        fprintf(f, "QUEST|%d|%d|%d|%d|%s\n",
                questDay[i], questStat[i], questReward[i],
                questDone[i], questName[i]);
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

    /* Reset ALL per-player state first, including streak. */
    strcpy(curId, id);
    curName[0]      = '\0';
    curLevel        = 1;
    curXp           = 0;
    curGold         = 0;
    curClass        = 0;
    curDiscipline   = 0;
    dailyStreak     = 1;
    lastStreakDay   = -1;
    lastLoginTime   = time(NULL);
    questCount      = 0;
    logCount        = 0;
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

            if (strcmp(line, "[QUESTS]") == 0)   { section = 1; continue; }
            if (strcmp(line, "[ACTIVITY]") == 0) { section = 2; continue; }

            if (section == 0)
            {
                if (strncmp(line, "NAME: ", 6) == 0)
                    snprintf(curName, sizeof(curName), "%.*s", (int)sizeof(curName) - 1, line + 6);
                sscanf(line, "LEVEL: %d", &curLevel);
                sscanf(line, "XP: %d", &curXp);
                sscanf(line, "GOLD: %d", &curGold);
                sscanf(line, "CLASS: %d", &curClass);
                sscanf(line, "LAST_LOGIN: %ld", &lastLoginTime);
                sscanf(line, "DAILY_STREAK: %d", &dailyStreak);
                sscanf(line, "LAST_STREAK_DAY: %ld", &lastStreakDay);
                sscanf(line, "INTELLECTUAL: %d", &curStats[0]);
                sscanf(line, "STAMINA: %d", &curStats[1]);
                sscanf(line, "DISCIPLINE: %d", &curStats[2]);
                sscanf(line, "SOCIAL: %d", &curStats[3]);
                sscanf(line, "CREATIVITY: %d", &curStats[4]);
                sscanf(line, "DISCIPLINE_SCORE: %d", &curDiscipline);
            }
            else if (section == 1 && questCount < MAX_QUESTS)
            {
                int d, s, r, done;
                char qn[64];
                if (sscanf(line, "QUEST|%d|%d|%d|%d|%63[^\n]",
                           &d, &s, &r, &done, qn) == 5 &&
                    d >= 0 && d < 7 && s >= 0 && s < 5 &&
                    r >= 0 && r <= 1000 && (done == 0 || done == 1))
                {
                    questDay   [questCount] = d;
                    questStat  [questCount] = s;
                    questReward[questCount] = r;
                    questDone  [questCount] = done;
                    strcpy(questName[questCount], qn);
                    questCount++;
                }
            }
            else if (section == 2 && logCount < MAX_LOGS && strlen(line) > 0)
            {
                snprintf(logText[logCount], LOG_LEN, "%.*s", LOG_LEN - 1, line);
                logCount++;
            }
        }
        fclose(f);
    }

    /* Sanity clamps. */
    if (curClass < 0 || curClass > 2)
    {
        curClass = 0;
    }
    if (curLevel < 1)
    {
        curLevel = 1;
    }
    if (curXp < 0)
    {
        curXp = 0;
    }
    for (int i = 0; i < 5; i++)
    {
        if (curStats[i] < 0)
        {
            curStats[i] = 0;
        }
        if (curStats[i] > 100)
        {
            curStats[i] = 100;
        }
    }

    refreshDailyStreak();

    /* Random daily fortune. */
    int roll = rand() % 3;
    if (roll == 0)      strcpy(dailyFortune, "Double Gold Day!");
    else if (roll == 1) strcpy(dailyFortune, "Inspiration Strike!");
    else                strcpy(dailyFortune, "Balanced Energy");

    /* Weekly quest reset. */
    long now = time(NULL);
    if (now - lastLoginTime > 604800)
    {
        for (int i = 0; i < questCount; i++)
        {
            questDone[i] = 0;
        }
        addLog("Weekly quest reset.");
    }
    lastLoginTime = now;
}

/* ============================================================
   GAME ACTIONS
   ============================================================ */

void addXp(int amount, int statIndex)
{
    if (statIndex < 0 || statIndex > 4)
    {
        return;
    }

    /* Class bonus. */
    if ((curClass == 0 && statIndex == 0) ||
        (curClass == 1 && statIndex == 1) ||
        (curClass == 2 && statIndex == 4))
    {
        amount += amount / 5;
        addLog("Class XP bonus applied.");
    }

    /* Daily fortune bonus. */
    if (strcmp(dailyFortune, "Inspiration Strike!") == 0)
    {
        amount += 5;
    }

    curXp += amount;

    int gold = amount / 2;
    if (strcmp(dailyFortune, "Double Gold Day!") == 0)
    {
        gold *= 2;
    }
    curGold += gold;

    int req = curLevel * 100;
    while (curXp >= req)
    {
        curXp -= req;
        curLevel++;
        addLog("Level up!");
        req = curLevel * 100;
    }
}

void completeQuest(int index)
{
    if (index < 0 || index >= questCount)
    {
        return;
    }
    if (questStat[index] < 0 || questStat[index] >= 5)
    {
        return;
    }
    if (questDone[index])
    {
        return;
    }

    questDone[index] = 1;
    curDiscipline++;

    curStats[questStat[index]] += 1;
    if (curStats[questStat[index]] > 100)
    {
        curStats[questStat[index]] = 100;
    }

    if (dailyStreak > 1)
    {
        int bonus = dailyStreak * 5;
        curGold += bonus;
        addLog(TextFormat("Streak bonus +%d gold.", bonus));
    }

    addXp(questReward[index], questStat[index]);
    addLog("Quest completed.");
    savePlayer();
}

/* ============================================================
   LOGIN / CREATE / TITLE
   ============================================================ */

const char *getTitle(void)
{
    int highest = 0;
    for (int i = 1; i < 5; i++)
    {
        if (curStats[i] > curStats[highest])
        {
            highest = i;
        }
    }
    if (curLevel < 3)
    {
        return "Novice Adventurer";
    }
    if (highest == 0)
    {
        return "Grand Scholar";
    }
    if (highest == 1)
    {
        return "Iron Athlete";
    }
    if (highest == 2)
    {
        return "Master of Discipline";
    }
    if (highest == 3)
    {
        return "Charismatic Diplomat";
    }
    return "Visionary Creator";
}

int loginAccount(void)
{
    if (selectedAccount < 0 || selectedAccount >= accountCount)
    {
        return 0;
    }
    if (strcmp(pinInput, accountIds[selectedAccount]) != 0)
    {
        return 0;
    }

    loadPlayer(accountIds[selectedAccount]);
    strcpy(curName, accountNames[selectedAccount]);
    addLog("Player logged in.");
    savePlayer();

    /* Reset transient UI state. */
    pinInput[0]        = '\0';
    amountInput[0]      = '\0';
    developerPin[0]    = '\0';
    developerUnlocked  = 0;
    selectedQuest      = -1;
    saveMessageTimer   = 0;
    screen             = 2;
    page               = 0;
    return 1;
}

void createNewAccount(void)
{
    if (accountCount >= MAX_ACCOUNTS)
    {
        return;
    }
    if (strlen(nameInput) == 0)
    {
        return;
    }

    char newId[16];
    int  exists;
    do
    {
        int r = 1000 + rand() % 9000;
        sprintf(newId, "KSM-%04d", r);

        exists = 0;
        for (int i = 0; i < accountCount; i++)
        {
            if (strcmp(accountIds[i], newId) == 0) { exists = 1; break; }
        }
    } while (exists);

    strcpy(accountIds  [accountCount], newId);
    strcpy(accountNames[accountCount], nameInput);
    accountCount++;
    saveAccounts();

    loadPlayer(newId);
    strcpy(curName, nameInput);
    curClass = selectedClass;
    addLog("Welcome to Life RPG!");
    savePlayer();

    /* Reset transient UI state. */
    nameInput[0]       = '\0';
    pinInput[0]        = '\0';
    amountInput[0]      = '\0';
    developerPin[0]    = '\0';
    developerUnlocked  = 0;
    selectedQuest      = -1;
    saveMessageTimer   = 0;
    screen             = 2;
    page               = 0;
}

/* ============================================================
   LOGIN SCREEN
   ============================================================ */

void DrawLoginScreen(void)
{
    Rectangle card = { 250, 80, 600, 590 };
    DrawPanel(card, NULL);

    DrawUI("LIFE RPG",             380, 108, 42, ACCENT);
    DrawUI("Turn your daily goals into quests",
             383, 158, 16, TEXT_DIM);

    DrawLine(280, 198, 820, 198, BORDER);

    DrawUI("SELECT PLAYER", 300, 218, 13, TEXT_DIM);

    if (accountCount == 0)
    {
        DrawUI("No accounts yet.", 300, 258, 17, TEXT);
        DrawUI("Create one to begin your journey.", 300, 284, 14, TEXT_DIM);
    }
    else
    {
        int shown = accountCount;
        if (shown > 5)
        {
            shown = 5;
        }
        for (int i = 0; i < shown; i++)
        {
            Rectangle r = { 280, (float)(250 + i * 46), 540, 38 };
            Vector2 m = GetMousePosition();
            int hover = CheckCollisionPointRec(m, r);

            Color bg = PANEL;
            if (selectedAccount == i)
            {
                bg = ACCENT_DARK;
            }
            else if (hover)
            {
                bg = PANEL_LIGHT;
            }
            DrawRectangleRounded(r, 0.25f, 6, bg);
            if (selectedAccount == i)
            {
                DrawRectangleRoundedLines(r, 0.25f, 6, ACCENT);
            }

            DrawUI(accountNames[i], 300, 250 + i * 46 + 10, 16, TEXT);

            if (hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                selectedAccount = i;
                pinInput[0] = '\0';
            }
        }
    }

    /* KSM input + Login (only if a player is selected). */
    if (selectedAccount >= 0 && selectedAccount < accountCount)
    {
        InputBox(MakeRectangle( 280, 545, 380, 42 ),
                 "KSM ID (Password)", pinInput, 1);
        if (Button(MakeRectangle( 680, 545, 140, 42 ), "Login"))
        {
            if (!loginAccount())
            {
                addLog("Incorrect login attempt.");
            }
        }
    }

    if (Button(MakeRectangle( 280, 610, 540, 45 ), "Create New Account"))
    {
        screen        = 1;
        nameInput[0]  = '\0';
        selectedClass = 0;
    }
}

/* ============================================================
   CREATE ACCOUNT SCREEN
   ============================================================ */

void DrawCreateScreen(void)
{
    Rectangle card = { 250, 100, 600, 570 };
    DrawPanel(card, NULL);

    DrawUI("CREATE ACCOUNT", 380, 128, 32, ACCENT);
    DrawUI("Start a new Life RPG character",
             383, 168, 15, TEXT_DIM);

    InputBox(MakeRectangle( 300, 235, 500, 42 ),
             "Player Name", nameInput, 1);

    DrawUI("Choose Your Class", 300, 305, 15, TEXT);

    for (int i = 0; i < 3; i++)
    {
        Rectangle r = { (float)(300 + i * 170), 335, 160, 50 };
        if (SelectButton(r, classNames[i], selectedClass == i))
        {
            selectedClass = i;
        }
    }

    DrawUI(classDescs[selectedClass], 300, 402, 14, COLOR_GOLD);

    if (Button(MakeRectangle( 300, 470, 240, 48 ), "Create Account"))
    {
        createNewAccount();
    }
    if (Button(MakeRectangle( 560, 470, 240, 48 ), "Back to Login"))
    {
        screen = 0;
    }
}

/* ============================================================
   SIDEBAR + TOP BAR
   ============================================================ */

void DrawSidebar(void)
{
    DrawRectangle(0, 0, SIDEBAR_W, SCREEN_HEIGHT, PANEL);
    DrawLine(SIDEBAR_W, 0, SIDEBAR_W, SCREEN_HEIGHT, BORDER);

    DrawUI("LIFE RPG",         24, 22, 24, ACCENT);
    DrawUI("Personal Growth",  24, 52, 11, TEXT_DIM);

    int y = 100;
    if (NavButton(y, "Dashboard",   page == 0)) page = 0;
    y += 52;
    if (NavButton(y, "Quests",      page == 1)) page = 1;
    y += 52;
    if (NavButton(y, "Profile",     page == 2)) page = 2;
    y += 52;
    if (NavButton(y, "Activity",    page == 3)) page = 3;
    y += 52;
    if (NavButton(y, "Reward Shop", page == 4)) page = 4;
    y += 52;
    if (NavButton(y, "Developer",   page == 5)) page = 5;

    if (Button(MakeRectangle( 15, 585, 170, 42 ), "Save Game"))
    {
        savePlayer();
        saveMessageTimer = 120;
    }
    if (Button(MakeRectangle( 15, 640, 170, 42 ), "Logout"))
    {
        savePlayer();
        screen            = 0;
        selectedAccount   = -1;
        page              = 0;
        pinInput[0]       = '\0';
        amountInput[0]     = '\0';
        developerPin[0]   = '\0';
        developerUnlocked = 0;
        selectedQuest     = -1;
        saveMessageTimer  = 0;
    }

    if (saveMessageTimer > 0)
    {
        DrawUI("Game saved!", 15, 710, 13, ACCENT);
        saveMessageTimer--;
    }
}

void DrawTopBar(void)
{
    DrawRectangle(SIDEBAR_W, 0, SCREEN_WIDTH - SIDEBAR_W, TOPBAR_H, PANEL);
    DrawLine(SIDEBAR_W, TOPBAR_H, SCREEN_WIDTH, TOPBAR_H, BORDER);

    DrawUI("WELCOME BACK", SIDEBAR_W + 25, 10, 11, TEXT_DIM);
    int nameSize = 23;
    while (nameSize > 14 && MeasureUI(curName, nameSize) > 255) nameSize--;
    DrawUI(curName, SIDEBAR_W + 25, 26, nameSize, TEXT);
    DrawUI(TextFormat("KSM ID: %s", curId), SIDEBAR_W + 25, 56, 12, ACCENT);

    DrawUI("LEVEL", SIDEBAR_W + 300, 14, 11, TEXT_DIM);
    DrawUI(TextFormat("%d", curLevel), SIDEBAR_W + 300, 32, 24, ACCENT);

    DrawUI("XP", SIDEBAR_W + 390, 14, 11, TEXT_DIM);
    DrawProgressBar(
        MakeRectangle( (float)(SIDEBAR_W + 390), 50, 110, 12 ),
        (float)curXp / (curLevel * 100), ACCENT
    );
    DrawUI(TextFormat("%d/%d", curXp, curLevel * 100),
             SIDEBAR_W + 508, 48, 12, TEXT_DIM);

    DrawUI("GOLD", SIDEBAR_W + 620, 14, 11, TEXT_DIM);
    DrawUI(TextFormat("%d", curGold),
             SIDEBAR_W + 620, 32, 23, COLOR_GOLD);
}
/* ============================================================
   DASHBOARD
   ============================================================ */

void DrawAvatar(int cx, int cy)
{
    Color skin = MakeColor( 238, 194, 155, 255 );
    Color hair = MakeColor( 56, 43, 39, 255 );
    Color shirt = MakeColor( 73, 133, 190, 255 );

    DrawCircle(cx, cy, 78, MakeColor( 80, 200, 170, 22 ));
    DrawCircleLines(cx, cy, 78, MakeColor( 80, 200, 170, 90 ));

    DrawRectangleRounded(MakeRectangle( cx - 39, cy + 12, 78, 65 ),
                         0.35f, 8, shirt);
    DrawTriangle(MakeVector2( (float)cx - 14, (float)cy + 12 ),
                 MakeVector2( (float)cx, (float)cy + 28 ),
                 MakeVector2( (float)cx + 14, (float)cy + 12 ),
                 MakeColor( 235, 240, 250, 255 ));
    DrawRectangle(cx - 9, cy - 2, 18, 22, skin);

    DrawCircle(cx - 25, cy - 22, 6, skin);
    DrawCircle(cx + 25, cy - 22, 6, skin);
    DrawCircle(cx, cy - 22, 27, skin);
    DrawCircle(cx, cy - 31, 27, hair);
    DrawRectangle(cx - 26, cy - 22, 52, 27, skin);

    DrawCircle(cx - 9, cy - 20, 2, MakeColor( 48, 42, 42, 255 ));
    DrawCircle(cx + 9, cy - 20, 2, MakeColor( 48, 42, 42, 255 ));
    DrawLine(cx - 5, cy - 8, cx + 5, cy - 8, MakeColor( 150, 81, 77, 255 ));
}
void DrawDashboard(void)
{
    /* Player card (left). */
    Rectangle pc = { SIDEBAR_W + 20, TOPBAR_H + 20, 420, 410 };
    DrawPanel(pc, "PLAYER");

    DrawAvatar(SIDEBAR_W + 230, TOPBAR_H + 165);

    DrawUI(TextFormat("LEVEL %d", curLevel),
             SIDEBAR_W + 60, TOPBAR_H + 290, 22, COLOR_GOLD);
    DrawUI(classNames[curClass],
             SIDEBAR_W + 60, TOPBAR_H + 320, 15, ACCENT);
    DrawUI(getTitle(),
             SIDEBAR_W + 60, TOPBAR_H + 342, 13, TEXT_DIM);

    DrawUI("EXPERIENCE",
             SIDEBAR_W + 60, TOPBAR_H + 375, 10, TEXT_DIM);
    DrawProgressBar(
        MakeRectangle( (float)(SIDEBAR_W + 60),
                     (float)(TOPBAR_H + 392),
                     340, 12 ),
        (float)curXp / (curLevel * 100),
        ACCENT
    );
    DrawUI(TextFormat("%d / %d XP", curXp, curLevel * 100),
             SIDEBAR_W + 60, TOPBAR_H + 408, 11, TEXT_DIM);

    /* Attributes (right). */
    Rectangle ac = { SIDEBAR_W + 460, TOPBAR_H + 20, 420, 410 };
    DrawPanel(ac, "ATTRIBUTES");

    for (int i = 0; i < 5; i++)
    {
        DrawStatBar(SIDEBAR_W + 480,
                    TOPBAR_H + 78 + i * 65,
                    380, statNames[i], curStats[i]);
    }

    /* Progress + Fortune (bottom). */
    Rectangle bc = { SIDEBAR_W + 20, TOPBAR_H + 450, 860, 175 };
    DrawPanel(bc, "PROGRESS & FORTUNE");

    int completed = 0;
    for (int i = 0; i < questCount; i++)
    {
        if (questDone[i])
        {
            completed++;
        }
    }

    DrawUI(TextFormat("%d", questCount),
             SIDEBAR_W + 45, TOPBAR_H + 510, 32, ACCENT);
    DrawUI("TOTAL QUESTS",
             SIDEBAR_W + 45, TOPBAR_H + 550, 11, TEXT_DIM);

    DrawUI(TextFormat("%d", completed),
             SIDEBAR_W + 200, TOPBAR_H + 510, 32, ACCENT);
    DrawUI("COMPLETED",
             SIDEBAR_W + 200, TOPBAR_H + 550, 11, TEXT_DIM);

    DrawUI(TextFormat("%d", dailyStreak),
             SIDEBAR_W + 360, TOPBAR_H + 510, 32, COLOR_GOLD);
    DrawUI("DAY STREAK",
             SIDEBAR_W + 360, TOPBAR_H + 550, 11, TEXT_DIM);

    DrawUI("TODAY'S FORTUNE",
             SIDEBAR_W + 540, TOPBAR_H + 495, 11, TEXT_DIM);
    DrawUI(dailyFortune,
             SIDEBAR_W + 540, TOPBAR_H + 518, 17, COLOR_GOLD);
}

/* ============================================================
   QUESTS
   ============================================================ */

void DrawQuestScreen(void)
{
    Rectangle lp = { SIDEBAR_W + 20, TOPBAR_H + 20, 420, 630 };
    DrawPanel(lp, "CREATE QUEST");

    int lx = SIDEBAR_W + 45;
    DrawUI("Day", lx, 160, 13, TEXT_DIM);
    for (int i = 0; i < 7; i++)
    {
        Rectangle r = { (float)(lx + i * 52), 181, 48, 30 };
        if (SelectButton(r, days[i], selectedDay == i))
        {
            selectedDay = i;
        }
    }

    DrawUI("Attribute", lx, 225, 13, TEXT_DIM);
    const char *shortStats[5] = { "Intellect", "Stamina", "Discipline", "Social", "Creativity" };
    for (int i = 0; i < 5; i++)
    {
        Rectangle r = { (float)(lx + i * 76), 246, 72, 32 };
        if (SelectButton(r, shortStats[i], selectedStat == i))
        {
            selectedStat = i;
            selectedActivity = 0;
            amountInput[0] = '\0';
        }
    }

    DrawUI(TextFormat("Choose an activity for %s", statNames[selectedStat]),
             lx, 291, 13, TEXT_DIM);
    for (int i = 0; i < 4; i++)
    {
        int row = i / 2;
        int col = i % 2;
        Rectangle r = { (float)(lx + col * 190), (float)(313 + row * 42), 180, 34 };
        if (SelectButton(r, activityOptions[selectedStat][i], selectedActivity == i))
        {
            selectedActivity = i;
            amountInput[0] = '\0';
        }
    }

    DrawUI(TextFormat("Amount (%s)", activityUnits[selectedStat][selectedActivity]),
             lx, 406, 13, TEXT_DIM);
    InputBox(MakeRectangle( (float)lx, 427, 370, 40 ),
             NULL, amountInput, 1);
    DrawUI("Enter a total. Running accepts decimals; other activities use whole numbers.",
             lx, 472, 11, TEXT_DIM);

    DrawUI("Difficulty", lx, 501, 13, TEXT_DIM);
    const char *diffLabels[3] = { "Easy +10", "Medium +20", "Hard +40" };
    for (int i = 0; i < 3; i++)
    {
        Rectangle r = { (float)(lx + i * 125), 522, 115, 34 };
        if (SelectButton(r, diffLabels[i], selectedDifficulty == i))
        {
            selectedDifficulty = i;
        }
    }

    int amountValid = 0;
    char *amountEnd = NULL;
    double amount = strtod(amountInput, &amountEnd);
    if (amountInput[0] != '\0' && amountEnd != amountInput && *amountEnd == '\0' &&
        amount > 0.0 && amount <= 9999.0)
    {
        int running = selectedStat == 1 && selectedActivity == 0;
        if (running)
        {
            amountValid = 1;
        }
        else if (amount == (double)(int)amount)
        {
            amountValid = 1;
        }
    }

    if (Button(MakeRectangle( (float)lx, 578, 370, 44 ), "Add Quest"))
    {
        if (amountValid && questCount < MAX_QUESTS)
        {
            if (selectedStat == 1 && selectedActivity == 0)
            {
                snprintf(questName[questCount], sizeof(questName[questCount]),
                         "%s: %.2f km", activityOptions[selectedStat][selectedActivity], amount);
            }
            else
            {
                snprintf(questName[questCount], sizeof(questName[questCount]),
                         "%s: %.0f %s", activityOptions[selectedStat][selectedActivity],
                         amount, activityUnits[selectedStat][selectedActivity]);
            }
            questDay[questCount]    = selectedDay;
            questStat[questCount]   = selectedStat;
            int rewards[3] = { 10, 20, 40 };
            questReward[questCount] = rewards[selectedDifficulty];
            questDone[questCount]   = 0;
            questCount++;
            amountInput[0] = '\0';
            addLog("Created a new quest.");
            savePlayer();
        }
    }
    DrawUI("PREVIEW", lx, 638, 11, TEXT_DIM);
    const char *shownAmount = "0";
    if (amountInput[0] != '\0')
    {
        shownAmount = amountInput;
    }
    Color previewColor = TEXT_DIM;
    if (amountValid)
    {
        previewColor = ACCENT;
    }
    DrawUI(TextFormat("%s: %s %s", activityOptions[selectedStat][selectedActivity],
                      shownAmount, activityUnits[selectedStat][selectedActivity]),
           lx, 657, 13, previewColor);

    Rectangle rp = { SIDEBAR_W + 460, TOPBAR_H + 20, 420, 630 };
    DrawPanel(rp, "YOUR QUESTS");

    if (questCount == 0)
    {
        DrawUI("No quests yet.", SIDEBAR_W + 480, TOPBAR_H + 100, 15, TEXT_DIM);
    }

    int shown = questCount;
    if (shown > 12)
    {
        shown = 12;
    }
    for (int i = 0; i < shown; i++)
    {
        Rectangle row = {
            (float)(SIDEBAR_W + 480),
            (float)(TOPBAR_H + 62 + i * 44),
            380, 38
        };
        Vector2 m = GetMousePosition();
        int hover = CheckCollisionPointRec(m, row);
        Color bg = PANEL;
        if (selectedQuest == i)
        {
            bg = ACCENT_DARK;
        }
        else if (hover)
        {
            bg = PANEL_LIGHT;
        }
        DrawRectangleRounded(row, 0.2f, 6, bg);
        if (selectedQuest == i)
        {
            DrawRectangleRoundedLines(row, 0.2f, 6, ACCENT);
        }

        Color dot = TEXT_DIM;
        if (questDone[i])
        {
            dot = ACCENT;
        }
        DrawCircle(SIDEBAR_W + 500, (int)(TOPBAR_H + 62 + i * 44 + 19), 5, dot);
        Color questTextColor = TEXT;
        if (questDone[i])
        {
            questTextColor = TEXT_DIM;
        }
        DrawUI(questName[i], SIDEBAR_W + 518,
               TOPBAR_H + 62 + i * 44 + 4, 14, questTextColor);
        DrawUI(TextFormat("%s | %s | +%d XP",
                            statNames[questStat[i]], days[questDay[i]], questReward[i]),
                 SIDEBAR_W + 518, TOPBAR_H + 62 + i * 44 + 22, 11, TEXT_DIM);

        if (hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            selectedQuest = i;
        }
    }

    Rectangle completeBtn = {
        (float)(SIDEBAR_W + 480), (float)(TOPBAR_H + 595), 380, 40
    };
    if (Button(completeBtn, "Complete Selected Quest"))
    {
        completeQuest(selectedQuest);
    }
}
/* ============================================================
   PROFILE
   ============================================================ */

void DrawProfile(void)
{
    Rectangle panel = { SIDEBAR_W + 20, TOPBAR_H + 20, 860, 630 };
    DrawPanel(panel, "PLAYER PROFILE");

    int x = SIDEBAR_W + 55;
    int y = TOPBAR_H + 90;
    int step = 38;

    DrawUI(TextFormat("Name: %s", curName),         x, y, 18, TEXT); y += step;
    DrawUI(TextFormat("KSM ID: %s", curId),         x, y, 18, TEXT); y += step;
    DrawUI(TextFormat("Class: %s", classNames[curClass]),
             x, y, 18, TEXT); y += step;
    DrawUI(TextFormat("Title: %s", getTitle()),     x, y, 18, COLOR_GOLD); y += step;
    DrawUI(TextFormat("Level: %d", curLevel),       x, y, 18, TEXT); y += step;
    DrawUI(TextFormat("XP: %d / %d", curXp, curLevel * 100),
             x, y, 18, TEXT); y += step;
    DrawUI(TextFormat("Gold: %d", curGold),         x, y, 18, COLOR_GOLD); y += step;
    DrawUI(TextFormat("Discipline Score: %d", curDiscipline),
             x, y, 18, TEXT);

    DrawUI("ATTRIBUTE SCORES", SIDEBAR_W + 520, TOPBAR_H + 90, 18, ACCENT);
    for (int i = 0; i < 5; i++)
    {
        DrawStatBar(SIDEBAR_W + 520,
                    TOPBAR_H + 140 + i * 60,
                    340, statNames[i], curStats[i]);
    }
}

/* ============================================================
   ACTIVITY (newest first)
   ============================================================ */

void DrawActivity(void)
{
    Rectangle panel = { SIDEBAR_W + 20, TOPBAR_H + 20, 860, 630 };
    DrawPanel(panel, "ACTIVITY LOG");

    if (logCount == 0)
    {
        DrawUI("No activity recorded yet.",
                 SIDEBAR_W + 45, TOPBAR_H + 90, 15, TEXT_DIM);
        return;
    }

    int maxShow = 15;
    int shown = logCount;
    if (shown > maxShow)
    {
        shown = maxShow;
    }

    for (int i = 0; i < shown; i++)
    {
        int idx = logCount - 1 - i;           /* newest first */
        int y   = TOPBAR_H + 70 + i * 34;

        if (i % 2 == 0)
        {
            DrawRectangle(SIDEBAR_W + 30, y - 4,
                          840, 30, MakeColor( 26, 34, 48, 140 ));
        }

        DrawCircle(SIDEBAR_W + 50, y + 10, 4, ACCENT);
        DrawUI(logText[idx], SIDEBAR_W + 68, y + 2, 14, TEXT);
    }

    if (logCount > maxShow)
    {
        DrawUI(
            TextFormat("Showing %d newest of %d entries.",
                       maxShow, logCount),
            SIDEBAR_W + 45, TOPBAR_H + 615, 12, TEXT_DIM);
    }
}

/* ============================================================
   REWARD SHOP
   ============================================================ */

void DrawRewardShop(void)
{
    Rectangle panel = { SIDEBAR_W + 20, TOPBAR_H + 20, 860, 630 };
    DrawPanel(panel, "REWARD SHOP");

    DrawUI(TextFormat("Your Gold: %d", curGold),
             SIDEBAR_W + 45, TOPBAR_H + 70, 20, COLOR_GOLD);

    const char *items[3]  = {
        "1 Hour Free Time",
        "Favorite Snack",
        "Day Off / Sleep In"
    };
    int prices[3] = { 50, 100, 250 };

    for (int i = 0; i < 3; i++)
    {
        Rectangle card = {
            (float)(SIDEBAR_W + 45),
            (float)(TOPBAR_H + 110 + i * 130),
            810, 105
        };
        DrawRectangleRounded(card, 0.08f, 8, PANEL_LIGHT);
        DrawRectangleRoundedLines(card, 0.08f, 8, BORDER);

        DrawUI(items[i],
                 SIDEBAR_W + 75, TOPBAR_H + 130 + i * 130, 20, TEXT);
        DrawUI(TextFormat("%d Gold", prices[i]),
                 SIDEBAR_W + 75, TOPBAR_H + 165 + i * 130, 15, COLOR_GOLD);

        Rectangle buyBtn = {
            (float)(SIDEBAR_W + 700),
            (float)(TOPBAR_H + 138 + i * 130),
            130, 48
        };
        if (Button(buyBtn, "Purchase"))
        {
            if (curGold >= prices[i])
            {
                curGold -= prices[i];
                addLog("Purchased a reward.");
                savePlayer();
            }
        }
    }
}

/* ============================================================
   DEVELOPER MODE
   ============================================================ */

void DrawDeveloper(void)
{
    Rectangle panel = { SIDEBAR_W + 20, TOPBAR_H + 20, 860, 630 };
    DrawPanel(panel, "DEVELOPER MODE");

    if (!developerUnlocked)
    {
        DrawUI("Enter the developer PIN to unlock admin tools.",
                 SIDEBAR_W + 45, TOPBAR_H + 80, 15, TEXT_DIM);

        InputBox(MakeRectangle( (float)(SIDEBAR_W + 45),
                              (float)(TOPBAR_H + 140),
                              300, 42 ),
                 "Developer PIN", developerPin, 1);

        if (Button(MakeRectangle( (float)(SIDEBAR_W + 45),
                                (float)(TOPBAR_H + 210),
                                150, 42 ), "Unlock"))
        {
            if (strcmp(developerPin, "KSM-321") == 0)
            {
                developerUnlocked = 1;
                developerPin[0]   = '\0';
            }
        }
        return;
    }

    DrawUI("Developer tools unlocked.",
             SIDEBAR_W + 45, TOPBAR_H + 80, 16, COLOR_GOLD);

    if (Button(MakeRectangle( (float)(SIDEBAR_W + 45),
                            (float)(TOPBAR_H + 140),
                            300, 45 ), "Add 500 XP + 250 Gold"))
    {
        addXp(500, 0);
        curGold += 250;
        addLog("Developer XP + gold granted.");
        savePlayer();
    }

    if (Button(MakeRectangle( (float)(SIDEBAR_W + 45),
                            (float)(TOPBAR_H + 200),
                            300, 45 ), "Max Out All Stats"))
    {
        for (int i = 0; i < 5; i++)
        {
            curStats[i] = 100;
        }
        addLog("Developer max stats activated.");
        savePlayer();
    }

    if (Button(MakeRectangle( (float)(SIDEBAR_W + 45),
                            (float)(TOPBAR_H + 260),
                            300, 45 ), "Lock Developer Mode"))
    {
        developerUnlocked = 0;
    }
}

/* ============================================================
   GAME ROOT
   ============================================================ */

void DrawGame(void)
{
    DrawSidebar();
    DrawTopBar();

    if (page == 0)
    {
        DrawDashboard();
    }
    else if (page == 1)
    {
        DrawQuestScreen();
    }
    else if (page == 2)
    {
        DrawProfile();
    }
    else if (page == 3)
    {
        DrawActivity();
    }
    else if (page == 4)
    {
        DrawRewardShop();
    }
    else if (page == 5)
    {
        DrawDeveloper();
    }

    DrawUI("ESC: Logout", 15, 735, 11, TEXT_DIM);

    if (IsKeyPressed(KEY_ESCAPE))
    {
        savePlayer();
        screen            = 0;
        selectedAccount   = -1;
        page              = 0;
        pinInput[0]       = '\0';
        amountInput[0]     = '\0';
        developerPin[0]   = '\0';
        developerUnlocked = 0;
        selectedQuest     = -1;
        saveMessageTimer  = 0;
    }
}

/* ============================================================
   MAIN
   ============================================================ */

int main(void)
{
    srand((unsigned int)time(NULL));

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT,
               "Life RPG - Personal Growth Game");

    uiFont = GetFontDefault();
    if (FileExists(ARIAL_FONT_PATH))
    {
        Font arial = LoadFontEx(ARIAL_FONT_PATH, 32, NULL, 0);
        if (arial.texture.id != 0)
        {
            uiFont = arial;
            arialFontLoaded = 1;
        }
    }
    SetTargetFPS(60);

    loadAccounts();

    while (!WindowShouldClose())
    {
        /* Text input routing. */
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
                readAmount(amountInput, 16);
            }
            else if (page == 5 && !developerUnlocked)
            {
                readText(developerPin, 16);
            }
        }

        BeginDrawing();
        ClearBackground(BG);

        if (screen == 0)
        {
            DrawLoginScreen();
        }
        else if (screen == 1)
        {
            DrawCreateScreen();
        }
        else if (screen == 2)
        {
            DrawGame();
        }

        EndDrawing();
    }

    if (strlen(curId) > 0)
    {
        savePlayer();
    }
    saveAccounts();
    if (arialFontLoaded)
    {
        UnloadFont(uiFont);
    }
    CloseWindow();
    return 0;
}

