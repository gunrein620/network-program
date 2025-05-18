#include	"unp.h"

int
main(int argc, char **argv)
{
	int					sockfd, n, port;
	char				recvline[MAXLINE + 1];
	struct sockaddr_in	servaddr;

	if (argc != 3)
		err_quit("usage: %s <IPaddress> <Port>", argv[0]);

	port = atoi(argv[2]);
	if (port <= 0 || port > 65535)
		err_quit("invalid port: %d", port);

	// 소켓 생성
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		err_sys("socket error");

	// 서버 주소 설정
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(port);
	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
		err_quit("inet_pton error for %s", argv[1]);

	printf("Connecting to %s on port %d...\n", argv[1], port);

	// 연결 시도
	if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0)
		err_sys("connect error");

	// 수신 및 출력
	while ((n = read(sockfd, recvline, MAXLINE)) > 0) {
		recvline[n] = 0;
		if (fputs(recvline, stdout) == EOF)
			err_sys("fputs error");
	}
	if (n < 0)
		err_sys("read error");

	exit(0);
}