/* ============================================================================
   LIFE TRACKER RPG - Group 3 (Mamoon, Sajid, Khadija)
   Programming Fundamentals Lab - FAST NUCES
   High-Quality, Optimized, Interactive GUI Life Tracker in C using Raylib
   ============================================================================ */

#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <time.h>

#ifdef _WIN32
  #include <direct.h>
  #define MKDIR(d) _mkdir(d)
#else
  #include <sys/stat.h>
  #define MKDIR(d) mkdir(d, 0755)
#endif

/* ------------------------------------------------------------- CONSTANTS ---*/
#define W            1020
#define H            620
#define MAX_ACC      50
#define MAX_QUESTS   40
#define STR_LEN      40

/* ---------------------------------------------------------------- THEME ---*/
#define BG_COLOR     ((Color){ 16,  20,  32, 255 })
#define PANEL_BG     ((Color){ 26,  33,  50, 255 })
#define PANEL_ALT    ((Color){ 36,  46,  70, 255 })
#define MINT         ((Color){ 72, 224, 168, 255 })
#define MINT_DARK    ((Color){ 34, 102,  78, 255 })
#undef GOLD
#define GOLD         ((Color){ 255, 204,  76, 255 })
#define SKY          ((Color){ 96, 172, 252, 255 })
#undef PINK
#define PINK         ((Color){ 246, 120, 160, 255 })
#undef RED
#define RED          ((Color){ 238,  88,  88, 255 })
#undef PURPLE
#define PURPLE       ((Color){ 186, 140, 255, 255 })
#define DIM_TEXT     ((Color){ 145, 156, 178, 255 })
#define TEXT_WHITE   ((Color){ 238, 242, 250, 255 })
#define INK_BLACK    ((Color){ 20,  24,  36, 255 })

/* ------------------------------------------------------------ DATA ARRAYS --*/
static const char *days[7]       = { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };
static const char *shortDays[7]  = { "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" };
static const char *statNames[5]  = { "Intellect", "Stamina", "Discipline", "Social", "Creativity" };
static const Color statColors[5] = { SKY, MINT, GOLD, PINK, PURPLE };
static const char *classNames[3] = { "Scholar", "Athlete", "Artist" };
static const char *classDesc[3]  = { "Master of mind and wisdom", "Master of physical endurance", "Master of creativity and arts" };
static const char *diffNames[3]  = { "Easy", "Normal", "Hard" };

static const char *acts[5][5] = {
    { "Read a Book",     "Solve Math",     "Watch Docu",     "Learn Words",      "Coding Time" },
    { "Running",         "Push Ups",       "Pull Ups",       "Sit Ups",          "Cycling" },
    { "Wake Up Early",   "Meditate",       "No Junk Food",   "Make My Bed",      "Plan My Day" },
    { "Meet New People", "Call Family",    "Group Study",    "Compliment Others","Help Someone" },
    { "Draw Art",        "Write a Story",  "Play Music",     "Make a Craft",     "Take Photos" }
};

static const char *units[5][5] = {
    { "pages", "problems", "minutes", "words", "minutes" },
    { "km",    "reps",     "reps",    "reps",  "km" },
    { "days",  "minutes",  "days",    "days",  "minutes" },
    { "people","minutes",  "hours",   "times", "times" },
    { "drawings","lines",  "minutes", "crafts","photos" }
};

static const char *tips[8] = {
    "Click DONE only when you really did it!",
    "Hard quests grant massive Gold and XP bonuses!",
    "Stay hydrated, hero! Water keeps your mind sharp.",
    "Log in daily to keep your streak burning hot!",
    "Spend your hard-earned gold on real-life treats!",
    "Small daily habits forge legendary futures.",
    "Try a Creativity quest - nourish your soul!",
    "Sleep well tonight - your stats recharge during rest."
};

static const char *fortunes[7] = {
    "Monday momentum: conquer your goals!",
    "Tuesday focus: small steps yield big levels.",
    "Midweek victory: your mind is razor sharp.",
    "Thursday grind: push through to greatness.",
    "Friday glory: finish strong and celebrate.",
    "Weekend training: balance effort and rest.",
    "Sunday strategy: plan ahead and win."
};

/* ----------------------------------------------------------- QUEST STRUCT --*/
typedef struct {
    char title[STR_LEN];
    int day, stat, act, amt, diff, done;
} Quest;

/* ----------------------------------------------------------- GLOBAL STATE --*/
static char accNames[MAX_ACC][STR_LEN];
static char accPins[MAX_ACC][10];
static int  accCount = 0;

static char myName[STR_LEN] = "";
static int  level = 1, xp = 0, gold = 50, cls = 0;
static int  stats[5] = { 10, 10, 10, 10, 10 };
static int  streak = 1;
static long lastDay = -1;

static Quest quests[MAX_QUESTS];
static int   qCount = 0;

static Font      font;
static Texture2D devImg;
static int       devImgLoaded = 0;

static int   screen = 0;         /* 0 = Login, 1 = Signup, 2 = Game   */
static int   page = 0;           /* 0 = Home, 1 = Quests, 2 = Profile */
static int   popup = 0;          /* 0 = None, 1 = Quest, 2 = Dev, 3 = Logs */
static float slide = 0.0f;       /* Sliding panel animation 0.0 .. 1.0 */
static int   isLocked = 0;       /* Locks background screen when popup is active */
static int   activeField = 0;    /* Currently focused text box */

static char  msgText[90] = "";
static int   msgTimer = 0;

static char  talkBubble[90] = "";
static float talkTimer = 0.0f;
static float avatarBounce = 0.0f;

static int   devUnlocked = 0;
static char  devPinInput[STR_LEN] = "";

static char  loginUser[STR_LEN] = "", loginPin[10] = "";
static char  newUser[STR_LEN]   = "", newPin[10]   = "";
static int   selClass = 0;

static int   selDay = 0;
static int   mkDay = 0, mkStat = 0, mkAct = 0, mkDiff = 2;
static char  mkTitle[STR_LEN] = "", mkAmt[10] = "15";

typedef struct { float x, y, vx, vy, life; Color col; } Particle;
static Particle bits[80];
static int      bitCount = 0;

// below is for cutting enter and spaces off text
static void Trim(char *s)
{
    int n = (int)strlen(s);
    while (n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r' || s[n - 1] == ' ')) s[--n] = 0;
}

// below is for getting current day index from 0 to 6
static int TodayIndex(void)
{
    time_t t = time(NULL);
    struct tm *d = localtime(&t);
    return (d->tm_wday + 6) % 7;
}

// below is for calculating xp needed for next level
static int XpNeeded(void)
{
    return level * 100;
}

// below is for showing temporary toast notification
static void ShowToast(const char *text)
{
    snprintf(msgText, sizeof(msgText), "%s", text);
    msgTimer = 180;
}

// below is for drawing high quality text
static void DrawTxt(const char *s, int x, int y, int size, Color c)
{
    DrawTextEx(font, s, (Vector2){ (float)x, (float)y }, (float)size, 1.0f, c);
}

// below is for measuring text width
static int MeasureTxt(const char *s, int size)
{
    return (int)MeasureTextEx(font, s, (float)size, 1.0f).x;
}

// below is for drawing centered text
static void DrawTxtMid(const char *s, int cx, int y, int size, Color c)
{
    DrawTxt(s, cx - MeasureTxt(s, size) / 2, y, size, c);
}

// below is for drawing a styled rounded panel card
static void DrawCard(int x, int y, int w, int h, Color borderColor)
{
    DrawRectangleRounded((Rectangle){ (float)x - 2, (float)y - 2, (float)w + 4, (float)h + 4 }, 0.12f, 8, borderColor);
    DrawRectangleRounded((Rectangle){ (float)x, (float)y, (float)w, (float)h }, 0.12f, 8, PANEL_BG);
}

// below is for making celebration confetti fly
static void SpawnConfetti(int x, int y)
{
    for (int i = 0; i < 26 && bitCount < 80; i++) {
        Particle *p = &bits[bitCount++];
        p->x = (float)x; p->y = (float)y;
        float angle = (float)(rand() % 360) * DEG2RAD;
        float speed = 70.0f + (float)(rand() % 190);
        p->vx = cosf(angle) * speed; p->vy = sinf(angle) * speed - 100.0f;
        p->life = 1.0f;
        p->col = (rand() % 3 == 0) ? GOLD : ((rand() % 2) ? MINT : PINK);
    }
}

// below is for updating and drawing confetti particles
static void UpdateConfetti(void)
{
    float dt = GetFrameTime();
    for (int i = 0; i < bitCount; i++) {
        Particle *p = &bits[i];
        p->x += p->vx * dt; p->y += p->vy * dt;
        p->vy += 280.0f * dt; p->life -= dt * 1.3f;
        if (p->life <= 0) { *p = bits[--bitCount]; i--; }
    }
    for (int i = 0; i < bitCount; i++) {
        Particle *p = &bits[i];
        DrawRectangle((int)p->x, (int)p->y, 6, 6, Fade(p->col, p->life));
    }
}

// below is for drawing interactive button with glow and hover
static int DrawButton(int x, int y, int w, int h, const char *label, Color accent)
{
    Rectangle r = { (float)x, (float)y, (float)w, (float)h };
    int hovered = CheckCollisionPointRec(GetMousePosition(), r) && !isLocked;

    Color border = hovered ? TEXT_WHITE : Fade(accent, 0.6f + 0.35f * (float)sin(GetTime() * 3.5));
    DrawRectangleRounded((Rectangle){ (float)x - 2, (float)y - 2, (float)w + 4, (float)h + 4 }, 0.28f, 8, border);
    DrawRectangleRounded(r, 0.28f, 8, hovered ? MINT_DARK : PANEL_ALT);

    DrawTxt(label, x + (w - MeasureTxt(label, 17)) / 2, y + (h - 17) / 2, 17, TEXT_WHITE);
    return hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

// below is for handling keyboard typing into input buffer
static void HandleTyping(char *buf, int maxLen, int numsOnly)
{
    int ch = GetCharPressed();
    while (ch > 0) {
        int valid = (ch >= 32 && ch <= 125 && ch != ';' && (int)strlen(buf) < maxLen - 1);
        if (numsOnly) valid = valid && (ch >= '0' && ch <= '9');
        if (valid) { int n = (int)strlen(buf); buf[n] = (char)ch; buf[n + 1] = 0; }
        ch = GetCharPressed();
    }
    if (IsKeyPressed(KEY_BACKSPACE)) {
        int n = (int)strlen(buf);
        if (n > 0) buf[n - 1] = 0;
    }
}

// below is for drawing text input box with label and cursor
static void DrawInput(int id, int x, int y, int w, char *buf, int maxLen, const char *label, int numsOnly)
{
    Rectangle r = { (float)x, (float)y, (float)w, 38.0f };
    int active = (activeField == id);
    if (!isLocked && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), r))
        activeField = id;

    DrawTxt(label, x, y - 20, 14, DIM_TEXT);
    DrawRectangleRounded(r, 0.25f, 8, PANEL_ALT);
    DrawRectangleRoundedLines(r, 0.25f, 8, active ? MINT : Fade(DIM_TEXT, 0.6f));
    DrawTxt(buf, x + 12, y + 10, 16, TEXT_WHITE);
    if (active && ((int)(GetTime() * 2.5) % 2 == 0))
        DrawTxt("_", x + 14 + MeasureTxt(buf, 16), y + 10, 16, MINT);
    if (active) HandleTyping(buf, maxLen, numsOnly);
}

// below is for interactive value drag slider
static int DrawSlider(int x, int y, int w, int val, int minVal, int maxVal, Color col)
{
    float ratio = (float)(val - minVal) / (float)(maxVal - minVal);
    DrawRectangleRounded((Rectangle){ (float)x, (float)y + 6, (float)w, 10 }, 0.5f, 6, PANEL_ALT);
    DrawRectangleRounded((Rectangle){ (float)x, (float)y + 6, (float)(w * ratio), 10 }, 0.5f, 6, col);
    DrawCircleV((Vector2){ (float)x + ratio * w, (float)y + 11 }, 13, col);
    DrawCircleV((Vector2){ (float)x + ratio * w, (float)y + 11 }, 8, PANEL_ALT);

    static int dragging = 0;
    Rectangle hit = { (float)x - 14, (float)y - 14, (float)w + 28, 50 };
    if (!isLocked && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), hit))
        dragging = 1;
    if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT)) dragging = 0;

    if (dragging) {
        float mouseFrac = (GetMouseX() - x) / (float)w;
        if (mouseFrac < 0.0f) mouseFrac = 0.0f;
        if (mouseFrac > 1.0f) mouseFrac = 1.0f;
        val = minVal + (int)((maxVal - minVal) * mouseFrac + 0.5f);
    }
    return val;
}

// below is for drawing attribute progress bar
static void DrawStatBar(int x, int y, int index)
{
    int v = stats[index];
    if (v < 0) v = 0;
    else if (v > 100) v = 100;

    DrawTxt(statNames[index], x, y, 17, TEXT_WHITE);
    DrawRectangleRounded((Rectangle){ (float)x + 140, (float)y + 4, 310, 14 }, 0.5f, 6, PANEL_ALT);
    DrawRectangleRounded((Rectangle){ (float)x + 140, (float)y + 4, (float)(310 * v / 100), 14 }, 0.5f, 6, statColors[index]);
    DrawTxt(TextFormat("%d / 100", v), x + 465, y, 15, DIM_TEXT);
}

// below is for drawing level and xp progress bar
static void DrawXpBar(int x, int y, int w)
{
    DrawTxt(TextFormat("LEVEL %d", level), x, y - 4, 19, GOLD);
    int needed = XpNeeded();
    int current = (xp > needed) ? needed : xp;

    DrawRectangleRounded((Rectangle){ (float)x, (float)y + 22, (float)w, 16 }, 0.5f, 7, PANEL_ALT);
    DrawRectangleRounded((Rectangle){ (float)x, (float)y + 22, (float)(w * current / needed), 16 }, 0.5f, 7, GOLD);
    DrawTxt(TextFormat("XP: %d / %d", xp, needed), x + w + 16, y + 21, 15, DIM_TEXT);
}

// below is for drawing the interactive animated avatar
static void DrawAvatar(int cx, int cy, int scale)
{
    float t = (float)GetTime();
    int bob = (int)(sinf(t * 2.4f) * 2.5f * scale);
    cy += bob - (int)avatarBounce;

    Color suit = (cls == 0) ? SKY : ((cls == 1) ? MINT : PINK);

    /* Body, arms and head */
    DrawRectangleRounded((Rectangle){ (float)cx - 26 * scale, (float)cy + 14 * scale, (float)52 * scale, (float)40 * scale }, 0.4f, 6, suit);
    DrawCircleV((Vector2){ (float)cx - 32 * scale, (float)cy + 34 * scale }, 9.0f * scale, suit);
    DrawCircleV((Vector2){ (float)cx + 32 * scale, (float)cy + 34 * scale }, 9.0f * scale, suit);
    DrawCircleV((Vector2){ (float)cx, (float)cy }, 24.0f * scale, (Color){ 245, 214, 180, 255 });

    /* Class headgear: Scholar cap, Athlete headband, Artist beret */
    if (cls == 0) {
        DrawRectangleRounded((Rectangle){ (float)cx - 26 * scale, (float)cy - 28 * scale, (float)52 * scale, (float)12 * scale }, 0.4f, 5, SKY);
        DrawCircle((int)(cx + 20 * scale), (int)(cy - 20 * scale), 4.0f * scale, GOLD);
    } else if (cls == 1) {
        DrawRectangleRounded((Rectangle){ (float)cx - 24 * scale, (float)cy - 14 * scale, (float)48 * scale, (float)9 * scale }, 0.4f, 4, RED);
    } else {
        DrawCircleV((Vector2){ (float)cx, (float)cy - 18 * scale }, 14.0f * scale, GOLD);
        DrawCircleV((Vector2){ (float)cx + 8 * scale, (float)cy - 22 * scale }, 4.0f * scale, PURPLE);
    }

    /* Eyes with blinking and mouse tracking pupils */
    int blink = ((int)(t * 1.6f) % 5 == 0) ? 1 : 4;
    float lookX = (GetMouseX() - cx) * 0.02f;
    if (lookX < -2.0f) lookX = -2.0f; else if (lookX > 2.0f) lookX = 2.0f;

    DrawRectangle(cx - 12 * scale + (int)lookX, cy - 6 * scale, 8 * scale, blink * scale, INK_BLACK);
    DrawRectangle(cx + 4 * scale + (int)lookX, cy - 6 * scale, 8 * scale, blink * scale, INK_BLACK);
    DrawLineEx((Vector2){ (float)cx - 8 * scale, (float)cy + 8 * scale }, (Vector2){ (float)cx + 8 * scale, (float)cy + 8 * scale }, 2.5f * scale, INK_BLACK);

    /* Speech bubble when talking */
    if (talkTimer > 0.0f) {
        int bubbleW = MeasureTxt(talkBubble, 15) + 30;
        int bx = cx - bubbleW / 2, by = cy - 76 * scale;
        DrawRectangleRounded((Rectangle){ (float)bx, (float)by, (float)bubbleW, 36 }, 0.35f, 8, TEXT_WHITE);
        DrawTriangle((Vector2){ (float)cx - 8, (float)by + 36 }, (Vector2){ (float)cx + 8, (float)by + 36 }, (Vector2){ (float)cx, (float)by + 46 }, TEXT_WHITE);
        DrawTxtMid(talkBubble, cx, by + 10, 15, INK_BLACK);
    }
}

// below is for drawing twinkling background stars
static void DrawStars(void)
{
    static int init = 0, starX[45], starY[45];
    if (!init) {
        for (int i = 0; i < 45; i++) { starX[i] = rand() % W; starY[i] = rand() % (H - 50); }
        init = 1;
    }
    for (int i = 0; i < 45; i++) {
        int alpha = 100 + (int)(110 * sinf((float)GetTime() * 2.2f + i));
        DrawCircle(starX[i], starY[i], 1.5f, (Color){ 210, 220, 255, (unsigned char)alpha });
    }
}

// below is for logging player actions to user text file
static void LogNote(const char *fmt, ...)
{
    if (myName[0] == 0) return;
    MKDIR("Logs");
    FILE *f = fopen(TextFormat("Logs/%s.txt", myName), "a");
    if (!f) return;

    char sentence[160];
    va_list ap; va_start(ap, fmt); vsnprintf(sentence, sizeof(sentence), fmt, ap); va_end(ap);

    time_t now = time(NULL);
    struct tm *d = localtime(&now);
    char dateBuf[30];
    strftime(dateBuf, sizeof(dateBuf), "%d-%m-%Y %H:%M", d);

    fprintf(f, "%s (%s)\n", sentence, dateBuf);
    fclose(f);
}

// below is for saving all accounts to accounts.txt
static void SaveAccounts(void)
{
    FILE *f = fopen("accounts.txt", "w");
    if (!f) return;
    for (int i = 0; i < accCount; i++) {
        fprintf(f, "Name: %s\nPIN: %s\n\n", accNames[i], accPins[i]);
    }
    fclose(f);
}

// below is for loading accounts from accounts.txt
static void LoadAccounts(void)
{
    accCount = 0;
    FILE *f = fopen("accounts.txt", "r");
    if (!f) return;

    char line[128], tempName[STR_LEN] = "";
    while (fgets(line, sizeof(line), f) && accCount < MAX_ACC) {
        Trim(line);
        if (strncmp(line, "Name:", 5) == 0) {
            char *p = line + 5; while (*p == ' ') p++;
            snprintf(tempName, STR_LEN, "%s", p);
        } else if (strncmp(line, "PIN:", 4) == 0 && tempName[0]) {
            char *p = line + 4; while (*p == ' ') p++;
            snprintf(accNames[accCount], STR_LEN, "%s", tempName);
            snprintf(accPins[accCount], 10, "%s", p);
            accCount++;
            tempName[0] = 0;
        }
    }
    fclose(f);
}

// below is for finding account index by name
static int FindAccount(const char *name)
{
    for (int i = 0; i < accCount; i++) if (strcmp(accNames[i], name) == 0) return i;
    return -1;
}

// below is for saving player data and quests to file
static void SavePlayerData(void)
{
    if (myName[0] == 0) return;
    FILE *f = fopen(TextFormat("%s.txt", myName), "w");
    if (!f) return;

    fprintf(f, "%d %d %d %d %d %ld\n", level, xp, gold, cls, streak, lastDay);
    for (int i = 0; i < 5; i++) fprintf(f, "%d ", stats[i]);
    fprintf(f, "\n%d\n", qCount);

    for (int i = 0; i < qCount; i++) {
        fprintf(f, "%s;%d;%d;%d;%d;%d;%d\n",
                quests[i].title, quests[i].day, quests[i].stat,
                quests[i].act, quests[i].amt, quests[i].diff, quests[i].done);
    }
    fclose(f);
}

// below is for loading player data and quests from file
static int LoadPlayerData(void)
{
    FILE *f = fopen(TextFormat("%s.txt", myName), "r");
    if (!f) return 0;

    if (fscanf(f, "%d %d %d %d %d %ld", &level, &xp, &gold, &cls, &streak, &lastDay) < 6) { fclose(f); return 0; }
    for (int i = 0; i < 5; i++) fscanf(f, "%d", &stats[i]);
    fscanf(f, "%d", &qCount);

    char line[140];
    fgets(line, sizeof(line), f);
    for (int i = 0; i < qCount && i < MAX_QUESTS; i++) {
        if (!fgets(line, sizeof(line), f)) break;
        Trim(line);
        sscanf(line, "%39[^;];%d;%d;%d;%d;%d;%d",
               quests[i].title, &quests[i].day, &quests[i].stat,
               &quests[i].act, &quests[i].amt, &quests[i].diff, &quests[i].done);
    }
    fclose(f);
    return 1;
}

// below is for rewarding player and leveling up
static void RewardPlayer(int statIdx, int diff)
{
    int gain = diff * 25;
    gold += gain; xp += gain;
    stats[statIdx] += diff;
    if (stats[statIdx] > 100) stats[statIdx] = 100;

    SpawnConfetti(W / 2, H / 2);

    while (xp >= XpNeeded()) {
        xp -= XpNeeded();
        level++;
        ShowToast(TextFormat("LEVEL UP! You reached Level %d!", level));
        LogNote("Leveled up to Level %d on %s", level, days[TodayIndex()]);
        snprintf(talkBubble, sizeof(talkBubble), "Level %d reached! Amazing!", level);
        talkTimer = 3.5f; avatarBounce = 20.0f;
    }
}

// below is for removing a quest from the list
static void RemoveQuest(int index)
{
    for (int j = index; j < qCount - 1; j++) quests[j] = quests[j + 1];
    qCount--;
}

// below is for triggering avatar interaction speech
static void TriggerAvatarTalk(void)
{
    snprintf(talkBubble, sizeof(talkBubble), "%s", tips[rand() % 8]);
    talkTimer = 3.5f; avatarBounce = 16.0f;
    SpawnConfetti(GetMouseX(), GetMouseY());
}

// below is for the login screen
static void ScreenLogin(void)
{
    DrawAvatar(190, 300, 3);
    if (!isLocked && CheckCollisionPointCircle(GetMousePosition(), (Vector2){ 190, 300 }, 80) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        TriggerAvatarTalk();

    DrawTxtMid("LIFE TRACKER RPG", 620, 80, 42, GOLD);
    DrawTxtMid("Level Up Your Real Life Through Action", 620, 134, 17, DIM_TEXT);

    int bx = 470;
    DrawInput(1, bx, 220, 310, loginUser, STR_LEN, "YOUR HERO NAME", 0);
    DrawInput(2, bx, 300, 310, loginPin, 10, "YOUR 4-DIGIT PIN", 1);

    if (DrawButton(bx, 375, 310, 46, "LOGIN", MINT)) {
        int idx = FindAccount(loginUser);
        if (idx >= 0 && strcmp(accPins[idx], loginPin) == 0) {
            snprintf(myName, STR_LEN, "%s", loginUser);
            LoadPlayerData();

            long t = (long)(time(NULL) / 86400);
            if (lastDay >= 0 && t - lastDay == 1) { streak++; ShowToast(TextFormat("Welcome back! %d-day streak!", streak)); }
            else if (t != lastDay) streak = 1;
            lastDay = t;

            LogNote("Logged in on %s", days[TodayIndex()]);
            SavePlayerData();
            screen = 2;
        } else ShowToast("Invalid Hero Name or PIN!");
    }

    if (DrawButton(bx, 435, 310, 40, "NEW HERO? CREATE ACCOUNT", GOLD)) screen = 1;
}

// below is for the signup screen
static void ScreenSignup(void)
{
    DrawAvatar(190, 300, 3);
    DrawTxtMid("HERO REGISTRATION", 620, 70, 36, GOLD);
    int bx = 470;

    DrawInput(3, bx, 150, 310, newUser, STR_LEN, "HERO NAME", 0);
    DrawInput(4, bx, 230, 310, newPin, 10, "SET PIN (NUMBERS)", 1);

    DrawTxt("CHOOSE YOUR HERO CLASS", bx, 285, 14, DIM_TEXT);
    for (int i = 0; i < 3; i++)
        if (DrawButton(bx + i * 106, 308, 98, 36, classNames[i], selClass == i ? MINT : SKY)) selClass = i;
    DrawTxt(classDesc[selClass], bx, 354, 14, DIM_TEXT);

    if (DrawButton(bx, 395, 310, 46, "JOIN THE REALM", MINT)) {
        if (strlen(newUser) == 0 || strlen(newPin) == 0) ShowToast("Name and PIN are required!");
        else if (FindAccount(newUser) >= 0) ShowToast("Hero name already registered!");
        else {
            for (int i = 0; newUser[i]; i++) if (newUser[i] == ' ') newUser[i] = '_';
            snprintf(accNames[accCount], STR_LEN, "%s", newUser);
            snprintf(accPins[accCount], 10, "%s", newPin);
            accCount++;
            SaveAccounts();

            cls = selClass; level = 1; xp = 0; gold = 50; streak = 1; lastDay = (long)(time(NULL) / 86400);
            for (int i = 0; i < 5; i++) stats[i] = 10;
            qCount = 0;
            snprintf(myName, STR_LEN, "%s", newUser);
            LogNote("Account created on %s", days[TodayIndex()]);
            LogNote("Logged in on %s", days[TodayIndex()]);
            SavePlayerData();
            ShowToast("Welcome, Hero! Begin your journey.");
            screen = 2;
        }
    }
    if (DrawButton(bx, 455, 310, 38, "BACK TO LOGIN", SKY)) screen = 0;
}

// below is for top navigation bar
static void DrawTopBar(void)
{
    DrawRectangle(0, 0, W, 56, PANEL_BG);
    DrawRectangle(0, 54, W, 2, MINT);

    const char *tabs[3] = { "HOME", "QUESTS", "PROFILE" };
    for (int i = 0; i < 3; i++)
        if (DrawButton(20 + i * 140, 9, 125, 38, tabs[i], page == i ? MINT : SKY)) { page = i; popup = 0; }

    /* Clean right-side stats & dev button (NO small center avatar) */
    DrawCircle(W - 320, 28, 9, GOLD);
    DrawTxt(TextFormat("%d Gold", gold), W - 304, 18, 18, GOLD);
    DrawTxt(TextFormat("%d-Day Streak", streak), W - 200, 18, 17, MINT);

    if (DrawButton(W - 86, 9, 70, 38, "DEV", RED)) {
        popup = 2;
        slide = 0.0f;
        devPinInput[0] = 0;
        activeField = 7;
    }
}

// below is for home dashboard page
static void PageHome(void)
{
    int x = 40, y = 80;
    DrawAvatar(120, 190, 2);
    if (!isLocked && CheckCollisionPointCircle(GetMousePosition(), (Vector2){ 120, 190 }, 60) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        TriggerAvatarTalk();

    DrawTxt(TextFormat("Greetings, %s the %s!", myName, classNames[cls]), 240, 120, 28, GOLD);
    DrawTxt(classDesc[cls], 240, 160, 15, DIM_TEXT);
    DrawXpBar(240, 198, 380);

    y += 225;
    for (int i = 0; i < 5; i++) { DrawStatBar(x, y, i); y += 38; }

    /* Today's Mission Card */
    int tx = 640, ty = 280;
    DrawCard(tx, ty, 340, 250, MINT);
    DrawTxt("TODAY'S MISSION", tx + 24, ty + 18, 20, MINT);
    DrawTxt(days[TodayIndex()], tx + 24, ty + 46, 25, TEXT_WHITE);
    DrawTxt(fortunes[TodayIndex()], tx + 24, ty + 84, 14, GOLD);

    int pendingToday = 0;
    for (int i = 0; i < qCount; i++) if (quests[i].day == TodayIndex() && !quests[i].done) pendingToday++;

    DrawTxt(TextFormat("Active quests today: %d", pendingToday), tx + 24, ty + 114, 15, DIM_TEXT);
    DrawTxt(TextFormat("Current streak: %d days", streak), tx + 24, ty + 140, 15, PINK);
    DrawTxt(TextFormat("Available gold: %d", gold), tx + 24, ty + 166, 15, GOLD);

    if (DrawButton(tx + 24, ty + 196, 140, 34, "REST TODAY", SKY))
        ShowToast("Rest and recovery are vital for champions!");
}

// below is for weekly quests page
static void PageQuests(void)
{
    for (int i = 0; i < 7; i++) {
        Color col = (selDay == i) ? MINT : ((i == TodayIndex()) ? GOLD : PANEL_ALT);
        if (DrawButton(40 + i * 94, 76, 86, 36, shortDays[i], col)) selDay = i;
    }

    DrawTxt(TextFormat("%s QUESTS", days[selDay]), 40, 126, 21, TEXT_WHITE);

    int y = 160, count = 0;
    for (int i = 0; i < qCount; i++) {
        if (quests[i].day != selDay) continue;
        count++;

        DrawCard(40, y, 840, 60, quests[i].done ? MINT : PANEL_ALT);
        DrawTxt(quests[i].title, 60, y + 10, 18, TEXT_WHITE);
        DrawTxt(TextFormat("%s: %d %s  |  %s  |  %s",
                           acts[quests[i].stat][quests[i].act], quests[i].amt,
                           units[quests[i].stat][quests[i].act],
                           statNames[quests[i].stat], diffNames[quests[i].diff - 1]),
                60, y + 34, 14, DIM_TEXT);

        if (quests[i].done) {
            DrawTxt("COMPLETED", 725, y + 20, 17, MINT);
        } else {
            if (DrawButton(680, y + 11, 85, 38, "DONE", MINT)) {
                quests[i].done = 1;
                RewardPlayer(quests[i].stat, quests[i].diff);
                ShowToast(TextFormat("+%d Gold and XP earned! Great job!", quests[i].diff * 25));
                LogNote("Quest completed on %s (+%d gold): %s", days[TodayIndex()], quests[i].diff * 25, quests[i].title);
                SavePlayerData();
            }
            if (DrawButton(775, y + 11, 50, 38, "X", RED)) {
                LogNote("Quest deleted on %s: %s", days[TodayIndex()], quests[i].title);
                RemoveQuest(i);
                SavePlayerData();
                continue;
            }
        }
        y += 68;
    }

    if (count == 0) DrawTxtMid("No quests scheduled for this day yet. Add one with + ADD QUEST!", W / 2, 280, 17, DIM_TEXT);

    if (DrawButton(W - 200, H - 68, 160, 44, "+ ADD QUEST", GOLD)) {
        popup = 1; slide = 0.0f;
        mkTitle[0] = 0;
        activeField = 5;
    }
}

// below is for real life shop item purchase
static void BuyShopReward(int cost, const char *toastMsg, const char *logMsg)
{
    if (gold >= cost) {
        gold -= cost;
        ShowToast(toastMsg);
        LogNote("%s (%d gold) on %s", logMsg, cost, days[TodayIndex()]);
        SavePlayerData();
    } else {
        ShowToast("Not enough gold yet! Complete more quests!");
    }
}

// below is for profile and real life reward shop
static void PageProfile(void)
{
    int x = 40, y = 80;
    DrawAvatar(110, 145, 1);

    DrawTxt(TextFormat("%s the %s", myName, classNames[cls]), 210, y, 28, GOLD);
    DrawTxt(TextFormat("Level %d  |  XP %d/%d  |  %d Gold  |  %d-Day Streak", level, xp, XpNeeded(), gold, streak), 210, y + 40, 16, TEXT_WHITE);

    y += 120;
    DrawTxt("CORE ATTRIBUTES", x, y, 18, MINT);
    y += 28;
    for (int i = 0; i < 5; i++) { DrawStatBar(x, y, i); y += 38; }

    /* Real-life reward shop */
    int sy = 410;
    DrawCard(x, sy, 660, 140, GOLD);
    DrawTxt("REAL-LIFE REWARD SHOP (SPEND YOUR GOLD)", x + 24, sy + 14, 17, GOLD);

    if (DrawButton(x + 24, sy + 44, 190, 38, "TREAT (50g)", MINT))
        BuyShopReward(50, "Enjoy your delicious treat!", "Bought treat");
    if (DrawButton(x + 230, sy + 44, 190, 38, "1 HR GAMING (100g)", MINT))
        BuyShopReward(100, "Enjoy 1 hour of guilt-free gaming!", "Bought 1 hr gaming");
    if (DrawButton(x + 436, sy + 44, 190, 38, "MOVIE NIGHT (200g)", MINT))
        BuyShopReward(200, "Movie time! Relax and enjoy!", "Bought movie night");

    if (DrawButton(x + 24, sy + 92, 220, 36, "VIEW ACTIVITY LOGS", SKY)) { popup = 3; slide = 0.0f; }
}

// below is for sliding popup window frame from left side
static int SlidePanel(int panelWidth)
{
    float dt = GetFrameTime();
    if (popup > 0) {
        slide += dt * 5.5f;
        if (slide > 1.0f) slide = 1.0f;
    } else {
        slide -= dt * 5.5f;
        if (slide < 0.0f) slide = 0.0f;
    }

    float ease = 1.0f - (1.0f - slide) * (1.0f - slide);
    int px = (int)(-(panelWidth + 30) * (1.0f - ease));

    /* Dim background behind sliding panel */
    DrawRectangle(0, 0, W, H, (Color){ 8, 10, 16, (unsigned char)(160 * ease) });

    /* Clicking on darkened area outside panel closes it */
    if (popup > 0 && slide > 0.8f && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (GetMouseX() > px + panelWidth) popup = 0;
    }

    DrawRectangleRounded((Rectangle){ (float)px, 8, (float)panelWidth, (float)H - 16 }, 0.06f, 10, MINT);
    DrawRectangleRounded((Rectangle){ (float)px + 2, 10, (float)panelWidth - 4, (float)H - 20 }, 0.06f, 10, PANEL_BG);

    return px;
}

// below is for quest maker sliding window
static void WindowQuestMaker(void)
{
    int pw = 490;
    int px = SlidePanel(pw) + 26, py = 20;

    DrawTxt("CREATE NEW REAL-LIFE QUEST", px, py, 21, GOLD);
    if (DrawButton(px + pw - 60, py - 4, 34, 30, "X", RED)) { popup = 0; return; }
    py += 32;

    /* Step 1: Quest Title */
    DrawInput(5, px, py, 430, mkTitle, STR_LEN, "STEP 1: QUEST TITLE", 0);
    py += 58;

    /* Step 2: Schedule Day */
    DrawTxt("STEP 2: SCHEDULE DAY", px, py, 14, DIM_TEXT);
    py += 20;
    for (int i = 0; i < 7; i++)
        if (DrawButton(px + i * 62, py, 56, 30, shortDays[i], mkDay == i ? MINT : PANEL_ALT)) mkDay = i;
    py += 36;

    /* Step 3: Life Attribute */
    DrawTxt("STEP 3: LIFE ATTRIBUTE", px, py, 14, DIM_TEXT);
    py += 20;
    for (int i = 0; i < 5; i++)
        if (DrawButton(px + i * 86, py, 82, 30, statNames[i], mkStat == i ? statColors[i] : PANEL_ALT)) { mkStat = i; mkAct = 0; }
    py += 36;

    /* Step 4: Activity */
    if (mkStat >= 0 && mkStat < 5) {
        DrawTxt(TextFormat("STEP 4: %s ACTIVITY", statNames[mkStat]), px, py, 14, DIM_TEXT);
        py += 20;
        for (int i = 0; i < 5; i++)
            if (DrawButton(px + (i % 2) * 220, py + (i / 2) * 32, 210, 28, acts[mkStat][i], mkAct == i ? MINT : PANEL_ALT)) mkAct = i;
        py += 3 * 32 + 4;
    }

    /* Step 5: Amount */
    if (mkStat >= 0 && mkAct >= 0) {
        DrawTxt(TextFormat("STEP 5: TARGET AMOUNT (%s)", units[mkStat][mkAct]), px, py, 14, DIM_TEXT);
        py += 20;
        DrawInput(6, px, py, 180, mkAmt, 10, "QUANTITY", 1);
        py += 52;
    }

    /* Step 6: Difficulty */
    DrawTxt("STEP 6: QUEST DIFFICULTY", px, py, 14, DIM_TEXT);
    py += 20;
    mkDiff = DrawSlider(px, py, 430, mkDiff, 1, 3, GOLD);
    py += 32;
    DrawTxt(TextFormat("Tier: %s  (+%d Gold & XP)", diffNames[mkDiff - 1], mkDiff * 25), px, py, 15, GOLD);
    py += 30;

    if (DrawButton(px, py, 210, 42, "CONFIRM QUEST", MINT)) {
        int amt = atoi(mkAmt);
        if (strlen(mkTitle) == 0) {
            ShowToast("Please enter a Quest Title!");
            activeField = 5;
        } else if (amt <= 0) {
            ShowToast("Please enter a valid amount!");
            activeField = 6;
        } else if (qCount >= MAX_QUESTS) {
            ShowToast("Quest list is full!");
        } else {
            snprintf(quests[qCount].title, STR_LEN, "%s", mkTitle);
            quests[qCount].day = mkDay;
            quests[qCount].stat = (mkStat >= 0) ? mkStat : 0;
            quests[qCount].act = (mkAct >= 0) ? mkAct : 0;
            quests[qCount].amt = amt;
            quests[qCount].diff = (mkDiff >= 1 && mkDiff <= 3) ? mkDiff : 2;
            quests[qCount].done = 0;
            qCount++;
            SavePlayerData();
            LogNote("Quest added on %s: %s", days[mkDay], mkTitle);
            popup = 0;
            ShowToast("Quest added successfully!");
            snprintf(talkBubble, sizeof(talkBubble), "Quest locked in! You got this!");
            talkTimer = 3.0f;
            avatarBounce = 15.0f;
        }
    }
    if (DrawButton(px + 220, py, 210, 42, "CANCEL", PANEL_ALT)) popup = 0;
}

// below is for slider with label in developer mode
static int DevSlider(int x, int *py, int val, int min, int max, const char *name, Color c)
{
    val = DrawSlider(x, *py, 320, val, min, max, c);
    DrawTxt(TextFormat("%s: %d", name, val), x + 335, *py + 4, 16, c);
    *py += 44;
    return val;
}

// below is for developer mode sliding window
static void WindowDevMode(void)
{
    int pw = 510;
    int px = SlidePanel(pw) + 26, py = 20;

    DrawTxt("DEVELOPER CONTROL PANEL", px, py, 21, RED);
    if (DrawButton(px + pw - 60, py - 4, 34, 30, "X", RED)) { popup = 0; return; }
    py += 32;

    /* PIN Gate for Developer Access (Passcode: KSM-321) */
    if (!devUnlocked) {
        DrawTxt("Enter Developer PIN to unlock full manipulation:", px, py, 14, DIM_TEXT);
        py += 24;
        DrawInput(7, px, py, 320, devPinInput, STR_LEN, "PIN (KSM-321)", 0);
        py += 54;
        if (DrawButton(px, py, 155, 40, "UNLOCK", RED)) {
            if (strcmp(devPinInput, "KSM-321") == 0) {
                devUnlocked = 1;
                ShowToast("Developer Mode Granted: KSM-321");
                LogNote("Developer mode unlocked on %s", days[TodayIndex()]);
            } else ShowToast("Incorrect Developer PIN!");
        }
        if (DrawButton(px + 165, py, 155, 40, "CANCEL", PANEL_ALT)) popup = 0;
        return;
    }

    /* Developer header with KSM-321 badge & image */
    if (devImgLoaded) {
        DrawTexturePro(devImg, (Rectangle){ 0, 0, (float)devImg.width, (float)devImg.height },
                       (Rectangle){ (float)px, (float)py, 46, 46 }, (Vector2){ 0, 0 }, 0.0f, WHITE);
        DrawTxt("KSM-321 Authenticated Access", px + 56, py + 4, 17, GOLD);
        DrawTxt("Group 3: Mamoon, Sajid, Khadija", px + 56, py + 24, 14, DIM_TEXT);
    } else {
        DrawTxt("KSM-321 Developer Access", px, py + 4, 17, GOLD);
    }
    py += 52;

    /* Sliders for Gold, Level, Streak */
    gold = DevSlider(px, &py, gold, 0, 9999, "Gold", GOLD);
    level = DevSlider(px, &py, level, 1, 99, "Level", SKY);
    if (xp >= XpNeeded()) xp = 0;
    streak = DevSlider(px, &py, streak, 1, 365, "Streak", PINK);

    /* Attribute Quick Booster (+5 each) */
    DrawTxt("QUICK ATTRIBUTE BOOST (+5 EACH)", px, py, 14, DIM_TEXT);
    py += 20;
    for (int i = 0; i < 5; i++) {
        if (DrawButton(px + i * 88, py, 84, 30, statNames[i], statColors[i])) {
            stats[i] += 5;
            if (stats[i] > 100) stats[i] = 100;
        }
    }
    py += 42;

    /* Power Actions */
    if (DrawButton(px, py, 215, 38, "COMPLETE ALL TODAY", MINT)) {
        for (int i = 0; i < qCount; i++) {
            if (quests[i].day == TodayIndex() && !quests[i].done) {
                quests[i].done = 1;
                RewardPlayer(quests[i].stat, quests[i].diff);
            }
        }
        LogNote("Developer completed all today's quests on %s", days[TodayIndex()]);
        SavePlayerData();
        ShowToast("All today's quests completed!");
    }

    if (DrawButton(px + 225, py, 215, 38, "WIPE ALL QUESTS", RED)) {
        qCount = 0;
        LogNote("Developer wiped all quests on %s", days[TodayIndex()]);
        SavePlayerData();
        ShowToast("All quests wiped!");
    }
    py += 46;

    if (DrawButton(px, py, 215, 38, "RESET STREAK TO 1", SKY)) {
        streak = 1; ShowToast("Streak reset to 1.");
    }
    if (DrawButton(px + 225, py, 215, 38, "CLOSE DEV PANEL", PANEL_ALT)) popup = 0;
}

// below is for activity log sliding window
static void WindowLogs(void)
{
    int pw = 510;
    int px = SlidePanel(pw) + 26, py = 20;

    DrawTxt("YOUR ADVENTURE LOG", px, py, 21, MINT);
    if (DrawButton(px + pw - 60, py - 4, 34, 30, "X", RED)) { popup = 0; return; }
    DrawTxt(TextFormat("Saved at Logs/%s.txt", myName), px, py + 26, 14, DIM_TEXT);
    py += 54;

    FILE *f = fopen(TextFormat("Logs/%s.txt", myName), "r");
    if (!f) {
        DrawTxt("No activity logged yet. Complete quests to record history!", px, py, 15, DIM_TEXT);
    } else {
        char line[160];
        int y = py;
        while (fgets(line, sizeof(line), f) && y < H - 80) {
            Trim(line);
            if (line[0]) { DrawTxt(line, px, y, 14, TEXT_WHITE); y += 22; }
        }
        fclose(f);
    }

    if (DrawButton(px, H - 60, 440, 38, "CLOSE LOGS", PANEL_ALT)) popup = 0;
}

// below is for main game screen and popups
static void ScreenGame(void)
{
    /* Lock clicks to underlying page if popup is currently open */
    isLocked = (popup > 0);

    DrawTopBar();
    if (page == 0) PageHome();
    else if (page == 1) PageQuests();
    else if (page == 2) PageProfile();

    /* Unlock clicks so sliding popup widgets (buttons, inputs, sliders) work smoothly */
    isLocked = 0;

    if (popup == 1) WindowQuestMaker();
    else if (popup == 2) WindowDevMode();
    else if (popup == 3) WindowLogs();

    if (IsKeyPressed(KEY_ESCAPE)) popup = 0;
}

// below is for main program entry and initialization
int main(void)
{
    InitWindow(W, H, "Life Tracker RPG - Group 3");
    SetTargetFPS(60);

    // below is for centering window on monitor
    SetWindowPosition((GetMonitorWidth(0) - W) / 2, (GetMonitorHeight(0) - H) / 2);

    // below is for the window icon and developer mode picture KSM-321
    if (FileExists("KSM-321.png")) {
        SetWindowIcon(LoadImage("KSM-321.png"));
        devImg = LoadTexture("KSM-321.png");
        devImgLoaded = 1;
    }

    // below is for loading clear, high-quality font
    if (FileExists("C:/Windows/Fonts/segoeui.ttf")) {
        font = LoadFontEx("C:/Windows/Fonts/segoeui.ttf", 32, NULL, 0);
        SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
    } else if (FileExists("C:/Windows/Fonts/arial.ttf")) {
        font = LoadFontEx("C:/Windows/Fonts/arial.ttf", 32, NULL, 0);
        SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
    } else {
        font = GetFontDefault();
    }

    MKDIR("Logs");
    LoadAccounts();
    srand((unsigned)time(NULL));

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();
        if (msgTimer > 0) msgTimer--;
        if (talkTimer > 0.0f) talkTimer -= dt;
        if (avatarBounce > 0.0f) {
            avatarBounce -= dt * 60.0f;
            if (avatarBounce < 0.0f) avatarBounce = 0.0f;
        }

        UpdateConfetti();

        static int prevScreen = -1;
        if (screen != prevScreen) { activeField = 0; prevScreen = screen; }

        BeginDrawing();
        ClearBackground(BG_COLOR);
        DrawStars();

        if (screen == 0) ScreenLogin();
        else if (screen == 1) ScreenSignup();
        else ScreenGame();

        // below is for toast message notification at bottom
        if (msgTimer > 0) {
            int toastW = MeasureTxt(msgText, 17) + 48;
            int tx = (W - toastW) / 2, ty = H - 52;
            DrawRectangleRounded((Rectangle){ (float)tx, (float)ty, (float)toastW, 38 }, 0.5f, 8, GOLD);
            DrawTxtMid(msgText, W / 2, ty + 10, 17, INK_BLACK);
        }

        EndDrawing();
    }

    // below is for saving player data and accounts on exit
    SavePlayerData();
    SaveAccounts();

    if (devImgLoaded) UnloadTexture(devImg);
    CloseWindow();
    return 0;
}
