// Client side implementation of UDP client-server model
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

typedef struct threadArgs
{
    int sockfd;
    struct sockaddr_in servaddr;

} args;
void readServer(int sockfd, struct sockaddr_in servaddr)
{
    for (;;)
    {
        srand(time(NULL));
        int n, len;
        char buffer[MAX];
        n = recvfrom(sockfd, (char *)buffer, MAX,
                     MSG_WAITALL, (struct sockaddr *)&servaddr,
                     &len);
        buffer[n] = '\0';

        int random = rand() % 10; //only accepts 90% of messages
        if (random != 0)
            printf("Server : %s\n", buffer);
    }
}
void *handleSubscription(void *context)
{
    struct threadArgs *args = context;

    int sockfd = args->sockfd;
    struct sockaddr_in servaddr = args->servaddr;
    char buff[MAX];
    printf("\nWhat do you want to do [JOIN/QUIT]? ");
    do
    {
        int n;
        bzero(buff, sizeof(buff));
        n = 0;
        while ((buff[n++] = getchar()) != '\n')
            ;

        sendto(sockfd, (const char *)buff, strlen(buff) - 1,
               MSG_CONFIRM, (const struct sockaddr *)&servaddr,
               sizeof(servaddr));
        printf("%s Request sent.\n", buff);

    } while (strncmp(buff, "QUIT", strlen("QUIT")) != 0);
}
void setupClient(char *groupName)
{
    char buff[MAX];
    int n;
    bzero(buff, sizeof(buff));
    printf("\nWhat group do you want to join? ");
    n = 0;
    while ((buff[n++] = getchar()) != '\n')
        ;
    strcpy(groupName, buff);
    groupName[strlen(groupName) - 1] = '\0';
}

// Driver code
int main(int argc, char **argv)
{
    int sockfd;
    char buffer[MAX];
    struct sockaddr_in servaddr;
    struct threadArgs args1;
    pthread_t connectionHandler;

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[2]));
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);

    //Setup client server relation here
    char groupName[MAX];
    setupClient(groupName);

    int n, len;

    sendto(sockfd, (const char *)groupName, strlen(groupName),
           MSG_CONFIRM, (const struct sockaddr *)&servaddr,
           sizeof(servaddr));
    printf("Group name sent.\n");

    n = recvfrom(sockfd, (char *)buffer, MAX,
                 MSG_WAITALL, (struct sockaddr *)&servaddr,
                 &len);
    buffer[n] = '\0';
    printf("Server : %s\n", buffer);

    //POPULATE ARGUMENT STRUCT
    args1.sockfd = sockfd;
    args1.servaddr = servaddr;
    //SPIN UP A THREAD TO HANDLE JOINS AND QUITS IN THE BACKGROUND
    pthread_create(&connectionHandler, NULL, handleSubscription, (void *)&args1);

    readServer(sockfd, servaddr);

    close(sockfd);
    return 0;
}