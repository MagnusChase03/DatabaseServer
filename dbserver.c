#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>

#include "msg.h"

// Struct for thread
struct clientStruct
{

    int clientfd;
    struct sockaddr_in clientInfo;
};

void setOpenIndex(int databasefd);
struct record findID(int databasefd, uint32_t id);
void startServer(int port);
void *manageClient(void *args);

int main(int argc, char **argv)
{

    if (argc < 2)
    {

        printf("Usage: ./dbserver [PORT]");
        return 1;
    }

    startServer(atoi(argv[1]));

    return 0;
}

// Finds the next open spot in database to write to
void setOpenIndex(int databasefd)
{

    lseek(databasefd, 0, SEEK_SET);
    struct record buffer;

    while (read(databasefd, &buffer, sizeof(buffer)) > 0)
    {

        if (buffer.id == 0)
        {

            return;
        }
    }
}

// Finds record with said ID
struct record findID(int databasefd, uint32_t id)
{

    lseek(databasefd, 0, SEEK_SET);
    struct record buffer;

    while (read(databasefd, &buffer, sizeof(buffer)) > 0)
    {

        if (buffer.id == id)
        {

            return buffer;
        }
    }

    struct record notFound;
    notFound.id = 0;
    return notFound;
}

// Starts listening on port
void startServer(int port)
{

    // Create socket
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd == -1)
    {

        perror("Could not create server socket");
        exit(1);
    }

    // Create socket information
    struct sockaddr_in socketInfo;
    socketInfo.sin_family = AF_INET;
    socketInfo.sin_addr.s_addr = INADDR_ANY;
    socketInfo.sin_port = htons(port);

    // Bind and listen
    if (bind(socketfd, (struct sockaddr *)&socketInfo, sizeof(socketInfo)) == -1)
    {

        perror("Failed to bind socket");
        exit(1);
    }

    if (listen(socketfd, 3) == -1)
    {

        perror("Failed to listen on socket");
        exit(1);
    }

    // Accept connections
    while (1 == 1)
    {

        struct sockaddr_in clientInfo;
        socklen_t clientInfoSize = sizeof(clientInfo);
        int clientfd = accept(socketfd, (struct sockaddr *)&clientInfo, &clientInfoSize);

        struct clientStruct clientInfoStruct;
        clientInfoStruct.clientfd = clientfd;
        clientInfoStruct.clientInfo = clientInfo;

        pthread_t clientThread;
        if (pthread_create(&clientThread, NULL, manageClient, &clientInfoStruct) != 0)
        {

            perror("Error creating thread for client");
            exit(1);
        }
    }

    close(socketfd);
}

// Manage connections with a client
void *manageClient(void *args)
{

    struct clientStruct clientInfoStruct = *((struct clientStruct *)args);

    // Open database
    int databasefd = open("./database.txt", O_RDWR | O_CREAT, S_IRWXU);

    // Handle data from client
    struct msg clientMsg;
    while (read(clientInfoStruct.clientfd, &clientMsg, sizeof(clientMsg)) > 0)
    {

        // Handle put
        if (clientMsg.type == 1)
        {

            setOpenIndex(databasefd);
            write(databasefd, &(clientMsg.rd), sizeof(clientMsg.rd));
        }

        // Handle get
        else if (clientMsg.type == 2)
        {

            struct record found = findID(databasefd, clientMsg.rd.id);
            printf("Found: %s\n", found.name);
        }
    }

    close(clientInfoStruct.clientfd);

    return NULL;
}