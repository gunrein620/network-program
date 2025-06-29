#include	"unp.h"

void
sig_chld(int signo)
{
	pid_t	pid;
	int		stat;

	while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0)
		printf("child %d terminated\n", pid);
	return;
}

int
main(int argc, char **argv)
{
	int					listenfd, connfd;
	pid_t				childpid;
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;
	char				str[INET_ADDRSTRLEN];
	char				recvline[MAXLINE];
	int					port;
	ssize_t				n;
	const int			student_id = 202212981;
	int					received_number;

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

	// SIGCHLD 시그널 핸들러 등록 (좀비 프로세스 방지)
	Signal(SIGCHLD, sig_chld);

	printf("TCP Concurrent 서버가 포트 %d에서 대기 중입니다...\n", port);
	printf("(학번 %d 수신시 해당 클라이언트 연결 종료)\n", student_id);

	for ( ; ; ) {
		clilen = sizeof(cliaddr);
		connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);

		if ( (childpid = Fork()) == 0) {	/* child process */
			Close(listenfd);	/* child는 listen socket을 닫음 */
			
			// 클라이언트 IP 주소 변환
			inet_ntop(AF_INET, &cliaddr.sin_addr, str, INET_ADDRSTRLEN);
			printf("[PID %d][%s] connected\n", getpid(), str);

			// 클라이언트로부터 데이터를 반복적으로 읽기
			while ((n = Read(connfd, recvline, MAXLINE)) > 0) {
				recvline[n] = 0;  // null terminate
				// 개행문자 제거
				if (recvline[n-1] == '\n') {
					recvline[n-1] = 0;
				}
				printf("[PID %d][%s] %s\n", getpid(), str, recvline);
				
				// 받은 숫자가 학번인지 확인
				received_number = atoi(recvline);
				if (received_number == student_id) {
					printf("[PID %d][%s] 학번을 수신했습니다. 이 클라이언트 연결을 종료합니다.\n", 
						   getpid(), str);
					break;
				}
			}

			// child process에서 연결 종료
			Close(connfd);
			printf("[PID %d][%s] close\n", getpid(), str);
			exit(0);	/* child process 종료 */
		}
		
		Close(connfd);	/* parent는 connected socket을 닫음 */
	}
}
