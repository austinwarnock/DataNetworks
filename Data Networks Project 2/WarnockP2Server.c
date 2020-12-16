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

char *concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 2);
    strcpy(result, s1);
    strcat(result, " ");
    strcat(result, s2);
    return result;
}
void handleReciever(int senderfd, int recieverfd)
{
    char buff[MAX];
    bzero(buff, MAX); //clear buffer
    int n;
    // read the message from client
    read(senderfd, buff, sizeof(buff));
    while ((n=strncmp(&buff[0], "q", 1)) != 0)
    {
            write(recieverfd, buff, sizeof(buff));
            read(senderfd, buff, sizeof(buff));
            write(recieverfd, buff, sizeof(buff));
            read(recieverfd, buff, sizeof(buff));
            write(senderfd, buff, sizeof(buff));
    }
    //we got the quit code now close
    write(recieverfd, buff, sizeof(buff));
    //write(senderfd, buff, sizeof(buff));
    close(senderfd);
    close(recieverfd);
    printf("CLOSING CONNECTIONS");
}
void openReciever(char *portName, char *ipAddress, int portNumber, int senderfd)
{
    int clisoc;
    struct sockaddr_in cliaddr;
    bzero(&cliaddr, sizeof(cliaddr));
    cliaddr.sin_family = AF_INET;
    printf("%i\n", portNumber);
    cliaddr.sin_port = htons(portNumber);
    cliaddr.sin_addr.s_addr = inet_addr(ipAddress);

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

    for (;;)
    {
        handleReciever(senderfd, clisoc);
    }
}
int passwordVerification(char *username, char *password)
{
    FILE *fp;
    char buff[80];
    char *userAndPass = concat(username, password);
    int fr;
    if ((fp = fopen("userList.txt", "r")) < 0)
    {
        printf("Error Opening userList.txt\n");
        exit(0);
    }
    fseek(fp, 0, SEEK_SET); //make sure we read from the beginning

    if ((fr = fread(&buff, sizeof(char), 80, fp)) < 0)
    {
        printf("Error reading userlist.txt\n");
        exit(0);
    }

    char *array[9];
    int i = 0;

    array[i] = strtok(buff, "\n"); // put header into one array slot

    while (array[i] != NULL)
    {
        array[++i] = strtok(NULL, "\n");
    }

    fflush(stdout);

    for (i = 1; i < 5; i++)
    {
        if (strncmp(array[i], userAndPass, strlen(userAndPass)) == 0) //matching pair found
            return 1;
    }
    return -1;
}
int portVerification(char *portName, char *ipAddress)
{
    FILE *fp;
    char buff[250];
    char *combinedPort = concat(portName, ipAddress);
    int fr;
    if ((fp = fopen("recieverList.txt", "r")) < 0)
    {
        printf("Error Opening recieverList.txt\n");
        exit(0);
    }
    fseek(fp, 0, SEEK_SET); //make sure we read from the beginning

    if ((fr = fread(&buff, sizeof(char), 250, fp)) < 0)
    {
        printf("Error reading recieverlist.txt\n");
        exit(0);
    }

    char *array[9];
    int i = 0;

    array[i] = strtok(buff, "\n"); // put header into one array slot

    while (array[i] != NULL)
    {
        array[++i] = strtok(NULL, "\n");
    }

    fflush(stdout);

    for (i = 1; i < 7; i++)
    {
        if (strncmp(array[i], combinedPort, strlen(combinedPort)) == 0) //matching pair found
            return 1;
    }
    return -1;
}
int handleUsername(int sockfd, char *username)
{
    char buff[MAX];
    int n = 0;
    bzero(buff, MAX); //clear buffer

    // read the message from client
    read(sockfd, buff, sizeof(buff));

    strcpy(username, buff);                // copy over the results
    username[strlen(username) - 1] = '\0'; // removes newline for comparison later
    bzero(buff, MAX);                      //clear buffer

    // copy server message in the buffer
    char successMessage[MAX] = "Successfully Recieved Username\n";
    memcpy(&buff[0], successMessage, strlen(successMessage));
    while ((buff[n++]) != '\n')
        ;
    // and send that buffer to client and zero it out
    write(sockfd, buff, sizeof(buff));
    return 1;
}
int handlePassword(int sockfd, char *username, char *password)
{
    char buff[MAX];
    int n = 0;
    double isValid = -6;
    bzero(buff, MAX); //clear buffer
    // read the message from client
    read(sockfd, buff, sizeof(buff));

    strcpy(password, buff);
    password[strlen(password) - 1] = '\0'; // removes newline for comparison later
    bzero(buff, MAX);                      //clear buffer

    isValid = passwordVerification(username, password);
    // copy server message in the buffer
    char successMessage[MAX] = "-1\n";
    if (isValid > 0)
    {
        strcpy(successMessage, "Successful Username Password Combo\n");
    }
    memcpy(&buff[0], successMessage, strlen(successMessage));
    while ((buff[n++]) != '\n')
        ;
    // and send that buffer to client and zero it out
    write(sockfd, buff, sizeof(buff));
    return isValid;
}
int handlePortName(int sockfd, char *portName)
{
    char buff[MAX];
    int n = 0;
    bzero(buff, MAX); //clear buffer

    // read the message from client
    read(sockfd, buff, sizeof(buff));

    strcpy(portName, buff);                // copy over the results
    portName[strlen(portName) - 1] = '\0'; // removes newline for comparison later
    bzero(buff, MAX);                      //clear buffer

    // copy server message in the buffer
    char successMessage[MAX] = "Successfully Recieved Port Name\n";
    memcpy(&buff[0], successMessage, strlen(successMessage));
    while ((buff[n++]) != '\n')
        ;
    // and send that buffer to client and zero it out
    write(sockfd, buff, sizeof(buff));
    return 1;
}
int handleipAddress(int sockfd, char *portName, char *ipAddress, int portNumber)
{
    char buff[MAX];
    int n = 0;
    double isValid = -6;
    bzero(buff, MAX); //clear buffer
    // read the message from client
    read(sockfd, buff, sizeof(buff));

    strcpy(ipAddress, buff);
    ipAddress[strlen(ipAddress) - 1] = '\0'; // removes newline for comparison later
    bzero(buff, MAX);                        //clear buffer

    isValid = portVerification(portName, ipAddress);
    // copy server message in the buffer
    char successMessage[MAX] = "-1\n";
    if (isValid > 0)
    {
        strcpy(successMessage, "Successful Username Port Combo\n");
    }
    memcpy(&buff[0], successMessage, strlen(successMessage));
    while ((buff[n++]) != '\n')
        ;
    // and send that buffer to client and zero it out
    write(sockfd, buff, sizeof(buff));
    if (isValid > 0)
    {
        openReciever(portName, ipAddress, portNumber, sockfd);
    }
    return isValid;
}
int main(int argc, char **argv)
{
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

    char username[MAX];
    char password[MAX];
    char ipAddress[MAX];
    char portName[MAX];
    for (;;)
    {
        conntfd = accept(sockfd, (struct sockaddr *)&cli, &len);
        do
        {
            handleUsername(conntfd, username);
        } while (handlePassword(conntfd, username, password) < 0);

        do
        {
            handlePortName(conntfd, portName);
        } while (handleipAddress(conntfd, portName, ipAddress, atoi(argv[1]) + 1) < 0);
    }
}