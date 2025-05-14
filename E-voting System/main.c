#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <stdlib.h>

// Defining the maximum voters and other constants
#define ADMIN_PASSWORD "admin2024"
#define MAX_LOGIN_ATTEMPTS 3
#define MAX_NAME_LENGTH 50
#define MAX_REG_ID_LENGTH 20
#define MAX_EMAIL_LENGTH 50
#define MAX_PASSWORD_LENGTH 20
#define CANDIDATE_FILE "candidate.txt"
#define VOTER_FILE "voter.txt"

// Structure to store the information of a voter
typedef struct {
    char name[MAX_NAME_LENGTH];
    char email[MAX_EMAIL_LENGTH];
    char regID[MAX_REG_ID_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    char rollNumber[9]; // 8 characters + 1 for null terminator
    int dateOfBirth[3]; // [DD, MM, YYYY]
    int hasVoted; // 0: not voted, 1: voted
} Voter;

typedef struct {
    char name[MAX_NAME_LENGTH];
    char candidateID[MAX_REG_ID_LENGTH];
    char partyName[MAX_NAME_LENGTH]; 
    int votes;
} Candidate;

// Global Variables
int maxVoters = 100;
int maxCandidates = 10;
int votingStatus = 0; // 0: Not started, 1: Started, 2: Ended
int candidateCount = 0;
int voterCount = 0; // To keep track of the number of voters
Voter *voters = NULL;
Candidate *candidates = NULL;

// Function prototypes
void displayVoterMenu();
void displayAdminMenu();
void registerVoter();
void displayVoters();
void generateRegistrationID(char name[MAX_NAME_LENGTH], int dateOfBirth[3], char regID[MAX_REG_ID_LENGTH]);
int verifyPassword(const char *password);
void loginVoter();
int verifyAdminPassword();
int validateEmail(char *email);
int containsDotAfterAt(const char *email);
int verifyEmail(char *email);
int isValidDate(int day, int month, int year);
void registerCandidates();
void displayCandidates();
void castVote();
void displayElectionReport();
int findCandidateIndex(char *candidateID);
int validateRollNumber(char *rollNumber);
void getValidDOB(int dateOfBirth[3]);
int isValidName(const char *name);
void saveDataToFile();
void loadDataFromFile();
char* generateCandidateID();
void startVoting();
void endVoting();
void announceWinner();
void reallocVoters();
void initializeArrays();
void reallocCandidates();
void cleanup();

// Function implementations
void cleanup() {
    free(voters);
    free(candidates);
}

void initializeArrays() {
    voters = (Voter*)malloc(maxVoters * sizeof(Voter));
    if (voters == NULL) {
        printf("Memory allocation for voters failed.\n");
        exit(1);
    }
    candidates = (Candidate*)malloc(maxCandidates * sizeof(Candidate));
    if (candidates == NULL) {
        printf("Memory allocation for candidates failed.\n");
        exit(1);
    }
}

void reallocVoters() {
    maxVoters *= 2;
    voters = (Voter*)realloc(voters, maxVoters * sizeof(Voter));
    if (voters == NULL) {
        printf("Memory reallocation for voters failed.\n");
        exit(1);
    }
}

void reallocCandidates() {
    maxCandidates *= 2;
    candidates = (Candidate*)realloc(candidates, maxCandidates * sizeof(Candidate));
    if (candidates == NULL) {
        printf("Memory reallocation for candidates failed.\n");
        exit(1);
    }
}

// Function to check if the name contains only letters
int isValidName(const char *name) {
    for (int i = 0; name[i] != '\0'; i++) {
        if (!isalpha(name[i]) && name[i] != ' ') { // Allow spaces for multi-word names
            return 0; // Invalid character found
        }
    }
    return 1; // Valid name
}

// Function to get a valid date of birth in the format DD-MM-YYYY
void getValidDOB(int dateOfBirth[3]) {
    int valid = 0;
    while (!valid) {
        printf("Enter your date of birth (DD-MM-YYYY): \n");
        char input[11];
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;

        // Check if the input matches the expected format
        if (sscanf(input, "%d-%d-%d", &dateOfBirth[0], &dateOfBirth[1], &dateOfBirth[2]) == 3) {
            // Ensure the date is valid
            if (isValidDate(dateOfBirth[0], dateOfBirth[1], dateOfBirth[2])) {
                valid = 1; // Valid date
            } else {
                printf("Invalid date of birth.\n");
            }
        } else {
            printf("Invalid format. Please enter date of birth in the format (DD-MM-YYYY).\n");
        }
    }
}

// Function to validate roll number format
int validateRollNumber(char *rollNumber) {
    // Check if number is exactly 8 characters long
    if (strlen(rollNumber) != 8) {
        return 0; // Invalid length
    }
    // Check if first two characters are digits
    for (int i = 0; i < 2; i++) {
        if (!isdigit(rollNumber[i])) {
            return 0; // Not a digit
        }
    }
    if (rollNumber[2] != 'K' && rollNumber[2] != 'k') {
        return 0; // Not 'K' or 'k'
    }
    if (rollNumber[3] != '-') {
        return 0; // '-' not found
    }
    
    // Check if last four characters are digits
    for (int i = 4; i < 8; i++) {
        if (!isdigit(rollNumber[i])) {
            return 0; // Not a digit
        }
    }
    
    return 1; // Valid roll number
}

// Function to register voters
void registerVoter() {
    // Check if voting has started or ended:
    if (votingStatus == 0) { // Voting not started
        printf("Voting has not started yet. You cannot register to vote.\n");
        return; // Exit if voting has not started
    }
    if (votingStatus == 2) { // Voting ended
        printf("Voting has already ended. You cannot register to vote.\n");
        return; // Exit if voting has ended
    }

    // If voting has started, proceed with registration:
    Voter newVoter;
    char roll_no[9];

    // Get the current year
    time_t now;
    struct tm *local;
    time(&now);
    local = localtime(&now);
    int currentYear = local->tm_year + 1900;

    // Prompt the user to enter the name
    printf("Enter your name: \n");
    fgets(newVoter.name, sizeof(newVoter.name), stdin);
    newVoter.name[strcspn(newVoter.name, "\n")] = 0; // Remove newline character

    // Validate the name
    if (!isValidName(newVoter.name)) {
        printf("Invalid name. Please use letters only.\n");
        return; // Exit the function if the name is invalid
    }

    getValidDOB(newVoter.dateOfBirth);
    
    printf("Enter your roll number:\n");
    fgets(roll_no, sizeof(roll_no), stdin);
    roll_no[strcspn(roll_no, "\n")] = 0; // Clear the newline character
    if (!validateRollNumber(roll_no)) {
        printf("Invalid roll number format.\n");
        return; // Exit if roll number is invalid
    }

    // Condition to check the age
    int age = currentYear - newVoter.dateOfBirth[2];
    if (age < 18) {
        printf("You are under-age. You are not eligible to cast vote.\n");
        return;
    } else {
        printf("You are eligible to cast vote.\n");
    }

    // Email validation
    int validEmail;
    do {
        printf("Enter your email: \n");
        getchar();
        fgets(newVoter.email, sizeof(newVoter.email), stdin);
        newVoter.email[strcspn(newVoter.email, "\n")] = 0; // Remove newline character
        validEmail = verifyEmail(newVoter.email); // Check if the email is valid
    } while (!validEmail); // Repeat until a valid email is entered

    // Password validation
    int validPassword;
    do {
        printf("Enter a strong password (at least 8 characters): \n");
        fgets(newVoter.password, sizeof(newVoter.password), stdin);
        newVoter.password[strcspn(newVoter.password, "\n")] = '\0';
        validPassword = verifyPassword(newVoter.password);
    } while (!validPassword);
    
    generateRegistrationID(newVoter.name, newVoter.dateOfBirth, newVoter.regID); // Generate a registration ID
    
    // Realloc memory if necessary
    if (voterCount >= maxVoters) {
        reallocVoters();
    }
    
    voters[voterCount++] = newVoter; // Add new voter to the list
    printf("Registration Successful!\nYour registration ID is: %s\n", newVoter.regID);

    // Save voter data to file
    saveDataToFile();

    // Ask user what they want to do next
    int choice;
    printf("Select one of these options:\n1. Login/Cast vote\n2. Return to main menu\n");
    scanf("%d", &choice);
    getchar(); // Clear newline from input buffer
    switch(choice) {
        case 1:
            loginVoter(); // Logic for voter login or casting vote
            break;
        case 2:
            printf("Returning to main menu...\n"); // Logic to return to main menu
            break;
        default:
            printf("Invalid choice. Returning to main menu.\n");
            break;
    }
}
// Function to login a voter
void loginVoter() {
    char regID[MAX_REG_ID_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    int found = 0;

    printf("Enter your registration ID: ");
    fgets(regID, sizeof(regID), stdin);
    regID[strcspn(regID, "\n")] = 0; // Remove newline character

    for (int i = 0; i < voterCount; i++) {
        if (strcmp(voters[i].regID, regID) == 0) {
            found = 1;
            printf("Enter your password: "); 
            fgets(password, sizeof(password), stdin);
            password[strcspn(password, "\n")] = 0; // Remove newline character

            if (strcmp(voters[i].password, password) == 0) {
                printf("Login successful! Welcome, %s.\n", voters[i].name);
                castVote(); // Proceed to allow the voter to cast their vote or view candidates
            } else {
                printf("Incorrect password. Please try again.\n");
            }
            break;
        }
    }

    if (!found) {
        printf("Registration ID not found. Please register first.\n");
    }
}

// Function to generate a random candidate ID
char* generateCandidateID() {
    static char candidateID[MAX_REG_ID_LENGTH];
    int randomID;
    int unique;
    do {
        unique = 1;
        randomID = rand() % 10000; // Generate a random number between 0 and 9999
        snprintf(candidateID, sizeof(candidateID), "C%d", randomID); // Format as "CXXXX"
        
        // Check for uniqueness
        for (int i = 0; i < candidateCount; i++) {
            if (strcmp(candidates[i].candidateID, candidateID) == 0) {
                unique = 0; // Not unique
                break;
            }
        }
    } while (!unique);
    return candidateID;
}

// Function to register candidates
void registerCandidates() {
    if (candidateCount >= maxCandidates) {
        printf("Maximum candidate limit reached.\n");
        return;
    }

    Candidate newCandidate;
    
    // Validate Candidate Name
    while (1) {
        printf("Enter candidate's full name: ");
        fgets(newCandidate.name, sizeof(newCandidate.name), stdin);
        newCandidate.name[strcspn(newCandidate.name, "\n")] = 0;

        // Check if name is valid
        if (!isValidName(newCandidate.name)) {
            printf("Invalid name. Please use only alphabets and spaces.\n");
            continue;
        }
        break;
    }

    // Validate Party Name
    while (1) {
        printf("Enter candidate's party name: ");
        fgets(newCandidate.partyName, sizeof(newCandidate.partyName), stdin);
        newCandidate.partyName[strcspn(newCandidate.partyName, "\n")] = 0;

        // Check if party name is valid
        if (!isValidName(newCandidate.partyName)) {
            printf("Invalid party name. Please use only alphabets and spaces.\n");
            continue;
        }

        // Optional: Check for minimum party name length
        if (strlen(newCandidate.partyName) < 3) {
            printf("Party name must be at least 3 characters long.\n");
            continue;
        }
        break;
    }

    // Age Validation
    int candidateAge;
    while (1) {
        int birthYear;
        printf("Enter candidate's birth year: ");
        if (scanf("%d", &birthYear) != 1) {
            printf("Invalid input. Please enter a valid year.\n");
            while(getchar() !=  '\n'); // Clear input buffer
            continue;
        }

        // Get current year
        time_t now;
        struct tm *local;
        time(&now);
        local = localtime(&now);
        int currentYear = local->tm_year + 1900;

        // Calculate age
        candidateAge = currentYear - birthYear;

        // Check age eligibility
        if (candidateAge < 25) {
            printf("Candidate must be at least 25 years old. Current age: %d\n", candidateAge);
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }

        if (candidateAge > 75) {
            printf("Candidate cannot be older than 75 years. Current age: %d\n", candidateAge);
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }

        break;
    }
    while (getchar() != '\n'); // Clear input buffer after age input

    // Generate a unique candidate ID
    strcpy(newCandidate.candidateID, generateCandidateID());

    // Initialize votes to 0
    newCandidate.votes = 0; 
    
    // Add the new candidate to the array
    candidates[candidateCount++] = newCandidate;
    
    printf("Candidate registered successfully!\n");
    printf("Candidate Name: %s\n", newCandidate.name);
    printf("Party Name: %s\n", newCandidate.partyName);
    printf("Candidate ID: %s\n", newCandidate.candidateID);
    printf("Candidate Age: %d\n", candidateAge);

    // Save candidate data to file
    saveDataToFile();

    // Ask if the user wants to register another candidate
    char choice;
    printf("Do you want to register another candidate? (y/n): ");
    scanf(" %c", &choice);
    getchar(); // Clear newline from input buffer

    if (choice == 'y' || choice == 'Y') {
        registerCandidates();
    }
}

// Function to save data to a file
void saveDataToFile() {
    // Save Candidates
    FILE *candidateFile = fopen(CANDIDATE_FILE, "w");
    if (candidateFile == NULL) {
        perror("Error opening candidate file for writing");
        return;
    }

    fprintf(candidateFile, "%d\n", candidateCount);
    for (int i = 0; i < candidateCount; i++) {
        if (fprintf(candidateFile, "%s,%s,%s,%d\n",
                    candidates[i].name,
                    candidates[i].partyName,
                    candidates[i].candidateID,
                    candidates[i].votes) < 0) {
            printf("Error writing candidate data to file.\n");
        }
    }
    fclose(candidateFile);

    // Save Voters
    FILE *voterFile = fopen(VOTER_FILE, "w");
    if (voterFile == NULL) {
        perror("Error opening voter file for writing");
        return;
    }

    fprintf(voterFile, "%d\n", voterCount);
    for (int i = 0; i < voterCount; i++) {
        if (fprintf(voterFile, "%s,%s,%s,%s,%s,%d-%d-%d,%d\n",
                    voters[i].name,
                    voters[i].email,
                    voters[i].regID,
                    voters[i].password,
                    voters[i].rollNumber,
                    voters[i].dateOfBirth[0],
                    voters[i].dateOfBirth[1],
                    voters[i].dateOfBirth[2],
                    voters[i].hasVoted) < 0) {
            printf("Error writing voter data to file.\n");
        }
    }
    fclose(voterFile);
}

// Function to load data from a file
void loadDataFromFile() {
    // Load Candidates
    FILE *candidateFile = fopen(CANDIDATE_FILE, "r");
    if (candidateFile != NULL) {
        // Read total number of candidates
        fscanf(candidateFile, "%d\n", &candidateCount);

        // Limit candidateCount to maximum allowed
        if (candidateCount > maxCandidates) {
            candidateCount = maxCandidates;
        }

        // Read candidate details including party name
        for (int i = 0; i < candidateCount; i++) {
            fscanf(candidateFile, "%[^,],%[^,],%[^,],%d\n",
                   candidates[i].name,
                   candidates[i].partyName, // Read party name
                   candidates[i].candidateID,
                   &candidates[i].votes);
        }
        fclose(candidateFile);
    }

    // Load Voters
    FILE *voterFile = fopen(VOTER_FILE, "r");
    if (voterFile !=NULL) {
        // Read total number of voters
        fscanf(voterFile, "%d\n", &voterCount);

        // Limit voterCount to maximum allowed
        if (voterCount > maxVoters) {
            voterCount = maxVoters;
        }

        // Read voter details
        for (int i = 0; i < voterCount; i++) {
            fscanf(voterFile, "%[^,],%[^,],%[^,],%[^,],%[^,],%d-%d-%d,%d\n",
                   voters[i].name,
                   voters[i].email,
                   voters[i].regID,
                   voters[i].password,
                   voters[i].rollNumber,
                   &voters[i].dateOfBirth[0],
                   &voters[i].dateOfBirth[1],
                   &voters[i].dateOfBirth[2],
                   &voters[i].hasVoted);
        }
        fclose(voterFile);
    }
}

// Function to display registered candidates
void displayCandidates() {
    if (candidateCount == 0) {
        printf("No candidates registered yet.\n");
        return;
    }

    printf("\t\t\t\t ---====Registered Candidates====---\n");
    for (int i = 0; i < candidateCount; i++) {
        printf("Candidate Name: %s\n", candidates[i].name);
        printf("Party Name: %s\n", candidates[i].partyName);
        printf("Candidate ID: %s\n", candidates[i].candidateID);
        printf("Votes: %d\n\n", candidates[i].votes);
    }

    // Ask if the user wants to return to the main menu
    char choice;
    printf("Do you want to return to the main menu? (y/n): ");
    scanf(" %c", &choice);
    
    getchar(); // Clear newline from input buffer

    if (choice == 'y' || choice == 'Y') {
        printf("Returning to main menu...\n");
    }
}

// Function to cast a vote
void castVote() {
    if (votingStatus != 1) {
        printf("Voting is not currently active.\n");
        return;
    }

    char voterID[MAX_REG_ID_LENGTH];
    printf("Enter your voter registration ID: ");
    fgets(voterID, sizeof(voterID), stdin);
    voterID[strcspn(voterID, "\n")] = 0;

    for (int i = 0; i < voterCount; i++) {
        if (strcmp(voters[i].regID, voterID) == 0) {
            if (voters[i].hasVoted) {
                printf("You have already cast your vote.\n");
                return;
            }

            printf("\n---=== Available Candidates ===---\n");
            for (int j = 0; j < candidateCount; j++) {
                printf("%d. Name: %s\n   Candidate ID: %s\n", 
                       j + 1, 
                       candidates[j].name, 
                       candidates[j].candidateID);
            }

            char candidateID[MAX_REG_ID_LENGTH];
            printf("\nEnter the candidate ID to vote for: ");
            fgets(candidateID, sizeof(candidateID), stdin);
            candidateID[strcspn(candidateID, "\n")] = 0;

            int candidateIndex = findCandidateIndex(candidateID);
            if (candidateIndex == -1) {
                printf("Invalid candidate ID. Please try again.\n");
                return;
            }

            printf("\nConfirm your vote for:\n");
            printf("Candidate Name: %s\n", candidates[candidateIndex].name);
            printf("Candidate ID: %s\n", candidates[candidateIndex].candidateID);
            
            char confirm;
            printf("Are you sure you want to vote for this candidate? (y/n): ");
            scanf(" %c", &confirm);
            getchar(); // Clear input buffer

            if (confirm == 'y' || confirm == 'Y') {
                candidates[candidateIndex].votes++;
                voters[i].hasVoted = 1;
                printf("Vote cast successfully.\n");
                saveDataToFile();
            } else {
                printf("Vote cancelled.\n");
            }
            return;
        }
    }
    printf("Voter not found. Please register first.\n");
}

// Function to find candidate index
int findCandidateIndex(char *candidateID) {
    for (int i = 0; i < candidateCount; i++) {
        if (strcmp(candidates[i].candidateID, candidateID) == 0) {
            return i;
        }
    }
    return -1;
}

// Function to display election report
void displayElectionReport() {
    if (candidateCount == 0) {
                printf("No candidates registered yet.\n");
        return;
    }

    printf("\t\t\t\t ---====Election Results====---\n");
    for (int i = 0; i < candidateCount; i++) {
        printf("Candidate Name: %s\n", candidates[i].name);
        printf("Candidate ID: %s\n", candidates[i].candidateID);
        printf("Votes: %d\n", candidates[i].votes);
    }
}

// Function to validate email
int validateEmail(char *email) {
    if (strlen(email) == 0) {
        return 0;
    }
    char *atPosition = strchr(email, '@');
    if (atPosition == NULL || strchr(atPosition + 1, '@') != NULL) {
        return 0;
    }
    if (!containsDotAfterAt(email)) {
        return 0;
    }
    return 1;
}

// Function to validate correct email format
int containsDotAfterAt(const char *email) {
    char *atPosition = strchr(email, '@');
    return atPosition != NULL && strchr(atPosition + 1, '.') != NULL;
}

// Function to verify email format
int verifyEmail(char *email) {
    // Check basic format:
    if (!validateEmail(email)) {
        printf("Invalid email format.\n");
        return 0; // Invalid format
    }
    
    // Check for uniqueness:
    for (int i = 0; i < voterCount; i++) {
        if (strcmp(voters[i].email, email) == 0) {
            printf("This email is already registered. Please use another email.\n");
            return 0; // Email already in use
        }
    }
    return 1; // Email is valid and unique
}

// Function to verify if the date is valid
int isValidDate(int day, int month, int year) {
    if (year < 1900 || year > 2100) {
        return 0;
    }
    
    if (month < 1 || month > 12) {
        return 0; // Invalid month
    }
    
    // Days in each month
    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    // Check for leap year
    if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) {
        daysInMonth[1] = 29;
    }
    
    // Check if the day is valid for the given month
    if (day < 1 || day > daysInMonth[month - 1]) {
        return 0; // Invalid day
    }
    return 1; // Valid date
}

// Function to verify admin password
int verifyAdminPassword() {
    char adminPassword[MAX_PASSWORD_LENGTH];
    int attempts = MAX_LOGIN_ATTEMPTS;
    
    while (attempts > 0) {
        printf("Enter the password (You have only %d attempts): ", attempts);
        fgets(adminPassword, sizeof(adminPassword), stdin);
        adminPassword[strcspn(adminPassword, "\n")] = 0;
        
        if (strcmp(adminPassword, ADMIN_PASSWORD) == 0) {
            printf("You have been logged in successfully.\n");
            return 1; // Successful login.
        }
        
        attempts--;
        if (attempts > 0) {
            printf("Incorrect password.\n");
        } else {
            printf("Maximum login attempts exceeded. System locked.\n");
            return 0; // Failed login
        }
    }
    return 0; // In case of unexpected exit
}

// Function to display voter menu
void displayVoterMenu() {
    printf("\n============================\n");
    printf("1. Register yourself\n");
    printf("2. Login (if already registered)\n");
    printf("3. View Candidates\n");
    printf("4. Cast a vote\n");
    printf("5. Return to main menu\n");
    printf("============================\n");
}

// Function to display admin menu
void displayAdminMenu() {
    printf("\n============================\n");
    printf("1. Register Candidates\n");
    printf("2. View Candidates\n");
    printf("3. View Election Report\n");
    printf("4. View Voters\n");
    printf("5. Start Voting\n");
    printf("6. End Voting\n");
    printf("7. Announce Winner\n");
    printf("8. Return to main menu\n");
    printf("============================\n");
}

// Function to generate registration ID
void generateRegistrationID(char name[MAX_NAME_LENGTH], int dateOfBirth[3], char regID[MAX_REG_ID_LENGTH]) {
    int j = 0;
    char initialsOfName[3] = {'\0', '\0', '\0'};
    for (int i = 0; name[i] != '\0' && j < 2; i++) {
        if (i == 0 || name[i - 1] == ' ') {
            initialsOfName[j++] = name[i];
        }
    }
    snprintf(regID, MAX_REG_ID_LENGTH, "%c%c%02d%02d%04d", initialsOfName[0], initialsOfName[1], dateOfBirth[0], dateOfBirth[1], dateOfBirth[2]);
}

// Function to display registered voters
void displayVoters() {
    if (voterCount == 0) {
        printf("No voters registered yet.\n");
        return;
    }
    printf("---===Registered Voters===---\n");
    for (int i = 0; i < voterCount; i++) {
        printf("Name: %s\nDate of Birth: %02d-%02d-%04d\nVoter ID: %s\nRoll Number: %s\n",
               voters[i].name,
               voters[i].dateOfBirth[0],
               voters[i].dateOfBirth[1],
               voters[i].dateOfBirth[2],
               voters[i].regID,
               voters[i].rollNumber);
    }
}

// Function to verify password
int verifyPassword(const char *password) {
    int lowerCount = 0, upperCount = 0, specialCharacter = 0, isDigit = 0, repeatedCount = 0;
    int length = strlen(password);

    if (length < 8) {
        printf("Password must contain at least 8 characters.\n");
        return 0;
    }

    for (int i = 0; i < length; i++) {
        if (isupper(password[i])) {
            upperCount = 1;
        }
        if (islower(password[i])) {
            lowerCount = 1;
        }
        if (ispunct(password[i])) {
            specialCharacter = 1;
        }
        if (isdigit(password[i])) {
            isDigit = 1;
        }
        if (i > 0 && password[i] == password[i - 1]) {
            repeatedCount++;
        }
    }
    if (!lowerCount) {
        printf("It must contain at least one lower case alphabet.\n");
    }
    if (!upperCount) {
        printf("It must contain at least one upper case alphabet.\n");
    }
    if (!specialCharacter) {
        printf("It must contain at least one special character.\n");
    }
    if (repeatedCount > 3) {
        printf("It is containing repeated characters.\n");
    }
    if (lowerCount && upperCount && specialCharacter && isDigit) {
        printf("Password has been saved successfully.\n");
        return 1;
    } else {
        printf("Password is too simple.\n");
        return 0;
    }
}

// Function to start the voting process
void startVoting() {
    if (votingStatus == 0) {
        votingStatus = 1;
        printf("Voting process has started.\n");
    } else if (votingStatus == 1) {
        printf("Voting is already in progress.\n");
    } else {
        printf("Voting has already ended. You cannot start it again.\n");
    }
}

// Function to end the voting process
void endVoting() {
    if (votingStatus == 1) {
        votingStatus = 2;
        printf("Voting process has ended.\n");
    } else if (votingStatus == 0) {
        printf("Voting has not started yet. You cannot end it.\n");
    } else {
        printf("Voting is already ended.\n");
    }
}

// Function to announce the winner
void announceWinner() {
    if (votingStatus != 2) {
        printf("Voting must be ended before announcing the winner.\n");
        return;
    }

    int maxVotes = 0, winner = -1;
    for (int i = 0; i < candidateCount; i++) {
        if (candidates[i].votes > maxVotes) {
            maxVotes = candidates[i].votes;
            winner = i;
        }
    }

    if (winner == -1) {
        printf("No votes were cast. No winner can be determined.\n");
    } else {
        printf("Candidate %s is the winner with %d votes!\n", candidates[winner].name, maxVotes);
    }
}

// Main function to start the program
int main() {
    initializeArrays();
     srand(time(NULL)); // Seed random number generator
    loadDataFromFile(); // Load existing data from file

    int choice, flag = 1; // Choice as a voter or admin
    int voterChoice; // The choice of a voter
    int adminChoice; // The choice of an admin 
    
    printf("\t\t\t\t\t ---====E-Voting System====---\n");
    while (1) {
        printf("Kindly enter your choice: \n");
        printf("1. Voter\n");
        printf("2. Admin\n");
        printf("3. To exit the program.\n");
        scanf("%d", &choice);
        getchar(); // Clear newline from input buffer

        // Switch case for voter and admin
        switch (choice) {
            case 1: {
                do {
                    displayVoterMenu();
                    scanf("%d", &voterChoice);
                    getchar(); // Clear newline from input buffer
                    // Nested switch for voter choices
                    switch (voterChoice) {
                        case 1:
                            registerVoter();
                            break;
                        case 2:
                            loginVoter();
                            break;
                        case 3:
                            displayCandidates();
                            break;
                        case 4:
                            castVote();
                            break;
                        case 5:
                            printf("Returning to main menu.....\n");
                            flag = 0; // Exit the loop
                            break;
                        default:
                            printf("Invalid Choice. Please try again.\n");
                            // Do not set flag to 0, so it will continue to prompt for valid input
                            break;
                    }
                } while (voterChoice < 1 || voterChoice > 5 || flag != 0);
                break;
            }
            case 2: {
                if (verifyAdminPassword() == 0) {
                    // If login failed, return to main menu
                    break; // Exit the case for admin
                }
                // If login successful, show admin menu:
                do {
                    displayAdminMenu();
                    scanf("%d", &adminChoice);
                    getchar(); // Clear newline from input buffer
                    
                    // Nested switch for admin choices
                    switch (adminChoice) {
                        case 1:
                            registerCandidates();
                            break;
                        case 2:
                            displayCandidates();
                            break;
                        case 3:
                            displayElectionReport();
                            break;
                        case 4:
                            displayVoters();
                            break;
                        case 5:
                            startVoting();
                            break;
                        case 6:
                            endVoting();
                            break;
                        case 7:
                            announceWinner(); // Adjusted to call without parameters
                            break;
                        case 8:
                            printf("Returning to main menu.....\n");
                            break;
                        default:
                            printf("Invalid Choice. Please try again.\n");
                            // Do not set flag to 0, so it will continue to prompt for valid input
                            break;
                    }
                } while (adminChoice < 1 || adminChoice > 8);
                break;
            }
            case 3:
                printf("Exiting the system. Thank You !!\n");
                cleanup();
                return 0;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
    cleanup();
    return 0;
}
