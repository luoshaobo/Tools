#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MAX_BUF_SIZE 		1024
#define ASSERT_NOT(x) \
	if (x) \
	{ \
		perror("ERROR"); \
		exit(1); \
	}
	
void process(int sockfd);

int main(int argc, char* argv[])
{
	struct sockaddr_in sin = { sizeof(struct sockaddr_in) };
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr("192.168.2.31");
	sin.sin_port = htons(1777);

	for (;;)
	{
		int sockfd = socket(PF_INET, SOCK_STREAM, 0);
		if (sockfd == -1)
		{
			perror("Error");
			continue;
		}

		int ret = connect(sockfd, (struct sockaddr*)&sin, 
			sizeof(struct sockaddr));
		if (ret == -1)
		{
			perror("Error");
			close(sockfd);
			continue;
		}

		process(sockfd);

		close(sockfd);
		usleep(100000);
	}

	return 0;
}

void process(int sockfd)
{
	char buf[MAX_BUF_SIZE];
	static int i = 0;
	snprintf(buf, sizeof(buf), "request message %d", i++);
	int count = write(sockfd, buf, strlen(buf));
	ASSERT_NOT(count == -1);

	/*
	count = read(sockfd, buf, sizeof(buf));
	ASSERT_NOT(count == -1);
	buf[count] = '\0';
	printf("%s\n", buf);
	*/
}
