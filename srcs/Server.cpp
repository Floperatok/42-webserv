
#include "Server.hpp"


/* ########## Constructor ########## */

Server::Server(void)
{
	_sockfd = -1;
	_serverport = -1;
	std::memset(&_servaddr, 0, sizeof(_servaddr));
}

Server::Server(const Server &copy)
{
	if (&copy != this)
		*this = copy;
}

Server::~Server(void)
{
}


/* ########## Operator overload ########## */

Server &Server::operator=(const Server &other)
{
	if (&other != this)
	{
		_sockfd						= other._sockfd;
		_port						= other._port;
		_servaddr.sin_family		= other._servaddr.sin_family;
		_servaddr.sin_addr.s_addr	= other._servaddr.sin_addr.s_addr;
		_servaddr.sin_port			= other._servaddr.sin_port;
	}
	return (*this);
}


/* ########## Setter/Getter ########## */

int	Server::getPort(void) const
{
	return (_port);
}

int	Server::getSockfd(void) const
{
	return (_sockfd);
}

sockaddr_in	Server::getServaddr(void) const
{
	return (_servaddr);
}

void	Server::setPort(const int port)
{
	_port = port;
}


/* ########## Member function ########## */

void	Server::_setupServAddr(void)
{
	std::memset(&_servaddr, 0, sizeof(_servaddr));
	_servaddr.sin_family = AF_INET;
	_servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	_servaddr.sin_port = htons(_port);
}

void	Server::setup(void)
{
	if ((_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		throw (CannotCreateSocket(errno));
	
	_setupServAddr();
	
	int option_value = 1;
    setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option_value, sizeof(int));
	
	if ((bind(_sockfd, (struct sockaddr *) &_servaddr, sizeof(_servaddr))) < 0)
		throw (CannotBindSocket(_port, errno));
	if ((listen(_sockfd, 512)) < 0)
		throw (CannotListenSocket(errno));
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

