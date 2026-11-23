#include <stdio.h>

int main()
{
    char saved_name[40] = "";
    char saved_pin[20] = "";
    char entered_name[40];
    char entered_pin[20];
    char developer_pin[8] = "321";
    char entered_developer_pin[8];
    int account_created = 0;
    int logged_in = 0;
    int running = 1;
    int choice;
    int i;
    int name_matches;
    int pin_matches;
    int xp = 0;
    int level = 1;
    int earned_xp;

    while (running == 1)
    {
        if (logged_in == 0)
        {
            printf("\n1. Create account\n2. Login\n3. Exit\nChoose: ");
            scanf("%d", &choice);

            if (choice == 1)
            {
                printf("Enter a username: ");
                scanf("%39s", saved_name);
                printf("Create a PIN: ");
                scanf("%19s", saved_pin);
                account_created = 1;
                logged_in = 1;
                printf("Account created. Welcome, %s.\n", saved_name);
            }
            else if (choice == 2)
            {
                if (account_created == 0)
                {
                    printf("Create an account first.\n");
                }
                else
                {
                    printf("Username: ");
                    scanf("%39s", entered_name);
                    printf("PIN: ");
                    scanf("%19s", entered_pin);
                    name_matches = 1;
                    i = 0;
                    while (saved_name[i] != 0 || entered_name[i] != 0)
                    {
                        if (saved_name[i] != entered_name[i])
                        {
                            name_matches = 0;
                            break;
                        }
                        i++;
                    }
                    pin_matches = 1;
                    i = 0;
                    while (saved_pin[i] != 0 || entered_pin[i] != 0)
                    {
                        if (saved_pin[i] != entered_pin[i])
                        {
                            pin_matches = 0;
                            break;
                        }
                        i++;
                    }
                    if (name_matches == 1 && pin_matches == 1)
                    {
                        logged_in = 1;
                        printf("Login successful. Welcome, %s.\n", saved_name);
                    }
                    else
                    {
                        printf("Username or PIN is incorrect.\n");
                    }
                }
            }
            else if (choice == 3)
            {
                running = 0;
            }
            else
            {
                printf("Invalid choice.\n");
            }
        }
        else
        {
            printf("\nPlayer Hub - %s\n", saved_name);
            printf("Level: %d  XP: %d\n", level, xp);
            printf("1. Add XP\n2. Developer mode\n3. Logout\nChoose: ");
            scanf("%d", &choice);

            if (choice == 1)
            {
                printf("XP earned: ");
                scanf("%d", &earned_xp);
                if (earned_xp > 0)
                {
                    xp = xp + earned_xp;
                    while (xp >= level * 100)
                    {
                        xp = xp - level * 100;
                        level++;
                        printf("Level up! New level: %d\n", level);
                    }
                    printf("XP added.\n");
                }
                else
                {
                    printf("Enter a positive XP amount.\n");
                }
            }
            else if (choice == 2)
            {
                printf("Developer PIN (demo: 321): ");
                scanf("%7s", entered_developer_pin);
                pin_matches = 1;
                i = 0;
                while (developer_pin[i] != 0 || entered_developer_pin[i] != 0)
                {
                    if (developer_pin[i] != entered_developer_pin[i])
                    {
                        pin_matches = 0;
                        break;
                    }
                    i++;
                }
                if (pin_matches == 1)
                {
                    printf("Developer mode: %s, level %d, XP %d.\n", saved_name, level, xp);
                    printf("1. Add 500 XP\n2. Back\nChoose: ");
                    scanf("%d", &choice);
                    if (choice == 1)
                    {
                        xp = xp + 500;
                        while (xp >= level * 100)
                        {
                            xp = xp - level * 100;
                            level++;
                        }
                        printf("Testing XP added. Level is now %d.\n", level);
                    }
                    else if (choice == 2)
                    {
                        printf("Back to the player hub.\n");
                    }
                    else
                    {
                        printf("Invalid choice.\n");
                    }
                }
                else
                {
                    printf("Developer PIN is incorrect.\n");
                }
            }
            else if (choice == 3)
            {
                logged_in = 0;
                printf("Logged out.\n");
            }
            else
            {
                printf("Invalid choice.\n");
            }
        }
    }
    printf("Goodbye.\n");
    return 0;
}
