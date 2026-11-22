#include <stdio.h>
#include <string.h>
int curGold = 300;
#define MAX_LOGS 200
#define MAX_LEN 128

int logCount = 0;
char logText[MAX_LOGS][MAX_LEN];

// Helper Function: Save activity in log
void addLog(const char *msg)
{
    if (logCount < MAX_LOGS)
    {
        strcpy(logText[logCount], msg);
        logCount++;
    }
}

// Helper Function: Pause screen
void pressEnter(void)
{
    printf("\nPress Enter to continue...");
    getchar();
}

// Reward Shop Function
void rewardShop(void)
{
    char items[3][32] = {
        "1 Hour Free Time",
        "Favorite Snack",
        "Day Off / Sleep In"
    };

    int prices[3] = {50, 100, 250};

    int choice;

    do
    {
        printf("\n REWARD SHOP \n");

        printf("Your Gold: %d Gold\n", curGold);

        printf("1. %s - %d Gold\n", items[0], prices[0]);
        printf("2. %s - %d Gold\n", items[1], prices[1]);
        printf("3. %s - %d Gold\n", items[2], prices[2]);
        printf("4. Go Back\n");

        printf("Choose reward to buy: ");

        scanf("%d", &choice);
        getchar();

        if (choice >= 1 && choice <= 3)
        {
            int idx = choice - 1;

            if (curGold >= prices[idx])
            {
                curGold -= prices[idx];

                printf("\nSuccessfully purchased: %s!\n", items[idx]);

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

// Demo main function
int main(void)
{
    rewardShop();

    printf("\nRemaining Gold: %d\n", curGold);

    return 0;
}
