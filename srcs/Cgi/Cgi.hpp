
#pragma once

#include "Libs.hpp"
#include "Logger.hpp"
#include "Utils.hpp"

class Cgi
{
private:
	Cgi(void);
	Cgi(const Cgi &copy);
	Cgi &operator=(const Cgi &other);
public:
	~Cgi(void);

	static int			getContent(const std::string &filepath, const std::string &cgiPath, \
										std::string &content, char **env);
	static std::string	GenerateCgiPage();
};
