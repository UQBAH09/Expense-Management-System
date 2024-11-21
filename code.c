#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure to hold the details of each expense
struct Expense
{
    char category[50];        // Category of the expense (e.g., Food, Transport)
    char itemName[50];        // Name of the item purchased
    int amount;               // Amount of the expense
    char date[50];            // Date of the expense (DD/MM/YYYY)
    char description[100];    // Detailed description of the expense
};

// Function to initialize and open a file for reading/writing with error handling
void initializeFile(FILE **file, const char *filename, const char *mode)
{
    *file = fopen(filename, mode); // Open the file with specified mode
    if (*file == NULL)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);  // Exit with failure code if the file cannot be opened
    }
}

// Function to check if a username exists in the file
int checkIfUsernameExists(FILE *file, const char username[])
{
    rewind(file); // Move file pointer to the beginning to read from start
    char storedUsername[50], line[150];
    while (fgets(line, sizeof(line), file) != NULL) // Read line by line
    {
        sscanf(line, "%[^,]", storedUsername); // Extract the username from the line
        if (strcmp(storedUsername, username) == 0) // Compare with given username
        {
            return 1; // Return 1 if username exists
        }
    }
    return 0; // Return 0 if username does not exist
}

// Function to create a new account (username and password) with error handling
void createNewAccount(FILE *file)
{
    int usernameExists = 1;
    char username[50], password[50];
    while (usernameExists)
    {
        printf("Enter Username: ");
        if (fgets(username, sizeof(username), stdin) == NULL)
        {
            perror("Error reading username");
            exit(EXIT_FAILURE);  // Exit if reading username fails
        }
        username[strcspn(username, "\n")] = '\0'; // Remove newline character from input

        usernameExists = checkIfUsernameExists(file, username); // Check if the username is unique

        if (usernameExists)
        {
            printf("This username already exists. Please choose another one.\n");
        }
    }
    printf("Enter Password: ");
    if (fgets(password, sizeof(password), stdin) == NULL)
    {
        perror("Error reading password");
        exit(EXIT_FAILURE);  // Exit if reading password fails
    }
    password[strcspn(password, "\n")] = '\0'; // Remove newline character from password

    if (fprintf(file, "%s,%s,%s.csv\n", username, password, username) < 0) // Save username, password, and file name
    {
        perror("Error writing to file");
        exit(EXIT_FAILURE); // Exit if writing to file fails
    }
    fflush(file); // Ensure data is written to disk
}

// Function to handle user login by verifying username and password with error handling
char *login(FILE *file)
{
    char username[50], password[50];

    // Input username and password
    printf("Enter Username: ");
    if (fgets(username, sizeof(username), stdin) == NULL)
    {
        perror("Error reading username");
        exit(EXIT_FAILURE); // Exit if reading username fails
    }
    username[strcspn(username, "\n")] = '\0'; // Remove newline character

    printf("Enter Password: ");
    if (fgets(password, sizeof(password), stdin) == NULL)
    {
        perror("Error reading password");
        exit(EXIT_FAILURE); // Exit if reading password fails
    }
    password[strcspn(password, "\n")] = '\0'; // Remove newline character

    char line[150], storedUsername[50], storedPassword[50], userFile[50];
    int usernameFound = 0;

    rewind(file); // Move file pointer to the beginning
    while (fgets(line, sizeof(line), file) != NULL)
    {
        sscanf(line, "%49[^,],%49[^,],%49[^\n]", storedUsername, storedPassword, userFile); // Parse user data

        if (strcmp(username, storedUsername) == 0) // Check if username matches
        { 
            usernameFound = 1;
            break;
        }
    }

    if (usernameFound == 1)
    {
        if (strcmp(storedPassword, password) == 0) // Check if password matches
        {
            printf("Successfully logged in.\n");
            return strdup(userFile); // Return the filename associated with the user
        }
        printf("Wrong password.\n");
        return NULL; // Return NULL if password is incorrect
    }

    printf("Username doesn't exist.\n");
    return NULL; // Return NULL if username doesn't exist
}

// Function to dynamically increase memory space for Expenses array with error handling
void increaseMemoryForExpenses(struct Expense **expenses, int count)
{
    struct Expense *temp = realloc(*expenses, (count + 1) * sizeof(struct Expense)); // Reallocate memory for additional entry
    if (temp == NULL)
    {
        perror("Error reallocating memory for expenses");
        free(*expenses); // Free the previously allocated memory to prevent leaks
        exit(EXIT_FAILURE);  // Exit the program if memory allocation fails
    }
    *expenses = temp; // Update pointer to the new memory location
}

// Function to read expense data from a file and store it in the Expense array with error handling
void extractExpensesFromFile(FILE *file, struct Expense **expenses, int *expenseCount)
{
    char line[300];
    int i = 0;
    while (fgets(line, sizeof(line), file) != NULL) // Read file line by line
    {
        increaseMemoryForExpenses(expenses, i); // Expand space for a new entry

        // Parse and store data in the Expense structure
        if (sscanf(line, "%49[^,],%49[^,],%d,%49[^,],%99[^\n]", (*expenses)[i].category, (*expenses)[i].itemName, &(*expenses)[i].amount, (*expenses)[i].date, (*expenses)[i].description) != 5)
        {
            printf("Error: Invalid expense data format.\n");
            exit(EXIT_FAILURE);  // Exit if the data format is invalid
        }
        i++;
    }
    *expenseCount = i; // Update the total count of expenses
}

// Function to add a new expense entry with error handling
void addExpense(struct Expense **expenses, int *expenseCount)
{
    increaseMemoryForExpenses(expenses, *expenseCount); // Allocate space for a new expense entry

    // Prompt user to enter expense details
    printf("Enter category name: ");
    if (fgets((*expenses)[*expenseCount].category, sizeof((*expenses)[*expenseCount].category), stdin) == NULL)
    {
        perror("Error reading category name");
        exit(EXIT_FAILURE); // Exit if reading category fails
    }
    (*expenses)[*expenseCount].category[strcspn((*expenses)[*expenseCount].category, "\n")] = '\0'; // Remove newline character

    printf("Enter item name: ");
    if (fgets((*expenses)[*expenseCount].itemName, sizeof((*expenses)[*expenseCount].itemName), stdin) == NULL)
    {
        perror("Error reading item name");
        exit(EXIT_FAILURE); // Exit if reading item name fails
    }
    (*expenses)[*expenseCount].itemName[strcspn((*expenses)[*expenseCount].itemName, "\n")] = '\0'; // Remove newline character

    printf("Enter amount: ");
    if (scanf("%d", &(*expenses)[*expenseCount].amount) != 1) // Input expense amount
    {
        printf("Invalid amount entered.\n");
        exit(EXIT_FAILURE); // Exit if the amount is not a valid integer
    }
    getchar(); // Clear newline left by scanf

    printf("Enter date (DD/MM/YYYY): ");
    if (fgets((*expenses)[*expenseCount].date, sizeof((*expenses)[*expenseCount].date), stdin) == NULL)
    {
        perror("Error reading date");
        exit(EXIT_FAILURE); // Exit if reading date fails
    }
    (*expenses)[*expenseCount].date[strcspn((*expenses)[*expenseCount].date, "\n")] = '\0'; // Remove newline character

    printf("Enter description: ");
    if (fgets((*expenses)[*expenseCount].description, sizeof((*expenses)[*expenseCount].description), stdin) == NULL)
    {
        perror("Error reading description");
        exit(EXIT_FAILURE); // Exit if reading description fails
    }
    (*expenses)[*expenseCount].description[strcspn((*expenses)[*expenseCount].description, "\n")] = '\0'; // Remove newline character

    (*expenseCount)++; // Increment the count of expenses
    printf("Expense added successfully.\n");
}

// Function to delete an expense from the array with error handling
void deleteExpense(struct Expense *expenses, int *expenseCount)
{
    int itemNumber;
    printf("Enter item number to delete: ");
    if (scanf("%d", &itemNumber) != 1)
    {
        printf("Invalid input for item number.\n");
        exit(EXIT_FAILURE); // Exit if invalid input for item number
    }

    if (itemNumber < 1 || itemNumber > *expenseCount)
    {
        printf("Error: Invalid item number.\n");
        return; // Exit if invalid item number
    }

    // Shift remaining items to fill the gap
    for (int i = itemNumber - 1; i < *expenseCount - 1; i++)
    {
        expenses[i] = expenses[i + 1];
    }

    (*expenseCount)--; // Decrement the count of items
    printf("Expense item %d deleted successfully.\n", itemNumber);
}

// Function to view all expenses stored in memory
void viewAllExpenses(struct Expense expenses[], int expenseCount)
{
    // Print a table header
    printf("+----------+----------------+---------------+--------+------------+----------------------------+\n");
    printf("| ITEM NUM |    CATEGORY    |    ITEM NAME  | AMOUNT |    DATE    |         DESCRIPTION        |\n");
    printf("+----------+----------------+---------------+--------+------------+----------------------------+\n");

    for (int i = 0; i < expenseCount; i++)
    {
        // Print each expense entry in a formatted way
        printf("|    %-6d| %-14s | %-13s | %-6d | %-10s | %-26s |\n",
               i + 1, expenses[i].category, expenses[i].itemName, expenses[i].amount, expenses[i].date, expenses[i].description);
    }
    printf("+----------+----------------+---------------+--------+------------+----------------------------+\n");
}

// Function to save the updated expenses back to the file with error handling
void saveExpensesToFile(FILE *file, struct Expense *expenses, int expenseCount)
{
    freopen(NULL, "w", file); // Truncate the file before writing
    for (int i = 0; i < expenseCount; i++)
    {
        // Write each expense entry to the file
        if (fprintf(file, "%s,%s,%d,%s,%s\n", expenses[i].category, expenses[i].itemName, expenses[i].amount, expenses[i].date, expenses[i].description) < 0)
        {
            perror("Error writing to file");
            exit(EXIT_FAILURE);  // Exit if writing to file fails
        }
    }
    fflush(file); // Ensure the data is written to disk
}

// Main function to drive the program
int main(void)
{
    FILE *passwordFile;
    const char *accountFileName = "password.csv";
    initializeFile(&passwordFile, accountFileName, "a+"); // Open password file

    int userChoice;
    printf("Press 1 to login and 2 to create a new account: ");
    if (scanf("%d", &userChoice) != 1)
    {
        printf("Invalid input.\n");
        exit(EXIT_FAILURE); // Exit if invalid input
    }
    getchar(); // Clear newline left by scanf

    char *expenseFile = NULL;

    switch (userChoice)
    {
    case 1:
        expenseFile = login(passwordFile); // Attempt login
        if (expenseFile == NULL)
        {
            fclose(passwordFile);
            return 1; // Exit if login fails
        }
        break;

    case 2:
        createNewAccount(passwordFile); // Create a new account
        printf("\nSuccessfully created an account.\n");
        fclose(passwordFile);
        return 0;

    default:
        printf("Wrong choice.\n");
        fclose(passwordFile);
        return 1;
    }

    fclose(passwordFile);

    FILE *userFile;
    initializeFile(&userFile, expenseFile, "a+"); // Open user-specific file
    printf("User file (%s) opened successfully.\n", expenseFile);

    struct Expense *expenses = NULL;
    int expenseCount = 0;

    extractExpensesFromFile(userFile, &expenses, &expenseCount); // Extract existing expenses
    fclose(userFile); // Close user-specific file

    userChoice = 0;

    while (1)
    {
        printf("\nPress 1 to enter expense, 2 to delete expenses, 3 to view expenses, 4 to save and exit: ");
        if (scanf("%d", &userChoice) != 1)
        {
            printf("Invalid input.\n");
            exit(EXIT_FAILURE); // Exit if invalid input
        }
        getchar(); // Clear newline left by scanf

        switch (userChoice)
        {
        case 1:
            addExpense(&expenses, &expenseCount); // Add new expense
            break;
        case 2:
            deleteExpense(expenses, &expenseCount); // Delete expense
            break;
        case 3:
            viewAllExpenses(expenses, expenseCount); // View all expenses
            break;
        case 4:
            initializeFile(&userFile, expenseFile, "w+"); // Reopen file in write mode
            saveExpensesToFile(userFile, expenses, expenseCount);

            free(expenseFile); // Free dynamically allocated memory
            fclose(userFile);  // Close user-specific file

            printf("Changes saved. Exiting program.\n");
            return 0;
        default:
            printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}
