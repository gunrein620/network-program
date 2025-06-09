#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>              // fcntl 함수용
#include <errno.h>             // errno 사용

#define MAXLINE 1024
#define SERV_PORT 9877
#define SA struct sockaddr

int main() {
    int tcpfd, udpfd, connfd;
    socklen_t len;
    char buf[MAXLINE];
    ssize_t n;
    struct sockaddr_in servaddr, cliaddr;

    // TCP 소켓 생성
    tcpfd = socket(AF_INET, SOCK_STREAM, 0);
    // TCP 소켓을 non-blocking으로 설정
    fcntl(tcpfd, F_SETFL, O_NONBLOCK);

    // UDP 소켓 생성
    udpfd = socket(AF_INET, SOCK_DGRAM, 0);

    // 서버 주소 설정
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    // TCP 바인드 & 리슨
    bind(tcpfd, (SA *)&servaddr, sizeof(servaddr));
    listen(tcpfd, 10);

    // UDP 바인드
    bind(udpfd, (SA *)&servaddr, sizeof(servaddr));

    printf("TCP and UDP echo server is running on port %d...\n", SERV_PORT);

    for (;;) {
        // UDP 처리
        len = sizeof(cliaddr);
        n = recvfrom(udpfd, buf, MAXLINE, MSG_DONTWAIT, (SA *)&cliaddr, &len);
        if (n > 0) {
            buf[n] = 0;
            printf("[UDP echo] %s", buf);
            sendto(udpfd, buf, n, 0, (SA *)&cliaddr, len);
        }

        // TCP 처리
        len = sizeof(cliaddr);
        connfd = accept(tcpfd, (SA *)&cliaddr, &len);
        if (connfd >= 0) {
            printf("[TCP connection accepted]\n");
            // accept된 소켓을 blocking 모드로 변경
            int flags = fcntl(connfd, F_GETFL, 0);
            fcntl(connfd, F_SETFL, flags & ~O_NONBLOCK);
            
            while ((n = read(connfd, buf, MAXLINE)) > 0) {
                buf[n] = 0;
                printf("[TCP recv] %s", buf);
                write(connfd, buf, n);
            }
            if (n < 0) {
                perror("TCP read error");
            }
            close(connfd);
            printf("[TCP connection closed]\n");
        } else if (errno != EAGAIN && errno != EWOULDBLOCK) {
            perror("accept error");
        }

        // CPU 사용 조절
        usleep(1000);
    }

    // 이 코드는 루프 밖이라 도달하지 않지만 예의상 작성
    close(tcpfd);
    close(udpfd);
    return 0;
}