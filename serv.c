#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
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
    int sock, a_sock;
    struct sockaddr_in serv;
    struct sockaddr_in cli;
    int r_bytes;
    socklen_t c_size;

    signal(SIGUSR1, SigProc);

    bzero(&cli, sizeof(cli));
    bzero(&serv, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(20000);
    serv.sin_addr.s_addr = INADDR_ANY;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Create socket failed.\n");
        return -1;
    }

    if (bind(sock, (const struct sockaddr *)&serv, sizeof(serv)) < 0) {
        perror("Bind socket failed.\n");
        return -1;
    }

    if (listen(sock, 5) < 0) {
        perror("Listen socket failed.\n");
        return -1;
    }
    sleep(300);

    if ((a_sock = accept(sock, (struct sockaddr *)&cli, &c_size)) < 0) {
        perror("Accept error.\n");
        return -1;
    }

    printf("Client: ip: %s, port: %d\n", inet_ntoa(cli.sin_addr), ntohs(cli.sin_port));
    while (flag)
    {
        if (( r_bytes = recv(a_sock, buf, BUFSIZ, 0)) < 0) {
            perror("Recv error.\n");
            continue;
        }
        buf[r_bytes] = 0;
        printf("Recv: %s\n", buf);
        
    }

    shutdown(a_sock, SHUT_RDWR);
    close(a_sock);
    close(sock);

    return 0;
}
