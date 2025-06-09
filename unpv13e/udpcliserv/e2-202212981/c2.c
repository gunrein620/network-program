#include	"unp.h"

int
main(int argc, char **argv)
{
	int					sockfd;
	struct sockaddr_in	servaddr;
	int					port;
	char				sendline[MAXLINE];

	if (argc != 4)
		err_quit("usage: tcpcli <IPaddress> <port> <number>");

	port = atoi(argv[2]);
	if (port <= 0 || port > 65535)
		err_quit("invalid port number");

	// 숫자 확인
	int number = atoi(argv[3]);
	snprintf(sendline, MAXLINE, "%d\n", number);

	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	printf("서버 %s:%d에 연결을 시도합니다...\n", argv[1], port);

	Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));

	printf("서버에 연결되었습니다!\n");
	printf("숫자 %d를 전송합니다.\n", number);

	// 숫자 전송
	Write(sockfd, sendline, strlen(sendline));

	printf("전송 완료. 연결을 종료합니다.\n");

	Close(sockfd);
	exit(0);
}
