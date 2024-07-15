
#include <iostream>
#include "Webserv.hpp"

int main(int ac, char **av)
{
	if (ac > 2)
	{
		std::cout << "usage: ./webserv <configuration_file>" << std::endl;
		return (1);
	}
	if (ac == 1)
	{
		Webserv webserv("default.conf");
	}
	else
	{
		Webserv webserv(av[1]);
	}
	return (0);
}
