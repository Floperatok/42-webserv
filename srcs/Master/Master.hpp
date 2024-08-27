
#pragma once

#include "Libs.hpp"
#include "Server.hpp"
#include "Response.hpp"
#include "Logger.hpp"
#include "Cgi.hpp"
#include "Cookie.hpp"
#include "Client.hpp"

class Client;

class Master
{
private:
	// Attributes
	std::vector<Server>			_servers;
	unsigned int				_nbServers;
	struct pollfd				_fds[MAX_CLIENT];
	std::map<unsigned int, Client>		_clients;

	// Methods
	void		_initFds(void);
	void		_storeFd(Client &client, const short events);
	void		_checkServersConnections(void);
	void		_manageClientsRequests(char **env, size_t *index);
	void		_RemoveFd(unsigned int index);
	bool		_isKeepAlive(const std::string &request);
	// debug
	void		_displayInfos(void);

public:
	// Constructors
	Master();
	Master(std::vector<Server> &servers);
	Master(const Master &copy);
	~Master(void);

	Master &operator=(const Master &other);

	// Methods
	bool	setupServers(void);
	void	runServers(char **env);

	// Getters and setters
	void	setServers(std::vector<Server> servers);
};
