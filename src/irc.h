#ifndef IRC_H_INCLUDED
#define IRC_H_INCLUDED

#include <stdint.h>

int initWSA(void);

int openSocket(void);

int connectSocket(const char *host, uint16_t port);

int botHello(const char *nick, const char *user, const char *name);

void botLoop(void);

void ircCleanup(void);

int prepareBot(const char *host, uint16_t port);

#endif // IRC_H_INCLUDED
