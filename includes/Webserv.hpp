
#ifndef WEBSERV_HPP
# define WEBSERV_HH

# include <string>
# include <sstream>
# include <fstream>
# include <utility>
# include "parsing.hpp"

class Webserv
{
private:
public:
	Webserv(const char *configFilename);
	Webserv(const Webserv &copy);
	Webserv &operator=(const Webserv &other);
	~Webserv();
};

#endif