// ============================================
// MY PART - KHADIJA: DISPLAY, SAVE & QUIT, LOAD + RESET
// ============================================

// --- PART 1: DISPLAY ---
// Ye welcome + profile screen hai
void show_display_screen(Player *p) {
    printf("\n========================================\n");
    printf(" WELCOME TO LIFE RPG - Level Up Your Real Life \n");
    printf("========================================\n");
    printf(" Player: %s | Level: %d | XP: %d\n", p->playerName, p->playerLevel, p->playerXP);
    printf(" ----------------------------------------\n");
    printf(" Intellectual: %d\n", p->stats[0]);
    printf(" Stamina:      %d\n", p->stats[1]);
    printf(" Discipline:   %d\n", p->stats[2]);
    printf(" Social Skills:%d\n", p->stats[3]);
    printf(" Creativity:   %d\n", p->stats[4]);
    printf("========================================\n");
    printf(" [1] Continue Game\n");
    printf(" [2] SAVE AND QUIT\n");
    printf("========================================\n");
}

// --- PART 2: RESET ---
// Agar file nahi mili ya corrupt hai to sab zero
void reset_all_variables(Player *p) {
    printf("\n[RESET] New User or File Error. Resetting...\n");
    p->playerLevel = 1;
    p->playerXP = 0;
    for(int i = 0; i < 5; i++) {
        p->stats[i] = 0;
    }
    p->questCount = 0;
    p->activityCount = 0;
}

// --- PART 3: LOAD ---
// File se load karo. 1 = success, 0 = fail
int load_progress_module(Player *p, char *filename) {
    FILE *file = fopen(filename, "r");
    if(file == NULL) {
        return 0; // File nahi hai = New User
    }
    // Uske wale loadPlayer jaisa hi, bas check ke saath
    // Agar file kharab hai to bhi 0 return karo
    int result = loadPlayerFile(file, p); // uska function
    fclose(file);

    // Sir wali condition: Level >=1 AND Level < 100
    if(result == 1 && p->playerLevel >= 1 && p->playerLevel < 100) {
        printf("loaded = 1 (true) - Save Loaded!\n");
        return 1;
    } else {
        printf("loaded = 0 (false) - File corrupt\n");
        return 0;
    }
}

// --- PART 4: SAVE AND QUIT ---
int save_progress_module(Player *p, char *filename) {
    FILE *file = fopen(filename, "w"); // 1. Open
    if(file == NULL) {
        printf("Save Failed!\n");
        return 0;
    }
    savePlayerFile(file, p); // 2. Write - uska function
    fclose(file); // 3. Close
    printf("File Saved Successfully! Quitting...\n");
    return 1;
}

// --- TUMHARA MAIN LOGIC ---
// Isko uske main menu me lagana hai
void my_part_menu(Player *p, char *filename) {
    int loaded = load_progress_module(p, filename);
    if(loaded == 0) {
        reset_all_variables(p);
    }
    show_display_screen(p);

    int choice;
    printf("Enter choice: ");
    scanf("%d", &choice);
    if(choice == 2) {
        save_progress_module(p, filename);
    }
}