#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

#include "msg.h"

void connectServer(char *hostname, int port);
void startShell(int socketfd);

int main(int argc, char **argv)
{

    if (argc < 3)
    {

        printf("Usage: ./dbserver [HOSTNAME] [PORT]");
        return 1;
    }

    connectServer(argv[1], atoi(argv[2]));

    return 0;
}

// Starts listening on port
void connectServer(char *hostname, int port)
{

    // Create socket
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd == -1)
    {

        perror("Could not create server socket");
        exit(1);
    }

    // Create socket information
    struct addrinfo *res;
    struct addrinfo hints;

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(hostname, NULL, &hints, &res) != 0)
    {

        perror("Could not resolve hostname");
        exit(1);
    }

    struct sockaddr_in socketInfo = *((struct sockaddr_in *)(res->ai_addr));
    socketInfo.sin_port = htons(port);

    // Connect to server
    if (connect(socketfd, (struct sockaddr *)&socketInfo, sizeof(socketInfo)) == -1)
    {

        perror("Failed to connect to socket");
        exit(1);
    }

    // Database Shell
    startShell(socketfd);
}

void startShell(int socketfd)
{

    char *buffer = (char *)malloc(sizeof(char));
    size_t bufferSize = sizeof(buffer);

    while (strcmp(buffer, "exit") != 0)
    {

        printf("Enter Your Choice: ");
        getline(&buffer, &bufferSize, stdin);
        buffer[strlen(buffer) - 1] = '\0';

        if (strcmp(buffer, "1") == 0)
        {

            char *name = (char *)malloc(sizeof(char) * 128);
            size_t nameSize = sizeof(name);

            printf("Enter Name: ");
            getline(&name, &nameSize, stdin);
            name[strlen(name) - 1] = '\0';

            struct msg writeMsg;
            strcpy(writeMsg.rd.name, name);

            write(socketfd, &writeMsg, sizeof(writeMsg));

            free(name);
        }
    }

    free(buffer);
    close(socketfd);
}