
#include "Server.hpp"
#include "Master.hpp"
#include "webserv.hpp"
#include "Logger.hpp"

std::vector<Server>	tempServerInit(const unsigned int n)
{
	std::vector<Server>	servers;

	for (unsigned int i = 0; i < n; i++)
	{
		Server	server;
		server.setPort(8000 + i);
		servers.push_back(server);
	}
	return (servers);
}

int main(void)
{
	std::vector<Server> servers = tempServerInit(3);

	Master master(servers);

	try
	{
		master.setupServers();
		master.runServers();
	}
	catch(const std::exception& e)
	{
		Logger::error(e.what());
	}
	
}