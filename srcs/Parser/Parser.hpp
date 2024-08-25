#pragma once

#include "Libs.hpp"
#include "Utils.hpp"
#include "Master.hpp"
#include "CheckConfig.hpp"

class Master;

class Parser
{
	public:
		// Methods
		static std::vector<std::string> SplitServerContents(const std::string &content);
		static void	ParseConfigFile(const std::string &configPath, Master &master, char **env);
		
	private:
		// Constructors
		Parser();
		Parser(const Parser &toCopy);
		Parser &operator=(const Parser &rhs);
		~Parser();

		// Methods
		static void _RemoveComments(std::string &content);
		static void _RemoveWhiteSpaces(std::string &content);
		static void	_ParseServer(std::vector<std::string> &parameters, Server &server);
		static void	_ParseLocations(std::vector<std::string> &parameters, Server &server);
		static void	_CreateUploadsDir(const std::string &root, char **env);
};