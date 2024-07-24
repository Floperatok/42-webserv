
#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "Libs.hpp"
# include "Logger.hpp"

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