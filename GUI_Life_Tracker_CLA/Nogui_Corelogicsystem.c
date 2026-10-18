/*
 * LIFE RPG - Level Up Your Real Life
 * ----------------------------------
 * A console RPG where real-life habits become weekly quests.
 * Completing quests earns XP, raises stats, and builds discipline.
 * Uses only standard C libraries.
 *
 * Compile: gcc lifeRpg.c -o lifeRpg
 * Run:     ./lifeRpg
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

/* ============================================================
 * GAME CONSTANTS
 * ============================================================ */
#define ACCOUNTS_FILE_NAME       "accounts.txt"
#define MAX_ACCOUNTS             500
#define MAX_QUESTS               50
#define MAX_ACTIVITY_LOGS        200
#define MAX_TEXT_LENGTH          128
#define NAME_LENGTH              64
#define ID_LENGTH                16
#define PLAYER_FILE_NAME_LENGTH  32
#define DEVELOPER_PIN            "KSM-321"
#define MAX_PASSWORD_ATTEMPTS    3
#define DAYS_IN_WEEK             7
#define STAT_COUNT               5
#define STAT_START_VALUE         10
#define STAT_MAX_VALUE           100
#define STAT_GAIN_PER_QUEST      1
#define STAT_BAR_WIDTH           20
#define XP_BAR_WIDTH             20
#define BASE_XP_PER_LEVEL        100
#define STARTING_LEVEL           1
#define DEVELOPER_XP_BOOST       500
#define DIFFICULTY_COUNT         3

/* Indexes for the five core stats */
enum StatIndex {
    STAT_INTELLECTUAL,
    STAT_STAMINA,
    STAT_DISCIPLINE,
    STAT_SOCIAL_SKILLS,
    STAT_CREATIVITY
};

/* Sections used while reading a player save file */
enum SaveFileSection {
    SECTION_HEADER,
    SECTION_QUESTS,
    SECTION_ACTIVITY
};

/* Day, stat and difficulty labels shown to the player */
static const char *dayNames[DAYS_IN_WEEK] = {
    "Monday", "Tuesday", "Wednesday", "Thursday",
    "Friday", "Saturday", "Sunday"
};

static const char *statDisplayNames[STAT_COUNT] = {
    "Intellectual", "Stamina", "Discipline", "Social Skills", "Creativity"
};

/* Labels written to and read from KSM-xxxx.txt files */
static const char *statSaveLabels[STAT_COUNT] = {
    "INTELLECTUAL: ", "STAMINA: ", "DISCIPLINE: ", "SOCIAL: ", "CREATIVITY: "
};

static const char *difficultyLabels[DIFFICULTY_COUNT] = {
    "Easy (+10 XP)", "Medium (+20 XP)", "Hard (+40 XP)"
};

static const int difficultyXpRewards[DIFFICULTY_COUNT] = {10, 20, 40};

/* ============================================================
 * DATA STRUCTURES
 * ============================================================ */

/* One registered account (from accounts.txt) */
typedef struct {
    char accountId[ID_LENGTH];
    char accountName[NAME_LENGTH];
} Account;

/* One habit or quest placed on a day of the week */
typedef struct {
    char questName[NAME_LENGTH];
    int dayIndex;
    int targetStatIndex;
    int experienceReward;
    int isCompleted;
} Quest;

/* Full game progress for one logged-in player */
typedef struct {
    char accountId[ID_LENGTH];
    char playerName[NAME_LENGTH];
    int playerLevel;
    int experiencePoints;
    int statValues[STAT_COUNT];
    int disciplineScore;
    int questCount;
    Quest quests[MAX_QUESTS];
    int activityCount;
    char activityLogs[MAX_ACTIVITY_LOGS][MAX_TEXT_LENGTH];
} Player;

/* ============================================================
 * GLOBAL STATE
 * ============================================================ */
static Account accountList[MAX_ACCOUNTS];
static int accountCount = 0;
static Player currentPlayer;
static int isPlayerLoggedIn = 0;

/* ============================================================
 * INPUT HELPERS
 * Handle reading and cleaning up text from the keyboard.
 * ============================================================ */

/* Removes the trailing newline from a string read by fgets */
static void stripNewline(char *text)
{
    size_t textLength = strlen(text);

    if (textLength > 0 && text[textLength - 1] == '\n') {
        text[textLength - 1] = '\0';
    }
}

/* Removes spaces and tabs from the start and end of a string */
static void trimWhitespace(char *text)
{
    size_t textLength = strlen(text);
    size_t startIndex = 0;
    size_t endIndex = textLength;

    while (startIndex < textLength && isspace((unsigned char)text[startIndex])) {
        startIndex++;
    }
    while (endIndex > startIndex && isspace((unsigned char)text[endIndex - 1])) {
        endIndex--;
    }

    memmove(text, text + startIndex, endIndex - startIndex);
    text[endIndex - startIndex] = '\0';
}

/* Reads one full line of user input safely into the buffer */
static void readLine(char *buffer, int bufferSize)
{
    int currentCharacter;

    if (fgets(buffer, bufferSize, stdin) == NULL) {
        printf("\nInput closed. Goodbye!\n");
        exit(0);
    }

    /* If the line was too long, throw away the rest of it */
    if (strchr(buffer, '\n') == NULL) {
        while ((currentCharacter = getchar()) != '\n' && currentCharacter != EOF) {
            /* discard extra characters */
        }
    }

    stripNewline(buffer);
}

/* Asks for a number and repeats until it is inside the allowed range */
static int readIntegerInRange(const char *promptText, int minimumValue, int maximumValue)
{
    char inputBuffer[MAX_TEXT_LENGTH];
    char *endPointer = NULL;
    long parsedValue = 0;

    while (1) {
        printf("%s", promptText);
        readLine(inputBuffer, sizeof(inputBuffer));

        parsedValue = strtol(inputBuffer, &endPointer, 10);

        if (endPointer != inputBuffer
            && *endPointer == '\0'
            && parsedValue >= minimumValue
            && parsedValue <= maximumValue) {
            return (int)parsedValue;
        }

        printf("Invalid choice. Please enter a number from %d to %d.\n",
               minimumValue, maximumValue);
    }
}

/* Pauses so the player can read a message before the screen changes */
static void waitForEnter(void)
{
    char dummyBuffer[MAX_TEXT_LENGTH];

    printf("\nPress Enter to continue...");
    readLine(dummyBuffer, sizeof(dummyBuffer));
}

/* ============================================================
 * FILE STORAGE
 * Reads and writes accounts.txt and KSM-xxxx.txt files.
 * ============================================================ */

/* Reads a line like "LEVEL: 5" and stores the number if the label matches */
static int matchLabelInteger(const char *lineText, const char *labelText, int *valueOut)
{
    size_t labelLength = strlen(labelText);

    if (strncmp(lineText, labelText, labelLength) != 0) {
        return 0;
    }

    *valueOut = atoi(lineText + labelLength);
    return 1;
}

/* Rewrites accounts.txt using the format: KSM-xxxx, Name */
static void saveAccountsFile(void)
{
    FILE *accountsFile = fopen(ACCOUNTS_FILE_NAME, "w");
    int accountIndex;

    if (accountsFile == NULL) {
        printf("Error: could not write %s.\n", ACCOUNTS_FILE_NAME);
        return;
    }

    for (accountIndex = 0; accountIndex < accountCount; accountIndex++) {
        fprintf(accountsFile, "%s, %s\n",
                accountList[accountIndex].accountId,
                accountList[accountIndex].accountName);
    }

    fclose(accountsFile);
}

/* Loads every registered account from accounts.txt at startup */
static void loadAccountsFile(void)
{
    FILE *accountsFile = fopen(ACCOUNTS_FILE_NAME, "r");
    char lineBuffer[MAX_TEXT_LENGTH * 2];
    Account *currentAccount;

    if (accountsFile == NULL) {
        return; /* First run: no accounts yet */
    }

    while (accountCount < MAX_ACCOUNTS
           && fgets(lineBuffer, sizeof(lineBuffer), accountsFile) != NULL) {
        stripNewline(lineBuffer);
        currentAccount = &accountList[accountCount];

        if (sscanf(lineBuffer, "%15[^,], %63[^\n]",
                   currentAccount->accountId,
                   currentAccount->accountName) == 2) {
            trimWhitespace(currentAccount->accountId);
            trimWhitespace(currentAccount->accountName);
            accountCount++;
        }
    }

    fclose(accountsFile);
}

/* Saves one player's stats, quests and activity log to KSM-xxxx.txt */
static int savePlayerFile(const Player *playerToSave)
{
    char fileName[PLAYER_FILE_NAME_LENGTH];
    FILE *playerFile;
    int index;

    snprintf(fileName, sizeof(fileName), "%s.txt", playerToSave->accountId);
    playerFile = fopen(fileName, "w");

    if (playerFile == NULL) {
        return 0;
    }

    /* Player header and stats */
    fprintf(playerFile, "ID: %s\n", playerToSave->accountId);
    fprintf(playerFile, "NAME: %s\n", playerToSave->playerName);
    fprintf(playerFile, "LEVEL: %d\n", playerToSave->playerLevel);
    fprintf(playerFile, "XP: %d\n", playerToSave->experiencePoints);

    for (index = 0; index < STAT_COUNT; index++) {
        fprintf(playerFile, "%s%d\n", statSaveLabels[index], playerToSave->statValues[index]);
    }

    fprintf(playerFile, "DISCIPLINE_SCORE: %d\n", playerToSave->disciplineScore);

    /* Quest board */
    fprintf(playerFile, "[QUESTS]\n");
    for (index = 0; index < playerToSave->questCount; index++) {
        fprintf(playerFile, "QUEST|%d|%d|%d|%d|%s\n",
                playerToSave->quests[index].dayIndex,
                playerToSave->quests[index].targetStatIndex,
                playerToSave->quests[index].experienceReward,
                playerToSave->quests[index].isCompleted,
                playerToSave->quests[index].questName);
    }

    /* Activity history */
    fprintf(playerFile, "[ACTIVITY]\n");
    for (index = 0; index < playerToSave->activityCount; index++) {
        fprintf(playerFile, "%s\n", playerToSave->activityLogs[index]);
    }

    fclose(playerFile);
    return 1;
}

/* Loads one player's KSM-xxxx.txt file into the given Player struct */
static int loadPlayerFile(const char *accountId, Player *playerToFill)
{
    char fileName[PLAYER_FILE_NAME_LENGTH];
    char lineBuffer[MAX_TEXT_LENGTH * 2];
    char questNameBuffer[NAME_LENGTH];
    FILE *playerFile;
    Quest *questSlot;
    int sectionType = SECTION_HEADER;
    int statIndex;
    int dayValue;
    int statValue;
    int rewardValue;
    int completedValue;

    memset(playerToFill, 0, sizeof(Player));
    snprintf(playerToFill->accountId, ID_LENGTH, "%s", accountId);
    snprintf(fileName, sizeof(fileName), "%s.txt", accountId);

    playerFile = fopen(fileName, "r");
    if (playerFile == NULL) {
        return 0;
    }

    while (fgets(lineBuffer, sizeof(lineBuffer), playerFile) != NULL) {
        stripNewline(lineBuffer);

        /* Section markers switch how the next lines are read */
        if (strcmp(lineBuffer, "[QUESTS]") == 0) {
            sectionType = SECTION_QUESTS;
            continue;
        }
        if (strcmp(lineBuffer, "[ACTIVITY]") == 0) {
            sectionType = SECTION_ACTIVITY;
            continue;
        }

        if (sectionType == SECTION_HEADER) {
            /* Read the player name and numeric stats */
            if (strncmp(lineBuffer, "NAME: ", 6) == 0) {
                snprintf(playerToFill->playerName, NAME_LENGTH, "%s", lineBuffer + 6);
            }
            for (statIndex = 0; statIndex < STAT_COUNT; statIndex++) {
                matchLabelInteger(lineBuffer, statSaveLabels[statIndex],
                                  &playerToFill->statValues[statIndex]);
            }
            matchLabelInteger(lineBuffer, "LEVEL: ", &playerToFill->playerLevel);
            matchLabelInteger(lineBuffer, "XP: ", &playerToFill->experiencePoints);
            matchLabelInteger(lineBuffer, "DISCIPLINE_SCORE: ", &playerToFill->disciplineScore);
        }
        else if (sectionType == SECTION_QUESTS) {
            /* Each quest line: QUEST|day|stat|xp|completed|name */
            if (sscanf(lineBuffer, "QUEST|%d|%d|%d|%d|%63[^\n]",
                       &dayValue, &statValue, &rewardValue,
                       &completedValue, questNameBuffer) == 5
                && playerToFill->questCount < MAX_QUESTS) {
                questSlot = &playerToFill->quests[playerToFill->questCount];
                questSlot->dayIndex = dayValue;
                questSlot->targetStatIndex = statValue;
                questSlot->experienceReward = rewardValue;
                questSlot->isCompleted = completedValue;
                snprintf(questSlot->questName, NAME_LENGTH, "%s", questNameBuffer);
                playerToFill->questCount++;
            }
        }
        else if (sectionType == SECTION_ACTIVITY) {
            if (strlen(lineBuffer) > 0 && playerToFill->activityCount < MAX_ACTIVITY_LOGS) {
                snprintf(playerToFill->activityLogs[playerToFill->activityCount],
                         MAX_TEXT_LENGTH, "%s", lineBuffer);
                playerToFill->activityCount++;
            }
        }
    }

    fclose(playerFile);
    return 1;
}

/* ============================================================
 * SAVING AND EXITING
 * ============================================================ */

/* Saves the logged-in player's progress, or accounts if nobody is logged in */
static void saveCurrentProgress(void)
{
    if (isPlayerLoggedIn) {
        if (savePlayerFile(&currentPlayer)) {
            printf("\nProgress saved for %s.\n", currentPlayer.playerName);
        }
        else {
            printf("\nError: could not save progress!\n");
        }
    }
    else {
        saveAccountsFile();
        printf("\nAccounts saved.\n");
    }
}

/* Saves everything and closes the game */
static void exitGame(void)
{
    if (isPlayerLoggedIn) {
        savePlayerFile(&currentPlayer);
    }
    saveAccountsFile();

    printf("\nThanks for playing LIFE RPG. Keep leveling up your real life!\n");
    exit(0);
}

/* ============================================================
 * NAVIGATION HELPERS
 * Every menu shows Go back, Save progress and Exit.
 * ============================================================ */

/* Prints the three standard navigation options, starting at the given number */
static void showNavigationOptions(int firstOptionNumber)
{
    printf("%d. Go back\n", firstOptionNumber);
    printf("%d. Save progress\n", firstOptionNumber + 1);
    printf("%d. Exit\n", firstOptionNumber + 2);
}

/* Handles a navigation choice. Returns 1 when the player goes back. */
static int runNavigationChoice(int choice, int firstOptionNumber)
{
    if (choice == firstOptionNumber) {
        return 1;
    }
    if (choice == firstOptionNumber + 1) {
        saveCurrentProgress();
        return 0;
    }
    if (choice == firstOptionNumber + 2) {
        exitGame();
    }
    return 0;
}

/* Shows a numbered list with navigation, returns the 0-based index chosen or -1 for Go back */
static int chooseFromList(const char *headingText, const char **optionNames, int optionCount)
{
    int optionIndex;
    int choice;

    while (1) {
        printf("\n%s\n", headingText);
        for (optionIndex = 0; optionIndex < optionCount; optionIndex++) {
            printf("  %d. %s\n", optionIndex + 1, optionNames[optionIndex]);
        }
        showNavigationOptions(optionCount + 1);

        choice = readIntegerInRange("Choose: ", 1, optionCount + 3);

        if (choice <= optionCount) {
            return choice - 1;
        }
        if (runNavigationChoice(choice, optionCount + 1)) {
            return -1;
        }
    }
}

/* ============================================================
 * GAME LOGIC
 * XP, levels, stats and activity history rules.
 * ============================================================ */

/* Returns how much XP is needed to reach the next level */
static int getRequiredExperience(int playerLevel)
{
    return playerLevel * BASE_XP_PER_LEVEL;
}

/* Adds a timestamped entry to the player's activity history */
static void addActivityLog(Player *playerData, const char *activityText)
{
    char timeText[32];
    time_t currentTime = time(NULL);
    struct tm *localTime = localtime(&currentTime);
    int newLogIndex;

    if (localTime != NULL) {
        strftime(timeText, sizeof(timeText), "%Y-%m-%d %H:%M", localTime);
    }
    else {
        snprintf(timeText, sizeof(timeText), "unknown time");
    }

    /* Drop the oldest entry when the history is full */
    if (playerData->activityCount >= MAX_ACTIVITY_LOGS) {
        memmove(playerData->activityLogs[0],
                playerData->activityLogs[1],
                (MAX_ACTIVITY_LOGS - 1) * MAX_TEXT_LENGTH);
        playerData->activityCount = MAX_ACTIVITY_LOGS - 1;
    }

    newLogIndex = playerData->activityCount;
    snprintf(playerData->activityLogs[newLogIndex], MAX_TEXT_LENGTH,
             "[%s] %s", timeText, activityText);
    playerData->activityCount++;
}

/* Sets up a brand-new player with starting level, stats and an empty quest board */
static void initializePlayer(Player *newPlayer, const char *accountId, const char *playerName)
{
    int statIndex;

    memset(newPlayer, 0, sizeof(Player));
    snprintf(newPlayer->accountId, ID_LENGTH, "%s", accountId);
    snprintf(newPlayer->playerName, NAME_LENGTH, "%s", playerName);

    newPlayer->playerLevel = STARTING_LEVEL;
    newPlayer->experiencePoints = 0;
    newPlayer->disciplineScore = 0;
    newPlayer->questCount = 0;

    for (statIndex = 0; statIndex < STAT_COUNT; statIndex++) {
        newPlayer->statValues[statIndex] = STAT_START_VALUE;
    }

    addActivityLog(newPlayer, "Account created. Welcome to LIFE RPG!");
}

/* Raises a stat, but never above the maximum */
static void increaseStat(Player *playerData, int statIndex, int amountToAdd)
{
    playerData->statValues[statIndex] += amountToAdd;

    if (playerData->statValues[statIndex] > STAT_MAX_VALUE) {
        playerData->statValues[statIndex] = STAT_MAX_VALUE;
    }
}

/* Adds XP and handles every level up it causes */
static void addExperience(Player *playerData, int experienceAmount)
{
    int requiredExperience;
    char levelUpText[MAX_TEXT_LENGTH];

    playerData->experiencePoints += experienceAmount;
    requiredExperience = getRequiredExperience(playerData->playerLevel);

    while (playerData->experiencePoints >= requiredExperience) {
        playerData->experiencePoints -= requiredExperience;
        playerData->playerLevel++;

        printf("\n*** LEVEL UP! You are now level %d! ***\n", playerData->playerLevel);

        snprintf(levelUpText, sizeof(levelUpText),
                 "Reached level %d", playerData->playerLevel);
        addActivityLog(playerData, levelUpText);

        requiredExperience = getRequiredExperience(playerData->playerLevel);
    }
}

/* ============================================================
 * DISPLAY HELPERS
 * Text bars for stats, level progress and the quest board.
 * ============================================================ */

/* Prints one stat as a text bar, for example: Stamina  [####----] 40/100 */
static void printStatBar(const char *statLabel, int statValue)
{
    int filledBlocks = statValue * STAT_BAR_WIDTH / STAT_MAX_VALUE;
    int blockIndex;

    printf("%-14s [", statLabel);
    for (blockIndex = 0; blockIndex < STAT_BAR_WIDTH; blockIndex++) {
        putchar(blockIndex < filledBlocks ? '#' : '-');
    }
    printf("] %3d/%d\n", statValue, STAT_MAX_VALUE);
}

/* Prints the level bar with the XP counter directly below it */
static void printLevelBar(const Player *playerData)
{
    int requiredExperience = getRequiredExperience(playerData->playerLevel);
    int filledBlocks = playerData->experiencePoints * XP_BAR_WIDTH / requiredExperience;
    int blockIndex;

    printf("Level %-3d [", playerData->playerLevel);
    for (blockIndex = 0; blockIndex < XP_BAR_WIDTH; blockIndex++) {
        putchar(blockIndex < filledBlocks ? '#' : '-');
    }
    printf("]\n");

    /* XP shown directly below the level bar as Current/Required */
    printf("           %d/%d\n", playerData->experiencePoints, requiredExperience);
}

/* Prints every quest grouped under its day of the week */
static void printQuestBoard(const Player *playerData)
{
    int dayIndex;
    int questIndex;
    int foundQuestForDay;

    printf("\n=== WEEKLY QUEST BOARD ===\n");

    for (dayIndex = 0; dayIndex < DAYS_IN_WEEK; dayIndex++) {
        printf("\n%s:\n", dayNames[dayIndex]);
        foundQuestForDay = 0;

        for (questIndex = 0; questIndex < playerData->questCount; questIndex++) {
            if (playerData->quests[questIndex].dayIndex == dayIndex) {
                printf("  %2d. [%c] %s (%s, +%d XP)\n",
                       questIndex + 1,
                       playerData->quests[questIndex].isCompleted ? 'X' : ' ',
                       playerData->quests[questIndex].questName,
                       statDisplayNames[playerData->quests[questIndex].targetStatIndex],
                       playerData->quests[questIndex].experienceReward);
                foundQuestForDay = 1;
            }
        }

        if (!foundQuestForDay) {
            printf("  (no quests)\n");
        }
    }
}

/* ============================================================
 * QUEST ACTIONS
 * Create, complete and reset habits/quests.
 * ============================================================ */

/* Walks the player through creating a quest with a day, stat and difficulty */
static void createNewQuest(Player *playerData)
{
    char questNameBuffer[NAME_LENGTH];
    char activityText[MAX_TEXT_LENGTH];
    int dayChoice;
    int statChoice;
    int difficultyChoice;
    Quest *newQuest;

    printf("\n=== CREATE A NEW QUEST / HABIT ===\n");

    if (playerData->questCount >= MAX_QUESTS) {
        printf("Your quest board is full (%d quests maximum).\n", MAX_QUESTS);
        waitForEnter();
        return;
    }

    printf("Enter the quest or habit name (leave empty to go back): ");
    readLine(questNameBuffer, sizeof(questNameBuffer));
    trimWhitespace(questNameBuffer);

    if (strlen(questNameBuffer) == 0) {
        return;
    }

    dayChoice = chooseFromList("Which day of the week?", dayNames, DAYS_IN_WEEK);
    if (dayChoice < 0) {
        return;
    }

    statChoice = chooseFromList("Which stat should it train?", statDisplayNames, STAT_COUNT);
    if (statChoice < 0) {
        return;
    }

    difficultyChoice = chooseFromList("How hard is it?", difficultyLabels, DIFFICULTY_COUNT);
    if (difficultyChoice < 0) {
        return;
    }

    newQuest = &playerData->quests[playerData->questCount];
    snprintf(newQuest->questName, NAME_LENGTH, "%s", questNameBuffer);
    newQuest->dayIndex = dayChoice;
    newQuest->targetStatIndex = statChoice;
    newQuest->experienceReward = difficultyXpRewards[difficultyChoice];
    newQuest->isCompleted = 0;
    playerData->questCount++;

    snprintf(activityText, sizeof(activityText), "Created quest '%s' on %s",
             questNameBuffer, dayNames[dayChoice]);
    addActivityLog(playerData, activityText);

    printf("\nQuest added to %s!\n", dayNames[dayChoice]);
    waitForEnter();
}

/* Marks a quest as done, rewards XP, raises a stat and increases discipline */
static void completeQuest(Player *playerData)
{
    int questChoice;
    Quest *selectedQuest;
    char activityText[MAX_TEXT_LENGTH];

    if (playerData->questCount == 0) {
        printf("\nYou have no quests yet. Create one first!\n");
        waitForEnter();
        return;
    }

    while (1) {
        printQuestBoard(playerData);
        printf("\nPick a quest number to complete.\n");
        showNavigationOptions(playerData->questCount + 1);

        questChoice = readIntegerInRange("Choose: ", 1, playerData->questCount + 3);

        if (questChoice > playerData->questCount) {
            if (runNavigationChoice(questChoice, playerData->questCount + 1)) {
                return;
            }
            continue;
        }

        selectedQuest = &playerData->quests[questChoice - 1];

        if (selectedQuest->isCompleted) {
            printf("\nThat quest is already completed this week.\n");
            waitForEnter();
            continue;
        }

        selectedQuest->isCompleted = 1;
        playerData->disciplineScore++;
        increaseStat(playerData, selectedQuest->targetStatIndex, STAT_GAIN_PER_QUEST);

        printf("\nQuest completed: %s\n", selectedQuest->questName);
        printf("+%d XP and +%d %s\n",
               selectedQuest->experienceReward,
               STAT_GAIN_PER_QUEST,
               statDisplayNames[selectedQuest->targetStatIndex]);

        addExperience(playerData, selectedQuest->experienceReward);

        snprintf(activityText, sizeof(activityText), "Completed quest '%s' (+%d XP)",
                 selectedQuest->questName, selectedQuest->experienceReward);
        addActivityLog(playerData, activityText);

        waitForEnter();
        return;
    }
}

/* Marks every quest as not completed so habits can be repeated next week */
static void resetQuestBoard(Player *playerData)
{
    int questIndex;

    for (questIndex = 0; questIndex < playerData->questCount; questIndex++) {
        playerData->quests[questIndex].isCompleted = 0;
    }

    addActivityLog(playerData, "Started a new week: all quests reset");
    printf("\nAll quests are reset for a new week!\n");
    waitForEnter();
}

/* ============================================================
 * SUB-MENUS
 * Quests, developer tools, profile and activity history.
 * ============================================================ */

/* Quest and habit sub-menu */
static void questBoardMenu(Player *playerData)
{
    int choice;

    while (1) {
        printf("\n=== QUESTS AND HABITS ===\n");
        printf("1. View weekly quest board\n");
        printf("2. Create a new quest\n");
        printf("3. Complete a quest\n");
        printf("4. Reset quests for a new week\n");
        showNavigationOptions(5);

        choice = readIntegerInRange("Choose an option: ", 1, 7);

        if (choice == 1) {
            printQuestBoard(playerData);
            waitForEnter();
        }
        else if (choice == 2) {
            createNewQuest(playerData);
        }
        else if (choice == 3) {
            completeQuest(playerData);
        }
        else if (choice == 4) {
            resetQuestBoard(playerData);
        }
        else if (runNavigationChoice(choice, 5)) {
            return;
        }
    }
}

/* Developer mode, protected by the PIN KSM-321 */
static void developerMode(Player *playerData)
{
    char pinBuffer[MAX_TEXT_LENGTH];
    int choice;
    int statIndex;

    printf("\n=== DEVELOPER MODE ===\n");
    printf("Enter developer PIN (leave empty to go back): ");
    readLine(pinBuffer, sizeof(pinBuffer));
    trimWhitespace(pinBuffer);

    if (strlen(pinBuffer) == 0) {
        return;
    }

    if (strcmp(pinBuffer, DEVELOPER_PIN) != 0) {
        printf("Wrong PIN. Access denied.\n");
        waitForEnter();
        return;
    }

    addActivityLog(playerData, "Developer mode unlocked");

    while (1) {
        printf("\n=== DEVELOPER TOOLS ===\n");
        printf("1. Add %d XP (test level up)\n", DEVELOPER_XP_BOOST);
        printf("2. Set all stats to maximum\n");
        printf("3. Reset all stats to starting values\n");
        printf("4. Mark all quests as not completed\n");
        showNavigationOptions(5);

        choice = readIntegerInRange("Choose an option: ", 1, 7);

        if (choice == 1) {
            addExperience(playerData, DEVELOPER_XP_BOOST);
            addActivityLog(playerData, "Developer added bonus XP");
        }
        else if (choice == 2) {
            for (statIndex = 0; statIndex < STAT_COUNT; statIndex++) {
                playerData->statValues[statIndex] = STAT_MAX_VALUE;
            }
            addActivityLog(playerData, "Developer set all stats to maximum");
            printf("All stats set to maximum.\n");
        }
        else if (choice == 3) {
            for (statIndex = 0; statIndex < STAT_COUNT; statIndex++) {
                playerData->statValues[statIndex] = STAT_START_VALUE;
            }
            addActivityLog(playerData, "Developer reset all stats");
            printf("All stats reset to starting values.\n");
        }
        else if (choice == 4) {
            resetQuestBoard(playerData);
        }
        else if (runNavigationChoice(choice, 5)) {
            return;
        }
    }
}

/* Profile screen showing level, XP, stats and discipline */
static void showProfile(Player *playerData)
{
    int choice;
    int statIndex;

    while (1) {
        printf("\n=== PROFILE: %s ===\n", playerData->playerName);
        printf("Account ID: %s\n", playerData->accountId);

        printf("\nLEVEL PROGRESS\n");
        printLevelBar(playerData);

        printf("\nSTATS\n");
        for (statIndex = 0; statIndex < STAT_COUNT; statIndex++) {
            printStatBar(statDisplayNames[statIndex], playerData->statValues[statIndex]);
        }

        printf("\nDiscipline score (quests completed): %d\n", playerData->disciplineScore);
        printf("Quests on your board: %d\n", playerData->questCount);

        showNavigationOptions(1);
        choice = readIntegerInRange("Choose an option: ", 1, 3);

        if (runNavigationChoice(choice, 1)) {
            return;
        }
    }
}

/* Activity history screen, newest entries first */
static void showActivityHistory(Player *playerData)
{
    int logIndex;
    int choice;

    while (1) {
        printf("\n=== ACTIVITY HISTORY (%d entries, newest first) ===\n",
               playerData->activityCount);

        if (playerData->activityCount == 0) {
            printf("No activity yet.\n");
        }

        for (logIndex = playerData->activityCount - 1; logIndex >= 0; logIndex--) {
            printf("%s\n", playerData->activityLogs[logIndex]);
        }

        showNavigationOptions(1);
        choice = readIntegerInRange("Choose an option: ", 1, 3);

        if (runNavigationChoice(choice, 1)) {
            return;
        }
    }
}

/* ============================================================
 * PLAYER MENU AND LOGIN
 * The main hub for a logged-in player.
 * ============================================================ */

/* Main hub shown after a successful login */
static void playerMenu(void)
{
    int choice;

    while (1) {
        printf("\n=== %s's LIFE HUB (Level %d) ===\n",
               currentPlayer.playerName, currentPlayer.playerLevel);
        printf("1. View profile and stats\n");
        printf("2. Quests and habits\n");
        printf("3. Activity history\n");
        printf("4. Developer mode\n");
        showNavigationOptions(5);

        choice = readIntegerInRange("Choose an option: ", 1, 7);

        if (choice == 1) {
            showProfile(&currentPlayer);
        }
        else if (choice == 2) {
            questBoardMenu(&currentPlayer);
        }
        else if (choice == 3) {
            showActivityHistory(&currentPlayer);
        }
        else if (choice == 4) {
            developerMode(&currentPlayer);
        }
        else if (runNavigationChoice(choice, 5)) {
            return;
        }
    }
}

/* Asks for the KSM ID as password, then opens the player's hub */
static void attemptLogin(int accountIndex)
{
    char passwordBuffer[MAX_TEXT_LENGTH];
    int attemptNumber;

    for (attemptNumber = 1; attemptNumber <= MAX_PASSWORD_ATTEMPTS; attemptNumber++) {
        printf("\nEnter KSM ID for %s (0 = go back): ",
               accountList[accountIndex].accountName);
        readLine(passwordBuffer, sizeof(passwordBuffer));
        trimWhitespace(passwordBuffer);

        if (strcmp(passwordBuffer, "0") == 0) {
            return;
        }

        if (strcmp(passwordBuffer, accountList[accountIndex].accountId) == 0) {
            if (!loadPlayerFile(accountList[accountIndex].accountId, &currentPlayer)) {
                printf("Error: could not load the save file for this account.\n");
                waitForEnter();
                return;
            }

            isPlayerLoggedIn = 1;
            printf("\nWelcome back, %s!\n", currentPlayer.playerName);

            playerMenu();

            /* Logging out saves progress automatically */
            savePlayerFile(&currentPlayer);
            isPlayerLoggedIn = 0;
            printf("Logged out. Progress saved.\n");
            return;
        }

        printf("Wrong KSM ID. Attempts left: %d\n",
               MAX_PASSWORD_ATTEMPTS - attemptNumber);
    }

    printf("Too many wrong attempts. Returning to the main menu.\n");
    waitForEnter();
}

/* ============================================================
 * ACCOUNT CREATION
 * Generates a unique KSM ID and creates the player's save file.
 * ============================================================ */

/* Creates a KSM-xxxx ID that no existing account is already using */
static void generateUniqueAccountId(char *idBuffer, int bufferSize)
{
    int randomNumber;
    int accountIndex;
    int isDuplicate;

    do {
        randomNumber = 1000 + rand() % 9000;
        snprintf(idBuffer, bufferSize, "KSM-%04d", randomNumber);

        isDuplicate = 0;
        for (accountIndex = 0; accountIndex < accountCount; accountIndex++) {
            if (strcmp(accountList[accountIndex].accountId, idBuffer) == 0) {
                isDuplicate = 1;
                break;
            }
        }
    } while (isDuplicate);
}

/* Registers a new account, saves it, and creates its KSM-xxxx.txt file */
static void createNewAccount(void)
{
    char nameBuffer[NAME_LENGTH];
    Account *newAccount;
    Player newPlayer;

    printf("\n=== CREATE NEW ACCOUNT ===\n");

    if (accountCount >= MAX_ACCOUNTS) {
        printf("The account limit has been reached.\n");
        waitForEnter();
        return;
    }

    printf("Enter your full name (leave empty to go back): ");
    readLine(nameBuffer, sizeof(nameBuffer));
    trimWhitespace(nameBuffer);

    if (strlen(nameBuffer) == 0) {
        return;
    }

    newAccount = &accountList[accountCount];
    generateUniqueAccountId(newAccount->accountId, sizeof(newAccount->accountId));
    snprintf(newAccount->accountName, NAME_LENGTH, "%s", nameBuffer);
    accountCount++;

    saveAccountsFile();

    initializePlayer(&newPlayer, newAccount->accountId, newAccount->accountName);
    if (!savePlayerFile(&newPlayer)) {
        printf("Warning: could not create the save file for this account.\n");
    }

    printf("\nAccount created for %s!\n", newAccount->accountName);
    printf("Your KSM ID (your password): %s\n", newAccount->accountId);
    printf("Write it down. You need it to log in.\n");
    waitForEnter();
}

/* ============================================================
 * MAIN MENU
 * Lists account names only (IDs stay hidden) and handles the start screen.
 * ============================================================ */

/* Start screen: shows names, create account, and navigation options */
static void mainMenu(void)
{
    int choice;
    int accountIndex;

    while (1) {
        printf("\n==============================================\n");
        printf("       LIFE RPG - Level Up Your Real Life\n");
        printf("==============================================\n");

        if (accountCount == 0) {
            printf("No accounts yet. Create one to begin!\n");
        }
        else {
            printf("Registered players (choose a number to log in):\n");
            for (accountIndex = 0; accountIndex < accountCount; accountIndex++) {
                printf("  %d. %s\n", accountIndex + 1, accountList[accountIndex].accountName);
            }
        }

        printf("\n%d. Create new account\n", accountCount + 1);
        printf("%d. Go back (you are already on the main menu)\n", accountCount + 2);
        printf("%d. Save progress\n", accountCount + 3);
        printf("%d. Exit\n", accountCount + 4);

        choice = readIntegerInRange("Choose an option: ", 1, accountCount + 4);

        if (choice <= accountCount) {
            attemptLogin(choice - 1);
        }
        else if (choice == accountCount + 1) {
            createNewAccount();
        }
        else if (choice == accountCount + 2) {
            printf("You are already on the main menu.\n");
        }
        else {
            runNavigationChoice(choice, accountCount + 2);
        }
    }
}

/* ============================================================
 * PROGRAM START
 * ============================================================ */

/* Starts the random generator, loads accounts, and opens the main menu */
int main(void)
{
    srand((unsigned int)time(NULL));
    loadAccountsFile();

    printf("Welcome to LIFE RPG!\n");
    mainMenu();

    return 0;
}