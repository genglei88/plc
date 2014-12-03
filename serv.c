#define _GNU_SOURCE 
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <poll.h>

#include "chat.h"

static int clientFd[BACKLOG];
static Client clients[BACKLOG];

#define BUFSIZE 100
char buf[BUFSIZE]= {0};
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
    if (sig == SIGPIPE) {
        printf("Cautch SIGPIPE.\n");
    }
    flag = 0;
}

int main(int argc, char *argv[])
{
    int sock, a_sock;
    struct sockaddr_in serv;
    struct sockaddr_in cli;
    int r_bytes;
    fd_set conn;
    //socklen must be initialed.
    socklen_t c_size = sizeof(cli);
    struct timeval intv;
    intv.tv_sec = 1;
    intv.tv_usec = 0;
    int maxfd;
    int retselect, i;

    struct pollfd conns;

    signal(SIGUSR1, SigProc);
    signal(SIGPIPE, SigProc);

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

    if (listen(sock, BACKLOG) < 0) {
        perror("Listen socket failed.\n");
        return -1;
    }

    memset(&clients, 0, sizeof(clients));
    //maxfd = sock+1;
    //FD_ZERO(&conn);
    //FD_SET(sock, &conn);
    memset(&conns, 0 , sizeof(conns));
    conns.fd = sock;
    conns.events = POLLIN;
    while (1) {

        maxfd = sock+1;
        FD_ZERO(&conn);
        FD_SET(sock, &conn);
        //printf("sock is %d\n", FD_ISSET(sock, &conn));
        retselect = select(maxfd, &conn, NULL, NULL, &intv);
        //retselect = poll(&conns, 1, 1000);
        log("reselect=%d\n", retselect);
        if (retselect < 0) { 
            perror("Select error.\n");
        }else if (retselect == 0){
            //continue;
        }else if ((retselect > 0)){
        //else if ((retselect > 0) && (conns.revents == (POLLIN)))
            if ((a_sock = accept4(sock, (struct sockaddr *)&cli, &c_size, SOCK_NONBLOCK)) < 0) {
                perror("Accept error.\n");
                //return -1;
            }

            if (a_sock != -1) {
                printf("Client: ip: %s, port: %d\n", inet_ntoa(cli.sin_addr), ntohs(cli.sin_port));
                for (i = 0; i < BACKLOG; i++) {
                    if (!clients[i].iswork) {
                        clients[i].iswork = 1;
                        clients[i].fd = a_sock;
                        clients[i].addr = cli;
                        break;
                    }
                }
                //shutdown(a_sock, SHUT_RDWR);
                //close(a_sock);
            }
        }
        
        for (i = 0; i < BACKLOG; i++) {
            //printf("client_%d: %d\n", i, clients[i].iswork);
            if (clients[i].iswork) {
                r_bytes = recv(clients[i].fd, buf, BUFSIZE, 0);
                if (r_bytes < 0) {
                    //perror("Recv failed.\n");
                }else if (r_bytes == 0) {
                    shutdown(clients[i].fd, SHUT_RDWR);
                    close(clients[i].fd);
                    memset(&clients[i], 0, sizeof(Client));
                }else {
                    printf("(%s:%d) %s\n", inet_ntoa(clients[i].addr.sin_addr), ntohs(clients[i].addr.sin_port), buf);
                }
                memset(buf, 0, BUFSIZE);
            }
        }
        sleep(1);
    }

    shutdown(sock, SHUT_RDWR);
    close(sock);
    return 0;
}
