#include	"unp.h"
#include	<time.h>

int
main(int argc, char **argv)
{
	int					listenfd, connfd;
	struct sockaddr_in	servaddr, cliaddr;
	socklen_t			clilen;
	char				buff[MAXLINE];
	char				client_ip[INET_ADDRSTRLEN];
	time_t				ticks;

	if (argc != 2)
		err_quit("usage: %s <Port>", argv[0]);

	int port = atoi(argv[1]);
	if (port <= 0 || port > 65535)
		err_quit("invalid port: %d", port);

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(port);

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));
	Listen(listenfd, LISTENQ);

	for ( ; ; ) {
		clilen = sizeof(cliaddr);
		connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);

		// 라이언트 IP와 포트 출력
		Inet_ntop(AF_INET, &cliaddr.sin_addr, client_ip, sizeof(client_ip));
		printf("connection from %s, port %d\n", client_ip, ntohs(cliaddr.sin_port));

		//시간 전송
		ticks = time(NULL);
		snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
		Write(connfd, buff, strlen(buff));

		Close(connfd);
	}
}