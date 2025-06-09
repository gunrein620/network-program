#include	"unp.h"

int
main(int argc, char **argv)
{
	int					listenfd, connfd;
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;
	char				str[INET_ADDRSTRLEN];
	int					port;

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

		// 클라이언트 IP와 포트번호 출력
		inet_ntop(AF_INET, &cliaddr.sin_addr, str, INET_ADDRSTRLEN);
		printf("클라이언트 연결: IP = %s, Port = %d\n", 
			   str, ntohs(cliaddr.sin_port));

		// 즉시 연결 종료
		Close(connfd);
		printf("연결이 종료되었습니다. 다음 연결을 기다립니다...\n\n");
	}
}
