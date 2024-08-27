#include "Server.hpp"



/* ################################## CONSTRUCTORS ################################## */

Server::Server(void) : _port(-1), _serverName(""), _root(""), _index(""), _maxBodySize(0),
			_errorPage400(""), _errorPage403(""), _errorPage404(""), _errorPage405(""),
			_errorPage408(""), _errorPage413(""), _errorPage500(""), _errorPage501(""),
			_errorPage502(""), _errorPage503(""), _errorPage504(""), _sockfd(-1) { }

Server::Server(const Server &copy)
{
	if (&copy != this)
		*this = copy;
}

Server::~Server(void)
{
	
}



/* ############################## OPERATOR'S OVERLOADS ############################## */

Server &Server::operator=(const Server &other)
{
	if (&other != this)
	{
		_port						= other._port;
		_serverName					= other._serverName;
		_host						= other._host;
		_root						= other._root;
		_index						= other._index;
		_maxBodySize				= other._maxBodySize;
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



/* ############################## GETTERS AND SETTERS ############################### */

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

void	Server::setMaxBodySize(size_t maxBodySize) {	_maxBodySize = maxBodySize;	}
size_t	Server::getMaxBodySize() const {	return (_maxBodySize);	}

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



/* ################################ MEMBER FUNCTIONS ################################ */

/*
 *	@brief Setups the server's adress structure.
*/
void	Server::_setupServAddr(void)
{
	std::memset(&_servaddr, 0, sizeof(_servaddr));
	_servaddr.sin_family = AF_INET;
	_servaddr.sin_addr.s_addr = _host;
	_servaddr.sin_port = htons(_port);
}

/*
 *	@brief Displays a line formatted to start and end with '*' and to be centered.
 *	@param text The text to display.
 *	@param fillWithStars Set on TRUE if you want to fill the line with '*' instead of spaces. Optional, defaults to FALSE.
 *	@param fieldWidth The length of the resulted line. Optional, defaults to 70 characters.
*/
void Server::_printFormattedLine(const std::string &text, bool fillWithStars, unsigned int fieldWidth) const
{
    int totalWidth = fieldWidth - 2;
    int textLength = text.length();

	if (textLength > totalWidth)
	{
		_printFormattedLine(text.substr(0, totalWidth - 4));
		_printFormattedLine(text.substr(totalWidth - 3));
		return ;
	}
    int leftPadding = (totalWidth - textLength) / 2;
	int	rightPadding = (totalWidth - textLength) - leftPadding;

    std::string formattedText = "*";
	if (fillWithStars)
	{
		formattedText.append(leftPadding, '*');
		formattedText += text;
		formattedText.append(rightPadding + 1, '*');
	}
	else
	{
		formattedText.append(leftPadding, ' ');
		formattedText += text;
		formattedText.append(rightPadding, ' ');
		formattedText += "*";
	}

    Logger::info(formattedText.c_str());
}

/*
 *	@bried Prints Server's attributes.
*/
void	Server::printServerAttributes() const
{
	Logger::info("");
	_printFormattedLine("", true);
	_printFormattedLine("SERVER INFOS");
	_printFormattedLine("", true);

	_printFormattedLine("");
	_printFormattedLine("Server name: " + _serverName);
	_printFormattedLine("Port: " + Utils::IntToStr(_port));

	char	hostIP[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(_host), hostIP, INET_ADDRSTRLEN);
	_printFormattedLine("Host: " + std::string(hostIP));

	_printFormattedLine("Root: " + _root);

	std::string indexInfo = "Index: " + _index;
	_printFormattedLine(indexInfo);

	std::vector<std::string> errorPages;
	if (!_errorPage400.empty()) errorPages.push_back(_errorPage400);
	if (!_errorPage403.empty()) errorPages.push_back(_errorPage403);
	if (!_errorPage404.empty()) errorPages.push_back(_errorPage404);
	if (!_errorPage405.empty()) errorPages.push_back(_errorPage405);
	if (!_errorPage408.empty()) errorPages.push_back(_errorPage408);
	if (!_errorPage413.empty()) errorPages.push_back(_errorPage413);
	if (!_errorPage500.empty()) errorPages.push_back(_errorPage500);
	if (!_errorPage501.empty()) errorPages.push_back(_errorPage501);
	if (!_errorPage502.empty()) errorPages.push_back(_errorPage502);
	if (!_errorPage503.empty()) errorPages.push_back(_errorPage503);
	if (!_errorPage504.empty()) errorPages.push_back(_errorPage504);

	std::string errorPagesStr = "Error pages: ";
	for (size_t i = 0; i < errorPages.size(); ++i)
	{
		if (i > 0) errorPagesStr += ", ";
		errorPagesStr += errorPages[i];
	}
	_printFormattedLine(errorPagesStr);

	std::vector<Location> locations = _locations;
	for (std::vector<Location>::iterator it = locations.begin() ; it != locations.end() ; it++)
	{
		Location location = *it;

		_printFormattedLine("");
		_printFormattedLine(" Location '" + location.getLocation() + "' ", true);
		_printFormattedLine("");

		if (!location.getRoot().empty())
			_printFormattedLine("Root: " + location.getRoot());

		if (!location.getIndex().empty())
			_printFormattedLine("Index: " + location.getIndex());

		std::string autoIndexStr = "Auto-index: " + std::string(location.getAutoIndex() ? "ON" : "OFF");
		_printFormattedLine(autoIndexStr);

		std::string					allowMethodsStr = "Allow methods: ";
		std::vector<std::string>	allowMethods = location.getAllowMethods();
		for (std::vector<std::string>::iterator ite = allowMethods.begin() ; ite != allowMethods.end() ; ite++)
			allowMethodsStr += *ite + " ";
		_printFormattedLine(allowMethodsStr);

		std::vector<std::string>	cgiPath = location.getCgiPath();
		if (!cgiPath.empty())
		{
			std::string cgiPathStr = "CGI Paths: ";
			for (std::vector<std::string>::iterator ite = cgiPath.begin() ; ite != cgiPath.end() ; ite++)
				cgiPathStr += *ite + " ";
			_printFormattedLine(cgiPathStr);
		}

		std::vector<std::string>	cgiExt = location.getCgiExt();
		if (!cgiExt.empty())
		{
			std::string cgiExtStr = "CGI Ext: ";
			for (std::vector<std::string>::iterator ite = cgiExt.begin() ; ite != cgiExt.end() ; ite++)
				cgiExtStr += *ite + " ";
			_printFormattedLine(cgiExtStr);
		}
	}

	_printFormattedLine("");
	_printFormattedLine("", true);
	Logger::info("");
}

/*
 *	@brief Setups a server by creating socket and setting up the server's adress.
 *	@return TRUE in case of success, FALSE in case of error.
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
    if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, 
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
