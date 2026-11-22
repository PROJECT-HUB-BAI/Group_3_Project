#include <stdio.h>
int curLevel = 1;
int curXp = 0;
int curGold = 0;
int curClass = 0;
int curStats[5] = {10, 10, 10, 10, 10};
int curDiscipline = 0;

char curId[16] = "KSM-1234";
char curName[64] = "Player";

char dailyFortune[128] = "Normal Day";



const char *statNames[5] = {
    "Intellectual",
    "Stamina",
    "Discipline",
    "Social Skills",
    "Creativity"
};

const char *classNames[3] = {
    "Scholar (+XP to Intellect)",
    "Athlete (+XP to Stamina)",
    "Artist (+XP to Creativity)"
};



void pressEnter(void) {

    char buf[10];

    printf("\nPress Enter to continue...");

    fgets(buf, sizeof(buf), stdin);
}


void printTitle(void) {

    int highestStatIdx = 0;
    int maxVal = curStats[0];

    for (int i = 1; i < 5; i++) {

        if (curStats[i] > maxVal) {

            maxVal = curStats[i];

            highestStatIdx = i;
        }
    }

    printf("Title: ");

    if (curLevel < 3) {

        printf("Novice Adventurer\n");
    }

    else if (highestStatIdx == 0) {

        printf("Grand Scholar ??\n");
    }

    else if (highestStatIdx == 1) {

        printf("Iron Athlete ?\n");
    }

    else if (highestStatIdx == 2) {

        printf("Master of Discipline ???\n");
    }

    else if (highestStatIdx == 3) {

        printf("Charismatic Diplomat ??\n");
    }

    else {

        printf("Visionary Creator ??\n");
    }
}



void viewProfile(void) {

    printf("\n=== PROFILE: %s ===\n", curName);

    printf("ID: %s | Class: %s\n",
           curId, classNames[curClass]);

    printTitle();

    printf("Level: %d (XP: %d/%d) | Gold: %d ??\n",
           curLevel,
           curXp,
           curLevel * 100,
           curGold);

    printf("\n[Daily Fortune]: %s\n", dailyFortune);

    printf("\nSTATS:\n");

    for (int i = 0; i < 5; i++) {

        printf("- %s: %d/100\n",
               statNames[i],
               curStats[i]);
    }

    printf("Discipline Score: %d\n", curDiscipline);

    pressEnter();
}