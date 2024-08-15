
#pragma once

#include "Libs.hpp"
#include "Server.hpp"
#include "Response.hpp"
#include "Logger.hpp"
#include "Cgi.hpp"


class Master
{
private:
	// Attributes
	std::vector<Server>			_servers;
	unsigned int				_nbServers;
	unsigned int				_nfds;
	struct pollfd				_fds[MAX_CLIENT];
	std::map<int, std::string>	_requests;

	// Methods
	void		_initFds(void);
	void 		_storeFd(int fd, const Server &server, const short events);
	void		_compressArray(void);
	int			_createClientSocket(Server &server);
	int			_readSocket(const int sockfd, std::string &receivedData);
	void		_sendResponse(Server &server, const int sockfd, const std::string &request);
	void		_checkServersConnections(void);
	void		_manageClientsRequests(char **env, size_t *index);
	void		_RemoveFd(unsigned int index);
	bool		_isKeepAlive(const std::string &request);
	// debug
	void		_displayInfos(void) const;
public:
	// Constructors
	Master();
	Master(std::vector<Server> &servers);
	Master(const Master &copy);
	~Master(void);

	Master &operator=(const Master &other);

	// Methods
	void	setupServers(void);
	void	runServers(char **env);

	// Getters and setters
	void	setServers(std::vector<Server> servers);
};
