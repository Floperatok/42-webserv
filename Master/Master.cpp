
#include "Master.hpp"


/* ########## Constructor ########## */

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
		
	}
	return (*this);
}


/* ########## Setter/Getter ########## */
void	Master::setServers(std::vector<Server> &servers)
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

		it->setup();
	}
}

void	Master::_initFds(void)
{
	Logger::debug("Fds initialization");
	_nfds = _nbServers;
	for (size_t i = 0; i < MAX_CLIENT; i++)
	{
		if (i < _servers.size())
		{
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
		return ;
	int i = 0;
	while (i < MAX_CLIENT && _fds[i].fd != -1)
		i++;
	if (i == MAX_CLIENT)
	{
		Response::ServiceUnavailable503(fd);
		close(fd);
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
		return (-1);
	}
	std::ostringstream oss;
	oss << "New socket created for client connection on port " << ntohs(server.getServaddr().sin_port)
		<< ", socket_fd: " << clientSocket;
	Logger::info(oss.str().c_str());
	return (clientSocket);
}

std::string	Master::_readSocket(const int sockfd)
{
	std::string receivedData("");
	char buffer[BUFFER_SIZE];
	int bytesread;
	do
	{
		bytesread = recv(sockfd, buffer, BUFFER_SIZE, MSG_DONTWAIT);
		if (bytesread < 0)
			throw (Logger::FunctionError("read", -1));
		receivedData += buffer;
		if (!std::strcmp(&buffer[bytesread - 4], "\r\n\r\n"))
		{
			std::ostringstream oss;
			oss << "Request received from socket " << sockfd;
			Logger::info(oss.str().c_str());
			oss.str("");
			oss.clear();
			oss << "Received request: \n\"" << receivedData << "\"";
			Logger::debug(oss.str().c_str());
			return (receivedData);
		}
	} while (1);
}

void	Master::_sendResponse(const int sockfd, const std::string &request)
{
	(void)request;
	// std::string placeholder = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 11\r\n\r\nHello world";
	// write(sockfd, placeholder.c_str(), placeholder.size());

	Response::BadRequest400(sockfd);

	// std::ostringstream oss;
	// oss << "Response send: \n\"" << placeholder << "\"";
	// Logger::info(oss.str().c_str());
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
		if (_fds[i].revents != POLLIN)
			Logger::error("revent is not POLLIN");
		if (_fds[i].revents & POLLIN)
		{
			std::ostringstream oss;
			oss << "New pending connection on the socket binded to the port "
				<< _servers[i].getPort();
			Logger::debug(oss.str().c_str());

			int clientSocket = _createClientSocket(_servers[i]);
			if ((clientSocket < 0))
				continue ;
			_storeFd(clientSocket, POLLIN);
		}
	}
}

void	Master::_manageClientsRequests(void)
{
	for (size_t i = _nbServers; i < _nfds; i++)
	{
		if (_fds[i].revents == 0)
			continue ;
		else if (_fds[i].revents != POLLIN)
		{
			Logger::error("revent is not POLLIN");
			return ;
		}
		else
		{
			std::ostringstream oss;
			oss << "Reading socket " << _fds[i].fd << "...";
			Logger::debug(oss.str().c_str());

			std::string request = _readSocket(_fds[i].fd);

			oss.str("");
			oss.clear();
			oss << "Sending response to socket_fd " << _fds[i].fd << "...";
			Logger::debug(oss.str().c_str());

			_sendResponse(_fds[i].fd, request);
		}
	}
}

void	Master::runServers(void)
{
	Logger::info("Launching servers");
	_initFds();
	Logger::info("Waiting for connections...");
	while (1)
	{
		// _displayInfos(); // debug tool
		if (poll(_fds, _nfds, -1) < 0)
			throw (Logger::FunctionError("poll", errno));

		_checkServersConnections();
		
		_manageClientsRequests();

		_compressArray();
	}
}


void	Master::_displayInfos(void) const
{	
	std::cout << "number of servers: " << _nbServers << std::endl;
	std::cout << "number of clients: " << _nfds - _nbServers << std::endl;
	std::cout << "fds: " << _nfds << std::endl;
	for (size_t i = 0; i < _nfds; i++)
	{
		std::cout << "\tfds[" << i << "] = " << _fds[i].fd << std::endl;
	}
	
}


/* ########## Exception ########## */


/* ########## Non-member function ########## */
