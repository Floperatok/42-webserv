#pragma once

#include <iostream>
#include <fstream>
#include <exception>
#include <list>
#include <stack>
#include <cctype>

class Parser
{
	public:
		// Constructors
		~Parser();

		// Getters and setters

		// Methods
		static void RemoveComments(std::string &content);
		static void RemoveWhiteSpaces(std::string &content);
		

		// Exceptions
		

	private:
		// Attributes
		std::string _serverName;
		int _port;
		std::string _indexPath;

		// Constructors
		Parser();
		Parser(const Parser &toCopy);
		Parser &operator=(const Parser &rhs);

		// Methods
};