
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
	std::string			_serverName;
	in_addr_t			_host;
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
	void		setPort(const int port);
	int			getPort(void) const;
	void		setServerName(std::string &serverName);
	const std::string	&getServerName() const;
	void		setHost(std::string host);
	in_addr_t	getHost(void) const;
	int			getSockfd(void) const;
	sockaddr_in	getServaddr(void) const;

	// Methods
	void	setup(void);
};

#endif