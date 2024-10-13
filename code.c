#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to initialize the file for storing usernames and passwords
void Initialize(FILE** filename) {
    // Open the file "password1.csv" in append mode. If it doesn't exist, it will be created.
    *filename = fopen("password.csv", "a+");
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

// Cheak and password and returns file name
char* getInfo(FILE *filename, char name[], char pass[]) {
    char line[150], nameCheck[50], passCheck[50], file[50];
    int flag = 0;
    
    // Read each line from the file
    while (fgets(line, sizeof(line), filename) != NULL) {
        // Extract name, password, and file from the line
        sscanf(line, "%49[^,],%49[^,],%49[^\n]", nameCheck, passCheck, file);
        
        // Compare the input name with the name in the file
        if (strcmp(name, nameCheck) == 0) {
            flag = 1;
            break;
        }
    }

    // Check if the username was found
    if (flag == 1) {
        // Compare the password
        if (strcmp(passCheck, pass) == 0) {
            printf("Successfully logged in\n");
            return strdup(file);  // Return a copy of the file name if login is successful
        }
        printf("Wrong password\n");
        return NULL;
    }

    printf("Username doesn't exist.\n");
    return NULL;
}

int main(void) {
    FILE *password; // Pointer for the file
    Initialize(&password); // Initialize the file
    
	int choice;
    // Prompt user to choose between login and account creation
    printf("Press 1 to login and 2 to create a new account: ");
    scanf("%d", &choice);

    getchar(); // Consume the newline left by scanf

	if (choice == 1) {
		char name[50], pass[50];
        
		printf("Enter name: ");
        fgets(name, 50, stdin);
        name[strcspn(name, "\n")] = '\0';  // Remove trailing newline
        
		printf("Enter password: ");
        fgets(pass, 50, stdin);
        pass[strcspn(pass, "\n")] = '\0';  // Remove trailing newline

        char *file = getInfo(password, name, pass);  // Call getInfo

        if (file == NULL) {
            return 1;  // Exit if login fails
    	}
    }
	else if (choice == 2) {
        createAccount(password);	// Call function to create a new account
        printf("Successfully created an account.\n");
    }
	else {
        printf("Wrong choice\n");
    }
    fclose(password); // Close the file
}
