
#include "Server.hpp"


/* ########## Constructor ########## */

Server::Server(const unsigned int serverport)
	: _serverport(serverport)
{
	_sockfd = -1;
	std::memset(&_servaddr, 0, sizeof(_servaddr));
}

Server::Server(const Server &copy)
	: _serverport(copy._serverport)
{
	if (&copy != this)
		*this = copy;
}

Server::~Server(void)
{
	close(_sockfd);
}


/* ########## Operator overload ########## */

Server &Server::operator=(const Server &other)
{
	if (&other != this)
	{
		_sockfd						= other._sockfd;
		_servaddr.sin_family		= other._servaddr.sin_family;
		_servaddr.sin_addr.s_addr	= other._servaddr.sin_addr.s_addr;
		_servaddr.sin_port			= other._servaddr.sin_port;
	}
	return (*this);
}


/* ########## Setter/Getter ########## */


/* ########## Member function ########## */

void	Server::setup(void)
{
	if ((_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		throw (CannotCreateSocket(errno));
	std::memset(&_servaddr, 0, sizeof(_servaddr));
	_servaddr.sin_family = AF_INET;
	_servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	_servaddr.sin_port = htons(_serverport);
	int option_value = 1;
    setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &option_value, sizeof(int));
	if ((bind(_sockfd, (struct sockaddr *) &_servaddr, sizeof(_servaddr))) < 0)
		throw (CannotBindSocket(_serverport, errno));
}

void	Server::run(void)
{
	if ((listen(_sockfd, 10)) < 0)
		throw (CannotListenSocket(errno));
	std::cout << "waiting for a connection on port " << _serverport << std::endl;;
	int connfd = accept(_sockfd, (struct sockaddr *) 0, 0);
	char recvline[_BUFFSIZE];
	std::memset(recvline, 0, _BUFFSIZE);
	int n;
	while ((n = read(connfd, recvline, _BUFFSIZE - 1)) > 0)
	{
		std::cout << recvline;
		if (recvline[n - 1] == '\n' && 
			recvline[n - 2] == '\r' &&
			recvline[n - 3] == '\n' &&
			recvline[n - 4] == '\r')
			break ;
		std::memset(recvline, 0, _BUFFSIZE);
	}
	std::cout << std::endl;
	if (n < 0)
		throw (ReadError());
	std::string response("HTTP/1.0 200 OK\r\n\r\nHello");
	write(connfd, response.c_str(), response.length());
	close(connfd);
}


/* ########## Exception ########## */

Server::CannotCreateSocket::CannotCreateSocket(const int errnum)
{
	std::ostringstream oss;

	oss << "Cannot create socket: " << strerror(errnum);
	_message = oss.str();
}
Server::CannotCreateSocket::~CannotCreateSocket(void) throw()
{
}
const char *Server::CannotCreateSocket::what(void) const throw()
{
	return (_message.c_str());
}

Server::CannotBindSocket::CannotBindSocket(const unsigned int port, const int errnum)
{
	std::ostringstream oss;

	oss << "Cannot bind socket at port " << port << ": " << strerror(errnum);
	_message = oss.str();
}
Server::CannotBindSocket::~CannotBindSocket(void) throw()
{
}
const char *Server::CannotBindSocket::what(void) const throw()
{
	return (_message.c_str());
}

Server::CannotListenSocket::CannotListenSocket(const int errnum)
{
	std::ostringstream oss;

	oss << "Cannot listen socket: " << strerror(errnum);
	_message = oss.str();
}
Server::CannotListenSocket::~CannotListenSocket(void) throw()
{
}
const char *Server::CannotListenSocket::what(void) const throw()
{
	return (_message.c_str());
}

const char *Server::ReadError::what(void) const throw()
{
	return ("Read error"); // devellop more
}


/* ########## Non-member function ########## */

