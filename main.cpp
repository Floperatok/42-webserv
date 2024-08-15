#include "Master.hpp"
#include "Logger.hpp"
#include "CheckConfig.hpp"
#include "Server.hpp"

int	main(int argc, char **argv, char **env)
{
	std::string	configFilePath = "Config/config.conf";
	if (argc == 2)
		std::string	configFilePath = argv[1];
	
	else if (argc > 2)
	{
		Logger::error("Too much arguments.");
		return (EXIT_FAILURE);
	}

	try
	{
		// Check config file
		CheckConfig::CheckConfigFile(configFilePath);

		// Parse config file
		Master master;
		Parser::ParseConfigFile(configFilePath, master);

		// Setup and run servers
		master.setupServers();
		master.runServers(env);

	}
	catch (const std::exception &e)
	{
		Logger::error(e.what());
	}

	return (EXIT_SUCCESS);
}
