#include	"unp.h"

int
main(int argc, char **argv)
{
	int					sockfd;
	struct sockaddr_in	servaddr;
	int					port;
	char				sendline[MAXLINE];
	char				input[MAXLINE];
	int					number;
	const int			student_id = 202212981;

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
	printf("숫자를 입력하세요 (학번입력시 종료): \n");

	while (1) {
		printf(">> ");
		if (fgets(input, MAXLINE, stdin) == NULL) {
			break;  // EOF
		}

		number = atoi(input);
		
		if (number == student_id) {
			printf("학번을 입력하셨습니다. 연결을 종료합니다.\n");
			break;
		}

		// 서버로 숫자 전송
		snprintf(sendline, MAXLINE, "%d\n", number);
		Write(sockfd, sendline, strlen(sendline));
		
		printf("숫자 %d를 서버로 전송했습니다.\n", number);
	}

	Close(sockfd);
	exit(0);
}
