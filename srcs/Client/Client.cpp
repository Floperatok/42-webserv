
#include "Client.hpp"


/* ########## Constructor ########## */

Client::Client(void)
	:	server(0), sockfd(-1), request("")
{
	
}

Client::Client(Server *serverArg)
	:	server(serverArg), request("")
{
	struct sockaddr_in			servaddr = server->getServaddr();
	socklen_t					servaddrLen = sizeof(servaddr);
	std::vector<std::string>	emptyVector;
	if ((sockfd = accept(server->getSockfd(), (struct sockaddr *)&servaddr, \
		&servaddrLen)) < 0)
	{
		Logger::warning("Failed to accept the socket binded on the port " + Utils::IntToStr(server->getPort()) + ".");
		Response::ServiceUnavailable503(server->getSockfd(), *server, emptyVector);
	}
	Logger::info("New client's socket created on port " + Utils::IntToStr(ntohs(server->getServaddr().sin_port)) \
				+ ", socket_fd: " + Utils::IntToStr(sockfd) + ".");
}

Client::Client(const Client &copy)
{
	if (&copy != this)
		*this = copy;
}

Client::~Client(void)
{
}


/* ########## Operator overload ########## */

Client &Client::operator=(const Client &other)
{
	if (&other != this)
	{
		server = other.server;
		sockfd = other.sockfd;
		request = other.request;
	}
	return (*this);
}


/* ########## Setter/Getter ########## */


/* ########## Member function ########## */

/*
 *	@brief Reads the request from the socket.
 *	@return 1 if the request was fully received, 0 if it was received partially, -1 in case of error.
*/
int	Client::readClientRequest(void)
{
	char			buffer[BUFFER_SIZE];
	ssize_t			bytesread;
	size_t			totalBytesRead = 0;
	size_t			contentLength = 0;
	bool			headersReceived = false;

	bytesread = recv(sockfd, buffer, BUFFER_SIZE, MSG_DONTWAIT);
	if (bytesread < 0)
	{
		Logger::error("Failed to read from socket " + Utils::IntToStr(sockfd) + ".");
		return (-1);
	}
	if (bytesread == 0) 
	{
		Logger::info("Connection to the socket " + Utils::IntToStr(sockfd) + " closed.");
		return (-1);
	}
	buffer[bytesread] = '\0';
	request.append(buffer, bytesread);
	totalBytesRead += bytesread;
	size_t headersEndPos = request.find("\r\n\r\n");

	if (!headersReceived && headersEndPos != std::string::npos)
	{
		headersReceived = true;

		size_t contentLengthPos = request.find("Content-Length: ");
		if (contentLengthPos != std::string::npos)
		{
			size_t contentLengthStart = contentLengthPos + 16;
			size_t contentLengthEnd = request.find("\r\n", contentLengthStart);
			contentLength = Utils::StrToInt(request.substr(contentLengthStart, contentLengthEnd - contentLengthStart));
		}
	}
	if (headersReceived && request.size() >= headersEndPos + 4 + contentLength)
	{
		Logger::info("Request fully received from socket " + Utils::IntToStr(sockfd) + ".");
		return (1);
	}
	return (0);
}




/* ########## Exception ########## */


/* ########## Non-member function ########## */

