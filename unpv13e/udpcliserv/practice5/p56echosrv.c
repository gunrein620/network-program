#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <errno.h>
#include <arpa/inet.h>

#define MAXLINE 1024
#define SERV_PORT 9877
#define SA struct sockaddr
#define LISTENQ 10

int main() {
    int listenfd, udpfd, connfd, sockfd;
    int nready, maxfdp1;
    ssize_t n;
    fd_set rset, allset;
    socklen_t len;
    struct sockaddr_in servaddr, cliaddr;
    char buf[MAXLINE];
    int client[FD_SETSIZE];  // 연결된 클라이언트 소켓들을 저장
    int i, maxi;

    // TCP 리스닝 소켓 생성
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    
    // UDP 소켓 생성  
    udpfd = socket(AF_INET, SOCK_DGRAM, 0);

    // 서버 주소 설정
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    // TCP 소켓 바인드 & 리슨
    bind(listenfd, (SA *)&servaddr, sizeof(servaddr));
    listen(listenfd, LISTENQ);

    // UDP 소켓 바인드
    bind(udpfd, (SA *)&servaddr, sizeof(servaddr));

    maxfdp1 = (listenfd > udpfd ? listenfd : udpfd) + 1;
    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);
    FD_SET(udpfd, &allset);

    // 클라이언트 배열 초기화
    for (i = 0; i < FD_SETSIZE; i++) {
        client[i] = -1;
    }
    maxi = -1;

    printf("TCP and UDP echo server (using select) is running on port %d...\n", SERV_PORT);

    for (;;) {
        rset = allset;
        nready = select(maxfdp1, &rset, NULL, NULL, NULL);

        // TCP 리스닝 소켓에 새 연결 요청
        if (FD_ISSET(listenfd, &rset)) {
            len = sizeof(cliaddr);
            connfd = accept(listenfd, (SA *)&cliaddr, &len);
            
            printf("[TCP connection accepted from %s:%d]\n", 
                   inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));

            // 빈 자리 찾아서 클라이언트 소켓 저장
            for (i = 0; i < FD_SETSIZE; i++) {
                if (client[i] < 0) {
                    client[i] = connfd;
                    break;
                }
            }
            if (i == FD_SETSIZE) {
                printf("too many clients\n");
                close(connfd);
            } else {
                FD_SET(connfd, &allset);
                if (connfd >= maxfdp1) {
                    maxfdp1 = connfd + 1;
                }
                if (i > maxi) {
                    maxi = i;
                }
            }

            if (--nready <= 0) continue;
        }

        // UDP 소켓에 데이터 수신
        if (FD_ISSET(udpfd, &rset)) {
            len = sizeof(cliaddr);
            n = recvfrom(udpfd, buf, MAXLINE, 0, (SA *)&cliaddr, &len);
            if (n > 0) {
                buf[n] = 0;
                printf("[UDP echo from %s:%d] %s", 
                       inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port), buf);
                sendto(udpfd, buf, n, 0, (SA *)&cliaddr, len);
            }

            if (--nready <= 0) continue;
        }

        // 연결된 TCP 클라이언트들 처리
        for (i = 0; i <= maxi; i++) {
            if ((sockfd = client[i]) < 0) continue;
            
            if (FD_ISSET(sockfd, &rset)) {
                if ((n = read(sockfd, buf, MAXLINE)) == 0) {
                    // 클라이언트가 연결 종료
                    printf("[TCP connection closed]\n");
                    close(sockfd);
                    FD_CLR(sockfd, &allset);
                    client[i] = -1;
                } else if (n > 0) {
                    // 에코 처리
                    buf[n] = 0;
                    printf("[TCP echo] %s", buf);
                    write(sockfd, buf, n);
                } else {
                    // 읽기 에러
                    printf("[TCP read error]\n");
                    close(sockfd);
                    FD_CLR(sockfd, &allset);
                    client[i] = -1;
                }

                if (--nready <= 0) break;
            }
        }
    }

    // 정리 (실제로는 도달하지 않음)
    close(listenfd);
    close(udpfd);
    return 0;
} 