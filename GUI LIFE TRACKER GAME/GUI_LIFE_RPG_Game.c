                  // Mamoon Modules Declaration and Defination of variables and constants
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
// constants used across the program
#define MAX_ACCOUNTS 500
#define MAX_QUESTS 50
#define MAX_LOGS 200
#define MAX_LEN 128

// global arrays used to store game data in a flat structure
// account storage
char accountIds[MAX_ACCOUNTS][16];
char accountNames[MAX_ACCOUNTS][64];
int accountCount = 0;

// current logged in player details
char curId[16];
char curName[64];
int curLevel = 1;
int curXp = 0;
int curGold = 0;
int curClass = 0; // class values: 0 scholar, 1 athlete, 2 artist
int curStats[5] = {10, 10, 10, 10, 10}; // s tat values: 0 intellect, 1 stamina, 2 discipline, 3 social, 4 creativity
int curDiscipline = 0;
int dailyStreak = 1;
long lastStreakDay = -1;
long lastLoginTime = 0;
char dailyFortune[128] = "Normal Day";

// quest storage
int questCount = 0;
char questName[MAX_QUESTS][64];
int questDay[MAX_QUESTS];
int questStat[MAX_QUESTS];
int questReward[MAX_QUESTS];
int questDone[MAX_QUESTS];

// activity log storage
int logCount = 0;
char logText[MAX_LOGS][MAX_LEN];

// text labels for days, stats, and classes
const char *days[7] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
const char *statNames[5] = {"Intellectual", "Stamina", "Discipline", "Social Skills", "Creativity"};
const char *classNames[3] = {"Scholar (+XP to Intellect)", "Athlete (+XP to Stamina)", "Artist (+XP to Creativity)"};


                       // mamoon module - helper functions
void trimString(char *str)
{
    int len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r' || str[len - 1] == ' '))
    {
        str[len - 1] = '\0';
        len--;
    }
}



void pressEnter(void)
{
    char buf[10];
    printf("\nPress Enter to continue...");
    fgets(buf, sizeof(buf), stdin);
}



void addLog(const char *msg)
{
    if (logCount < MAX_LOGS)
    {
        snprintf(logText[logCount], MAX_LEN, "%s", msg);
        logCount++;
    }
    else
    {
        for (int i = 0; i < MAX_LOGS - 1; i++)
        {
            strcpy(logText[i], logText[i + 1]);
        }
        snprintf(logText[MAX_LOGS - 1], MAX_LEN, "%s", msg);
    }
}



void refreshDailyStreak(void)
{
    long currentDay = time(NULL) / 86400L;

    if (lastStreakDay < 0)
    {
        dailyStreak = 1;
    }
    else if (currentDay == lastStreakDay)
    {
        return;
    }
    else if (currentDay == lastStreakDay + 1)
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


                      // khadija module - file saving & file loading.
void loadAccounts(void)
{
    FILE *f = fopen("accounts.txt", "r");
    if (!f) return;
    char line[256];
    while (fgets(line, sizeof(line), f) && accountCount < MAX_ACCOUNTS)
    {
        trimString(line);
        char *id = strtok(line, ", ");
        char *name = strtok(NULL, "\n");
        if (id && name)
        {
            strcpy(accountIds[accountCount], id);
            strcpy(accountNames[accountCount], name);
            accountCount++;
        }
    }
    fclose(f);
}



void saveAccounts(void)
{
    FILE *f = fopen("accounts.txt", "w");
    if (!f) return;
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
    if (!f) return;

    fprintf(f, "ID: %s\n", curId);
    fprintf(f, "NAME: %s\n", curName);
    fprintf(f, "LEVEL: %d\n", curLevel);
    fprintf(f, "XP: %d\n", curXp);
    fprintf(f, "GOLD: %d\n", curGold);
    fprintf(f, "CLASS: %d\n", curClass); // save the chosen character class.
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
        fprintf(f, "QUEST|%d|%d|%d|%d|%s\n", questDay[i], questStat[i], questReward[i], questDone[i], questName[i]);
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
    FILE *f = fopen(filename, "r");
    if (!f) return;

    strcpy(curId, id);
    curLevel = 1;
    curXp = 0;
    curGold = 0;
    curClass = 0;
    curDiscipline = 0;
    dailyStreak = 1;
    lastStreakDay = -1;
    lastLoginTime = time(NULL);
    questCount = 0;
    logCount = 0;
    for (int i = 0; i < 5; i++)
    {
        curStats[i] = 10;
    }

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
            if (strncmp(line, "NAME: ", 6) == 0) strcpy(curName, line + 6);
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
        else if (section == 1)
        {
            int d, s, r, dn;
            char qn[64];
            if (sscanf(line, "QUEST|%d|%d|%d|%d|%s", &d, &s, &r, &dn, qn) == 5 && questCount < MAX_QUESTS)
            {
                questDay[questCount] = d;
                questStat[questCount] = s;
                questReward[questCount] = r;
                questDone[questCount] = dn;
                strcpy(questName[questCount], qn);
                questCount++;
            }
        }
        else if (section == 2)
        {
            if (strlen(line) > 0 && logCount < MAX_LOGS)
            {
                strcpy(logText[logCount], line);
                logCount++;
            }
        }
    }
    fclose(f);

    refreshDailyStreak();

    // pick a random daily fortune or event when the player logs in.
    int fortuneRoll = rand() % 3;
    if (fortuneRoll == 0)
    {
        strcpy(dailyFortune, "Double Gold Day! (All gold earnings are doubled)");
    }
    else if (fortuneRoll == 1)
    {
        strcpy(dailyFortune, "Inspiration Strike! (+5 XP bonus on all quests today)");
    }
    else
    {
        strcpy(dailyFortune, "Balanced Energy (Standard progress day)");
    }

    // check if a week has passed to reset quests.
    long currentTime = time(NULL);
    if (currentTime - lastLoginTime > 604800)
    {
        printf("\n[NOTICE] It's been over a week! Resetting quests for a new week cycle.\n");
        for (int i = 0; i < questCount; i++)
        {
            questDone[i] = 0;
        }
        addLog("Automatic weekly quest reset triggered.");
    }
    lastLoginTime = currentTime;
}


                                 // sajid module - add xp.
void addXp(int amount, int statIndex)
{
    // class passive check, matching class to stat gives a small bonus xp.
    if ((curClass == 0 && statIndex == 0) || 
        (curClass == 1 && statIndex == 1) || 
        (curClass == 2 && statIndex == 4))
    {
        amount += (amount / 5); // add twenty percent bonus xp.
        printf("? Class Bonus Triggered! Extra XP granted!\n");
    }

    // daily event bonus check.
    if (strstr(dailyFortune, "Inspiration Strike") != NULL)
    {
        amount += 5;
    }

    curXp += amount;
    
    int goldEarned = amount / 2;
    if (strstr(dailyFortune, "Double Gold Day") != NULL)
    {
        goldEarned *= 2;
    }
    curGold += goldEarned;

    int req = curLevel * 100;
    while (curXp >= req)
    {
        curXp -= req;
        curLevel++;
        printf("\n*** LEVEL UP! You are now level %d! ***\n", curLevel);
        char msg[64];
        snprintf(msg, sizeof(msg), "Reached level %d", curLevel);
        addLog(msg);
        req = curLevel * 100;
    }
}


                     // khadija module - player profile & title.
void printTitle(void)
{
    int highestStatIdx = 0;
    int maxVal = curStats[0];
    for (int i = 1; i < 5; i++)
    {
        if (curStats[i] > maxVal)
        {
            maxVal = curStats[i];
            highestStatIdx = i;
        }
    }

    printf("Title: ");
    if (curLevel < 3)
    {
        printf("Novice Adventurer\n");
    }
    else if (highestStatIdx == 0)
    {
        printf("Grand Scholar ??\n");
    }
    else if (highestStatIdx == 1)
    {
        printf("Iron Athlete ?\n");
    }
    else if (highestStatIdx == 2)
    {
        printf("Master of Discipline ???\n");
    }
    else if (highestStatIdx == 3)
    {
        printf("Charismatic Diplomat ??\n");
    }
    else
    {
        printf("Visionary Creator ??\n");
    }
}



void viewProfile(void)
{
    printf("\n---===! PROFILE: %s !===---\n", curName);
    printf("ID: %s | Class: %s\n", curId, classNames[curClass]);
    printTitle(); // show the milestone title.
    printf("Level: %d (XP: %d/%d) | Gold: %d ??\n", curLevel, curXp, curLevel * 100, curGold);
    printf("Daily streak: %d day(s)\n", dailyStreak);
    
    printf("\n[Daily Fortune]: %s\n", dailyFortune);

    printf("\nSTATS:\n");
    for (int i = 0; i < 5; i++)
    {
        printf("- %s: %d/100\n", statNames[i], curStats[i]);
    }
    printf("Discipline Score: %d\n", curDiscipline);
    pressEnter();
}


                         // khadija module - quest management & completion.
void printQuestBoard(void)
{
    printf("\n---===! WEEKLY QUEST BOARD !===---\n");
    printf("Filter: 0=All, 1=Intellect, 2=Stamina, 3=Discipline, 4=Social, 5=Creativity, 6=Incomplete Only\n");
    printf("Enter filter choice: ");
    int filter; scanf("%d", &filter); getchar();

    for (int d = 0; d < 7; d++)
    {
        int found = 0;
        for (int i = 0; i < questCount; i++)
        {
            if (questDay[i] == d)
            {
                if (filter >= 1 && filter <= 5 && questStat[i] != (filter - 1)) continue;
                if (filter == 6 && questDone[i] == 1) continue;

                if (!found)
                {
                    printf("\n%s:\n", days[d]);
                    found = 1;
                }
                char mark = ' ';
                if (questDone[i])
                {
                    mark = 'X';
                }
                printf("  [%c] %d. %s (%s, +%d XP)\n",
                       mark, i + 1, questName[i],
                       statNames[questStat[i]], questReward[i]);
            }
        }
    }
}



void createQuest(void)
{
    if (questCount >= MAX_QUESTS)
    {
        printf("Quest board is full!\n");
        pressEnter();
        return;
    }
    char name[64];
    printf("Enter quest name: ");
    fgets(name, sizeof(name), stdin);
    trimString(name);
    if (strlen(name) == 0) return;

    printf("Choose day (0=Mon, 1=Tue, 2=Wed, 3=Thu, 4=Fri, 5=Sat, 6=Sun): ");
    int d; scanf("%d", &d); getchar();
    if (d < 0 || d > 6) return;

    printf("Choose stat (0=Intellect, 1=Stamina, 2=Discipline, 3=Social, 4=Creativity): ");
    int s; scanf("%d", &s); getchar();
    if (s < 0 || s > 4) return;

    printf("Choose difficulty (0=Easy (+10XP), 1=Med (+20XP), 2=Hard (+40XP)): ");
    int diff; scanf("%d", &diff); getchar();
    int rewards[3] = {10, 20, 40};

    strcpy(questName[questCount], name);
    questDay[questCount] = d;
    questStat[questCount] = s;
    if (diff >= 0 && diff <= 2)
    {
        questReward[questCount] = rewards[diff];
    }
    else
    {
        questReward[questCount] = 10;
    }
    questDone[questCount] = 0;
    questCount++;

    addLog("Created a new quest");
    printf("Quest added successfully!\n");
    pressEnter();
}



void completeQuest(void)
{
    if (questCount == 0)
    {
        printf("No quests available.\n");
        pressEnter();
        return;
    }
    printQuestBoard();
    printf("\nEnter quest number to complete: ");
    int idx; scanf("%d", &idx); getchar();
    idx--;

    if (idx >= 0 && idx < questCount)
    {
        if (questDone[idx])
        {
            printf("Already completed!\n");
        }
        else
        {
            questDone[idx] = 1;
            curDiscipline++;
            curStats[questStat[idx]] += 1;
            if (curStats[questStat[idx]] > 100) curStats[questStat[idx]] = 100;

            if (dailyStreak > 1)
            {
                int streakBonus = dailyStreak * 5;
                curGold += streakBonus;
                printf("Daily streak bonus: +%d Gold!\n", streakBonus);
            }
            
            addXp(questReward[idx], questStat[idx]); // pass the stat index so class bonus can be checked.
            addLog("Completed a quest");
            printf("Quest completed successfully!\n");
        }
    }
    else
    {
        printf("Invalid choice.\n");
    }
    pressEnter();
}



void questMenu(void)
{
    int choice;
    do
    {
        printf("\n---===! QUESTS & HABITS !===---\n");
        printf("1. View Quest Board\n");
        printf("2. Create New Quest\n");
        printf("3. Complete a Quest\n");
        printf("4. Go Back\n");
        printf("Choose option: ");
        scanf("%d", &choice); getchar();

        if (choice == 1) printQuestBoard(), pressEnter();
        else if (choice == 2) createQuest();
        else if (choice == 3) completeQuest();
    } while (choice != 4);
}



                     // khadija module - reward shop.
void rewardShop(void)
{
    char items[3][32] = {"1 Hour Free Time", "Favorite Snack", "Day Off / Sleep In"};
    int prices[3] = {50, 100, 250};

    int choice;
    do
    {
        printf("\n---===! REWARD SHOP !===---\n");
        printf("Your Gold: %d ??\n", curGold);
        printf("1. %s - %d Gold\n", items[0], prices[0]);
        printf("2. %s - %d Gold\n", items[1], prices[1]);
        printf("3. %s - %d Gold\n", items[2], prices[2]);
        printf("4. Go Back\n");
        printf("Choose reward to buy: ");
        scanf("%d", &choice); getchar();

        if (choice >= 1 && choice <= 3)
        {
            int idx = choice - 1;
            if (curGold >= prices[idx])
            {
                curGold -= prices[idx];
                printf("\n?? Successfully purchased: %s!\n", items[idx]);
                addLog("Purchased a reward from the shop");
            }
            else
            {
                printf("\nNot enough Gold!\n");
            }
            pressEnter();
        }
    } while (choice != 4);
}



                         // sajid module - developer mode.
void developerMode(void)
{
    char pin[16];
    printf("Enter Developer PIN (KSM-321): ");
    fgets(pin, sizeof(pin), stdin);
    trimString(pin);

    if (strcmp(pin, "KSM-321") != 0)
    {
        printf("Wrong PIN!\n");
        pressEnter();
        return;
    }

    int choice;
    do
    {
        printf("\n---===! DEVELOPER TOOLS !===---\n");
        printf("1. Add 500 XP & 250 Gold\n");
        printf("2. Max Out Stats\n");
        printf("3. Go Back\n");
        printf("Choose option: ");
        scanf("%d", &choice); getchar();

        if (choice == 1)
        { 
            addXp(500, 0); 
            curGold += 250;
            printf("Added 500 XP and 250 Gold!\n"); 
            pressEnter(); 
        }
        else if (choice == 2)
        {
            for (int i = 0; i < 5; i++)
        {
            curStats[i] = 100;
        }
            printf("Stats maxed out!\n");
            pressEnter();
        }
    } while (choice != 3);
}



                  // sajid module - player hub.
void playerHub(void)
{
    int choice;
    do
    {
        printf("\n---===! %s'S LIFE HUB !===---\n", curName);
        printf("1. View Profile & Title\n");
        printf("2. Quests & Habits\n");
        printf("3. Reward Shop ??\n");
        printf("4. Developer Mode\n");
        printf("5. Save & Logout\n");
        printf("Choose option: ");
        scanf("%d", &choice); getchar();


    
        if (choice == 1) viewProfile();
        else if (choice == 2) questMenu();
        else if (choice == 3) rewardShop();
        else if (choice == 4) developerMode();
        else if (choice == 5)
        {
            savePlayer();
            printf("Progress saved. Logged out.\n");
        }
    } while (choice != 5);
}



                    // sajid module - login player.
void loginPlayer(void)
{
    if (accountCount == 0)
    {
        printf("No accounts found. Please create one first.\n");
        pressEnter();
        return;
    }
    printf("\nRegistered Players:\n");
    for (int i = 0; i < accountCount; i++)
    {
        printf("%d. %s\n", i + 1, accountNames[i]);
    }
    printf("Select account number: ");
    int idx; scanf("%d", &idx); getchar();
    idx--;

    if (idx >= 0 && idx < accountCount)
    {
        char pin[16];
        printf("Enter KSM ID (Password) for %s: ", accountNames[idx]);
        fgets(pin, sizeof(pin), stdin);
        trimString(pin);

        if (strcmp(pin, accountIds[idx]) == 0)
        {
            loadPlayer(accountIds[idx]);
            playerHub();
        }
        else
        {
            printf("Incorrect KSM ID!\n");
            pressEnter();
        }
    }
}



                    // sajid module - create account.
void createAccount(void)
{
    if (accountCount >= MAX_ACCOUNTS)
    {
        printf("Account limit reached.\n");
        pressEnter();
        return;
    }
    char name[64];
    printf("Enter your name: ");
    fgets(name, sizeof(name), stdin);
    trimString(name);
    if (strlen(name) == 0) return;

    // let the player pick a class.
    printf("Choose your Character Class:\n");
    printf("0. Scholar (Bonus XP for Intellectual quests)\n");
    printf("1. Athlete (Bonus XP for Stamina quests)\n");
    printf("2. Artist (Bonus XP for Creativity quests)\n");
    printf("Select class (0-2): ");
    int chosenClass; scanf("%d", &chosenClass); getchar();
    if (chosenClass < 0 || chosenClass > 2) chosenClass = 0;

    int r = 1000 + (rand() % 9000);
    char newId[16];
    snprintf(newId, sizeof(newId), "KSM-%04d", r);

    strcpy(accountIds[accountCount], newId);
    strcpy(accountNames[accountCount], name);
    accountCount++;
    saveAccounts();

    loadPlayer(newId);
    strcpy(curId, newId);
    strcpy(curName, name);
    curClass = chosenClass; // save the selected class.
    dailyStreak = 1;
    lastStreakDay = time(NULL) / 86400L;
    savePlayer();

    printf("\nAccount created successfully!\n");
    printf("Your secret KSM ID (Password): %s\n", newId);
    printf("Save this ID to log back in!\n");
    pressEnter();
}


                   // mamoon module - main menu.
int main(void)
{
    srand(time(NULL));
    loadAccounts();

    int choice;
    do
    {
        printf("\n---===C<<< LIFE RPG - MAIN MENU >>>C===---\n");
        printf("1. Login to Account\n");
        printf("2. Create New Account\n");
        printf("3. Exit\n");
        printf("Choose option: ");
        scanf("%d", &choice); getchar();

        if (choice == 1) loginPlayer();
        else if (choice == 2) createAccount();
    } while (choice != 3);

    return 0;
}
