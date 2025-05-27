#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define MAXLINE 1024
#define SERV_PORT 8080  // 포트 번호는 서버와 맞춰야 함

void str_cli(FILE *fp, int sockfd);

int main(int argc, char **argv) {
    int sockfd;
    struct sockaddr_in servaddr;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <IPaddress>\n", argv[0]);
        exit(1);
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket error");
        exit(1);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);

    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
        fprintf(stderr, "inet_pton error for %s\n", argv[1]);
        exit(1);
    }

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("connect error");
        exit(1);
    }

    str_cli(stdin, sockfd);

    close(sockfd);
    return 0;
}

void str_cli(FILE *fp, int sockfd) {
    int maxfdp1;
    fd_set rset;
    char sendline[MAXLINE], recvline[MAXLINE];
    int n;

    FD_ZERO(&rset);

    for (;;) {
        FD_SET(fileno(fp), &rset);     // 키보드 입력
        FD_SET(sockfd, &rset);         // 서버 소켓

        maxfdp1 = (fileno(fp) > sockfd ? fileno(fp) : sockfd) + 1;
        if (select(maxfdp1, &rset, NULL, NULL, NULL) < 0) {
            perror("select error");
            exit(1);
        }

        // 서버로부터 데이터 수신
        if (FD_ISSET(sockfd, &rset)) {
            if ((n = read(sockfd, recvline, MAXLINE)) == 0) {
                fprintf(stderr, "server terminated\n");
                return;
            }
            recvline[n] = '\0';
            fputs(recvline, stdout);
        }

        // 키보드 입력 처리
        if (FD_ISSET(fileno(fp), &rset)) {
            if (fgets(sendline, MAXLINE, fp) == NULL)
                return;  // EOF: Ctrl+D 입력 시 종료
            write(sockfd, sendline, strlen(sendline));
        }
    }
}