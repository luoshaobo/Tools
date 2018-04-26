#include <windows.h>
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <unistd.h>
//#include <signal.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <sys/wait.h>
//#include <arpa/inet.h>
//#include <netinet/in.h>

#define close               closesocket

#define MAX_BUF_SIZE 		1024
#define ASSERT_NOT(x) \
	if (x) \
	{ \
		perror("ERROR"); \
		exit(1); \
	}
    
int errno;
char *strerror(int errnum) {
    return "(no info)";
}
void perror(const char *str) {
    printf("*** ERROR[%s]: GetLastError()=%d", str, ::GetLastError());
}
	
void process(int sockfd);

int wmain(int argc, char* argv[])
{
    {
        WSADATA wsaData;
        int iResult;

        iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    }
    
	struct sockaddr_in sin = { sizeof(struct sockaddr_in) };
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr("192.168.1.25");
	sin.sin_port = htons(3129);

	for (;;)
	{
		int sockfd = socket(PF_INET, SOCK_STREAM, 0);
		if (sockfd == -1)
		{
			perror("Error");
			//continue;
            break;
		}

		int ret = connect(sockfd, (struct sockaddr*)&sin, 
			sizeof(struct sockaddr));
		if (ret == -1)
		{
			perror("Error");
			close(sockfd);
			//continue;
            break;
		}

		process(sockfd);

		close(sockfd);
		//Sleep(5000);
        break;
	}

	return 0;
}

void process(int sockfd)
{
	char buf[MAX_BUF_SIZE];
	static int i = 0;
	_snprintf(buf, sizeof(buf), "request message %d", i++);
	int count = send(sockfd, buf, strlen(buf), 0);
	ASSERT_NOT(count == -1);

	count = recv(sockfd, buf, sizeof(buf), 0);
	ASSERT_NOT(count == -1);
	buf[count] = '\0';
	printf("%s\n", buf);
}
