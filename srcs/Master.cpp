
#include "Master.hpp"


/* ########## Constructor ########## */

Master::Master(std::vector<Server> &servers)
	:	_servers(servers)
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
	struct pollfd	fds[_servers.size()];
	int				i = 0;

	for ( ; it != ite; it++)
	{
		it->listenSock();
		fds[i].fd = it->getPort();
		fds[i].events = POLLIN | POLLOUT;
		i++;
	}
	while (1)
	{
		if (poll(fds, _servers.size(), -1) < 0)
			throw (FunctionError("poll", errno));
		for (int i = 0; i < _servers.size(); i++)
		{
			if (fds[i].revents & POLLIN || fds[i].revents & POLLOUT)
			{
				if ((newSocket = accept(_servers[i].getSockfd(), 
								(struct sockaddr *)&_servers[i].getServaddr(), 
								(socklen_t*)sizeof(_servers[i].getServaddr()))) < 0)
				{
					std::cerr << "accept: " << strerror(errno) << std::endl;
					continue ;
				}

				std::cout	<< "New connection, socketfd: " << newSocket
							<< ", ip: " << inet_ntoa(_servers[i].getServaddr().sin_addr)
							<< ", port: " << ntohs(_servers[i].getServaddr().sin_port)
							<< std::endl;
							
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

