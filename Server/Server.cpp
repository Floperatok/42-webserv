
#include "Server.hpp"


/* ########## Constructor ########## */

Server::Server(void)
{
	_sockfd = -1;
	_port = -1;
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

void	Server::setPort(const int port) {	_port = port;	}
int	Server::getPort(void) const {	return (_port);	}

int	Server::getSockfd(void) const {	return (_sockfd);	}

sockaddr_in	Server::getServaddr(void) const {	return (_servaddr);	}


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
	Logger::debug("Creating socket");
	if ((_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		throw (Logger::FunctionError("socket", errno));
	
	_setupServAddr();
	
	int option_value = 1;
    setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option_value, sizeof(int));
	
	std::ostringstream oss;
	oss << "Binding socket at port " << _port;
	Logger::debug(oss.str().c_str());
	if ((bind(_sockfd, (struct sockaddr *) &_servaddr, sizeof(_servaddr))) < 0)
		throw (Logger::FunctionError("bind", errno));
	
	Logger::debug("Listen for connections");
	if ((listen(_sockfd, 512)) < 0)
		throw (Logger::FunctionError("listen", errno));
}


/* ########## Exception ########## */


/* ########## Non-member function ########## */
