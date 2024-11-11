#include <netinet/in.h> // for address info
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h> // for socket APIs
#include <sys/types.h>
#include <signal.h> // for signal handling
#include "list.h"

#define PORT 9001
#define ACK "ACK:" // Prefix for server responses

// Global variables for socket cleanup
static int servSockD = -1;
static int clientSocket = -1;
static list_t *mylist = NULL;

// Handles graceful shutdown on Ctrl-C
void handle_sigint(int sig) {
    printf("\nReceived Ctrl-C. Cleaning up...\n");
    
    if (mylist != NULL) {
        list_free(mylist);
        mylist = NULL;
    }
    if (clientSocket != -1) {
        close(clientSocket);
        clientSocket = -1;
    }
    if (servSockD != -1) {
        close(servSockD);
        servSockD = -1;
    }
    
    printf("Server shutdown complete.\n");
    exit(0);
}

int main(int argc, char const* argv[]) {
    int n, val, idx;
    
    // Set up signal handler for Ctrl-C
    signal(SIGINT, handle_sigint);

    // Create server socket
    servSockD = socket(AF_INET, SOCK_STREAM, 0);
    
    char buf[1024];
    char sbuf[1024];
    char* token;
    
    struct sockaddr_in servAddr;
    
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(PORT);
    servAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket to IP and port
    if (bind(servSockD, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    // Start listening for connections
    listen(servSockD, 1);
    printf("Server listening on port %d...\n", PORT);

    // Accept a client connection
    clientSocket = accept(servSockD, NULL, NULL);
    printf("Client connected!\n");

    mylist = list_alloc();  // Create the list

    while(1) {
        // Receive message from client
        n = recv(clientSocket, buf, sizeof(buf), 0);
        buf[n] = '\0';

        if (n > 0) {
            token = strtok(buf, " ");

            // Handle different commands
            if (strcmp(token, "exit") == 0) {  // Exit command
                list_free(mylist);
                mylist = NULL;
                close(clientSocket);
                close(servSockD);
                printf("Server shutting down...\n");
                exit(0);
            }
            else if (strcmp(token, "get_length") == 0) {  // Get length
                val = list_length(mylist);
                sprintf(sbuf, "Length = %d", val);
            }
            else if (strcmp(token, "add_front") == 0) {  // Add to front
                token = strtok(NULL, " ");
                val = atoi(token);
                list_add_to_front(mylist, val);
                sprintf(sbuf, "%s %d added to front", ACK, val);
            }
            else if (strcmp(token, "add_back") == 0) {  // Add to back
                token = strtok(NULL, " ");
                val = atoi(token);
                list_add_to_back(mylist, val);
                sprintf(sbuf, "%s %d added to back", ACK, val);
            }
            else if (strcmp(token, "add_position") == 0) {  // Add at index
                token = strtok(NULL, " ");
                idx = atoi(token);
                token = strtok(NULL, " ");
                val = atoi(token);
                list_add_at_index(mylist, val, idx);
                sprintf(sbuf, "%s %d added at position %d", ACK, val, idx);
            }
            else if (strcmp(token, "remove_front") == 0) {  // Remove front
                val = list_remove_from_front(mylist);
                sprintf(sbuf, "%s %d removed from front", ACK, val);
            }
            else if (strcmp(token, "remove_back") == 0) {  // Remove back
                val = list_remove_from_back(mylist);
                sprintf(sbuf, "%s %d removed from back", ACK, val);
            }
            else if (strcmp(token, "remove_position") == 0) {  // Remove at index
                token = strtok(NULL, " ");
                idx = atoi(token);
                val = list_remove_at_index(mylist, idx);
                sprintf(sbuf, "%s %d removed from position %d", ACK, val, idx);
            }
            else if (strcmp(token, "get") == 0) {  // Get element at index
                token = strtok(NULL, " ");
                idx = atoi(token);
                val = list_get_elem_at(mylist, idx);
                if (val == -1) {
                    sprintf(sbuf, "Error: Invalid index");
                } else {
                    sprintf(sbuf, "Element at %d = %d", idx, val);
                }
            }
            else if (strcmp(token, "print") == 0) {  // Print list
                sprintf(sbuf, "%s", listToString(mylist));
            }
            else {  // Invalid command
                sprintf(sbuf, "Invalid command");
            }

            // Send response to client
            send(clientSocket, sbuf, sizeof(sbuf), 0);
        }
        memset(buf, '\0', 1024);
    }

    return 0;
}
