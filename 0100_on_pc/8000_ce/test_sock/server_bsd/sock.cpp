#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MAX_CONNECTIONS 	8
#define MAX_BUF_SIZE 		1024
#define ASSERT_NOT(x) \
	if (x) \
	{ \
		perror("ERROR"); \
		exit(1); \
	}
    
using namespace std;
	
int process(int csockfd);

int main(int argc, char* argv[])
{
	int sockfd = socket(PF_INET, SOCK_STREAM, 0);
	ASSERT_NOT(sockfd == -1);
	
	int reuse = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

	struct sockaddr_in sin = { sizeof(struct sockaddr_in) };
	sin.sin_family = AF_INET;
	//sin.sin_addr.s_addr = inet_addr("192.168.2.116");
	sin.sin_addr.s_addr = 0;
	sin.sin_port = htons(3129);
	
	int ret = bind(sockfd, (struct sockaddr*)&sin, sizeof(struct sockaddr_in));
	ASSERT_NOT(ret == -1);

	ret = listen(sockfd, MAX_CONNECTIONS);
	ASSERT_NOT(ret == -1);

	for (;;)
	{
		struct sockaddr_in csin = { sizeof(struct sockaddr_in) };
		socklen_t socklen = sizeof(struct sockaddr_in);
		int csockfd = accept(sockfd, (struct sockaddr*)&csin, &socklen);
		if (csockfd == -1)
		{
			perror("ERROR");
			continue;
		}
		
		pid_t pid = fork();
		if (pid == -1)
		{
			perror("ERROR");
			continue;
		}
		else if (pid > 0) // the father process
		{
			waitpid(pid, NULL, 0);
			close(csockfd);
			continue;
		}

		pid = fork();
		if (pid == -1)
		{
			perror("ERROR");
			exit(1);
		}
		else if (pid > 0) // the children process
			return 0;
		
		// the grandchildren process
		return process(csockfd);
	}
	
	// nerver reached!!!
	close(sockfd);
	return 0;
}

int process(int csockfd)
{
	char buf[MAX_BUF_SIZE];
	int count = read(csockfd, buf, sizeof(buf));
	ASSERT_NOT(count == -1);

    buf[count] = '\0';
	printf("%s\n", buf);

	strcat(buf, ": echoed from the server!");
	count = write(csockfd, buf, strlen(buf));
	ASSERT_NOT(count == -1);
	
	return 0;
}
