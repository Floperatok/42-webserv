
#ifndef SERVER_HPP
# define SERVER_HPP

# include "Libs.hpp"
# include "Utils.hpp"
# include "Logger.hpp"
# include "Location.hpp"


class Server
{
private:
	// Attributes
	static const int		_BUFFSIZE = 4096;
	int						_port;
	std::string				_serverName;
	in_addr_t				_host;
	std::string				_root;
	std::string				_index;
	std::string				_errorPage404;
	int						_sockfd;
	struct sockaddr_in		_servaddr;
	std::vector<Location>	_locations;
	
	// Methods
	void	_setupServAddr(void);
public:
	// Constructors
	Server(void);
	Server(const Server &copy);
	~Server(void);

	Server &operator=(const Server &other);
	
	// Setters and getters
	void						setPort(const int port);
	const int					&getPort(void) const;
	void						setServerName(std::string &serverName);
	const std::string			&getServerName() const;
	void						setHost(std::string host);
	const in_addr_t				&getHost(void) const;
	void						setRoot(std::string &root);
	const std::string			&getRoot() const;
	void						setIndex(std::string &index);
	const std::string			&getIndex() const;
	void						setErrorPage404(std::string &errorPage404);
	const std::string			&getErrorPage404() const;
	const int					&getSockfd(void) const;
	const sockaddr_in			&getServaddr(void) const;
	void						setLocations(std::vector<Location> &locations);
	const std::vector<Location>	&getLocations() const;

	// Methods
	void	setup(void);
	void	printServerAttributes() const;
};

#endif