
#include "Master.hpp"


/* ########## Constructor ########## */
Master::Master() : _nbServers(0), _nfds(0)
{

}

Master::Master(std::vector<Server> &servers)
	:	_servers(servers), _nbServers(servers.size())
{
}

Master::Master(const Master &copy)
{
	if (&copy != this)
		*this = copy;
}

Master::~Master(void)
{
}


/* ########## Operator overload ########## */

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


/* ########## Setter/Getter ########## */

void	Master::setServers(std::vector<Server> servers)
{
	_servers = servers;
}


/* ########## Member function ########## */

void	Master::setupServers(void)
{
	std::vector<Server>::iterator	it = _servers.begin();
	std::vector<Server>::iterator	ite = _servers.end();

	for ( ; it != ite; it++)
	{
		std::ostringstream oss;
		oss << "Setting up server at port " << it->getPort();
		Logger::info(oss.str().c_str());
		
		if (!it->setup())
		{
			Logger::error("Server setup failed.");
            Response::InternalServerError500(it->getSockfd());
		}
	}
	_nbServers = _servers.size();
}

void	Master::_initFds(void)
{
	Logger::debug("Fds initialization");
	_nfds = _nbServers;
	for (size_t i = 0; i < MAX_CLIENT; i++)
	{
		if (i < _servers.size())
		{
			if (_servers[i].getSockfd() < 0)
			{
				Logger::error(("Invalid server socket file descriptor: " + Utils::IntToStr(_servers[i].getSockfd())).c_str());
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
 *	Stores fd in the array struct pollfd _fds.
 *	Send a "Service Unavailable" response and close the connection
 *		if no client slot are available.
*/
void	Master::_storeFd(int fd, const short events)
{
	std::ostringstream oss;
	oss << "Storing client socket_fd " << fd << " in the fds array";
	Logger::debug(oss.str().c_str());
	if (fd < 0)
	{
		Logger::error(("Attempted to store an invalid file descriptor: " + Utils::IntToStr(fd)).c_str());
		return ;
	}
	int i = 0;
	while (i < MAX_CLIENT && _fds[i].fd != -1)
		i++;
	if (i == MAX_CLIENT)
	{
		Response::ServiceUnavailable503(fd);
		_RemoveFd(i);
		Logger::error("Max client limit reached, connection closed.");
	}
	_fds[i].fd = fd;
	_fds[i].events = events;
	_fds[i].revents = 0;
	_nfds++;
}

/*
 *	Squeezes together the array and decrements the number of file descriptors.
*/
void	Master::_compressArray(void)
{
	for (size_t i = 0; i < _nfds; i++)
	{
		if (_fds[i].fd == -1)
		{
			for (size_t j = i; j < _nfds - 1; j++)
				_fds[j].fd = _fds[j + 1].fd;
			i--;
			_nfds--;
		}
	}
}

/*
 *	Creates new client socket and returns it.
 *	Returns -1 in case of error.
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
		std::ostringstream oss;
		oss << "Failed to accept to connect the socket binded to the port " 
			<< server.getPort() << " with error: " << strerror(errno);
		Logger::warning(oss.str().c_str());
		Response::ServiceUnavailable503(server.getSockfd());
		return (-1);
	}
	std::ostringstream oss;
	oss << "New socket created for client connection on port " << ntohs(server.getServaddr().sin_port)
		<< ", socket_fd: " << clientSocket;
	Logger::info(oss.str().c_str());
	return (clientSocket);
}

/*
 *	Reads from the socket and stores the request in receivedData.
 *	Returns 0 if connection is closed, else 1.
*/
int Master::_readSocket(const int sockfd, std::string &receivedData)
{
    char			buffer[BUFFER_SIZE];
    ssize_t			bytesread;
	size_t			totalBytesRead = 0;
	size_t			contentLength = 0;
	bool			headersReceived = false;

    while (true)
	{
        bytesread = recv(sockfd, buffer, BUFFER_SIZE, MSG_DONTWAIT);
        if (bytesread < 0)
		{
			Logger::error(("Failed to read from socket " + Utils::IntToStr(sockfd) + ".").c_str());
            Response::ServiceUnavailable503(sockfd);
			return (0);
		}

        if (bytesread == 0) 
		{
			Logger::info(("Connection of the socket " + Utils::IntToStr(sockfd) + " closed.").c_str());
		    return (0);
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
			Logger::info(("Request received from socket " + Utils::IntToStr(sockfd) + ".").c_str());
			// Logger::debug(("Received request:\n'" + receivedData + "'").c_str());

			return (1);
        }
		else if (totalBytesRead >= MAX_REQUEST_SIZE)
        {
            Logger::error("Request size exceeds the maximum limit.");
			Response::ContentTooLarge413(sockfd);
            return (0);
        }
    }
}

/*
 *	check for new connection on any server socket
*/
void	Master::_checkServersConnections(void)
{
	for (size_t i = 0; i < _nbServers; i++)
	{
		if (_fds[i].revents == 0)
			continue ;
		if (_fds[i].revents & POLLIN)
		{
			std::ostringstream oss;
			oss << "New pending connection on the server socket binded to the port "
				<< _servers[i].getPort();
			Logger::debug(oss.str().c_str());

			int clientSocket = _createClientSocket(_servers[i]);
			if ((clientSocket < 0))
			{
				Logger::error("Failed to create a client socket.");
				continue ;
			}
			_storeFd(clientSocket, POLLIN);
		}
	}
}

void	Master::_manageClientsRequests(char **env)
{
	for (size_t i = _nbServers; i < _nfds; i++)
	{
		if (_fds[i].revents == 0)
			continue ;
		

		if (_fds[i].revents & POLLERR)
		{
			std::ostringstream oss;
			oss << "socket " << _fds[i].fd << " has POLLERR in revents. revents is " << _fds[i].revents << std::endl;
			Logger::error(oss.str().c_str());
		}
		if (_fds[i].revents & POLLHUP)
		{
			_RemoveFd(i);
			i--;
			continue ;
		}
		if (_fds[i].revents & POLLIN)
		{
			std::ostringstream oss;
			oss << "Reading socket " << _fds[i].fd << "...   revents is " << _fds[i].revents;
			Logger::debug(oss.str().c_str());

			std::string request("");
			if (!_readSocket(_fds[i].fd, request))
			{
				_RemoveFd(i);
				i--;
				continue ;
			}

			oss.str("");
			oss.clear();
			oss << "Sending response to socket_fd " << _fds[i].fd << "...";
			Logger::debug(oss.str().c_str());
			int	statusCode = Response::SendResponse(_servers, _fds[i].fd, request, env);
			if (statusCode != 200)
				Logger::error(("Failed to send response to the client. Status code: " \
								+ Utils::IntToStr(statusCode) + ".").c_str());
		}
	}
}

void	Master::runServers(char **env)
{
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
		_manageClientsRequests(env);
	}
}


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

void Master::_RemoveFd(unsigned int index)
{
	if (index >= _nfds || _fds[index].fd == -1)
		return ;
	std::ostringstream oss;
	oss << "Closing socket " << _fds[index].fd;
	Logger::info(oss.str().c_str());
	
	close(_fds[index].fd);

	for (unsigned int i = index; i < _nfds - 1; i++)
		_fds[i] = _fds[i + 1];

	_fds[_nfds - 1].fd = -1;
	_fds[_nfds - 1].events = 0;
	_fds[_nfds - 1].revents = 0;
	_nfds--;
}

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


/* ########## Exception ########## */


/* ########## Non-member function ########## */
