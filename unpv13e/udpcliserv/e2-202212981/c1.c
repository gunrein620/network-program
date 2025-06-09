#include	"unp.h"

int
main(int argc, char **argv)
{
	int					sockfd;
	struct sockaddr_in	servaddr;
	int					port;

	if (argc != 3)
		err_quit("usage: tcpcli <IPaddress> <port>");

	port = atoi(argv[2]);
	if (port <= 0 || port > 65535)
		err_quit("invalid port number");

	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	printf("서버 %s:%d에 연결을 시도합니다...\n", argv[1], port);

	Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));

	printf("서버에 연결되었습니다!\n");
	printf("연결을 종료합니다.\n");

	Close(sockfd);
	exit(0);
}
