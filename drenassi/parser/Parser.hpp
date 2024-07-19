#pragma once

#include <iostream>
#include <fstream>
#include <exception>

class Parser
{
	public:
		// Constructors
		Parser(const std::string &configPath);
		~Parser();

		// Getters and setters

		// Methods
		void CheckConfigFile();

		// Exceptions
		class FileNotOpenedException : public std::exception
		{
			public :
				virtual const char	*what() const throw();
		};

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