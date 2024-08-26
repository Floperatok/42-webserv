
#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Server.hpp"
# include "Response.hpp"
# include "Utils.hpp"

# include "Libs.hpp"

class Client
{
private:
public:
	Server		*server;
	int			sockfd;
	std::string	request;

	Client(void);
	Client(Server *serverArg);
	Client(const Client &copy);
	Client &operator=(const Client &other);
	~Client(void);

	int	readClientRequest(void);
	int	manageResponse(char **env);
};

#endif
