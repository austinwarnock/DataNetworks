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
void readCurency(int sockfd) //function to handle currency read/write
{
    char buff[MAX];
    int n;
    bzero(buff, sizeof(buff));
    printf("\nWhat type of currency do you have? ");
    n = 0;
    while ((buff[n++] = getchar()) != '\n')
        ;
    write(sockfd, buff, sizeof(buff));
    bzero(buff, sizeof(buff));
    read(sockfd, buff, sizeof(buff));
    printf("From Server : %s", buff);
}
void readPassword(int sockfd) // function to handle password read/write
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

    readCurency(clisoc);
    readPassword(clisoc);
    int cl;
    cl = close(clisoc);
    printf("Socket closed with code: %i\n", cl);

    return 0;
}
