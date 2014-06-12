#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>
#include <winsock.h>

#include "log.h"

#define IRC_BUFSIZ 128


static unsigned int sockfd = -1;
static WORD wsaVersion;
static WSADATA wsaData;
static struct sockaddr_in sock;


int initWSA(void) {
    wsaVersion = MAKEWORD(2, 0);
    return(WSAStartup(wsaVersion, &wsaData));
}

int openSocket(void) {
    return ((sockfd = socket(AF_INET, SOCK_STREAM, 0)));
}

int connectSocket(const char *host, uint16_t port) {
    sock.sin_family = AF_INET;
    sock.sin_port = htons(port);
    sock.sin_addr.s_addr = inet_addr(host);
    return (connect(sockfd, (struct sockaddr *) &sock, sizeof sock));
}

int botHello(const char *nick, const char *user, const char *name) {
    char buf[IRC_BUFSIZ+1];

    memset(buf, 0, sizeof buf);
    snprintf(buf, sizeof buf-1, "NICK %s\r\n", nick);
    if (send(sockfd, buf, strlen(buf), 0) < 0) {
        return -1;
    }
    memset(buf, 0, sizeof buf);
    snprintf(buf, sizeof buf-1, "USER %s \"\" \"\" :%s\r\n", user, name);
    if (send(sockfd, buf, strlen(buf), 0) < 0) {
        return -1;
    }
    return 0;
}

void botLoop(void) {
    int retval;
    char buffer[IRC_BUFSIZ+1], suffix[IRC_BUFSIZ+1];

    while (1)
    {
        memset(buffer, 0, sizeof buffer);
        memset(suffix, 0, sizeof suffix);
        retval = recv(sockfd, buffer, sizeof buffer-1, 0);
        if (retval <= 0)
        {
            break;
        }
        if (sscanf(buffer, "PING :%64s", suffix) > 0)
        {
            sprintf(buffer, "PONG :%64s\r\n", suffix);
            send(sockfd, buffer, strlen(buffer), 0);
        }
        D("IRC_BOT: %s\r\n", buffer)
    }
}

void ircCleanup(void) {
    closesocket(sockfd);
    WSACleanup();
}

int prepareBot(const char *host, uint16_t port) {
    int retval;

    if ((retval = initWSA()) != 0) {
        PE("WSA init failed");
        return retval;
    }

    if ((retval = openSocket()) < 0) {
        PE("open Socket failed");
        return retval;
    }

    if ((retval = connectSocket(host, port)) < 0) {
        PE("connect Socket failed");
        return retval;
    }
    return 0;
}
