
#include "Client.hpp"


/* ########## Constructor ########## */

Client::Client(Server &server)
{
	int					clientSocket;
	struct sockaddr_in	servaddr = server.getServaddr();
	socklen_t			servaddrLen = sizeof(servaddr);

	_sockfd = accept(server.getSockfd(), (struct sockaddr *)&servaddr, &servaddrLen);

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
		_sockfd = other._sockfd;
		_request = other._request;
		_server = other._server;
	}
	return (*this);
}


/* ########## Setter/Getter ########## */

int	Client::getSockfd(void) const
{
	return (_sockfd);
}

const Server	*Client::getServer(void) const
{
	return (_server);
}

const std::string	&Client::getRequest(void) const
{
	return (_request);
}


/* ########## Member function ########## */

void	Client::appendRequest(const std::string &str)
{
	_request += str;
}

void	Client::clearRequest(void)
{
	_request = "";
}


/* ########## Exception ########## */


/* ########## Non-member function ########## */

