/* ============================================================================
   LIFE TRACKER RPG - Group 3 (Mamoon, Sajid, Khadija)
   Programming Fundamentals Lab - FAST NUCES
   File: logic.c (Pure Game Logic & File Persistence)
   ============================================================================ */

#include "logic.h"

/* ------------------------------------------------------------ DATA ARRAYS --*/
const char *days[7]       = { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };
const char *shortDays[7]  = { "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" };
const char *statNames[5]  = { "Intellect", "Stamina", "Discipline", "Social", "Creativity" };
const char *classNames[3] = { "Scholar", "Athlete", "Artist" };
const char *classDesc[3]  = { "Master of mind and wisdom", "Master of physical endurance", "Master of creativity and arts" };
const char *diffNames[3]  = { "Easy", "Normal", "Hard" };

const char *acts[5][5] = {
    { "Read a Book",     "Solve Math",     "Meditation",     "Learn Words",      "Coding Time" },
    { "Running",         "Push Ups",       "Pull Ups",       "Sit Ups",          "Cycling" },
    { "Wake Up Early",   "Walk Outside",   "No Junk Food",   "Make My Bed",      "Plan My Day" },
    { "Meet New People", "Call Family",    "Group Study",    "Compliment Others","Help Someone" },
    { "Draw Art",        "Write a Story",  "Play Music",     "Make a Craft",     "Take Photos" }
};

const char *units[5][5] = {
    { "pages", "problems", "minutes", "words", "minutes" },
    { "km",    "reps",     "reps",    "reps",  "km" },
    { "days",  "minutes",  "days",    "days",  "minutes" },
    { "people","minutes",  "hours",   "times", "times" },
    { "drawings","lines",  "minutes", "crafts","photos" }
};

const char *tips[8] = {
    "Click DONE only when you really did it!",
    "Hard quests grant massive Gold and XP bonuses!",
    "Stay hydrated, hero! Water keeps your mind sharp.",
    "Log in daily to keep your streak burning hot!",
    "Spend your hard-earned gold on real-life treats!",
    "Small daily habits forge legendary futures.",
    "Try a Creativity quest - nourish your soul!",
    "Sleep well tonight - your stats recharge during rest."
};

const char *fortunes[7] = {
    "Monday momentum: conquer your goals!",
    "Tuesday focus: small steps yield big levels.",
    "Midweek victory: your mind is razor sharp.",
    "Thursday grind: push through to greatness.",
    "Friday glory: finish strong and celebrate.",
    "Weekend training: balance effort and rest.",
    "Sunday strategy: plan ahead and win."
};

/* ----------------------------------------------------------- GLOBAL STATE --*/
char accNames[MAX_ACC][STR_LEN];
char accPins[MAX_ACC][10];
int  accCount = 0;

char myName[STR_LEN] = "";
int  level = 1, xp = 0, gold = 50, cls = 0, streak = 1;
long lastDay = -1;
int  stats[5] = { 10, 10, 10, 10, 10 };

Quest quests[MAX_QUESTS];
int   qCount = 0;

/* -------------------------------------------------------- IMPLEMENTATIONS --*/

// below is for cutting enter and spaces off text
void TrimString(char *s)
{
    int length = (int)strlen(s);
    while (length > 0 && (s[length - 1] == '\n' || s[length - 1] == '\r' || s[length - 1] == ' ')) {
        length = length - 1;
        s[length] = '\0';
    }
}

// below is for getting current day index from 0 to 6
int TodayIndex(void)
{
    time_t timeNow = time(NULL);
    struct tm *dateInfo = localtime(&timeNow);
    return (dateInfo->tm_wday + 6) % 7;
}

// below is for calculating xp needed for next level
int XpNeeded(void)
{
    return level * 100;
}

// below is for logging one finished sentence to player log file
void LogNote(const char *sentence)
{
    if (myName[0] == '\0') {
        return;
    }
    MKDIR("Logs");
    char path[120];
    sprintf(path, "Logs/%s.txt", myName);
    FILE *logFile = fopen(path, "a");
    if (logFile == NULL) {
        return;
    }

    time_t timeNow = time(NULL);
    struct tm *dateInfo = localtime(&timeNow);
    fprintf(logFile, "%s (%02d-%02d-%d %02d:%02d)\n",
            sentence, dateInfo->tm_mday, dateInfo->tm_mon + 1, dateInfo->tm_year + 1900, dateInfo->tm_hour, dateInfo->tm_min);
    fclose(logFile);
}

// below is for saving all accounts to accounts.txt
void SaveAccounts(void)
{
    FILE *accountsFile = fopen("accounts.txt", "w");
    if (accountsFile == NULL) {
        return;
    }
    for (int i = 0; i < accCount; i++) {
        fprintf(accountsFile, "Name: %s\nPIN: %s\n\n", accNames[i], accPins[i]);
    }
    fclose(accountsFile);
}

// below is for loading accounts from accounts.txt
void LoadAccounts(void)
{
    FILE *accountsFile = fopen("accounts.txt", "r");
    char line[128];
    accCount = 0;
    if (accountsFile == NULL) {
        return;
    }

    while (fgets(line, sizeof(line), accountsFile) != NULL && accCount < MAX_ACC) {
        TrimString(line);
        if (strncmp(line, "Name: ", 6) == 0) {
            strncpy(accNames[accCount], &line[6], STR_LEN - 1);
            accNames[accCount][STR_LEN - 1] = '\0';
        } else if (strncmp(line, "PIN: ", 5) == 0) {
            strncpy(accPins[accCount], &line[5], 9);
            accPins[accCount][9] = '\0';
            accCount = accCount + 1;
        }
    }
    fclose(accountsFile);
}

// below is for finding account index by name
int FindAccount(const char *name)
{
    for (int i = 0; i < accCount; i++) {
        if (strcmp(accNames[i], name) == 0) {
            return i;
        }
    }
    return -1;
}

// below is for reading one piece of text from line, stopping at semicolon
int ReadField(const char *line, int pos, char *outText, int maxLen)
{
    int length = 0;
    while (line[pos] != ';' && line[pos] != '\0' && length < maxLen - 1) {
        outText[length] = line[pos];
        length = length + 1;
        pos = pos + 1;
    }
    outText[length] = '\0';
    if (line[pos] == ';') {
        pos = pos + 1;
    }
    return pos;
}

// below is for reading one number from line, stopping at semicolon
int ReadNumber(const char *line, int *pos)
{
    char box[24];
    *pos = ReadField(line, *pos, box, 24);
    return atoi(box);
}

// below is for saving player data and quests to file
void SavePlayerData(void)
{
    if (myName[0] == '\0') {
        return;
    }
    char path[120];
    sprintf(path, "%s.txt", myName);
    FILE *saveFile = fopen(path, "w");
    if (saveFile == NULL) {
        return;
    }

    fprintf(saveFile, "%d %d %d %d %d %ld\n", level, xp, gold, cls, streak, lastDay);
    for (int i = 0; i < 5; i++) {
        fprintf(saveFile, "%d ", stats[i]);
    }
    fprintf(saveFile, "\n%d\n", qCount);

    for (int i = 0; i < qCount; i++) {
        fprintf(saveFile, "%s;%d;%d;%d;%d;%d;%d\n",
                quests[i].title, quests[i].day, quests[i].stat,
                quests[i].act, quests[i].amt, quests[i].diff, quests[i].done);
    }
    fclose(saveFile);
}

// below is for loading player data and quests from file
int LoadPlayerData(void)
{
    char path[120], line[140];
    sprintf(path, "%s.txt", myName);
    FILE *loadFile = fopen(path, "r");
    if (loadFile == NULL) {
        return 0;
    }

    if (fscanf(loadFile, "%d %d %d %d %d %ld", &level, &xp, &gold, &cls, &streak, &lastDay) < 6) {
        fclose(loadFile);
        return 0;
    }

    for (int i = 0; i < 5; i++) {
        fscanf(loadFile, "%d", &stats[i]);
    }
    fscanf(loadFile, "%d", &qCount);
    fgets(line, sizeof(line), loadFile);

    for (int i = 0; i < qCount && i < MAX_QUESTS; i++) {
        if (fgets(line, sizeof(line), loadFile) == NULL) {
            break;
        }
        TrimString(line);

        int pos = 0;
        pos = ReadField(line, pos, quests[i].title, STR_LEN);
        quests[i].day  = ReadNumber(line, &pos);
        quests[i].stat = ReadNumber(line, &pos);
        quests[i].act  = ReadNumber(line, &pos);
        quests[i].amt  = ReadNumber(line, &pos);
        quests[i].diff = ReadNumber(line, &pos);
        quests[i].done = ReadNumber(line, &pos);
    }
    fclose(loadFile);
    return 1;
}

// below is for initializing basic game state and directories
void InitGameLogic(void)
{
    MKDIR("Logs");
    LoadAccounts();
    srand((unsigned)time(NULL));
}

// below is for verifying login credentials and calculating streak
int TryLogin(const char *name, const char *pin)
{
    int accountIndex = FindAccount(name);
    if (accountIndex >= 0 && strcmp(accPins[accountIndex], pin) == 0) {
        snprintf(myName, STR_LEN, "%s", name);
        LoadPlayerData();

        long timeNow = (long)(time(NULL) / 86400);
        if (lastDay >= 0 && timeNow - lastDay == 1) {
            streak = streak + 1;
        } else if (timeNow != lastDay) {
            streak = 1;
        }
        lastDay = timeNow;

        char note[120];
        sprintf(note, "Logged in on %s", days[TodayIndex()]);
        LogNote(note);

        SavePlayerData();
        return 1;
    }
    return 0;
}

// below is for registering a new player account
int TrySignup(char *name, const char *pin, int classChoice)
{
    if (strlen(name) == 0 || strlen(pin) == 0) {
        return 0;
    }
    if (FindAccount(name) >= 0) {
        return -1;
    }
    if (accCount >= MAX_ACC) {
        return -2;
    }

    for (int i = 0; name[i] != '\0'; i++) {
        if (name[i] == ' ') {
            name[i] = '_';
        }
    }

    snprintf(accNames[accCount], STR_LEN, "%s", name);
    snprintf(accPins[accCount], 10, "%s", pin);
    accCount = accCount + 1;
    SaveAccounts();

    cls = classChoice;
    level = 1;
    xp = 0;
    gold = 50;
    streak = 1;
    lastDay = (long)(time(NULL) / 86400);

    for (int i = 0; i < 5; i++) {
        stats[i] = 10;
    }
    qCount = 0;
    snprintf(myName, STR_LEN, "%s", name);

    char note[120];
    sprintf(note, "Account created on %s", days[TodayIndex()]);
    LogNote(note);
    sprintf(note, "Logged in on %s", days[TodayIndex()]);
    LogNote(note);

    SavePlayerData();
    return 1;
}

// below is for adding a new quest to the player quest list
int AddQuest(const char *title, int day, int stat, int act, int amount, int diff)
{
    if (strlen(title) == 0) {
        return 0;
    }
    if (amount <= 0) {
        return -1;
    }
    if (qCount >= MAX_QUESTS) {
        return -2;
    }

    snprintf(quests[qCount].title, STR_LEN, "%s", title);
    quests[qCount].day = day;
    if (stat < 0 || stat > 4) {
        stat = 0;
    }
    quests[qCount].stat = stat;

    if (act < 0 || act > 4) {
        act = 0;
    }
    quests[qCount].act = act;

    quests[qCount].amt = amount;

    if (diff < 1 || diff > 3) {
        diff = 2;
    }
    quests[qCount].diff = diff;
    quests[qCount].done = 0;
    qCount = qCount + 1;

    SavePlayerData();
    char note[120];
    sprintf(note, "Quest added on %s: %s", days[day], title);
    LogNote(note);
    return 1;
}

// below is for completing a quest, rewarding player, and checking level up
int CompleteQuest(int index, int *outGained)
{
    if (index < 0 || index >= qCount) {
        return 0;
    }

    quests[index].done = 1;
    int diff = quests[index].diff;
    int gain = diff * 25;
    if (outGained != NULL) {
        *outGained = gain;
    }

    int oldLevel = level;
    gold = gold + gain;
    xp = xp + gain;

    int statIndex = quests[index].stat;
    if (statIndex >= 0 && statIndex < 5) {
        stats[statIndex] = stats[statIndex] + diff;
        if (stats[statIndex] > 100) {
            stats[statIndex] = 100;
        }
    }

    while (xp >= XpNeeded()) {
        xp = xp - XpNeeded();
        level = level + 1;
        char levelNote[120];
        sprintf(levelNote, "Leveled up to Level %d on %s", level, days[TodayIndex()]);
        LogNote(levelNote);
    }

    char note[160];
    sprintf(note, "Quest completed on %s (+%d gold): %s", days[TodayIndex()], gain, quests[index].title);
    LogNote(note);

    SavePlayerData();

    if (level > oldLevel) {
        return level;
    }
    return 0;
}

// below is for deleting a quest from the quest list
void DeleteQuest(int index)
{
    if (index < 0 || index >= qCount) {
        return;
    }

    char note[160];
    sprintf(note, "Quest deleted on %s: %s", days[TodayIndex()], quests[index].title);
    LogNote(note);

    for (int shiftIndex = index; shiftIndex < qCount - 1; shiftIndex++) {
        quests[shiftIndex] = quests[shiftIndex + 1];
    }
    qCount = qCount - 1;
    SavePlayerData();
}

// below is for purchasing an item from real-life shop
int BuyShopReward(int cost, const char *rewardName)
{
    if (gold >= cost) {
        gold = gold - cost;
        char note[140];
        sprintf(note, "Bought %s (%d gold) on %s", rewardName, cost, days[TodayIndex()]);
        LogNote(note);
        SavePlayerData();
        return 1;
    }
    return 0;
}

// below is for developer mode completing all quests scheduled for today
void DevCompleteAllToday(void)
{
    for (int i = 0; i < qCount; i++) {
        if (quests[i].day == TodayIndex() && quests[i].done == 0) {
            quests[i].done = 1;
            int diff = quests[i].diff;
            int gain = diff * 25;
            gold = gold + gain;
            xp = xp + gain;
            int statIndex = quests[i].stat;
            if (statIndex >= 0 && statIndex < 5) {
                stats[statIndex] = stats[statIndex] + diff;
                if (stats[statIndex] > 100) {
                    stats[statIndex] = 100;
                }
            }
            while (xp >= XpNeeded()) {
                xp = xp - XpNeeded();
                level = level + 1;
            }
        }
    }
    char note[120];
    sprintf(note, "Developer completed all today's quests on %s", days[TodayIndex()]);
    LogNote(note);
    SavePlayerData();
}

// below is for developer mode wiping all quests
void DevWipeAllQuests(void)
{
    qCount = 0;
    char note[120];
    sprintf(note, "Developer wiped all quests on %s", days[TodayIndex()]);
    LogNote(note);
    SavePlayerData();
}

// below is for developer mode resetting streak to 1
void DevResetStreak(void)
{
    streak = 1;
}
