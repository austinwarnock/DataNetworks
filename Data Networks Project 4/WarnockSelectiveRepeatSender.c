// Server side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <time.h>

#define MAXLINE 1024

struct Packet
{
    int seq_num;
    char data[256];
};

int sendFile(int sockfd, struct sockaddr_in cliaddr, int len, char *fileName)
{
    int ack = 0, nack = 0;
    char buff[256];
    char response[256];
    struct Packet packet;
    packet.seq_num = 0;

    FILE *fp;
    fp = fopen(fileName, "r");
    int n;
    do
    {
        n = fread(buff, 256, 1, (FILE *)fp);
        printf("%d\n", n);
        memcpy(&packet.data, buff, 256);
        sendto(sockfd, (char *)&packet, sizeof(packet),
               MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
               len);
        packet.seq_num++;
        recvfrom(sockfd, (char *)response, 256,
                 MSG_WAITALL, (struct sockaddr *)&cliaddr,
                 &len);
        ack++;
        if (strncmp(response, "NACK", 4) == 0)
        {
            packet.seq_num--;
            sendto(sockfd, (char *)&packet, sizeof(packet),
                   MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
                   len);
            packet.seq_num++;
            nack++;
        }
    } while (n > 0);
    packet.seq_num = -5;
    sendto(sockfd, (char *)&packet, sizeof(packet),
           MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
           len);

    fseek(fp, 0L, SEEK_END);
    int size = ftell(fp);
    fclose(fp);
    struct stat t_stat;
    stat(fileName, &t_stat);
    struct tm *timeinfo = localtime(&t_stat.st_ctime);

    printf("File Summary\n------------------\n");
    printf("Reciever IP: %d\n Reciever Port: %d\n", cliaddr.sin_addr, cliaddr.sin_port);
    printf("File Name: %s\n File size %d bytes: \n", fileName, size);
    printf("Creation Date and Time: %s", asctime(timeinfo));
    printf("Number of packets sent: %d\n", ack);
    printf("Number of packets resent: %d\n", nack);
    printf("Number of ACK's sent: %d\n", ack);
    printf("Number of NACK's sent and SEQ #: %d and %d\n", nack, 2);
}
int recieveFileName(int sockfd, struct sockaddr_in cliaddr, int len)
{
    char fileName[MAXLINE];
    recvfrom(sockfd, (char *)fileName, 100,
             MSG_WAITALL, (struct sockaddr *)&cliaddr,
             &len);
    FILE *file;
    if ((file = fopen(fileName, "r")) > 0)
    {
        fclose(file);
        sendto(sockfd, (const char *)"1", strlen("1"),
               MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
               len);
        sendFile(sockfd, cliaddr, len, fileName);
        return 1;
    }
    else
    {
        sendto(sockfd, (const char *)"-1", strlen("-1"),
               MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
               len);
        return -1;
    }
}
char *concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 2);
    strcpy(result, s1);
    strcat(result, " ");
    strcat(result, s2);
    return result;
}
char *verifyUser(char *user, char *pass)
{
    FILE *fp;
    char buff[200];
    char *userAndPass = concat(user, pass);
    int fr;
    if ((fp = fopen("userList.txt", "r")) < 0)
    {
        printf("Error Opening userList.txt\n");
        exit(0);
    }
    fseek(fp, 0, SEEK_SET); //make sure we read from the beginning

    if ((fr = fread(&buff, sizeof(char), 200, fp)) < 0)
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

    for (i = 0; i < 7; i++)
    {
        if (strncmp(array[i], userAndPass, strlen(userAndPass)) == 0) //matching pair found
            return "1";
    }
    return "-1";
}

// Driver code
int main(int argc, char **argv)
{
    const int PORT = atoi(argv[1]);
    //const int WINDOW_SIZE = 1;
    int sockfd;
    char user[MAXLINE], pass[MAXLINE];
    struct sockaddr_in servaddr, cliaddr;

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
    servaddr.sin_port = htons(PORT);

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *)&servaddr,
             sizeof(servaddr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    int len, n;

    len = sizeof(cliaddr); //len is value/resuslt

    n = recvfrom(sockfd, (char *)user, MAXLINE,
                 MSG_WAITALL, (struct sockaddr *)&cliaddr,
                 &len);
    user[n] = '\0';

    n = recvfrom(sockfd, (char *)pass, MAXLINE,
                 MSG_WAITALL, (struct sockaddr *)&cliaddr,
                 &len);
    pass[n] = '\0';

    sendto(sockfd, (const char *)verifyUser(user, pass), strlen(verifyUser(user, pass)),
           MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
           len);

    recieveFileName(sockfd, cliaddr, len);

    return 0;
}
