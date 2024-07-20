
#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "Logger.hpp"

# include <sstream>

# include <unistd.h>

class Response
{
private:
	Response(void);
	Response(const Response &copy);
	Response &operator=(const Response &other);
public:
	static void	serviceUnavailable(int fd);
	~Response(void);
};

#endif
