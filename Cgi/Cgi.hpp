
#ifndef CGI_HPP
# define CGI_HPP

# include <unistd.h>

# include "Libs.hpp"
# include "Logger.hpp"

class Cgi
{
private:
	Cgi(void);
	Cgi(const Cgi &copy);
	Cgi &operator=(const Cgi &other);
public:
	~Cgi(void);

	static int	getResponse(std::string &filepath, std::string &cgiPath, std::string &response);
};

#endif
