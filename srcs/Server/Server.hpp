
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
	int						_port;
	std::string				_serverName;
	in_addr_t				_host;
	std::string				_root;
	std::string				_index;
	size_t					_maxBodySize;
	std::string				_errorPage400;
	std::string				_errorPage403;
	std::string				_errorPage404;
	std::string				_errorPage405;
	std::string				_errorPage408;
	std::string				_errorPage413;
	std::string				_errorPage500;
	std::string				_errorPage501;
	std::string				_errorPage502;
	std::string				_errorPage503;
	std::string				_errorPage504;
	int						_sockfd;
	struct sockaddr_in		_servaddr;
	std::vector<Location>	_locations;
	
	// Methods
	void						_setupServAddr(void);
	void						_printFormattedLine(const std::string &text, bool fillWithStars = false, \
													unsigned int fieldWidth = 70) const;
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
	void						setMaxBodySize(size_t maxBodySize);
	size_t						getMaxBodySize() const;
	void						setErrorPage400(std::string &errorPage404);
	const std::string			&getErrorPage400() const;
	void						setErrorPage403(std::string &errorPage404);
	const std::string			&getErrorPage403() const;
	void						setErrorPage404(std::string &errorPage404);
	const std::string			&getErrorPage404() const;
	void						setErrorPage405(std::string &errorPage404);
	const std::string			&getErrorPage405() const;
	void						setErrorPage408(std::string &errorPage404);
	const std::string			&getErrorPage408() const;
	void						setErrorPage413(std::string &errorPage404);
	const std::string			&getErrorPage413() const;
	void						setErrorPage500(std::string &errorPage404);
	const std::string			&getErrorPage500() const;
	void						setErrorPage501(std::string &errorPage404);
	const std::string			&getErrorPage501() const;
	void						setErrorPage502(std::string &errorPage404);
	const std::string			&getErrorPage502() const;
	void						setErrorPage503(std::string &errorPage404);
	const std::string			&getErrorPage503() const;
	void						setErrorPage504(std::string &errorPage404);
	const std::string			&getErrorPage504() const;
	const int					&getSockfd(void) const;
	const sockaddr_in			&getServaddr(void) const;
	void						setLocations(std::vector<Location> &locations);
	const std::vector<Location>	&getLocations() const;

	// Methods
	bool	setup(void);
	void	printServerAttributes() const;
};

#endif