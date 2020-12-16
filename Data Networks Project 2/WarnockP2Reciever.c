#include "netinet/in.h"
#include "sys/socket.h"
#include "stdio.h"
#include "string.h"
#include "time.h"
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#define MAX 100
int hammingDist(char *str1, char *str2)
{
    printf("Str1 :%s and Str 2:%s \n", str1, str2);
    int i = 0, count = 0;
    while (str1[i] != '\0')
    {
        if (str1[i] != str2[i])
            count++;
        i++;
    }
    return count;
}
int main(int argc, char **argv){
    struct sockaddr_in sa;
    struct sockaddr_in cli;
    int sockfd, conntfd;
    socklen_t len;
    int ch;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("error in socket\n");
        exit(0);
    }
    else
        printf("\nSocket opened");
    bzero(&sa, sizeof(sa));
    sa.sin_port = htons(atoi(argv[1]));
    sa.sin_addr.s_addr = htonl(0);
    if (bind(sockfd, (struct sockaddr *)&sa, sizeof(sa)) < 0)
    {
        printf("Error in binding\n");
    }
    else
        printf("\nBinded Successfully\n");
    listen(sockfd, 50);
    len = sizeof(ch);

    char storageArray[100][MAX];
    int numStored = 0;
    for (;;)
    {
        conntfd = accept(sockfd, (struct sockaddr *)&cli, &len);

        char buff[MAX];
        char response[MAX];
        bzero(buff, MAX); //clear buffer

        // read the message from client
        read(conntfd, buff, sizeof(buff));
        if((strncmp(&buff[0], "q", 1)) == 0){
            printf("Got The close Signal");
            exit(0);
        }
        strncpy(storageArray[numStored],buff, MAX);
        numStored++;

        read(conntfd, buff, sizeof(buff));
        strncpy(storageArray[numStored],buff, MAX);
        numStored++;
        if(numStored % 2 == 0){
            int ham;
            ham = hammingDist(storageArray[numStored-2],storageArray[numStored-1]);
            sprintf(response, "The Hamming Distance is %i\n", ham);
            write(conntfd, response, sizeof(response));
        }

    }
}