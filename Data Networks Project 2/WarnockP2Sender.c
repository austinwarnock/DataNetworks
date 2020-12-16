#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#define MAX 50
// Function designed for chat between client and server.
void readPortName(int sockfd)
{
    char buff[MAX];
    int n;
    bzero(buff, sizeof(buff));
    printf("\nWhat is your Port Name? ");
    n = 0;
    while ((buff[n++] = getchar()) != '\n')
        ;
    write(sockfd, buff, sizeof(buff));
    bzero(buff, sizeof(buff));
    read(sockfd, buff, sizeof(buff));
    printf("From Server : %s", buff);
}
int readIpAddress(int sockfd)
{
    char buff[MAX];
    int n;
    bzero(buff, sizeof(buff));
    printf("\nWhat is your Port Number? ");
    n = 0;
    while ((buff[n++] = getchar()) != '\n')
        ;
    write(sockfd, buff, sizeof(buff));
    bzero(buff, sizeof(buff));
    read(sockfd, buff, sizeof(buff));
    printf("From Server : %s", buff);
    return atoi(buff);
}
void readUsername(int sockfd) //function to handle username read/write
{
    char buff[MAX];
    int n;
    bzero(buff, sizeof(buff));
    printf("\nWhat is your username? ");
    n = 0;
    while ((buff[n++] = getchar()) != '\n')
        ;
    write(sockfd, buff, sizeof(buff));
    bzero(buff, sizeof(buff));
    read(sockfd, buff, sizeof(buff));
    printf("From Server : %s", buff);
}
int readPassword(int sockfd) // function to handle password read/write
{
    char buff[MAX];
    int n;
    bzero(buff, sizeof(buff));
    printf("What is your password? ");
    n = 0;
    while ((buff[n++] = getchar()) != '\n')
        ;
    write(sockfd, buff, sizeof(buff));
    bzero(buff, sizeof(buff));
    read(sockfd, buff, sizeof(buff));
    printf("From Server : %s", buff);
    return atoi(buff);
}
int readHammingStrings(int sockfd)
{
    char buff[MAX];
    int n;
    bzero(buff, sizeof(buff));
    printf("\nWhat is your first String? ");
    n = 0;
    while ((buff[n++] = getchar()) != '\n')
        ;
    write(sockfd, buff, sizeof(buff));
    if((strncmp(&buff[0], "q", 1)) == 0){
            printf("Got The close Signal");
            exit(0);
        }
    bzero(buff, sizeof(buff));

    printf("\nWhat is your second String? ");
    n = 0;
    while ((buff[n++] = getchar()) != '\n')
        ;
    write(sockfd, buff, sizeof(buff));
    if((strncmp(&buff[0], "q", 1)) == 0){
            printf("Got The close Signal");
            exit(0);
        }
    bzero(buff, sizeof(buff));

    read(sockfd, buff, sizeof(buff));
    printf("From Server : %s", buff);
    return 1;
}
int main(int argc, char **argv)
{
    int clisoc;

    struct sockaddr_in cliaddr;
    bzero(&cliaddr, sizeof(cliaddr));
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons(atoi(argv[2]));
    cliaddr.sin_addr.s_addr = inet_addr(argv[1]);

    if ((clisoc = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("\nSocket Errror");
        exit(0);
    }
    else
    {
        printf("\nSocket opened");
    }
    if ((connect(clisoc, (struct sockaddr *)&cliaddr, sizeof(cliaddr))) < 0)
    {
        perror("\nConnect Error");
        exit(0);
    }
    else
    {
        printf("\nConnected Successfully");
    }

    do
    {
        readUsername(clisoc);
    } while (readPassword(clisoc) < 0);

    //broke out of do while - must have successful pair
    do
    {
        readPortName(clisoc);
    } while (readIpAddress(clisoc) < 0);

    //broke out of do while - must have successful pair

    while(readHammingStrings(clisoc) > 0){
        readHammingStrings(clisoc);
    }
    return 0;
}
