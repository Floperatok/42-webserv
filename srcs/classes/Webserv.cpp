
#include "Webserv.hpp"
#include <iostream>

Webserv::Webserv(const char *configFilename)
{
	std::ifstream	file(configFilename);
	std::string		line;
	unsigned int	spacePos;
	std::pair<std::string, std::string> var;

	std::getline(file, line);
	while (!line.empty())
	{
		line = myStringTrim(line, " \t\n");
		spacePos = line.find(' ', 0);
		if (spacePos == std::string::npos)
		{
			// manage no space error
		}
		var.first = line.substr(0, spacePos);
		var.second = line.substr(spacePos + 1, spacePos + 1 - line.size());

		std::cout << var.first << " : " << var.second << std::endl;
		std::getline(file, line);
	} 
	file.close();
}

Webserv::~Webserv(void)
{
}
