
#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Server.hpp"

# include <vector>
# include <string>

# include <sys/socket.h>
# include <arpa/inet.h>

class Client
{
private:
	Server		*_server;
	int			_sockfd;
	std::string	_request;
public:
	Client(Server &server);
	Client(const Client &copy);
	Client &operator=(const Client &other);
	~Client(void);

	int					getSockfd(void) const;
	const std::string	&getRequest(void) const;
	const Server		*getServer(void) const;

	void				appendRequest(const std::string &str);
	void				clearRequest(void);
};

#endif
