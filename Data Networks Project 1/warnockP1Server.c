#include "netinet/in.h"
#include "sys/socket.h"
#include "stdio.h"
#include "string.h"
#include "time.h"
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#define MAX 50

void exitfunc(int sig)
{
    printf("\nSever Timeout Reached. Shutting Down.\n");
    exit(0);
}
double passwordVerification(char *userCurrency, char *userPassword)
{
    const char *CURRENCY_ARRAY[6] = {"US Dollar", "Canadian Dollar", "Euro", "British Pound", "Japanese Yen", "Swiss Franc"};
    const char *PASSWORD_ARRAY[6] = {"uCh781fY", "Cfw61RqV", "Pd82bG57", "Crc51RqV", "wD82bV67", "G6M7p8az"};
    const double BITCOIN_ARRAY[6] = {11081.00, 14632.87, 9359.20, 8578.96, 1158748.55, 10100.44};
    for (int i = 0; i < 6; ++i)
    {
        if (strcmp(userCurrency, CURRENCY_ARRAY[i]) == 0)
        {
            if (strcmp(userPassword, PASSWORD_ARRAY[i]) == 0)
            {
                return BITCOIN_ARRAY[i];
            }
        }
    }
    return -1;
}
int handleCurrency(int sockfd, char *currency)
{
    signal(SIGALRM, exitfunc);
    alarm(30); //alarm to end program after 30 seconds of inactivity
    char buff[MAX];
    int n = 0;
    bzero(buff, MAX); //clear buffer

    // read the message from client
    read(sockfd, buff, sizeof(buff));

    alarm(0); // cancel alarm because we got some data

    strcpy(currency, buff);                // copy over the results
    currency[strlen(currency) - 1] = '\0'; // removes newline for comparison later
    bzero(buff, MAX);                      //clear buffer

    // copy server message in the buffer
    char successMessage[MAX] = "Successfully Recieved Currency Type\n";
    memcpy(&buff[0], successMessage, strlen(successMessage));
    while ((buff[n++]) != '\n')
        ;
    // and send that buffer to client and zero it out
    write(sockfd, buff, sizeof(buff));
    return 1;
}
int handlePassword(int sockfd, char *currency, char *password)
{
    signal(SIGALRM, exitfunc);
    alarm(30); //alarm to end program after 30 seconds of inactivity
    char buff[MAX];
    int n = 0;
    double bitcoin_value = -6;
    bzero(buff, MAX); //clear buffer
    // read the message from client
    read(sockfd, buff, sizeof(buff));

    alarm(0); // cancel alarm because we got some data

    strcpy(password, buff);
    password[strlen(password) - 1] = '\0'; // removes newline for comparison later
    bzero(buff, MAX);                      //clear buffer

    bitcoin_value = passwordVerification(currency, password);
    // copy server message in the buffer
    char successMessage[MAX] = "Invalid Currency Password Combo\n";
    if (bitcoin_value > 0)
    {
        sprintf(successMessage, "%s%.2f", "Your Bitcoin value is: \n", bitcoin_value);
    }
    memcpy(&buff[0], successMessage, strlen(successMessage));
    while ((buff[n++]) != '\n')
        ;
    // and send that buffer to client and zero it out
    write(sockfd, buff, sizeof(buff));
    close(sockfd);
    return 1;
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
        printf("\nBinded Successfully");
    listen(sockfd, 50);
    len = sizeof(ch);

    char currency[MAX];
    char password[MAX];
    for (;;)
    {
        conntfd = accept(sockfd, (struct sockaddr *)&cli, &len);
        handleCurrency(conntfd, currency);
        handlePassword(conntfd, currency, password);
    }
}