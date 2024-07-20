
#ifndef MASTER_HPP
# define MASTER_HPP

# include "Server.hpp"
# include <iostream>
# include <vector>

# include <sys/poll.h>

class Master
{
private:
	std::vector<Server>			_servers;
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
