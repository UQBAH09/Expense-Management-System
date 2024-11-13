#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Initialize the file with "a+" mode to allow reading and appending
void Initialize(FILE **file, const char *filename)
{
    *file = fopen(filename, "a+");
    if (*file == NULL)
    {
        printf("Error: The file can't be opened.\n");
        exit(1);
    }
}

// Check if a username already exists in the file
int checkUsername(FILE *filename, char username[])
{
    rewind(filename); // Move file pointer to the beginning
    char currentUsername[50], line[150];
    while (fgets(line, sizeof(line), filename) != NULL)
    {
        sscanf(line, "%[^,]", currentUsername); // Extract username from the line
        if (strcmp(currentUsername, username) == 0)
        {
            return 1; // Username exists
        }
    }
    return 0; // Username does not exist
}

// Create a new account by adding a username and password to the file
void createAccount(FILE *filename)
{
    int error = 1;
    char username[50], pass[50];
    while (error)
    {
        printf("Enter Username: ");
        scanf("%49s", username);
        printf("Enter Password: ");
        scanf("%49s", pass);
        error = checkUsername(filename, username); // Check if username is unique

        if (error)
        {
            printf("This username already exists. Please choose another one.\n");
        }
    }
    fprintf(filename, "%s,%s,%s.csv\n", username, pass, username); // Write account info to file
}

// Retrieve account info if username and password are correct
char *getInfo(FILE *filename, char name[], char pass[])
{
    char line[150], nameCheck[50], passCheck[50], file[50];
    int flag = 0;

    while (fgets(line, sizeof(line), filename) != NULL)
    {
        sscanf(line, "%49[^,],%49[^,],%49[^\n]", nameCheck, passCheck, file); // Parse user data

        if (strcmp(name, nameCheck) == 0) // Check if username matches
        {
            flag = 1;
            break;
        }
    }

    if (flag == 1)
    {
        if (strcmp(passCheck, pass) == 0) // Check if password matches
        {
            printf("Successfully logged in\n");
            return strdup(file); // Return user-specific file on success
        }
        printf("Wrong password\n");
        return NULL;
    }

    printf("Username doesn't exist.\n");
    return NULL;
}

// Get the last item number in the file to generate the next unique item number
int lastItemNum(FILE *file)
{
    char line[300];
    int itemNum = 0;
    rewind(file); // Start reading from the beginning

    while (fgets(line, sizeof(line), file) != NULL)
    {
        sscanf(line, "%d", &itemNum); // Get last item number
    }

    return itemNum; // Return last item number
}

// Add a new expense entry to the file
void expenseEntry(FILE *file)
{
    char categorie[50], itemName[50], discription[100], date[50];
    int amount, itemNum;

    printf("Enter categorie name: ");
    fgets(categorie, sizeof(categorie), stdin);
    categorie[strcspn(categorie, "\n")] = '\0';

    printf("Enter item name: ");
    fgets(itemName, sizeof(itemName), stdin);
    itemName[strcspn(itemName, "\n")] = '\0';

    printf("Enter amount: ");
    scanf("%d", &amount);
    getchar();

    printf("Enter date(DD/MM/YYYY): ");
    fgets(date, sizeof(date), stdin);
    date[strcspn(date, "\n")] = '\0';

    printf("Enter discription: ");
    fgets(discription, sizeof(discription), stdin);
    discription[strcspn(discription, "\n")] = '\0';

    itemNum = lastItemNum(file) + 1; // Increment item number
    fprintf(file, "%d,Expense,%s,%s,%d,%s,%s\n", itemNum, categorie, itemName, amount, date, discription);
    fflush(file); // Ensure data is written to file
}

// Delete an expense entry based on item number
void deleteExpense(FILE *file, char *userFile)
{
    FILE *temp = fopen("temp.csv", "w");
    if (temp == NULL)
    {
        printf("Error: The file can't be opened.\n");
        exit(1);
    }

    rewind(file); // Start from the beginning of the file

    char line[300];
    int delNum, itemNum;

    printf("Enter item number to delete: ");
    scanf("%d", &delNum);

    if (delNum > lastItemNum(file))
    {
        printf("Item doesn't exist.");
        return;
    }
    while (fgets(line, sizeof(line), file) != NULL)
    {
        sscanf(line, "%d", &itemNum);
        if (itemNum != delNum) // Copy lines except the one to delete
        {
            fputs(line, temp);
        }
    }
    fclose(file);
    fclose(temp);

    // Replace original file with updated temp file
    remove(userFile);
    rename("temp.csv", userFile);

    // Reopen the updated file in append mode
    file = fopen(userFile, "a+");
    if (file == NULL)
    {
        printf("Error: The file can't be opened.\n");
        exit(1);
    }
}

// Display all expense entries in a table format
void viewExpense(FILE *file)
{
    rewind(file); // Start from the beginning
    printf("+----------+----------------+---------------+--------+----------+----------------------------+\n");
    printf("| ITEM NUM |    CATAGORY    |    ITEM NAME	| AMOUNT |   DATE   | 		DESCRIPTION	         |\n");
    printf("+----------+----------------+---------------+--------+----------+----------------------------+\n");

    char line[300], type[10], categorie[50], itemName[50], discription[100], date[50];
    int amount, itemNum;

    while (fgets(line, sizeof(line), file) != NULL)
    {
        sscanf(line, "%d,%[^,],%[^,],%[^,],%d,%[^,],%[^\n]", &itemNum, type, categorie, itemName, &amount, date, discription);
        if (strcmp(type, "Expense") == 0) // Only display entries of type "Expense"
        {
            printf("|  %d  |    %s    |    %s    | %d pkr |   %s   |    %s    |\n", itemNum, categorie, itemName, amount, date, discription);
        }
    }
    printf("+----------+----------------+---------------+--------+----------+----------------------------+\n");
}

int main(void)
{
    FILE *password;
    const char *filename = "password.csv";
    Initialize(&password, filename); // Open password file

    int choice;
    printf("Press 1 to login and 2 to create a new account: ");
    scanf("%d", &choice);

    getchar(); // Clear newline left by scanf

    char *userFile = NULL;

    switch (choice)
    {
    case 1:
    {
        char name[50], pass[50];

        printf("Enter name: ");
        fgets(name, 50, stdin);
        name[strcspn(name, "\n")] = '\0';

        printf("Enter password: ");
        fgets(pass, 50, stdin);
        pass[strcspn(pass, "\n")] = '\0';

        userFile = getInfo(password, name, pass); // Attempt login

        if (userFile == NULL)
        {
            fclose(password);
            return 1; // Exit if login fails
        }

        break;
    }

    case 2:
        createAccount(password); // Create a new account
        printf("\nSuccessfully created an account.\n");
        return 0;

    default:
        printf("Wrong choice\n");
        fclose(password);
        return 1;
    }

    fclose(password);

    FILE *file;
    Initialize(&file, userFile); // Open user-specific file
    printf("User file (%s) opened successfully.\n", userFile);

    choice = 0;

    while (1)
    {
        printf("\nPress 1 to enter expense, 2 to delete expenses, 3 to view expense and 4 to exit: ");
        scanf("%d", &choice);
        getchar();

        switch (choice)
        {
        case 1:
            expenseEntry(file); // Add expense
            break;
        case 2:
            deleteExpense(file, userFile); // Delete expense
            break;
        case 3:
            viewExpense(file); // View expenses
            break;
        case 4:
            return 0;
        default:
            printf("Wrong choice\n");
        }
    }

    fclose(file);
    free(userFile); // Free allocated memory for user file
}
