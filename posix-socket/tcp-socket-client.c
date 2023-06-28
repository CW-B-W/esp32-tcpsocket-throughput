#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>

#define IP   "192.168.4.1"
#define PORT 5000

int main(int argc , char *argv[])
{
    int sockfd = 0;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1) {
        printf("Fail to create a socket.");
    }

    struct sockaddr_in info;
    bzero(&info, sizeof(info));
    info.sin_family = PF_INET;

    info.sin_addr.s_addr = inet_addr(IP);
    info.sin_port = htons(PORT);


    int err = connect(sockfd, (struct sockaddr *)&info, sizeof(info));
    if(err == -1) {
        printf("Connection error");
    }

    // Transfer file
    {
        long size = 128L * 1024 * 1024;
        unsigned char buffer[32 * 1024];
        FILE *fp = fopen("./dummy.dat", "rb");

        long start, end;
        struct timeval timecheck;
        gettimeofday(&timecheck, NULL);
        start = (long)timecheck.tv_sec * 1000 + (long)timecheck.tv_usec / 1000;

        int checksum = 0;
        while (size > 0) {
            fread(buffer, sizeof(buffer), 1, fp);
            for (int i = 0; i < sizeof(buffer); ++i) {
                checksum += buffer[i];
            }
            checksum &= 0xFF;
            send(sockfd, buffer, sizeof(buffer), 0);
            size -= sizeof(buffer);
        }
        
        gettimeofday(&timecheck, NULL);
        end = (long)timecheck.tv_sec * 1000 + (long)timecheck.tv_usec / 1000;
        printf("checksum = %d\n", checksum);
        printf("%ld milliseconds elapsed\n", (end - start));
    }

    printf("close Socket\n");
    close(sockfd);
    return 0;
}
