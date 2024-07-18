
#include "webserv.hpp"
#include "Server.hpp"

int main(int ac, char **av)
{
	// if (ac != 2)
	// {
	// 	std::cerr << "Error at numbers of arguments" << std::endl;
	// 	return (1);
	// }
	(void)ac;
	(void)av;
	Server server(16003);
	try
	{
		server.setup();
		server.start();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return (0);
}