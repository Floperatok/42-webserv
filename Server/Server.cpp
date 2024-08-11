
#include "Server.hpp"


/* ########## Constructor ########## */

Server::Server(void)
{
	_sockfd = -1;
	_port = -1;
}

Server::Server(const Server &copy)
{
	if (&copy != this)
		*this = copy;
}

Server::~Server(void)
{
	// if (_sockfd != -1)
		// close(_sockfd);
}


/* ########## Operator overload ########## */

Server &Server::operator=(const Server &other)
{
	if (&other != this)
	{
		_port						= other._port;
		_serverName					= other._serverName;
		_host						= other._host;
		_root						= other._root;
		_index						= other._index;
		_errorPage404				= other._errorPage404;
		_sockfd						= other._sockfd;
		_servaddr.sin_family		= other._servaddr.sin_family;
		_servaddr.sin_addr.s_addr	= other._servaddr.sin_addr.s_addr;
		_servaddr.sin_port			= other._servaddr.sin_port;
		_locations					= other._locations;
	}
	return (*this);
}


/* ########## Setter/Getter ########## */

void	Server::setPort(const int port) {	_port = port;	}
const int	&Server::getPort(void) const {	return (_port);	}

void	Server::setHost(std::string host) {	_host = inet_addr(host.c_str());	}
const in_addr_t	&Server::getHost(void) const {	return (_host);	}

void	Server::setServerName(std::string &serverName) {	_serverName = serverName;	}
const std::string	&Server::getServerName() const {	return (_serverName);	}

void	Server::setRoot(std::string &root) {	_root = root;	}
const std::string	&Server::getRoot() const {	return (_root);	}

void	Server::setIndex(std::string &index) {	_index = index;	}
const std::string	&Server::getIndex() const {	return (_index);	}

void	Server::setErrorPage404(std::string &errorPage404) {	_errorPage404 = errorPage404;	}
const std::string	&Server::getErrorPage404() const {	return (_errorPage404);	}

const int	&Server::getSockfd(void) const {	return (_sockfd);	}

const sockaddr_in	&Server::getServaddr(void) const {	return (_servaddr);	}

void	Server::setLocations(std::vector<Location> &locations) {	_locations = locations;	}
const std::vector<Location>	&Server::getLocations() const {	return (_locations);	}


/* ########## Member function ########## */

void	Server::_setupServAddr(void)
{
	std::memset(&_servaddr, 0, sizeof(_servaddr));
	_servaddr.sin_family = AF_INET;
	_servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	_servaddr.sin_port = htons(_port);
}

void	Server::printServerAttributes() const
{
	Logger::debug("--------------- SERVER INFOS ---------------");

	std::string	str = "Server name: " + _serverName + " | Port: " + Utils::IntToStr(_port);
	Logger::debug(str.c_str());

	char	hostIP[20] = "";
	inet_ntop(AF_INET, &(_host), hostIP, INET_ADDRSTRLEN);
	str = "Host: " + std::string(hostIP) + " | Root: " + _root;
	Logger::debug(str.c_str());

	str = "Index: " + _index + " | Error page 404: " + _errorPage404;
	Logger::debug(str.c_str());

	std::vector<Location>	locations = _locations;
	for (std::vector<Location>::iterator it = locations.begin() ; it != locations.end() ; it++)
	{
		Location location = *it;

		str = "--------------- Location " + location.getLocation();
		Logger::debug(str.c_str());
		
		str = "Root: " + location.getRoot() + " | Index: " + location.getIndex() + " | Auto-Index: ";
		if (location.getAutoIndex())
			str += "ON";
		else
			str += "OFF";
		Logger::debug(str.c_str());

		str = "Allow methods: ";
		std::vector<std::string> allowMethods = location.getAllowMethods();
		for (std::vector<std::string>::iterator ite = allowMethods.begin() ; ite != allowMethods.end() ; ite++)
			str += *ite += " ";
		Logger::debug(str.c_str());

		str = "CGI Path: ";
		std::vector<std::string> cgiPath = location.getCgiPath();
		for (std::vector<std::string>::iterator ite = cgiPath.begin() ; ite != cgiPath.end() ; ite++)
			str += *ite += " ";
		str += " | CGI Ext: ";
		std::vector<std::string> cgiExt = location.getCgiExt();
		for (std::vector<std::string>::iterator ite = cgiExt.begin() ; ite != cgiExt.end() ; ite++)
			str += *ite += " ";
		Logger::debug(str.c_str());
	}

	Logger::debug("");
}

bool	Server::setup(void)
{
	Logger::debug("Creating socket.");
	if ((_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		Logger::error("Failed to create socket.");
		return (false);
	}
	
	_setupServAddr();
	
	int option_value = 1;
    if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
		&option_value, sizeof(int)) < 0)
	{
		Logger::error("Failed to set socket options.");
		return (false);
	}
	
	std::ostringstream oss;
	oss << "Binding socket at port " << _port << ".";
	Logger::debug(oss.str().c_str());
	if (bind(_sockfd, (struct sockaddr *) &_servaddr, sizeof(_servaddr)) < 0)
	{
		Logger::error("Failed to bind socket.");
		return (false);
	}
	
	Logger::debug("Listen for connections.");
	if (listen(_sockfd, 512) < 0)
	{
		Logger::error("Failed to listen on socket.");
		return (false);
	}
	
	Logger::debug("Setting up socket in non-blocking mode.");
	if (fcntl(_sockfd, F_SETFL, O_NONBLOCK) < 0)
	{
		Logger::error("Failed to set socket to non-blocking mode.");
		return false;
	}
	
	return (true);
}


/* ########## Exception ########## */


/* ########## Non-member function ########## */
