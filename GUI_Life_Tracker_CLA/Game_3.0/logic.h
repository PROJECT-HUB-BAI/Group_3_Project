#ifndef LOGIC_H
#define LOGIC_H

/* ============================================================================
   LIFE TRACKER RPG - Group 3 (Mamoon, Sajid, Khadija)
   Programming Fundamentals Lab - FAST NUCES
   File: logic.h (Game Logic Header & Data Definitions)
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

/* ----------------------------------------------------------- QUEST STRUCT --*/
typedef struct {
    char title[STR_LEN];
    int day;
    int stat;
    int act;
    int amt;
    int diff;
    int done;
} Quest;

/* ------------------------------------------------------------ DATA ARRAYS --*/
extern const char *days[7];
extern const char *shortDays[7];
extern const char *statNames[5];
extern const char *classNames[3];
extern const char *classDesc[3];
extern const char *diffNames[3];
extern const char *acts[5][5];
extern const char *units[5][5];
extern const char *tips[8];
extern const char *fortunes[7];

/* ----------------------------------------------------------- GLOBAL STATE --*/
extern char accNames[MAX_ACC][STR_LEN];
extern char accPins[MAX_ACC][10];
extern int  accCount;

extern char myName[STR_LEN];
extern int  level, xp, gold, cls, streak;
extern long lastDay;
extern int  stats[5];

extern Quest quests[MAX_QUESTS];
extern int   qCount;

/* ------------------------------------------------------- LOGIC PROTOTYPES --*/
void InitGameLogic(void);
void TrimString(char *s);
int  TodayIndex(void);
int  XpNeeded(void);
void LogNote(const char *sentence);
void SaveAccounts(void);
void LoadAccounts(void);
int  FindAccount(const char *name);
int  ReadField(const char *line, int pos, char *out, int maxLen);
int  ReadNumber(const char *line, int *pos);
void SavePlayerData(void);
int  LoadPlayerData(void);

int  TryLogin(const char *name, const char *pin);
int  TrySignup(char *name, const char *pin, int classChoice);

int  AddQuest(const char *title, int day, int stat, int act, int amt, int diff);
int  CompleteQuest(int index, int *outGained);
void DeleteQuest(int index);

int  BuyShopReward(int cost, const char *rewardName);

void DevCompleteAllToday(void);
void DevWipeAllQuests(void);
void DevResetStreak(void);

#endif /* LOGIC_H */
