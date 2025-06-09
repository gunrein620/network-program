#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#define MAXLINE 1024
#define SERV_PORT 9877

int main() {
    int listenfd, connfd;
    socklen_t clilen;
    char buf[MAXLINE];
    struct sockaddr_in servaddr, cliaddr;
    ssize_t n;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    listen(listenfd, 10);

    clilen = sizeof(cliaddr);
    connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);

    while ((n = read(connfd, buf, MAXLINE)) > 0) {
        write(connfd, buf, n);
    }

    close(connfd);
    close(listenfd);
    return 0;
}