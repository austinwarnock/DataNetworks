// Server side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>

#define MAX 1024
typedef struct Group
{
    char name[MAX];
    int numOfClients;
    int connectedClients;
    int numOfMessages;
} group;

typedef struct threadArgs
{
    struct Group group;
    int sockfd;
    struct sockaddr_in cliaddr;

} args;

struct threadArgs args1;

void *sendMessages()
{
    int sockfd = args1.sockfd;
    struct sockaddr_in cliaddr = args1.cliaddr;
    struct Group group = args1.group;
    
    int len, numSent = 0;
    len = sizeof(cliaddr); //len is value/resuslt

    while (numSent < group.numOfMessages)
    {
        char buff[MAX];
        int n;
        bzero(buff, sizeof(buff));
        printf("\nWhat do you want to broadcast? ");
        n = 0;
        while ((buff[n++] = getchar()) != '\n')
            ;
        n = 0;
        n = sendto(sockfd, (const char *)buff, strlen(buff),
                   MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
                   len);
        numSent++;
    }
}
void *handleClientMessage(void *context)
{
    struct threadArgs *args = context;
    pthread_t readHandler;
    int sockfd = args->sockfd;
    struct sockaddr_in cliaddr = args->cliaddr;
    struct Group group = args->group;
    for (;;)
    {
        printf("GROUP:%s, NumClients%i, ConnectedClients%i\n", group.name, group.numOfClients, group.connectedClients);

        char buff[MAX];
        int len, n;
        len = sizeof(cliaddr); //len is value/resuslt
        n = recvfrom(sockfd, (char *)buff, MAX,
                     MSG_WAITALL, (struct sockaddr *)&cliaddr,
                     &len);
        buff[n] = '\0';
        if (strncmp(buff, "JOIN", sizeof("JOIN")) == 0)
        {
            printf("Attempting to join\n");
        }
        else if (strncmp(buff, "QUIT", sizeof("QUIT")) == 0)
        {
            printf("Attempting to Quit\n");
            group.connectedClients--;
            close(sockfd);
        }
        else if (strncmp(buff, group.name, strlen(group.name)) == 0)
        {
            printf("Attempting to join group\n");
            if (group.connectedClients < group.numOfClients)
            {
                group.connectedClients++;
                char *buff = "Successfully connected";
                sendto(sockfd, (const char *)buff, strlen(buff),
                       MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
                       len);
            }
            else
            {
                //SEND ERROR TO CLIENT TOO MANY CONNECTIONS
                char *buff = "Cannot connect too many clients";
                sendto(sockfd, (const char *)buff, strlen(buff),
                       MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
                       len);
            }
        }
        else
        {
            printf("GROUP DOES NOT EXIST\n");
        }

        //UPDATE ANY CHANGED VALUES
        args1.sockfd = sockfd;
        args1.cliaddr = cliaddr;
        args1.group = group;
        pthread_create(&readHandler, NULL, sendMessages, NULL);
    }
    
}
struct Group serverStartUp()
{
    struct Group group;
    char buff[MAX];
    int n;
    bzero(buff, sizeof(buff));
    printf("\nWhat is name of your group? ");
    n = 0;
    while ((buff[n++] = getchar()) != '\n')
        ;
    memcpy(group.name, buff, sizeof(buff));
    group.name[strlen(group.name) - 1] = '\0';

    // ZERO OUT BUFF AND ASK FOR NUM CLIENTS
    bzero(buff, sizeof(buff));
    printf("\nWhat is the max number of clients? ");
    n = 0;
    while ((buff[n++] = getchar()) != '\n')
        ;
    group.numOfClients = atoi(buff);

    // ZERO OUT BUFF AND ASK FOR NUM OF MESSAGES TO SEND
    bzero(buff, sizeof(buff));
    printf("\nWhat is the number of messages to send? ");
    n = 0;
    while ((buff[n++] = getchar()) != '\n')
        ;
    group.numOfMessages = atoi(buff);
    group.connectedClients = 0;
    return group;
}

// Driver code
int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    struct Group group1;
    pthread_t connectionHandler;

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Filling server information
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(atoi(argv[1]));

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *)&servaddr,
             sizeof(servaddr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    //SETUP SERVER INFO HERE

    group1 = serverStartUp();

    //POPULATE ARGUMENT STRUCT
    args1.sockfd = sockfd;
    args1.cliaddr = cliaddr;
    args1.group = group1;

    //SPIN UP A THREAD TO HANDLE JOINS AND QUITS IN THE BACKGROUND
    pthread_create(&connectionHandler, NULL, handleClientMessage, (void *)&args1);
    pthread_join(connectionHandler, NULL);

    return 0;
}