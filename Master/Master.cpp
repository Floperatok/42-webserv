
#include "Master.hpp"


/* ########## Constructor ########## */

Master::Master(std::vector<Server> &servers)
	:	_servers(servers), _nbSocket(servers.size())
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

void	Master::_initFds(void)
{
	Logger::debug("Fds initialization");
	for (size_t i = 0; i < MAX_CLIENT; i++)
	{
		if (i < _servers.size() - 1)
		{
			_fds[i].fd = _servers[i].getPort();
			_fds[i].events = POLLIN | POLLOUT;
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
void Master::_storeFd(int fd, const short events)
{
	if (fd < 0)
		return ;
	int i = 0;
	while (i < MAX_CLIENT && _fds[i].fd != -1)
		i++;
	if (i == MAX_CLIENT)
	{
		Response::serviceUnavailable(fd);
		close(fd);
	}
	_fds[i].fd = fd;
	_fds[i].events = events;
}

/*
 *	Creates new client socket and stores it in the array struct pollfd _fds.
 *	Returns -1 in case of error.
*/
int	Master::_createClientSocket(Server &server)
{
	int	clientSocket;
	struct sockaddr_in servaddr = server.getServaddr();
	if ((clientSocket = accept(server.getPort(), 
		(struct sockaddr *)&servaddr,
		(socklen_t *)sizeof(servaddr))) < 0)
	{
		std::ostringstream oss;
		oss << "Failed to accept to connect the socket binded to the port " 
			<< server.getPort() << " with error: " << strerror(errno);
		Logger::warning(oss.str().c_str());
		return (-1);
	}
	std::ostringstream oss;
	oss << "New connection on port " << ntohs(server.getServaddr().sin_port)
		<< ", socket fd: " << clientSocket
		<< std::endl;
	Logger::info(oss.str().c_str());
	return (clientSocket);
}

int	Master::_readSocket(const int sockfd)
{
	char buff[BUFFER_SIZE];
	int bytesread = read(sockfd, buff, BUFFER_SIZE);
	if (bytesread < 0)
		throw (Logger::FunctionError("read", -1));
	if (bytesread == 0)
	{
		std::ostringstream oss;
		oss << "Client disconnected from socket " << sockfd;
		Logger::info(oss.str().c_str());
		return (-1);
	}
	buff[bytesread] = '\0';
	std::ostringstream oss;
	oss << "Received message: " << buff;
	Logger::info(oss.str().c_str());
	return (0);
}

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

void	Master::runServers(void)
{
	Logger::info("Running servers");
	_initFds();
	while (1)
	{
		if (poll(_fds, MAX_CLIENT, -1) < 0) // use nfds instead of MAX_CLIENT
			throw (Logger::FunctionError("poll", errno));
		
		// check for new connection on any server socket
		for (size_t i = 0; i < _nbSocket; i++)
		{
			if (_fds[i].revents & POLLIN)
			{
				int clientSocket;
				if ((clientSocket = _createClientSocket(_servers[i]) < 0))
					continue ;
				_storeFd(clientSocket, POLLIN | POLLOUT);
			}
		}

		// Check all clients for read / write readiness
		for (int i = _nbSocket; i < MAX_CLIENT; i++) // use nfds instead of MAX_CLIENT
		{
			if (_fds[i].fd == -1)
				continue ;

			// If fd is ready to be read
			if (_fds[i].revents & POLLIN)
			{
				if (_readSocket(_fds[i].fd) < 0)
				{
					close(_fds[i].fd);
					_fds[i].fd = -1;
				}
				else
					_fds[i].events |= POLLOUT;
			}

			// If fd is ready to be written
			if (_fds[i].revents & POLLOUT)
			{
				std::string hello = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 12\r\n\r\nHello world";
				write(_fds[i].fd, hello.c_str(), hello.size());
			}
		}
	}
}


/* ########## Exception ########## */


/* ########## Non-member function ########## */
