
#include "webserv.hpp"

void client(char *arg)
{
	char sendline[4096];
	int sockfd;
	
	//	socket creation
	//		AF_INET 	= IPv4 Internet protocols
	//		SOCK_STREAM	= two-way sequenced connection connection-based byte stream
	//		0			= default tcp connection
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		std::cerr << "Error at socket creation" << std::endl;
		return ;
	}

	//	standard port and IP struct
	//		sin_family	= IPv4 Internet protocols
	//		sin_port	= server port (standardized)
	//		sin_addr	= parsed ip address
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERVER_PORT);
	if (inet_pton(AF_INET, arg, &servaddr.sin_addr) <= 0)
	{
		std::cerr << "Error at inet_pton" << std::endl;
		return ;
	}
	
	//	connection
	if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
	{
		std::cerr << "Error at connection" << std::endl;
		return ;
	}

	//	writing request into buffer
	//		GET			= request type
	//		HTTP		= protocol
	//		1.1			= protocol version
	//		\r\n\r\n	= end of request
	sprintf(sendline, "GET / HTTP/1.1\r\n\r\n");
	long sendbytes = strlen(sendline);

	//	sending the request in the socket
	if (write(sockfd, sendline, sendbytes) != sendbytes)
	{
		std::cerr << "Error at writing on socket" << std::endl;
		return ;
	}

	//	receiving and printing server response to the request
	char recvline[4096];
	memset(recvline, 0, 4096);
	int n;
	while ((n = read(sockfd, recvline, 4095)) > 0)
	{
		printf("%s", recvline);
	}
	if (n < 0)
	{
		std::cerr << "Error at reading response" << std::endl;
		return ;
	}
}
