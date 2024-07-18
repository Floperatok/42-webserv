#pragma once

#include <iostream>

class Parser
{
	public:
		// Constructors
		Parser(const std::string &configPath);
		~Parser();

		// Getters and setters

		// Methods
		bool CheckConfigFile(const std::string &configPath);

	private:
		// Attributes
		const std::string _path;
		std::string _serverName;
		int _port;
		std::string _indexPath;
		


		// Constructors
		Parser();
		Parser(const Parser &toCopy);
		Parser &operator=(const Parser &rhs);

};