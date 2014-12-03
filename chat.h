#ifndef _CHAT_H_
#define _CHAT_H_

#define BACKLOG 10

typedef struct {
    char name[10];
    char msgtext[256];
} Msg, pMsg;

typedef struct {
    int iswork;
    int fd;
    struct sockaddr_in addr;
}Client, pClient;

#define DEBUG
#ifdef DEBUG
#define log(fmt, args...) \
    do { \
        printf("[%s:%d]"fmt,__FUNCTION__,__LINE__,##args);\
    }while(0)
#else
#define log(fmt, args...)
#endif


#endif
