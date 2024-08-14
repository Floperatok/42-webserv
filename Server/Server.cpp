
#include "Server.hpp"


/* ########## Constructor ########## */

Server::Server(void)
{
	_port = -1;
	_sockfd = -1;
	_serverName = "";
	_root = "";
	_index = "";
	_errorPage400 = "";
	_errorPage403 = "";
	_errorPage404 = "";
	_errorPage405 = "";
	_errorPage408 = "";
	_errorPage413 = "";
	_errorPage500 = "";
	_errorPage501 = "";
	_errorPage502 = "";
	_errorPage503 = "";
	_errorPage504 = "";
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
		_port						= other._port;
		_serverName					= other._serverName;
		_host						= other._host;
		_root						= other._root;
		_index						= other._index;
		_errorPage400				= other._errorPage400;
		_errorPage403				= other._errorPage403;
		_errorPage404				= other._errorPage404;
		_errorPage405				= other._errorPage405;
		_errorPage408				= other._errorPage408;
		_errorPage413				= other._errorPage413;
		_errorPage500				= other._errorPage500;
		_errorPage501				= other._errorPage501;
		_errorPage502				= other._errorPage502;
		_errorPage503				= other._errorPage503;
		_errorPage504				= other._errorPage504;
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

void	Server::setErrorPage400(std::string &errorPage400) {	_errorPage400 = errorPage400;	}
const std::string	&Server::getErrorPage400() const {	return (_errorPage400);	}

void	Server::setErrorPage403(std::string &errorPage403) {	_errorPage403 = errorPage403;	}
const std::string	&Server::getErrorPage403() const {	return (_errorPage403);	}

void	Server::setErrorPage404(std::string &errorPage404) {	_errorPage404 = errorPage404;	}
const std::string	&Server::getErrorPage404() const {	return (_errorPage404);	}

void	Server::setErrorPage405(std::string &errorPage405) {	_errorPage405 = errorPage405;	}
const std::string	&Server::getErrorPage405() const {	return (_errorPage405);	}

void	Server::setErrorPage408(std::string &errorPage408) {	_errorPage408 = errorPage408;	}
const std::string	&Server::getErrorPage408() const {	return (_errorPage408);	}

void	Server::setErrorPage413(std::string &errorPage413) {	_errorPage413 = errorPage413;	}
const std::string	&Server::getErrorPage413() const {	return (_errorPage413);	}

void	Server::setErrorPage500(std::string &errorPage500) {	_errorPage500 = errorPage500;	}
const std::string	&Server::getErrorPage500() const {	return (_errorPage500);	}

void	Server::setErrorPage501(std::string &errorPage501) {	_errorPage501 = errorPage501;	}
const std::string	&Server::getErrorPage501() const {	return (_errorPage501);	}

void	Server::setErrorPage502(std::string &errorPage502) {	_errorPage502 = errorPage502;	}
const std::string	&Server::getErrorPage502() const {	return (_errorPage502);	}

void	Server::setErrorPage503(std::string &errorPage503) {	_errorPage503 = errorPage503;	}
const std::string	&Server::getErrorPage503() const {	return (_errorPage503);	}

void	Server::setErrorPage504(std::string &errorPage504) {	_errorPage504 = errorPage504;	}
const std::string	&Server::getErrorPage504() const {	return (_errorPage504);	}

const int	&Server::getSockfd(void) const {	return (_sockfd);	}

const sockaddr_in	&Server::getServaddr(void) const {	return (_servaddr);	}

void	Server::setLocations(std::vector<Location> &locations) {	_locations = locations;	}
const std::vector<Location>	&Server::getLocations() const {	return (_locations);	}


/* ########## Member functions ########## */

/*
 *	@brief Setups the server's adress structure.
*/
void	Server::_setupServAddr(void)
{
	std::memset(&_servaddr, 0, sizeof(_servaddr));
	_servaddr.sin_family = AF_INET;
	_servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	_servaddr.sin_port = htons(_port);
}

/*
 *	@bried Prints Server's attributes.
*/
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

/*
 *	@brief Setups a server by creating socket and setting up the server's adress.
*/
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
