#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_USERS 100
#define MAX_PENDING_REQUESTS 100

typedef struct Post {
    char content[256];
    time_t timestamp;
} Post;

typedef struct User {
    char name[50];
    int age;
    char email[50];
    char bio[100];
    struct User* friends[MAX_USERS];
    int friendCount;
    struct User* pendingRequests[MAX_PENDING_REQUESTS];
    int pendingCount;
} User;

typedef struct SocialNetwork {
    User* users[MAX_USERS];
    int userCount;
} SocialNetwork;

int isValidEmail(char *email) {
    int atFound = 0;
    int dotFound = 0;
    int i = 0;
    
    while (email[i] != '\0') {
        if (email[i] == '@') {
            atFound = 1;
        }
        if (atFound && email[i] == '.') {
            dotFound = 1;
        }
        i++;
    }
    
    return (atFound && dotFound);
}

void toLowerCase(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
}

User* createUser(char* name, int age, char* email, char* bio) {
    User* user = (User *)malloc(sizeof(User));
    strcpy(user->name, name);
    user->age = age;
    strcpy(user->email, email);
    strcpy(user->bio, bio);
    user->friendCount = 0;
    user->pendingCount = 0;
    return user;
}

void addUser(SocialNetwork* network, char* name, int age, char* email, char* bio) {
    for (int i = 0; i < network->userCount; i++) {
        if (strcmp(network->users[i]->name, name) == 0) {
            printf("User %s already exists in the network. Please choose another username.\n", name);
            return;
        }
    }

    if (network->userCount < MAX_USERS) {
        User* newUser = createUser(name, age, email, bio);
        network->users[network->userCount++] = newUser;
        printf("User %s added to the network. Total users: %d\n", name, network->userCount);
    } else {
        printf("User limit reached!\n");
    }
}

void sendFriendRequest(SocialNetwork* network, char* senderName, char* receiverName) {
    toLowerCase(senderName);
    toLowerCase(receiverName);

    User* sender = NULL;
    User* receiver = NULL;

    for (int i = 0; i < network->userCount; i++) {
        char userNameLower[50];
        strcpy(userNameLower, network->users[i]->name);
        toLowerCase(userNameLower); 

        if (strcmp(userNameLower, senderName) == 0) {
            sender = network->users[i];
        }
        if (strcmp(userNameLower, receiverName) == 0) {
            receiver = network->users[i];
        }
    }

    if (sender && receiver) {
        if (strcmp(senderName, receiverName) == 0) {
            printf("You cannot send a friend request to yourself.\n");
            return;
        }

        for (int j = 0; j < receiver->pendingCount; j++) {
            if (receiver->pendingRequests[j] == sender) {
                printf("Friend request from %s to %s is already pending.\n", senderName, receiverName);
                return;
            }
        }

        for (int j = 0; j < sender->pendingCount; j++) {
            if (sender->pendingRequests[j] == receiver) {
                printf("You already have a friend request from %s.\n", receiverName);
                return;
            }
        }

        receiver->pendingRequests[receiver->pendingCount++] = sender;
        printf("Friend request sent from %s to %s.\n", senderName, receiverName);
    } else {
        printf("One or both users not found in the network.\n");
    }
}

void acceptFriendRequest(SocialNetwork* network, char* receiverName, char* senderName) {
    toLowerCase(receiverName);
    toLowerCase(senderName);

    User* receiver = NULL;
    User* sender = NULL;

    for (int i = 0; i < network->userCount; i++) {
        char userNameLower[50];
        strcpy(userNameLower, network->users[i]->name);
        toLowerCase(userNameLower); 

        if (strcmp(userNameLower, receiverName) == 0) {
            receiver = network->users[i];
        }
        if (strcmp(userNameLower, senderName) == 0) {
            sender = network->users[i];
        }
    }

    if (receiver && sender) {
        for (int j = 0; j < receiver->pendingCount; j++) {
            if (receiver->pendingRequests[j] == sender) {
                receiver->friends[receiver->friendCount++] = sender;
                sender->friends[sender->friendCount++] = receiver;

                for (int k = j; k < receiver->pendingCount - 1; k++) {
                    receiver->pendingRequests[k] = receiver->pendingRequests[k + 1];
                }
                receiver->pendingCount--;

                printf("%s and %s are now friends.\n", senderName, receiverName);
                return;
            }
        }
        printf("No pending friend request from %s to %s.\n", senderName, receiverName);
    } else {
        printf("One or both users not found in the network.\n");
    }
}

void modifyUserDetails(SocialNetwork* network, char* name) {
    toLowerCase(name);

    User* user = NULL;
    for (int i = 0; i < network->userCount; i++) {
        char userNameLower[50];
        strcpy(userNameLower, network->users[i]->name);
        toLowerCase(userNameLower); 

        if (strcmp(userNameLower, name) == 0) {
            user = network->users[i];
            break;
        }
    }

    if (user) {
        int choice;
        printf("\nModify details for user: %s\n", user->name);
        printf("1. Modify Name\n");
        printf("2. Modify Age\n");
        printf("3. Modify Email\n");
        printf("4. Modify Bio\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); 

        switch (choice) {
            case 1:
                {
                    char newName[50];
                    printf("Enter new name: ");
                    fgets(newName, sizeof(newName), stdin);
                    newName[strcspn(newName, "\n")] = 0;  
                    strcpy(user->name, newName);
                    printf("Name updated successfully.\n");
                    break;
                }
            case 2:
                {
                    int newAge;
                    printf("Enter new age: ");
                    scanf("%d", &newAge);
                    user->age = newAge;
                    printf("Age updated successfully.\n");
                    break;
                }
            case 3:
                {
                    char newEmail[50];
                    int validEmail = 0;
                    while (!validEmail) {
                        printf("Enter new email: ");
                        fgets(newEmail, sizeof(newEmail), stdin);
                        newEmail[strcspn(newEmail, "\n")] = 0; 
                        if (isValidEmail(newEmail)) {
                            validEmail = 1;
                            strcpy(user->email, newEmail);
                            printf("Email updated successfully.\n");
                        } else {
                            printf("Invalid email. Please try again.\n");
                        }
                    }
                    break;
                }
            case 4:
                {
                    char newBio[100];
                    printf("Enter new bio: ");
                    fgets(newBio, sizeof(newBio), stdin);
                    newBio[strcspn(newBio, "\n")] = 0;  
                    strcpy(user->bio, newBio);
                    printf("Bio updated successfully.\n");
                    break;
                }
            case 5:
                printf("Exiting modification menu.\n");
                break;
            default:
                printf("Invalid choice! Please try again.\n");
                break;
        }
    } else {
        printf("User %s not found in the network.\n", name);
    }
}

void displayPendingRequests(SocialNetwork* network, char* name) {
    for (int i = 0; i < network->userCount; i++) {
        if (strcmp(network->users[i]->name, name) == 0) {
            User* user = network->users[i];
            printf("Pending friend requests for %s:\n", user->name);
            if (user->pendingCount == 0) {
                printf("No pending requests.\n");
            } else {
                for (int j = 0; j < user->pendingCount; j++) {
                    printf("- %s\n", user->pendingRequests[j]->name);
                }
            }
            return;
        }
    }
    printf("User %s not found in the network.\n", name);
}

void displayAllUsers(SocialNetwork* network) {
    printf("\n+--------------------+-----+---------------------------+----------------------------------+\n");
    printf("| Name               | Age | Email                     | Bio                              |\n");
    printf("+--------------------+-----+---------------------------+----------------------------------+\n");
    for (int i = 0; i < network->userCount; i++) {
        printf("| %-18s | %-3d | %-25s | %-32s |\n", 
               network->users[i]->name, 
               network->users[i]->age, 
               network->users[i]->email , 
               network->users[i]->bio);
    }
    printf("+--------------------+-----+---------------------------+----------------------------------+\n");
}

void displayFriends(SocialNetwork* network, char* name) {
    for (int i = 0; i < network->userCount; i++) {
        if (strcmp(network->users[i]->name, name) == 0) {
            User* user = network->users[i];
            printf("Friends of %s:\n", user->name);
            if (user->friendCount == 0) {
                printf("No friends added yet.\n");
            } else {
                for (int j = 0; j < user->friendCount; j++) {
                    printf("- %s\n", user->friends[j]->name);
                }
            }
            return;
        }
    }
    printf("User %s not found in the network.\n", name);
}

int main() {
    SocialNetwork network;
    network.userCount = 0;

    int choice;
    char name1[50], name2[50];
    int age;
    char email[50], bio[100], postContent[256];

    do {
        printf("\nSocial Media Network Menu:\n");
        printf("1. Add User\n");
        printf("2. Send Friend Request\n");
        printf("3. Accept Friend Request\n");
        printf("4. Display Pending Requests\n");
        printf("5. Display Friends\n");
        printf("6. Display All Users\n");
        printf("7. Modify User Details\n");
        printf("8. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar();  

        switch (choice) {
            case 1:
                a:
                printf("Enter the name of the user to add: ");
                fgets(name1, sizeof(name1), stdin);
                name1[strcspn(name1, "\n")] = 0;  

                for (int i = 0; i < network.userCount; i++) {
                    if (strcmp(network.users[i]->name, name1) == 0) {
                        printf("User name already exists. Please choose another name.\n");
                        goto a;
                    }
                }

                printf("Enter age: ");
                scanf("%d", &age);
                getchar();  

                int validEmail = 0;
                while (!validEmail) {
                    printf("Enter email: ");
                    fgets(email, sizeof(email), stdin);
                    email[strcspn(email, "\n")] = 0;  

                    if (isValidEmail(email)) {
                        validEmail = 1;  
                    } else {
                        printf("Invalid email address. Please enter a valid email (e.g., example@domain.com).\n");
                    }
                }

                printf("Enter bio: ");
                fgets(bio, sizeof(bio), stdin);
                bio[strcspn(bio, "\n")] = 0;  

                addUser(&network, name1, age, email, bio);
                break;

            case 2:
                printf("Enter your name: ");
                fgets(name1, sizeof(name1), stdin);
                name1[strcspn(name1, "\n")] = 0;

                printf("Enter the name of the user to send a friend request to: ");
                fgets(name2, sizeof(name2), stdin);
                name2[strcspn(name2, "\n")] = 0;
                sendFriendRequest(&network, name1, name2);
                break;

            case 3:
                printf("Enter your name: ");
                fgets(name1, sizeof(name1), stdin);
                name1[strcspn(name1, "\n")] = 0;

                printf("Enter the name of the user whose friend request you want to accept: ");
                fgets(name2, sizeof(name2), stdin);
                name2[strcspn(name2, "\n")] = 0;

                acceptFriendRequest(&network, name1, name2);
                break;

            case 4:
                printf("Enter your name to display pending requests: ");
                fgets(name1, sizeof(name1), stdin);
                name1[strcspn(name1, "\n")] = 0;
                displayPendingRequests(&network, name1);
                break;

            case 5:
                printf("Enter the name of the user to display friends: ");
                fgets(name1, sizeof(name1), stdin);
                name1[strcspn(name1, "\n")] = 0;
                displayFriends(&network, name1);
                break;

            case 6:
                displayAllUsers(&network);
                break;

            case 7:
                printf("Enter the name of the user to modify details: ");
                fgets(name1, sizeof(name1), stdin);
                name1[strcspn(name1, "\n")] = 0;
                modifyUserDetails(&network, name1);
                break;

            case 8:
                printf("Exited\n");
                break;

            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 8);

    return 0;
}
