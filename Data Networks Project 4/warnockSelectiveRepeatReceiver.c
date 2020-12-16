// Client side implementation of UDP client-server model
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

int readFile(int sockfd, struct sockaddr_in servaddr, char *fileName, char *OUTPUT_FILE)
{
    int len, n, nack=1, ack=0, counter = 0;
    struct Packet packet;
    FILE *fp = fopen(OUTPUT_FILE, "ab+");
    ;

    do
    {
        n = recvfrom(sockfd, (char *)&packet, sizeof(packet),
                     MSG_WAITALL, (struct sockaddr *)&servaddr,
                     &len);
        counter++;
        fwrite(packet.data, 1, strlen(packet.data), fp);
        printf("Server : %s\n", packet.data);
        if (rand() % 10 != 0)
        {
            sendto(sockfd, (char *)"ACK", 256,
                   MSG_CONFIRM, (const struct sockaddr *)&servaddr,
                   sizeof(servaddr));
            ack++;
        }
        if (packet.seq_num != counter)
        {
            sendto(sockfd, (char *)"NACK", 256,
                   MSG_CONFIRM, (const struct sockaddr *)&servaddr,
                   sizeof(servaddr));
            nack++;
        }
    } while (packet.seq_num > 0);

    fseek(fp, 0L, SEEK_END);
    int size = ftell(fp);
    fclose(fp);
    struct stat t_stat;
    stat(fileName, &t_stat);
    struct tm * timeinfo = localtime(&t_stat.st_ctime);

    printf("File Summary\n------------------\n");
    printf("File Name: %s\n File size %d bytes: \n", fileName, size);
    printf("Creation Date and Time: %s", asctime(timeinfo));
    printf("Number of ACK's sent: %d\n", ack);
    printf("Number of NACK's sent: %d\n", nack);
}
int sendFile(int sockfd, struct sockaddr_in servaddr, char *OUTPUT_FILE)
{
    char fileName[100];
    int len;
    char buffer[MAXLINE];
    do
    {
        printf("What is filename you want to send?\n");
        gets(fileName);
        sendto(sockfd, fileName, 100,
               MSG_CONFIRM, (const struct sockaddr *)&servaddr,
               sizeof(servaddr));
        recvfrom(sockfd, (char *)buffer, MAXLINE,
                 MSG_WAITALL, (struct sockaddr *)&servaddr,
                 &len);
    } while (atoi(buffer) < 0);
    readFile(sockfd, servaddr, fileName, OUTPUT_FILE);
}
int auth(int sockfd, struct sockaddr_in servaddr)
{ //collect username and pass and send to server
    char user[100];
    char pass[100];

    printf("What is your username?\n");
    gets(user);
    sendto(sockfd, user, strlen(user),
           MSG_CONFIRM, (const struct sockaddr *)&servaddr,
           sizeof(servaddr));

    printf("What is your password?\n");
    gets(pass);
    sendto(sockfd, pass, strlen(pass),
           MSG_CONFIRM, (const struct sockaddr *)&servaddr,
           sizeof(servaddr));
    return 1;
}

// Driver code
int main(int argc, char **argv)
{
    const int PORT = atoi(argv[2]);
    const int WINDOW_SIZE = 1;
    const char *INPUT_FILE = argv[3];
    const char *OUTPUT_FILE = argv[4];
    int sockfd;
    char buffer[MAXLINE];
    struct sockaddr_in servaddr;

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);

    int n, len;
    auth(sockfd, servaddr);

    n = recvfrom(sockfd, (char *)buffer, MAXLINE,
                 MSG_WAITALL, (struct sockaddr *)&servaddr,
                 &len);
    buffer[n] = '\0';
    printf("Server : %s\n", buffer);
    if (atoi(buffer) > 0)
    {
        sendFile(sockfd, servaddr, OUTPUT_FILE);
    }

    close(sockfd);
    return 0;
}
