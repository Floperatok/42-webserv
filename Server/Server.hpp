
#ifndef SERVER_HPP
# define SERVER_HPP

# include "Libs.hpp"
# include "Logger.hpp"


class Server
{
private:
	// Attributes
	static const int	_BUFFSIZE = 4096;
	int					_port;
	int					_sockfd;
	struct sockaddr_in	_servaddr;
	
	// Methods
	void	_setupServAddr(void);
public:
	// Constructors
	Server(void);
	Server(const Server &copy);
	~Server(void);

	Server &operator=(const Server &other);
	
	// Setters and getters
	int			getPort(void) const;
	int			getSockfd(void) const;
	sockaddr_in	getServaddr(void) const;
	void		setPort(const int port);

	// Methods
	void	setup(void);
};

#endif