
#ifndef SERVER_HPP
# define SERVER_HPP

# include "Logger.hpp"

# include <sys/socket.h>
# include <unistd.h>
# include <arpa/inet.h>
# include <errno.h>

# include <cstring>
# include <exception>
# include <string>
# include <sstream>
# include <iostream>

class Server
{
private:
	static const int	_BUFFSIZE = 4096;
	int					_port;
	int					_sockfd;
	struct sockaddr_in	_servaddr;
	
	void	_setupServAddr(void);
public:
	Server(void);
	Server(const Server &copy);
	~Server(void);

	Server &operator=(const Server &other);
	
	int			getPort(void) const;
	int			getSockfd(void) const;
	sockaddr_in	getServaddr(void) const;
	void		setPort(const int port);

	void	setup(void);
};

#endif
