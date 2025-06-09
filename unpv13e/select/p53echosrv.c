#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>

#define MAXLINE 1024
#define SERV_PORT 9877
#define MAX_CLIENTS FD_SETSIZE

int main() {
    int listenfd, connfd, sockfd;
    int maxfd, maxi, i, nready, client[MAX_CLIENTS];
    ssize_t n;
    char buf[MAXLINE];
    fd_set allset, rset;
    socklen_t clilen;
    struct sockaddr_in servaddr, cliaddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    listen(listenfd, 10);

    maxfd = listenfd;
    maxi = -1;
    for (i = 0; i < MAX_CLIENTS; i++) client[i] = -1;

    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);

    printf("Echo server (select) running on port %d...\n", SERV_PORT);

    for (;;) {
        rset = allset;
        nready = select(maxfd + 1, &rset, NULL, NULL, NULL);

        if (FD_ISSET(listenfd, &rset)) {
            clilen = sizeof(cliaddr);
            connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);

            for (i = 0; i < MAX_CLIENTS; i++) {
                if (client[i] < 0) {
                    client[i] = connfd;
                    break;
                }
            }

            if (i == MAX_CLIENTS) {
                fprintf(stderr, "too many clients\n");
                close(connfd);
                continue;
            }

            FD_SET(connfd, &allset);
            if (connfd > maxfd) maxfd = connfd;
            if (i > maxi) maxi = i;

            if (--nready <= 0) continue;  // no more readable fds
        }

        for (i = 0; i <= maxi; i++) {
            if ((sockfd = client[i]) < 0) continue;

            if (FD_ISSET(sockfd, &rset)) {
                if ((n = read(sockfd, buf, MAXLINE)) == 0) {
                    close(sockfd);
                    FD_CLR(sockfd, &allset);
                    client[i] = -1;
                } else {
                    write(sockfd, buf, n);
                }

                if (--nready <= 0) break;
            }
        }
    }
}