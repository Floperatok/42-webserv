
#include "Server.hpp"
#include "Master.hpp"
#include "webserv.hpp"

std::vector<Server>	tempServerInit(const unsigned int n)
{
	std::vector<Server>	servers;

	for (unsigned int i = 0; i < n; i++)
	{
		Server	server(8000 + i);
		servers.push_back(server);
	}
	return (servers);
}

int main(void)
{
	std::vector<Server> servers = tempServerInit(5);
	Master				master(servers);

	master.setupServers();
	return (0);
}