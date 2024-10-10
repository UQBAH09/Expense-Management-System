#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to initialize the file for storing usernames and passwords
void Initialize(FILE** filename) {
    // Open the file "password1.csv" in append mode. If it doesn't exist, it will be created.
    *filename = fopen("password1.csv", "a+");
    if (*filename == NULL) {
        printf("Error: The file can't be opened.\n");
        exit(1);
    }
}


// Function to check if the username already exists in the file
int checkUsername(FILE* filename, char username[]) {
    rewind(filename); // Move file pointer to top
    char currentUsername[50], line[150];
    // Read each line from the file
    while (fgets(line, sizeof(line), filename) != NULL) {
        sscanf(line, "%[^,]", currentUsername); // Extract the username
        // Compare it with the provided username
        if (strcmp(currentUsername, username) == 0) {
            return 1; // Username exists
        }
    }
    return 0; // Username does not exist
}

// Function to create a new account
void createAccount(FILE* filename) {
    int error = 1;
    char username[50], pass[50];
    while (error) {
        // Input for username and password
        printf("Enter Username: ");
        scanf("%49s", username);
        printf("Enter Password: ");
        scanf("%49s", pass);
        error = checkUsername(filename, username);  // Check if the username already exists
        
		if (error) {
            printf("This username already exists. Please choose another one.\n");
        }
    }
    fprintf(filename, "%s,%s,%s.csv\n", username, pass, username); // Write username and password in the file
}

int main(void) {
    FILE *password; // Pointer for the file
    Initialize(&password); // Initialize the file
    int choice;
    // Prompt user to choose between login and account creation
    printf("Press 1 to login and 2 to create a new account: ");
    scanf("%d", &choice);
    if (choice == 1) {
        printf("Login functionality is not implemented yet.\n");
    } else if (choice == 2) {
        createAccount(password);	// Call function to create a new account
        printf("Successfully created an account.\n");
    } else {
        printf("Wrong choice\n");
    }
    fclose(password); // Close the file
}
