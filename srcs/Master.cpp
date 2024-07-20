
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


/* ########## Member function ########## */

void	Master::_initFds(void)
{
	for (int i = 0; i < MAX_CLIENT; i++)
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
int	Master::_createClientSocket(const Server &server)
{
	int	clientSocket;
	if ((clientSocket = accept(server.getPort(), 
		(struct sockaddr *)&server.getServaddr(), 
		(socklen_t *)sizeof(server.getServaddr()))) < 0)
	{
		return (-1);
	}
	_storeFd(clientSocket, POLLIN | POLLOUT);
	return (0);
}

void	Master::setupServers(void)
{
	std::vector<Server>::iterator	it = _servers.begin();
	std::vector<Server>::iterator	ite = _servers.end();

	for ( ; it != ite; it++)
	{
		it->setup();
	}
}

void	Master::runServers(void)
{
	int	newSocket;
	std::vector<Server>::iterator	it = _servers.begin();
	std::vector<Server>::iterator	ite = _servers.end();
	int				i = 0;

	_initFds();
	while (1)
	{
		if (poll(_fds, MAX_CLIENT, TIMEOUT) < 0)
			throw (FunctionError("poll", errno));
		for (int i = 0; i < _nbSocket; i++)
		{
			if (_fds[i].revents & POLLIN)
			{
				if (_createClientSocket(_servers[i]) < 0)
				{
					std::cerr << "Function accept failed with error: " << strerror(errno) << std::endl;
					continue ;
				}
			}
		}
	}
}


/* ########## Exception ########## */

Master::FunctionError::FunctionError(const char *funcName, const int errnum)
{
	std::ostringstream oss;

	oss << "Function " << funcName << " failed with error: " << strerror(errnum);
	_message = oss.str();
}
Master::FunctionError::~FunctionError(void) throw()
{
}
const char *Master::FunctionError::what(void) const throw()
{
	return (_message.c_str());
}


/* ########## Non-member function ########## */

