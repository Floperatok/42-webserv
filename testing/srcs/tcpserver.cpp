
#include "webserv.hpp"

void	server(void)
{
	int	sockfd;
	struct sockaddr_in	servaddr;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		std::cerr << "Error at socket creation" << std::endl;
		return ;
	}
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERVER_PORT);

	if ((bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))) < 0)
	{
		std::cerr << "Error at binding socket" << std::endl;
		return ;
	}

	if ((listen(sockfd, 10)) < 0)
	{
		std::cerr << "Error at listening socket" << std::endl;
		return ;
	}

	while (1)
	{
		printf("waiting for a connection on port %d\n", SERVER_PORT);
		int connfd = accept(sockfd, (struct sockaddr *) 0, 0);

		char recvline[4096];
		memset(recvline, 0, 4096);
		int n;
		while ((n = read(connfd, recvline, 4095)) > 0)
		{
			fprintf(stdout, "%s\n", recvline);

			if (recvline[n - 1] == '\n')
				break ;
			memset(recvline, 0, 4096);
		}
		if (n < 0)
		{
			std::cerr << "Error at reading" << std::endl;
			return ;
		}
		char buff[4096];
		snprintf(buff, sizeof(buff), "HTTP/1.0 200 OK\r\n\r\nHello");
		write(connfd, (char*)buff, strlen((char *)buff));
		close(connfd);
	}
}