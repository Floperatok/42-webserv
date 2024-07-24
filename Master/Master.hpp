
#ifndef MASTER_HPP
# define MASTER_HPP

# include "Libs.hpp"
# include "Server.hpp"
# include "Response.hpp"
# include "Logger.hpp"


class Master
{
private:
	// Attributes
	std::vector<Server>	_servers;
	unsigned int		_nbSocket;
	struct pollfd		_fds[MAX_CLIENT];

	// Methods
	void	_initFds(void);
	void 	_storeFd(int fd, const short events);
	void	_tooManyClients(struct pollfd &fd);
	int		_createClientSocket(Server &server);
	int		_readSocket(const int sockfd);
public:
	// Constructors
	Master(std::vector<Server> &servers);
	Master(const Master &copy);
	~Master(void);

	Master &operator=(const Master &other);

	// Methods
	void	setupServers(void);
	void	runServers(void);

	// Getters and setters
	void	setServers(std::vector<Server> &servers);
};

#endif