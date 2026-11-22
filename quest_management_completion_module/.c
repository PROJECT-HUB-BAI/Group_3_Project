#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUESTS 50

/* QUEST DATA */
int questCount = 0;

char questName[MAX_QUESTS][64];
int questDay[MAX_QUESTS];
int questStat[MAX_QUESTS];
int questReward[MAX_QUESTS];
int questDone[MAX_QUESTS];

/* DAY NAMES */
const char *days[7] = {
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday",
    "Sunday"
};

/* PLAYER STAT NAMES */
const char *statNames[5] = {
    "Intellectual",
    "Stamina",
    "Discipline",
    "Social Skills",
    "Creativity"
};

/* PRINT QUEST BOARD */
void printQuestBoard(void) {

    printf("\n=== WEEKLY QUEST BOARD ===\n");

    printf(
        "Filter: 0=All, 1=Intellect, 2=Stamina, "
        "3=Discipline, 4=Social, 5=Creativity, "
        "6=Incomplete Only\n"
    );

    printf("Enter filter choice: ");

    int filter;
    scanf("%d", &filter);
    getchar();

    for (int d = 0; d < 7; d++) {

        int found = 0;

        for (int i = 0; i < questCount; i++) {

            if (questDay[i] == d) {

                if (filter >= 1 && filter <= 5 &&
                    questStat[i] != (filter - 1)) {
                    continue;
                }

                if (filter == 6 && questDone[i] == 1) {
                    continue;
                }

                if (!found) {
                    printf("\n%s:\n", days[d]);
                    found = 1;
                }

                printf(
                    "  [%c] %d. %s (%s, +%d XP)\n",
                    questDone[i] ? 'X' : ' ',
                    i + 1,
                    questName[i],
                    statNames[questStat[i]],
                    questReward[i]
                );
            }
        }
    }
}