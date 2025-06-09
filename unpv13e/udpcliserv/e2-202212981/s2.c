#include	"unp.h"

int
main(int argc, char **argv)
{
	int					listenfd, connfd;
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;
	char				str[INET_ADDRSTRLEN];
	char				recvline[MAXLINE];
	int					port;
	ssize_t				n;

	if (argc != 2)
		err_quit("usage: tcpserv <port>");

	port = atoi(argv[1]);
	if (port <= 0 || port > 65535)
		err_quit("invalid port number");

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(port);

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

	printf("TCP 서버가 포트 %d에서 대기 중입니다...\n", port);

	for ( ; ; ) {
		clilen = sizeof(cliaddr);
		connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);

		// 클라이언트 IP 주소 변환
		inet_ntop(AF_INET, &cliaddr.sin_addr, str, INET_ADDRSTRLEN);
		printf("[%s] connected\n", str);

		// 클라이언트로부터 데이터 읽기
		if ((n = Read(connfd, recvline, MAXLINE)) > 0) {
			recvline[n] = 0;  // null terminate
			// 개행문자 제거
			if (recvline[n-1] == '\n') {
				recvline[n-1] = 0;
			}
			printf("[%s] %s\n", str, recvline);
		}

		// 연결 종료
		Close(connfd);
		printf("[%s] close\n\n", str);
	}
}
