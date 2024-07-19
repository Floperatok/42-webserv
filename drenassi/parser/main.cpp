#include "CheckConfig.hpp"
#include <cstdlib>

int main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cout << "Error: Need 1 argument" << std::endl;
		return (1);
	}

	try
	{
		CheckConfig::CheckConfigFile(av[1]);
	}
	catch(const std::exception &e)
	{
		std::cerr << e.what() << '\n';
	}
	
	return (EXIT_SUCCESS);
}