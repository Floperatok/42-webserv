
#ifndef MASTER_HPP
# define MASTER_HPP

# include "Server.hpp"
# include "Response.hpp"
# include "Logger.hpp"

# include <iostream>
# include <vector>

# include <sys/poll.h>

# define BUFFER_SIZE 4096
# define MAX_CLIENT 1024
# define TIMEOUT	10000

class Master
{
private:
	std::vector<Server>	_servers;
	unsigned int		_nbSocket;
	struct pollfd		_fds[MAX_CLIENT];

	void	_initFds(void);
	void 	_storeFd(int fd, const short events);
	void	_tooManyClients(struct pollfd &fd);
	int		_createClientSocket(Server &server);
	int		_readSocket(const int sockfd);
public:
	Master(std::vector<Server> &servers);
	Master(const Master &copy);
	~Master(void);

	Master &operator=(const Master &other);

	void	setupServers(void);
	void	runServers(void);
};

#endif
