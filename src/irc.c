#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <winsock.h>

int main(int argc, char * argv[])
{
    int eid;
    WORD wsaVersion;
    wsaVersion = MAKEWORD(2, 0);
    WSADATA wsaData;

    eid = WSAStartup(wsaVersion, &wsaData);

    if (eid != 0)
    {
        printf("WinSock startup failed.\r\n");
    }

    unsigned int fd;

    fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd < 0)
    {
        printf("Error in socket file descripter.\r\n");
    }

    struct sockaddr_in s;

    s.sin_family = AF_INET;
    s.sin_port = htons(6667);
    s.sin_addr.s_addr = inet_addr("208.53.152.214");

    eid = connect(fd, (struct sockaddr *) &s, sizeof s);
    if (eid < 0)
    {
        printf("Error connecting socket.\r\n");
    }

    char buffer[512], suffix[512];
    memset(buffer, 0, sizeof buffer);
    sprintf(buffer, "NICK %s\r\n", "windows");
    send(fd, buffer, strlen(buffer), 0);
    memset(buffer, 0, sizeof buffer);
    sprintf(buffer, "USER %s \"\" \"\" :%s\r\n", "svs", "c winsock");
    send(fd, buffer, strlen(buffer), 0);
    memset(buffer, 0, sizeof buffer);
    while (1)
    {
        eid = recv(fd, buffer, sizeof buffer, 0);
        if (eid <= 0)
        {
            break;
        }
        if (sscanf(buffer, "PING :%s", suffix) > 0)
        {
            sprintf(buffer, "PONG :%s\r\n", suffix);
            send(fd, buffer, strlen(buffer), 0);
        }
        printf("%s\r\n", buffer);
        memset(buffer, 0, sizeof buffer);
    }
    closesocket(fd);
    WSACleanup();
    getchar();
    return 0;
}
