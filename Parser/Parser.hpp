#pragma once

#include "Libs.hpp"
#include "Utils.hpp"
#include "Master.hpp"
#include "CheckConfig.hpp"

class Parser
{
	public:
		// Constructors
		~Parser();

		// Getters and setters

		// Methods
		static std::vector<std::string> SplitStr(std::string &str, const char *charset);
		static void TrimStr(std::string &str, const char *charset);
		static std::vector<std::string> SplitServerContents(std::string &content);
		static void	ParseConfigFile(const std::string &configPath, Master &master);
		

		// Exceptions
		

	private:
		// Constructors
		Parser();
		Parser(const Parser &toCopy);
		Parser &operator=(const Parser &rhs);

		// Methods
		static void _RemoveComments(std::string &content);
		static void _RemoveWhiteSpaces(std::string &content);
		static void	_ParseServer(std::vector<std::string> &parameters, Server &server);
		static void	_ParseLocations(std::vector<std::string> &parameters, Server &server);
};