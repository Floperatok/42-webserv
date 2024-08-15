#include "Master.hpp"



/* ################################## CONSTRUCTORS ################################## */

Master::Master() : _nbServers(0), _nfds(0) {}

Master::Master(std::vector<Server> &servers)
	:	_servers(servers), _nbServers(servers.size()) {}

Master::Master(const Master &copy)
{
	if (&copy != this)
		*this = copy;
}

Master::~Master(void) {}



/* ############################## OPERATOR'S OVERLOADS ############################## */

Master &Master::operator=(const Master &other)
{
	if (&other != this)
	{
		_servers = other._servers;
		_nbServers = other._nbServers;
		_nfds = other._nfds;
		_initFds();
	}
	return (*this);
}



/* ############################## GETTERS AND SETTERS ############################### */

void	Master::setServers(std::vector<Server> servers) {	_servers = servers;	}



/* ################################ MEMBER FUNCTIONS ################################ */

/*
 *	@brief Setups all serveurs by calling their setup function.
*/
void	Master::setupServers(void)
{
	std::vector<Server>::iterator	it = _servers.begin();
	std::vector<Server>::iterator	ite = _servers.end();

	for ( ; it != ite; it++)
	{
		Logger::info("Setting up server at port " + Utils::IntToStr(it->getPort()) + ".");
		
		if (!it->setup())
		{
			Logger::error("Server setup failed.");
            Response::InternalServerError500(it->getSockfd(), *it);
		}
	}
	_nbServers = _servers.size();
}

/*
 *	@brief Initializes the pollfd structures.
*/
void	Master::_initFds(void)
{
	Logger::debug("Fds initialization...");
	_nfds = _nbServers;
	for (size_t i = 0; i < MAX_CLIENT; i++)
	{
		if (i < _servers.size())
		{
			if (_servers[i].getSockfd() < 0)
			{
				Logger::error("Invalid server socket file descriptor: " + Utils::IntToStr(_servers[i].getSockfd()) + ".");
                continue;
			}
			_fds[i].fd = _servers[i].getSockfd();
			_fds[i].events = POLLIN;
			_fds[i].revents = 0;
		}
		else
			_fds[i].fd = -1;
	}
}

/*
 *	@brief Stores a socket's file descriptor and it's event into the array of pollfd structures.
 *	@param fd The socket's file descriptor to store.
 *	@param server The server corresponding to the client concerned.
 *	@param events The event to store.
*/
void	Master::_storeFd(int fd, const Server &server, const short events)
{
	Logger::debug("Storing client socket_fd " + Utils::IntToStr(fd) + " in the fds array.");
	if (fd < 0)
	{
		Logger::error("Attempted to store an invalid file descriptor: " + Utils::IntToStr(fd) + ".");
		return ;
	}
	int i = 0;
	while (i < MAX_CLIENT && _fds[i].fd != -1)
		i++;
	if (i == MAX_CLIENT)
	{
		Response::ServiceUnavailable503(fd, server);
		_RemoveFd(i);
		Logger::error("Max client limit reached, connection closed.");
	}
	_fds[i].fd = fd;
	_fds[i].events = events;
	_fds[i].revents = 0;
	_nfds++;
}

/*
 *	@brief Creates a socket for a client.
 *	@param server The server on which the client wants to connect.
 *	@return The file descriptor of the created socket, or -1 in case of error.
*/
int	Master::_createClientSocket(Server &server)
{
	int					clientSocket;
	struct sockaddr_in	servaddr = server.getServaddr();
	socklen_t			servaddrLen = sizeof(servaddr);

	if ((clientSocket = accept(server.getSockfd(), 
		(struct sockaddr *)&servaddr,
		&servaddrLen)) < 0)
	{
		Logger::warning("Failed to accept the socket binded on the port " + Utils::IntToStr(server.getPort()) + ".");
		Response::ServiceUnavailable503(server.getSockfd(), server);
		return (-1);
	}
	Logger::info("New client's socket created on port " + Utils::IntToStr(ntohs(server.getServaddr().sin_port)) \
				+ ", socket_fd: " + Utils::IntToStr(clientSocket) + ".");
	return (clientSocket);
}

/*
 *	@brief Gets the content of a request from a socket.
 *	@param sockfd The socket's file descriptor.
 *	@param receivedData A string where the request's content will be appended.
 *	@return 1 if the request was fully received, 0 if it was received partially, -1 in case of error.
*/
int	Master::_readSocket(const int sockfd, std::string &receivedData)
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
	receivedData.append(buffer, bytesread);
	totalBytesRead += bytesread;
	size_t headersEndPos = receivedData.find("\r\n\r\n");

	if (!headersReceived && headersEndPos != std::string::npos)
	{
		headersReceived = true;

		size_t contentLengthPos = receivedData.find("Content-Length: ");
		if (contentLengthPos != std::string::npos)
		{
			size_t contentLengthStart = contentLengthPos + 16;
			size_t contentLengthEnd = receivedData.find("\r\n", contentLengthStart);
			contentLength = Utils::StrToInt(receivedData.substr(contentLengthStart, contentLengthEnd - contentLengthStart));
		}
	}
	if (headersReceived && receivedData.size() >= headersEndPos + 4 + contentLength)
	{
		Logger::info("Request fully received from socket " + Utils::IntToStr(sockfd) + ".");
		// Logger::debug("Received request:\n'" + receivedData + "'");
		return (1);
	}
	// Logger::debug("Request received but not complete:\n" + receivedData + "'");
	return (0);
}

/*
 *	@brief Checks for new connections on each server. If a new connection is found, 
 *	creates a new client's socket and updates the array of pollfd structures.
*/
void	Master::_checkServersConnections(void)
{
	for (size_t i = 0; i < _nbServers; i++)
	{
		if (_fds[i].revents == 0)
			continue ;
		if (_fds[i].revents & POLLIN)
		{
			Logger::debug("New connection on the server's socket binded to the port " \
						+ Utils::IntToStr(_servers[i].getPort()) + ".");

			int clientSocket = _createClientSocket(_servers[i]);
			if ((clientSocket < 0))
			{
				Logger::error("Failed to create a client socket.");
				continue ;
			}
			_storeFd(clientSocket, _servers[i], POLLIN);
		}
	}
}

/*
 *	@brief Checks for clients' requests by checking poll events. If the event is POLLIN, 
 *	reads the socket's file descriptor. When socket is fully read, sends response.
 *	@param env The environnement to use for CGI Scripts.
 *	@param i A pointer to the client's index.
*/
void	Master::_manageClientsRequests(char **env, size_t *i)
{
	if (*i >= _nfds)
		*i = _nbServers;

	for (; *i < _nfds ; (*i)++)
	{
		if (_fds[*i].revents == 0)
			continue ;
		if (_fds[*i].revents & POLLERR)
		{
			Logger::error("Socket " + Utils::IntToStr(_fds[*i].fd) + " has POLLERR in revents. Revents is " \
						+ Utils::IntToStr(_fds[*i].revents) + ".");
			_RemoveFd(*i);
			i--;
			continue ;

		}
		if (_fds[*i].revents & POLLHUP)
		{
			Logger::warning("Connection suspended at socket " + Utils::IntToStr(_fds[*i].fd) + ".");
			_RemoveFd(*i);
			i--;
			continue ;
		}
		if (_fds[*i].revents & POLLIN)
		{
			Logger::debug("Reading socket " + Utils::IntToStr(_fds[*i].fd) + "...");

			int	r = _readSocket(_fds[*i].fd, _requests[*i]);
			if (r < 0)
			{
				_RemoveFd(*i);
				(*i)--;
				return ;
			}
			else if (r == 0)
			{
				(*i)--;
				return ;
			}
			
			int	statusCode = Response::SendResponse(_servers, _fds[*i].fd, _requests[*i], env);
			if (statusCode != 200)
				Logger::error("Failed to send response to the client. Status code: " \
								+ Utils::IntToStr(statusCode) + ".");
			_requests[*i] = "";
			return ;
		}
	}
}

/*
 *	@brief Runs servers with a poll waiting for new connections or requests.
 *	@param env The environnement to use for CGI Scripts.
*/
void	Master::runServers(char **env)
{
	size_t	i = _nbServers;

	Logger::info("Launching servers");
	_initFds();
	Logger::info("Waiting for connections...");
	while (1)
	{
		// _displayInfos(); // debug tool
		int rc = poll(_fds, _nfds, -1);

		if (rc < 0)
		{
			Logger::error("Poll error.");
			continue ;
		}
		if (rc == 0)
		{
			Logger::warning("Poll timed out.");
			continue ;
		}
		_checkServersConnections();
		_manageClientsRequests(env, &i);
	}
}

/*
 *	@brief Debug tool to display informations about servers and clients.
*/
void	Master::_displayInfos(void) const
{	
	std::cout << "number of servers: " << _nbServers << std::endl;
	std::cout << "number of clients: " << _nfds - _nbServers << std::endl;
	std::cout << "fds: " << _nfds << std::endl;
	for (size_t i = 0; i < _nbServers; i++)
		std::cout << "\tfds[" << i << "] = " << _fds[i].fd << " (server)" << std::endl;
	for (size_t i = _nbServers; i < _nfds; i++)
		std::cout << "\tfds[" << i << "] = " << _fds[i].fd << " (client)" << std::endl;
	
}

/*
 *	@brief Removes a socket from the array of pollfd structures 
 *	and updates the array by shifting all elements.
 *	@param index The index of the socket to remove.
*/
void Master::_RemoveFd(unsigned int index)
{
	if (index >= _nfds || _fds[index].fd == -1)
		return ;
	
	Logger::info("Closing socket " + Utils::IntToStr(_fds[index].fd) + ".");
	
	close(_fds[index].fd);

	for (unsigned int i = index; i < _nfds - 1; i++)
		_fds[i] = _fds[i + 1];

	_fds[_nfds - 1].fd = -1;
	_fds[_nfds - 1].events = 0;
	_fds[_nfds - 1].revents = 0;
	_nfds--;
}

/*
 *	@brief Checks if keep-alive parameter is active in a request.
 *	@param request The request to check.
 *	@return TRUE if keep-alive is active, FALSE if not.
*/
bool Master::_isKeepAlive(const std::string &request)
{
    std::string::size_type pos = request.find("Connection: ");

    if (pos != std::string::npos)
    {
        size_t end = request.find("\r\n", pos);
        std::string connectionValue = request.substr(pos + 12, end - (pos + 12));
        return (connectionValue == "keep-alive");
    }
    return (false);
}
