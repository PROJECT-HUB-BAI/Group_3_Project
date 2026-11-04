#include <stdio.h>
#include <string.h>

struct Player {
    char playerName[50];
    int playerLevel;
    int playerXP;
    int stats[5];
};

// PART 1: DISPLAY
void show_display_screen(struct Player *p) {
    printf("\n===== WELCOME TO LIFE RPG - Level Up Your Real Life =====\n");
    printf("Player: %s | Level: %d | XP: %d\n", p->playerName, p->playerLevel, p->playerXP);
    printf("Intellectual: %d\n", p->stats[0]);
    printf("Stamina: %d\n", p->stats[1]);
    printf("Discipline: %d\n", p->stats[2]);
    printf("Social: %d\n", p->stats[3]);
    printf("Creativity: %d\n", p->stats[4]);
}

// PART 2: RESET
void reset_all_variables(struct Player *p) {
    p->playerLevel = 1;
    p->playerXP = 0;
    p->stats[0] = 0;
    p->stats[1] = 0;
    p->stats[2] = 0;
    p->stats[3] = 0;
    p->stats[4] = 0;
    printf("New User or File Error. Resetting...\n");
}

// PART 3: LOAD
int load_progress_module(struct Player *p, char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        return 0;
    }
    fscanf(file, "%s %d %d", p->playerName, &p->playerLevel, &p->playerXP);
    fclose(file);

    if (p->playerLevel >= 1 && p->playerLevel < 100) {
        return 1;
    } else {
        return 0;
    }
}

// PART 4: SAVE AND QUIT
int save_progress_module(struct Player *p, char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Save Failed\n");
        return 0;
    }
    fprintf(file, "%s %d %d", p->playerName, p->playerLevel, p->playerXP);
    fclose(file);
    printf("File Saved Successfully! Quitting...\n");
    return 1;
}