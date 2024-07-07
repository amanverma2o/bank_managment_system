#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure to store account information
struct Account {
    int accountNumber;
    char username[50];
    float balance;
    int pin;
};

// Function prototypes
void createAccount();
void deposit();
void withdraw();
void checkBalance();
int authenticate(int accountNumber, int pin);

// File pointers for account data and transaction records
FILE *accountsFile;
FILE *transactionsFile;

int main() {
    int choice;
    accountsFile = fopen("accounts.txt", "a+"); // Open accounts file (create if not exist)
    transactionsFile = fopen("transactions.txt", "a+"); // Open transactions file (create if not exist)
    
    if (accountsFile == NULL || transactionsFile == NULL) {
        perror("Error opening file");
        return -1;
    }

    while (1) {
        printf("\n===== Banking System Menu =====\n");
        printf("1. Create Account\n");
        printf("2. Deposit\n");
        printf("3. Withdraw\n");
        printf("4. Check Balance\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                createAccount();
                break;
            case 2:
                deposit();
                break;
            case 3:
                withdraw();
                break;
            case 4:
                checkBalance();
                break;
            case 5:
                fclose(accountsFile);
                fclose(transactionsFile);
                printf("Exiting...\n");
                exit(0);
            default:
                printf("Invalid choice. Please enter a valid option.\n");
                break;
        }
    }

    return 0;
}

void createAccount() {
    struct Account newAccount;
    printf("Enter account number: ");
    scanf("%d", &newAccount.accountNumber);

    // Check if account number already exists
    struct Account tempAccount;
    rewind(accountsFile);
    while (fread(&tempAccount, sizeof(struct Account), 1, accountsFile)) {
        if (tempAccount.accountNumber == newAccount.accountNumber) {
            printf("Account number already exists. Please choose a different number.\n");
            return;
        }
    }

    printf("Enter username: ");
    scanf("%s", newAccount.username);
    printf("Enter initial balance: ");
    scanf("%f", &newAccount.balance);
    printf("Enter PIN (4 digits): ");
    scanf("%d", &newAccount.pin);

    fwrite(&newAccount, sizeof(struct Account), 1, accountsFile);
    printf("Account created successfully.\n");
}

void deposit() {
    int accountNumber, pin;
    float amount;
    printf("Enter account number: ");
    scanf("%d", &accountNumber);
    printf("Enter PIN: ");
    scanf("%d", &pin);

    if (authenticate(accountNumber, pin)) {
        printf("Enter amount to deposit: ");
        scanf("%f", &amount);

        struct Account tempAccount;
        while (fread(&tempAccount, sizeof(struct Account), 1, accountsFile)) {
            if (tempAccount.accountNumber == accountNumber) {
                tempAccount.balance += amount;
                fseek(accountsFile, -sizeof(struct Account), SEEK_CUR);
                fwrite(&tempAccount, sizeof(struct Account), 1, accountsFile);

                // Record transaction
                fprintf(transactionsFile, "Deposit of %.2f into account %d\n", amount, accountNumber);
                printf("Deposit successful. New balance: %.2f\n", tempAccount.balance);
                return;
            }
        }
        printf("Account not found.\n");
    } else {
        printf("Authentication failed. Incorrect account number or PIN.\n");
    }
}

void withdraw() {
    int accountNumber, pin;
    float amount;
    printf("Enter account number: ");
    scanf("%d", &accountNumber);
    printf("Enter PIN: ");
    scanf("%d", &pin);

    if (authenticate(accountNumber, pin)) {
        printf("Enter amount to withdraw: ");
        scanf("%f", &amount);

        struct Account tempAccount;
        while (fread(&tempAccount, sizeof(struct Account), 1, accountsFile)) {
            if (tempAccount.accountNumber == accountNumber) {
                if (tempAccount.balance >= amount) {
                    tempAccount.balance -= amount;
                    fseek(accountsFile, -sizeof(struct Account), SEEK_CUR);
                    fwrite(&tempAccount, sizeof(struct Account), 1, accountsFile);

                    // Record transaction
                    fprintf(transactionsFile, "Withdrawal of %.2f from account %d\n", amount, accountNumber);
                    printf("Withdrawal successful. New balance: %.2f\n", tempAccount.balance);
                    return;
                } else {
                    printf("Insufficient balance.\n");
                    return;
                }
            }
        }
        printf("Account not found.\n");
    } else {
        printf("Authentication failed. Incorrect account number or PIN.\n");
    }
}

void checkBalance() {
    int accountNumber, pin;
    printf("Enter account number: ");
    scanf("%d", &accountNumber);
    printf("Enter PIN: ");
    scanf("%d", &pin);

    if (authenticate(accountNumber, pin)) {
        struct Account tempAccount;
        while (fread(&tempAccount, sizeof(struct Account), 1, accountsFile)) {
            if (tempAccount.accountNumber == accountNumber) {
                printf("Account balance: %.2f\n", tempAccount.balance);
                return;
            }
        }
        printf("Account not found.\n");
    } else {
        printf("Authentication failed. Incorrect account number or PIN.\n");
    }
}

int authenticate(int accountNumber, int pin) {
    struct Account tempAccount;
    rewind(accountsFile);
    while (fread(&tempAccount, sizeof(struct Account), 1, accountsFile)) {
        if (tempAccount.accountNumber == accountNumber && tempAccount.pin == pin) {
            return 1; // Authentication successful
        }
    }
    return 0; // Authentication failed
}
