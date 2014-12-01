#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>


char buf[BUFSIZ]= {0};
static int flag = 1;

void SigProc(int sig)
{
    if (sig == SIGINT) {
        printf("Cautch SIGINT.\n");
    }
    if (sig == SIGQUIT) {
        printf("Cautch SIGQUIT.\n");
    }
    if (sig == SIGUSR1) {
        printf("Cautch SIGUSR1.\n");
    }
    flag = 0;
}

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in serv, cli;
    int r_bytes;

    signal(SIGUSR1, SigProc);

    bzero(&serv, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(20000);
    serv.sin_addr.s_addr = INADDR_ANY;

    bzero(&cli, sizeof(serv));
    cli.sin_family = AF_INET;
    cli.sin_port = htons(30000);
    cli.sin_addr.s_addr = INADDR_ANY;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Create socket failed.\n");
        return -1;
    }

    if (bind(sock, (const struct sockaddr *)&cli, sizeof(serv)) < 0) {
        perror("Bind socket failed.\n");
        return -1;
    }

    if (connect(sock, (const struct sockaddr *)&serv, sizeof(serv)) < 0) {
        perror("Connect socket failed.\n");
        return -1;
    }

    while (flag) 
    {
        memset(buf, 0, sizeof(buf));
        gets(buf);
        if (send(sock, buf, strlen(buf), 0) < 0) {
            perror("send message failed.\n");
            continue;
        }
    }

    shutdown(sock, SHUT_RDWR);
    close(sock);

    return 0;
}
