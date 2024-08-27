#include "Master.hpp"



/* ################################## CONSTRUCTORS ################################## */

Master::Master() : _nbServers(0) {}

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
		_clients = other._clients;
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
bool	Master::setupServers(void)
{
	std::vector<Server>::iterator	it = _servers.begin();
	std::vector<Server>::iterator	ite = _servers.end();
	std::vector<std::string> emptyVector;
	for ( ; it != ite; it++)
	{
		Logger::info("Setting up server at port " + Utils::IntToStr(it->getPort()) + ".");
		
		if (!it->setup())
		{
			Logger::error("Server setup failed.");
			return (false);
		}
	}

	_nbServers = _servers.size();
	return (true);
}

/*
 *	@brief Initializes the pollfd structures.
*/
void	Master::_initFds(void)
{
	Logger::debug("Fds initialization...");
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
void	Master::_storeFd(Client &client, const short events)
{
	std::vector<std::string> emptyVector;
	Logger::debug("Storing client socket_fd " + Utils::IntToStr(client.sockfd) + " in the fds array.");
	if (client.sockfd < 0)
	{
		Logger::error("Attempted to store an invalid file descriptor: " + Utils::IntToStr(client.sockfd) + ".");
		return ;
	}
	int i = 0;
	while (i < MAX_CLIENT && _fds[i].fd != -1)
		i++;
	if (i == MAX_CLIENT)
	{
		Response::ServiceUnavailable503(client.sockfd, *client.server, emptyVector);
		_RemoveFd(i);
		Logger::error("Max client limit reached, connection closed.");
	}
	_clients[i] = client;
	_fds[i].fd = client.sockfd;
	_fds[i].events = events;
	_fds[i].revents = 0;
}

/*
 *	@brief Removes a socket from the array of pollfd structures 
 *	and updates the array by shifting all elements.
 *	@param index The index of the socket to remove.
*/
void Master::_RemoveFd(unsigned int index)
{
	if (_fds[index].fd == -1)
		return ;
	
	Logger::info("Closing socket " + Utils::IntToStr(_fds[index].fd) + ".");
	
	close(_fds[index].fd);

	_fds[index].fd = -1;
	_fds[index].events = 0;
	_fds[index].revents = 0;

	_clients.erase(index);
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

			Client client(&_servers[i]);
			if (client.sockfd < 0)
				continue ;
			_storeFd(client, POLLIN);
		}
	}
}

/*
 *	@brief Checks for clients' requests by checking poll events. If the event is POLLIN, 
 *	reads the socket's file descriptor. When socket is fully read, sends response.
 *	@param env The environnement to use for CGI Scripts.
 *	@param i A pointer to the client's index.
*/
void	Master::	_manageClientsRequests(char **env, size_t *i)
{
	if (*i >= MAX_CLIENT)
		*i = _nbServers;
	if (*i < _nbServers)
		*i = _nbServers;
	for (; *i < MAX_CLIENT ; (*i)++)
	{
		if (_fds[*i].revents == 0)
			continue ;
		if (_fds[*i].revents & POLLERR)
		{
			Logger::warning("Socket " + Utils::IntToStr(_fds[*i].fd) + " has POLLERR in revents. Revents is " \
						+ Utils::IntToStr(_fds[*i].revents) + ".");
			_RemoveFd(*i);
			continue ;

		}
		if (_fds[*i].revents & POLLHUP)
		{
			Logger::warning("Connection suspended at socket " + Utils::IntToStr(_fds[*i].fd) + ".");
			_RemoveFd(*i);
			continue ;
		}
		if (_fds[*i].revents & POLLIN)
		{
			int	r = _clients[*i].readClientRequest();
			if (r < 0)
			{
				_RemoveFd(*i);
				return ;
			}
			else if (r == 0)
			{
				(*i)--;
				return ;
			}

			std::cout << _clients[*i].request << std::endl;

			/* cookies */
			std::string cookies = extractCookies(_clients[*i].request);
			std::string sessionID = getCookieValue(cookies, "session_id");
			if (sessionID.length() != 6)
				sessionID = generateRandomSessionID();
			if (sessionID.empty())
				Logger::debug("No session ID found in the request.");

			int	statusCode = Response::SendResponse(_clients[*i], env, sessionID);
			if (statusCode != 200 && statusCode != 301 && statusCode != 302)
			{
				Logger::error("Failed to send response to the client. Status code: "
								+ Utils::IntToStr(statusCode) + ".");
			}
			_RemoveFd(*i);
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
		int rc = poll(_fds, MAX_CLIENT, -1);

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
void	Master::_displayInfos(void)
{
	int	nbClients = 0;
	for (size_t i = 0; i < _nbServers; i++)
		std::cout << "\tfds[" << i << "] = " << _fds[i].fd << " (server)" << std::endl;
	for (size_t i = _nbServers; i < MAX_CLIENT; i++)
	{
		if (_fds[i].fd != -1)
		{
			std::cout << "\tfds[" << i << "].fd = " << _fds[i].fd << " (client)" << std::endl;
			nbClients++;
		}
	}
	std::cout << "number of servers: " << _nbServers << std::endl;
	std::cout << "number of clients: " << nbClients << std::endl;
	
}

/*
 *	@brief Checks if keep-alive parameter is active in a request.
 *	@param request The request to check.
 *	@return TRUE if keep-alive is active, FALSE if not.
*/
bool Master::_isKeepAlive(const std::string &request) // delete ?
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
