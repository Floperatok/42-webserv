
#include "Master.hpp"
#include "CheckConfig.hpp"
#include "Server.hpp"
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

int main(int argc, char **argv)
{
	// Check args
	if (argc != 2)
	{
		Logger::error("Need config file path as argument.");
		return (EXIT_FAILURE);
	}

	std::string	configFilePath = argv[1];

	try
	{
		// Check config file
		CheckConfig::CheckConfigFile(configFilePath);

		// Parse config file
		std::vector<Server> servers = tempServerInit(3);
		Master master(servers);
		Parser::ParseConfigFile(configFilePath, master);


		// Setup and run servers
		
		master.setupServers();
		master.runServers();

	}
	catch (const std::exception &e)
	{
		Logger::error(e.what());
	}
}