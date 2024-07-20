
#ifndef MASTER_HPP
# define MASTER_HPP

# include "Server.hpp"
# include "Response.hpp"

# include <iostream>
# include <vector>

# include <sys/poll.h>

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
	int		_createClientSocket(const Server &server);
public:
	Master(std::vector<Server> &servers);
	Master(const Master &copy);
	~Master(void);

	Master &operator=(const Master &other);

	void	setupServers(void);
	void	runServers(void);

	class FunctionError : public std::exception
	{
	private:
		std::string			_message;
	public:
		FunctionError(const char *funcName, const int errnum);
		virtual ~FunctionError(void) throw();
		virtual const char	*what(void) const throw();
	};
};

#endif
