/* ============================================================================
   LIFE TRACKER RPG - Group 3 (Mamoon, Sajid, Khadija)
   Programming Fundamentals Lab - FAST NUCES
   File: gui.c (Raylib Graphics, Interface & Main Window)

   This file handles all user interface rendering, window drawing,
   animations, screen navigation, input handling, and the main event loop.
   Written with clean, basic C syntax (simple loops and if/else statements).
   ============================================================================ */

#include "raylib.h"
#include "logic.h"
#include <math.h>

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

static const Color statColors[5] = { SKY, MINT, GOLD, PINK, PURPLE };

/* ------------------------------------------------------------ GUI STATE ---*/
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

typedef struct {
    float x, y, vx, vy, life;
    Color col;
} Particle;

static Particle bits[80];
static int      bitCount = 0;
static int      sliderDragging = 0;

static int starX[45];
static int starY[45];
static int starsReady = 0;

/* ------------------------------------------------- REUSABLE GUI WIDGETS ---*/

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

// below is for drawing clean interactive button without flickering
static int DrawButton(int x, int y, int w, int h, const char *label, Color col)
{
    Rectangle r = { (float)x, (float)y, (float)w, (float)h };
    int hovered = 0;
    if (!isLocked && CheckCollisionPointRec(GetMousePosition(), r)) {
        hovered = 1;
    }

    Color fill = PANEL_ALT;
    if (hovered == 1) {
        fill = MINT_DARK;
    }

    DrawRectangleRounded(r, 0.22f, 6, fill);
    DrawRectangleRoundedLines(r, 0.22f, 6, col);
    DrawTxtMid(label, x + w / 2, y + (h - 16) / 2, 16, TEXT_WHITE);

    if (hovered == 1 && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        return 1;
    }
    return 0;
}

// below is for handling keyboard typing into input buffer
static void HandleTyping(char *buf, int maxLen, int numsOnly)
{
    int ch = GetCharPressed();
    while (ch > 0) {
        int valid = 1;
        if (ch < 32 || ch > 125) valid = 0;
        if (ch == ';') valid = 0;
        if ((int)strlen(buf) >= maxLen - 1) valid = 0;
        if (numsOnly == 1 && (ch < '0' || ch > '9')) valid = 0;

        if (valid == 1) {
            int n = (int)strlen(buf);
            buf[n] = (char)ch;
            buf[n + 1] = '\0';
        }
        ch = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        int n = (int)strlen(buf);
        if (n > 0) {
            buf[n - 1] = '\0';
        }
    }
}

// below is for drawing text input box with cursor
static void DrawInput(int id, int x, int y, int w, char *buf, int maxLen, int numsOnly)
{
    Rectangle r = { (float)x, (float)y, (float)w, 36.0f };
    int active = 0;
    if (activeField == id) active = 1;

    if (!isLocked && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), r)) {
        activeField = id;
    }

    Color lineCol = DIM_TEXT;
    if (active == 1) lineCol = MINT;

    DrawRectangleRounded(r, 0.22f, 6, PANEL_ALT);
    DrawRectangleRoundedLines(r, 0.22f, 6, lineCol);
    DrawTxt(buf, x + 10, y + 10, 16, TEXT_WHITE);

    if (active == 1) {
        if (((int)(GetTime() * 2.0f) % 2) == 0) {
            DrawTxt("_", x + 12 + MeasureTxt(buf, 16), y + 10, 16, MINT);
        }
        HandleTyping(buf, maxLen, numsOnly);
    }
}

// below is for interactive value drag slider
static int DrawSlider(int x, int y, int w, int val, int minVal, int maxVal, Color col)
{
    float ratio = (float)(val - minVal) / (float)(maxVal - minVal);
    DrawRectangleRounded((Rectangle){ (float)x, (float)y + 6, (float)w, 10 }, 0.5f, 6, PANEL_ALT);
    DrawRectangleRounded((Rectangle){ (float)x, (float)y + 6, (float)(w * ratio), 10 }, 0.5f, 6, col);
    DrawCircleV((Vector2){ (float)x + ratio * w, (float)y + 11 }, 12, col);

    Rectangle hit = { (float)x - 10, (float)y - 10, (float)w + 20, 42 };
    if (!isLocked && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), hit)) {
        sliderDragging = 1;
    }
    if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        sliderDragging = 0;
    }

    if (sliderDragging == 1) {
        float frac = (float)(GetMouseX() - x) / (float)w;
        if (frac < 0.0f) frac = 0.0f;
        if (frac > 1.0f) frac = 1.0f;
        val = minVal + (int)((maxVal - minVal) * frac + 0.5f);
    }
    return val;
}

// below is for making celebration confetti fly
static void SpawnConfetti(int x, int y)
{
    for (int i = 0; i < 26; i++) {
        if (bitCount >= 80) break;
        int k = bitCount;
        bits[k].x = (float)x;
        bits[k].y = (float)y;

        float angle = (float)(rand() % 360) * DEG2RAD;
        float speed = 70.0f + (float)(rand() % 190);
        bits[k].vx = cosf(angle) * speed;
        bits[k].vy = sinf(angle) * speed - 100.0f;
        bits[k].life = 1.0f;

        int roll = rand() % 3;
        if (roll == 0) bits[k].col = GOLD;
        else if (roll == 1) bits[k].col = MINT;
        else bits[k].col = PINK;

        bitCount++;
    }
}

// below is for updating and drawing confetti particles
static void UpdateConfetti(void)
{
    float dt = GetFrameTime();
    for (int i = 0; i < bitCount; i++) {
        bits[i].x += bits[i].vx * dt;
        bits[i].y += bits[i].vy * dt;
        bits[i].vy += 280.0f * dt;
        bits[i].life -= dt * 1.3f;

        if (bits[i].life <= 0.0f) {
            bits[i] = bits[bitCount - 1];
            bitCount--;
            i--;
        }
    }
    for (int i = 0; i < bitCount; i++) {
        DrawRectangle((int)bits[i].x, (int)bits[i].y, 6, 6, Fade(bits[i].col, bits[i].life));
    }
}

// below is for drawing attribute progress bar
static void DrawStatBar(int x, int y, int index)
{
    int v = stats[index];
    if (v < 0) v = 0;
    if (v > 100) v = 100;

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
    int current = xp;
    if (current > needed) current = needed;

    DrawRectangleRounded((Rectangle){ (float)x, (float)y + 22, (float)w, 16 }, 0.5f, 7, PANEL_ALT);
    DrawRectangleRounded((Rectangle){ (float)x, (float)y + 22, (float)(w * current / needed), 16 }, 0.5f, 7, GOLD);
    DrawTxt(TextFormat("XP: %d / %d", xp, needed), x + w + 16, y + 21, 15, DIM_TEXT);
}

// below is for drawing the interactive animated avatar
static void DrawAvatar(int cx, int cy, int scale)
{
    float t = (float)GetTime();
    int bob = (int)(sinf(t * 2.4f) * 2.5f * scale);
    cy = cy + bob - (int)avatarBounce;

    Color suit = SKY;
    if (cls == 1) suit = MINT;
    else if (cls == 2) suit = PINK;

    DrawRectangleRounded((Rectangle){ (float)cx - 26 * scale, (float)cy + 14 * scale, (float)52 * scale, (float)40 * scale }, 0.4f, 6, suit);
    DrawCircleV((Vector2){ (float)cx - 32 * scale, (float)cy + 34 * scale }, 9.0f * scale, suit);
    DrawCircleV((Vector2){ (float)cx + 32 * scale, (float)cy + 34 * scale }, 9.0f * scale, suit);
    DrawCircleV((Vector2){ (float)cx, (float)cy }, 24.0f * scale, (Color){ 245, 214, 180, 255 });

    if (cls == 0) {
        DrawRectangleRounded((Rectangle){ (float)cx - 26 * scale, (float)cy - 28 * scale, (float)52 * scale, (float)12 * scale }, 0.4f, 5, SKY);
        DrawCircle((int)(cx + 20 * scale), (int)(cy - 20 * scale), 4.0f * scale, GOLD);
    } else if (cls == 1) {
        DrawRectangleRounded((Rectangle){ (float)cx - 24 * scale, (float)cy - 14 * scale, (float)48 * scale, (float)9 * scale }, 0.4f, 4, RED);
    } else {
        DrawCircleV((Vector2){ (float)cx, (float)cy - 18 * scale }, 14.0f * scale, GOLD);
        DrawCircleV((Vector2){ (float)cx + 8 * scale, (float)cy - 22 * scale }, 4.0f * scale, PURPLE);
    }

    int blink = 4;
    if ((int)(t * 1.6f) % 5 == 0) blink = 1;

    float lookX = (float)(GetMouseX() - cx) * 0.02f;
    if (lookX < -2.0f) lookX = -2.0f;
    if (lookX > 2.0f) lookX = 2.0f;

    DrawRectangle(cx - 12 * scale + (int)lookX, cy - 6 * scale, 8 * scale, blink * scale, INK_BLACK);
    DrawRectangle(cx + 4 * scale + (int)lookX, cy - 6 * scale, 8 * scale, blink * scale, INK_BLACK);
    DrawLineEx((Vector2){ (float)cx - 8 * scale, (float)cy + 8 * scale }, (Vector2){ (float)cx + 8 * scale, (float)cy + 8 * scale }, 2.5f * scale, INK_BLACK);

    if (talkTimer > 0.0f) {
        int bubbleW = MeasureTxt(talkBubble, 15) + 30;
        int bx = cx - bubbleW / 2;
        int by = cy - 76 * scale;
        DrawRectangleRounded((Rectangle){ (float)bx, (float)by, (float)bubbleW, 36 }, 0.35f, 8, TEXT_WHITE);
        DrawTriangle((Vector2){ (float)cx - 8, (float)by + 36 }, (Vector2){ (float)cx + 8, (float)by + 36 }, (Vector2){ (float)cx, (float)by + 46 }, TEXT_WHITE);
        DrawTxtMid(talkBubble, cx, by + 10, 15, INK_BLACK);
    }
}

// below is for triggering avatar speech
static void TriggerAvatarTalk(void)
{
    snprintf(talkBubble, sizeof(talkBubble), "%s", tips[rand() % 8]);
    talkTimer = 3.5f;
    avatarBounce = 16.0f;
    SpawnConfetti(GetMouseX(), GetMouseY());
}

// below is for drawing twinkling background stars
static void DrawStars(void)
{
    if (starsReady == 0) {
        for (int i = 0; i < 45; i++) {
            starX[i] = rand() % W;
            starY[i] = rand() % (H - 50);
        }
        starsReady = 1;
    }
    for (int i = 0; i < 45; i++) {
        int alpha = 100 + (int)(110 * sinf((float)GetTime() * 2.2f + i));
        DrawCircle(starX[i], starY[i], 1.5f, (Color){ 210, 220, 255, (unsigned char)alpha });
    }
}

// below is for sliding popup window drawer
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

    DrawRectangle(0, 0, W, H, (Color){ 8, 10, 16, (unsigned char)(160 * ease) });

    if (popup > 0 && slide > 0.8f && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (GetMouseX() > px + panelWidth) popup = 0;
    }

    DrawRectangleRounded((Rectangle){ (float)px, 8, (float)panelWidth, (float)H - 16 }, 0.06f, 10, MINT);
    DrawRectangleRounded((Rectangle){ (float)px + 2, 10, (float)panelWidth - 4, (float)H - 20 }, 0.06f, 10, PANEL_BG);
    return px;
}

/* ------------------------------------------------------------- SCREENS ----*/

// below is for the login screen
static void ScreenLogin(void)
{
    DrawAvatar(190, 300, 3);
    if (!isLocked && CheckCollisionPointCircle(GetMousePosition(), (Vector2){ 190, 300 }, 80) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        TriggerAvatarTalk();
    }

    DrawTxtMid("LIFE TRACKER RPG", 620, 80, 42, GOLD);
    DrawTxtMid("Level Up Your Real Life Through Action", 620, 134, 17, DIM_TEXT);

    int bx = 470;
    DrawTxt("YOUR HERO NAME", bx, 200, 14, DIM_TEXT);
    DrawInput(1, bx, 220, 310, loginUser, STR_LEN, 0);

    DrawTxt("YOUR 4-DIGIT PIN", bx, 275, 14, DIM_TEXT);
    DrawInput(2, bx, 295, 310, loginPin, 10, 1);

    if (DrawButton(bx, 365, 310, 44, "LOGIN", MINT)) {
        if (TryLogin(loginUser, loginPin) == 1) {
            if (streak > 1) ShowToast(TextFormat("Welcome back! %d-day streak!", streak));
            screen = 2;
        } else {
            ShowToast("Invalid Hero Name or PIN!");
        }
    }

    if (DrawButton(bx, 425, 310, 38, "NEW HERO? CREATE ACCOUNT", GOLD)) {
        screen = 1;
    }
}

// below is for the signup screen
static void ScreenSignup(void)
{
    DrawAvatar(190, 300, 3);
    DrawTxtMid("HERO REGISTRATION", 620, 70, 36, GOLD);
    int bx = 470;

    DrawTxt("HERO NAME", bx, 130, 14, DIM_TEXT);
    DrawInput(3, bx, 150, 310, newUser, STR_LEN, 0);

    DrawTxt("SET PIN (NUMBERS)", bx, 205, 14, DIM_TEXT);
    DrawInput(4, bx, 225, 310, newPin, 10, 1);

    DrawTxt("CHOOSE YOUR HERO CLASS", bx, 280, 14, DIM_TEXT);
    for (int i = 0; i < 3; i++) {
        Color col = (selClass == i) ? MINT : SKY;
        if (DrawButton(bx + i * 106, 302, 98, 34, classNames[i], col)) {
            selClass = i;
        }
    }
    DrawTxt(classDesc[selClass], bx, 348, 14, DIM_TEXT);

    if (DrawButton(bx, 390, 310, 44, "JOIN THE REALM", MINT)) {
        int res = TrySignup(newUser, newPin, selClass);
        if (res == 0) ShowToast("Name and PIN are required!");
        else if (res == -1) ShowToast("Hero name already registered!");
        else if (res == -2) ShowToast("Hero account limit reached!");
        else if (res == 1) {
            ShowToast("Welcome, Hero! Begin your journey.");
            screen = 2;
        }
    }

    if (DrawButton(bx, 448, 310, 36, "BACK TO LOGIN", SKY)) {
        screen = 0;
    }
}

// below is for top navigation bar
static void DrawTopBar(void)
{
    DrawRectangle(0, 0, W, 56, PANEL_BG);
    DrawRectangle(0, 54, W, 2, MINT);

    const char *tabs[3] = { "HOME", "QUESTS", "PROFILE" };
    for (int i = 0; i < 3; i++) {
        Color col = (page == i) ? MINT : SKY;
        if (DrawButton(20 + i * 140, 10, 125, 36, tabs[i], col)) {
            page = i;
            popup = 0;
        }
    }

    DrawCircle(W - 320, 28, 9, GOLD);
    DrawTxt(TextFormat("%d Gold", gold), W - 304, 18, 18, GOLD);
    DrawTxt(TextFormat("%d-Day Streak", streak), W - 200, 18, 17, MINT);

    if (DrawButton(W - 86, 10, 70, 36, "DEV", RED)) {
        popup = 2;
        slide = 0.0f;
        devPinInput[0] = '\0';
        activeField = 7;
    }
}

// below is for home dashboard page
static void PageHome(void)
{
    int x = 40, y = 80;
    DrawAvatar(120, 190, 2);
    if (!isLocked && CheckCollisionPointCircle(GetMousePosition(), (Vector2){ 120, 190 }, 60) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        TriggerAvatarTalk();
    }

    DrawTxt(TextFormat("Greetings, %s the %s!", myName, classNames[cls]), 240, 120, 28, GOLD);
    DrawTxt(classDesc[cls], 240, 160, 15, DIM_TEXT);
    DrawXpBar(240, 198, 380);

    y = y + 225;
    for (int i = 0; i < 5; i++) {
        DrawStatBar(x, y, i);
        y += 38;
    }

    int tx = 640, ty = 280;
    DrawCard(tx, ty, 340, 250, MINT);
    DrawTxt("TODAY'S MISSION", tx + 24, ty + 18, 20, MINT);
    DrawTxt(days[TodayIndex()], tx + 24, ty + 46, 25, TEXT_WHITE);
    DrawTxt(fortunes[TodayIndex()], tx + 24, ty + 84, 14, GOLD);

    int pendingToday = 0;
    for (int i = 0; i < qCount; i++) {
        if (quests[i].day == TodayIndex() && quests[i].done == 0) pendingToday++;
    }

    DrawTxt(TextFormat("Active quests today: %d", pendingToday), tx + 24, ty + 114, 15, DIM_TEXT);
    DrawTxt(TextFormat("Current streak: %d days", streak), tx + 24, ty + 140, 15, PINK);
    DrawTxt(TextFormat("Available gold: %d", gold), tx + 24, ty + 166, 15, GOLD);

    if (DrawButton(tx + 24, ty + 196, 140, 34, "REST TODAY", SKY)) {
        ShowToast("Rest and recovery are vital for champions!");
    }
}

// below is for weekly quests page
static void PageQuests(void)
{
    for (int i = 0; i < 7; i++) {
        Color col = PANEL_ALT;
        if (selDay == i) col = MINT;
        else if (i == TodayIndex()) col = GOLD;
        if (DrawButton(40 + i * 94, 76, 86, 34, shortDays[i], col)) {
            selDay = i;
        }
    }

    DrawTxt(TextFormat("%s QUESTS", days[selDay]), 40, 126, 21, TEXT_WHITE);

    int y = 160, count = 0;
    for (int i = 0; i < qCount; i++) {
        if (quests[i].day != selDay) continue;
        count++;

        Color cardCol = (quests[i].done == 1) ? MINT : PANEL_ALT;
        DrawCard(40, y, 840, 60, cardCol);

        DrawTxt(quests[i].title, 60, y + 10, 18, TEXT_WHITE);
        DrawTxt(TextFormat("%s: %d %s  |  %s  |  %s",
                           acts[quests[i].stat][quests[i].act], quests[i].amt,
                           units[quests[i].stat][quests[i].act],
                           statNames[quests[i].stat], diffNames[quests[i].diff - 1]),
                60, y + 34, 14, DIM_TEXT);

        if (quests[i].done == 1) {
            DrawTxt("COMPLETED", 725, y + 20, 17, MINT);
        } else {
            if (DrawButton(680, y + 12, 85, 36, "DONE", MINT)) {
                int gained = 0;
                int newLvl = CompleteQuest(i, &gained);
                SpawnConfetti(W / 2, H / 2);
                ShowToast(TextFormat("+%d Gold and XP earned! Great job!", gained));

                if (newLvl > 0) {
                    ShowToast(TextFormat("LEVEL UP! You reached Level %d!", newLvl));
                    snprintf(talkBubble, sizeof(talkBubble), "Level %d reached! Amazing!", newLvl);
                    talkTimer = 3.5f;
                    avatarBounce = 20.0f;
                }
            }
            if (DrawButton(775, y + 12, 50, 36, "X", RED)) {
                DeleteQuest(i);
                continue;
            }
        }
        y += 68;
    }

    if (count == 0) {
        DrawTxtMid("No quests scheduled for this day yet. Add one with + ADD QUEST!", W / 2, 280, 17, DIM_TEXT);
    }

    if (DrawButton(W - 200, H - 64, 160, 42, "+ ADD QUEST", GOLD)) {
        popup = 1;
        slide = 0.0f;
        mkTitle[0] = '\0';
        activeField = 5;
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
    for (int i = 0; i < 5; i++) {
        DrawStatBar(x, y, i);
        y += 38;
    }

    int sy = 410;
    DrawCard(x, sy, 660, 140, GOLD);
    DrawTxt("REAL-LIFE REWARD SHOP (SPEND YOUR GOLD)", x + 24, sy + 14, 17, GOLD);

    if (DrawButton(x + 24, sy + 44, 190, 36, "TREAT (50g)", MINT)) {
        if (BuyShopReward(50, "treat") == 1) ShowToast("Enjoy your delicious treat!");
        else ShowToast("Not enough gold yet! Complete more quests!");
    }
    if (DrawButton(x + 230, sy + 44, 190, 36, "1 HR GAMING (100g)", MINT)) {
        if (BuyShopReward(100, "1 hr gaming") == 1) ShowToast("Enjoy 1 hour of guilt-free gaming!");
        else ShowToast("Not enough gold yet! Complete more quests!");
    }
    if (DrawButton(x + 436, sy + 44, 190, 36, "MOVIE NIGHT (200g)", MINT)) {
        if (BuyShopReward(200, "movie night") == 1) ShowToast("Movie time! Relax and enjoy!");
        else ShowToast("Not enough gold yet! Complete more quests!");
    }

    if (DrawButton(x + 24, sy + 92, 220, 34, "VIEW ACTIVITY LOGS", SKY)) {
        popup = 3;
        slide = 0.0f;
    }
}

// below is for quest maker sliding window with clean layout and no overlapping text
static void WindowQuestMaker(void)
{
    int pw = 490;
    int px = SlidePanel(pw) + 26, py = 18;

    DrawTxt("CREATE NEW REAL-LIFE QUEST", px, py, 21, GOLD);
    if (DrawButton(px + pw - 60, py - 4, 34, 30, "X", RED)) {
        popup = 0;
        return;
    }
    py += 32;

    /* Step 1: Quest Title */
    DrawTxt("STEP 1: QUEST TITLE", px, py, 14, DIM_TEXT);
    py += 20;
    DrawInput(5, px, py, 430, mkTitle, STR_LEN, 0);
    py += 46;

    /* Step 2: Schedule Day */
    DrawTxt("STEP 2: SCHEDULE DAY", px, py, 14, DIM_TEXT);
    py += 20;
    for (int i = 0; i < 7; i++) {
        Color col = (mkDay == i) ? MINT : PANEL_ALT;
        if (DrawButton(px + i * 62, py, 56, 28, shortDays[i], col)) mkDay = i;
    }
    py += 36;

    /* Step 3: Life Attribute */
    DrawTxt("STEP 3: LIFE ATTRIBUTE", px, py, 14, DIM_TEXT);
    py += 20;
    for (int i = 0; i < 5; i++) {
        Color col = (mkStat == i) ? statColors[i] : PANEL_ALT;
        if (DrawButton(px + i * 86, py, 82, 28, statNames[i], col)) {
            mkStat = i;
            mkAct = 0;
        }
    }
    py += 36;

    /* Step 4: Activity */
    DrawTxt(TextFormat("STEP 4: %s ACTIVITY", statNames[mkStat]), px, py, 14, DIM_TEXT);
    py += 20;
    for (int i = 0; i < 5; i++) {
        Color col = (mkAct == i) ? MINT : PANEL_ALT;
        int colIdx = i % 2;
        int rowIdx = i / 2;
        if (DrawButton(px + colIdx * 220, py + rowIdx * 32, 210, 26, acts[mkStat][i], col)) {
            mkAct = i;
        }
    }
    py += 3 * 32 + 6; /* Space for the 3 rows of activities */

    /* Step 5: Amount - Distinct spacing with no text collision */
    DrawTxt(TextFormat("STEP 5: TARGET AMOUNT (%s)", units[mkStat][mkAct]), px, py, 14, DIM_TEXT);
    py += 20;
    DrawInput(6, px, py, 180, mkAmt, 10, 1);
    py += 46;

    /* Step 6: Difficulty */
    DrawTxt("STEP 6: QUEST DIFFICULTY", px, py, 14, DIM_TEXT);
    py += 18;
    mkDiff = DrawSlider(px, py, 430, mkDiff, 1, 3, GOLD);
    py += 28;
    DrawTxt(TextFormat("Tier: %s  (+%d Gold & XP)", diffNames[mkDiff - 1], mkDiff * 25), px, py, 14, GOLD);
    py += 24;

    /* Confirm & Cancel Buttons */
    if (DrawButton(px, py, 210, 38, "CONFIRM QUEST", MINT)) {
        int amt = atoi(mkAmt);
        int res = AddQuest(mkTitle, mkDay, mkStat, mkAct, amt, mkDiff);
        if (res == 0) {
            ShowToast("Please enter a Quest Title!");
            activeField = 5;
        } else if (res == -1) {
            ShowToast("Please enter a valid amount!");
            activeField = 6;
        } else if (res == -2) {
            ShowToast("Quest list is full!");
        } else if (res == 1) {
            popup = 0;
            ShowToast("Quest added successfully!");
            snprintf(talkBubble, sizeof(talkBubble), "Quest locked in! You got this!");
            talkTimer = 3.0f;
            avatarBounce = 15.0f;
        }
    }
    if (DrawButton(px + 220, py, 210, 38, "CANCEL", PANEL_ALT)) {
        popup = 0;
    }
}

// below is for developer mode sliding window
static void WindowDevMode(void)
{
    int pw = 510;
    int px = SlidePanel(pw) + 26, py = 20;

    DrawTxt("DEVELOPER CONTROL PANEL", px, py, 21, RED);
    if (DrawButton(px + pw - 60, py - 4, 34, 30, "X", RED)) {
        popup = 0;
        return;
    }
    py += 32;

    if (devUnlocked == 0) {
        DrawTxt("Enter Developer PIN (KSM-321):", px, py, 14, DIM_TEXT);
        py += 22;
        DrawInput(7, px, py, 320, devPinInput, STR_LEN, 0);
        py += 48;

        if (DrawButton(px, py, 155, 38, "UNLOCK", RED)) {
            if (strcmp(devPinInput, "KSM-321") == 0) {
                devUnlocked = 1;
                ShowToast("Developer Mode Granted: KSM-321");
                char note[120];
                sprintf(note, "Developer mode unlocked on %s", days[TodayIndex()]);
                LogNote(note);
            } else {
                ShowToast("Incorrect Developer PIN!");
            }
        }
        if (DrawButton(px + 165, py, 155, 38, "CANCEL", PANEL_ALT)) popup = 0;
        return;
    }

    if (devImgLoaded == 1) {
        DrawTexturePro(devImg, (Rectangle){ 0, 0, (float)devImg.width, (float)devImg.height },
                       (Rectangle){ (float)px, (float)py, 46, 46 }, (Vector2){ 0, 0 }, 0.0f, WHITE);
        DrawTxt("KSM-321 Authenticated Access", px + 56, py + 4, 17, GOLD);
        DrawTxt("Group 3: Mamoon, Sajid, Khadija", px + 56, py + 24, 14, DIM_TEXT);
    } else {
        DrawTxt("KSM-321 Developer Access", px, py + 4, 17, GOLD);
    }
    py += 52;

    gold = DrawSlider(px, py, 320, gold, 0, 9999, GOLD);
    DrawTxt(TextFormat("Gold: %d", gold), px + 335, py + 4, 16, GOLD);
    py += 42;

    level = DrawSlider(px, py, 320, level, 1, 99, SKY);
    DrawTxt(TextFormat("Level: %d", level), px + 335, py + 4, 16, SKY);
    if (xp >= XpNeeded()) xp = 0;
    py += 42;

    streak = DrawSlider(px, py, 320, streak, 1, 365, PINK);
    DrawTxt(TextFormat("Streak: %d", streak), px + 335, py + 4, 16, PINK);
    py += 44;

    DrawTxt("QUICK ATTRIBUTE BOOST (+5 EACH)", px, py, 14, DIM_TEXT);
    py += 20;
    for (int i = 0; i < 5; i++) {
        if (DrawButton(px + i * 88, py, 84, 28, statNames[i], statColors[i])) {
            stats[i] += 5;
            if (stats[i] > 100) stats[i] = 100;
        }
    }
    py += 40;

    if (DrawButton(px, py, 215, 36, "COMPLETE ALL TODAY", MINT)) {
        DevCompleteAllToday();
        ShowToast("All today's quests completed!");
    }
    if (DrawButton(px + 225, py, 215, 36, "WIPE ALL QUESTS", RED)) {
        DevWipeAllQuests();
        ShowToast("All quests wiped!");
    }
    py += 44;

    if (DrawButton(px, py, 215, 36, "RESET STREAK TO 1", SKY)) {
        DevResetStreak();
        ShowToast("Streak reset to 1.");
    }
    if (DrawButton(px + 225, py, 215, 36, "CLOSE DEV PANEL", PANEL_ALT)) popup = 0;
}

// below is for activity log sliding window
static void WindowLogs(void)
{
    int pw = 510;
    int px = SlidePanel(pw) + 26, py = 20;

    DrawTxt("YOUR ADVENTURE LOG", px, py, 21, MINT);
    if (DrawButton(px + pw - 60, py - 4, 34, 30, "X", RED)) {
        popup = 0;
        return;
    }
    DrawTxt(TextFormat("Saved at Logs/%s.txt", myName), px, py + 26, 14, DIM_TEXT);
    py += 54;

    FILE *f = fopen(TextFormat("Logs/%s.txt", myName), "r");
    if (f == NULL) {
        DrawTxt("No activity logged yet. Complete quests to record history!", px, py, 15, DIM_TEXT);
    } else {
        char line[160];
        int y = py;
        while (fgets(line, sizeof(line), f) != NULL && y < H - 80) {
            TrimString(line);
            if (line[0] != '\0') {
                DrawTxt(line, px, y, 14, TEXT_WHITE);
                y += 22;
            }
        }
        fclose(f);
    }

    if (DrawButton(px, H - 60, 440, 36, "CLOSE LOGS", PANEL_ALT)) popup = 0;
}

// below is for main game screen and popups
static void ScreenGame(void)
{
    isLocked = (popup > 0);

    DrawTopBar();
    if (page == 0) PageHome();
    else if (page == 1) PageQuests();
    else if (page == 2) PageProfile();

    isLocked = 0;

    if (popup == 1) WindowQuestMaker();
    else if (popup == 2) WindowDevMode();
    else if (popup == 3) WindowLogs();

    if (IsKeyPressed(KEY_ESCAPE)) popup = 0;
}

/* -------------------------------------------------------- MAIN FUNCTION ---*/

// below is for main program entry and initialization
int main(void)
{
    InitWindow(W, H, "Life Tracker RPG - Group 3");
    SetTargetFPS(60);

    SetWindowPosition((GetMonitorWidth(0) - W) / 2, (GetMonitorHeight(0) - H) / 2);

    if (FileExists("KSM-321.png")) {
        SetWindowIcon(LoadImage("KSM-321.png"));
        devImg = LoadTexture("KSM-321.png");
        devImgLoaded = 1;
    }

    if (FileExists("C:/Windows/Fonts/segoeui.ttf")) {
        font = LoadFontEx("C:/Windows/Fonts/segoeui.ttf", 32, NULL, 0);
        SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
    } else if (FileExists("C:/Windows/Fonts/arial.ttf")) {
        font = LoadFontEx("C:/Windows/Fonts/arial.ttf", 32, NULL, 0);
        SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
    } else {
        font = GetFontDefault();
    }

    InitGameLogic();

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
        if (screen != prevScreen) {
            activeField = 0;
            prevScreen = screen;
        }

        BeginDrawing();
        ClearBackground(BG_COLOR);
        DrawStars();

        if (screen == 0) ScreenLogin();
        else if (screen == 1) ScreenSignup();
        else ScreenGame();

        if (msgTimer > 0) {
            int toastW = MeasureTxt(msgText, 17) + 48;
            int tx = (W - toastW) / 2;
            int ty = H - 52;
            DrawRectangleRounded((Rectangle){ (float)tx, (float)ty, (float)toastW, 38 }, 0.5f, 8, GOLD);
            DrawTxtMid(msgText, W / 2, ty + 10, 17, INK_BLACK);
        }

        EndDrawing();
    }

    SavePlayerData();
    SaveAccounts();

    if (devImgLoaded == 1) UnloadTexture(devImg);
    CloseWindow();
    return 0;
}
