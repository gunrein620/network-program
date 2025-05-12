#include	"unp.h"
#include	<time.h>

int
main(int argc, char **argv)
{
	int					listenfd, connfd;
	struct sockaddr_in	servaddr;
	char				buff[MAXLINE];
	time_t				ticks;
	static int			counter = 0;    /* 클라이언트 연결 카운터 */
	
	if (argc != 2)
		err_quit("usage: a.out <port>");

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(atoi(argv[1]));	/* 사용자 지정 포트 */

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

	for ( ; ; ) {
		connfd = Accept(listenfd, (SA *) NULL, NULL);
        
        counter++;    /* 클라이언트가 연결될 때마다 카운터 증가 */
        
        ticks = time(NULL);
        snprintf(buff, sizeof(buff), "횟수: %d, 시간: %.24s\r\n", 
                counter, ctime(&ticks));
        Write(connfd, buff, strlen(buff));

		Close(connfd);
	}
}