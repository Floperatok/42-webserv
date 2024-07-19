#include "Parser.hpp"

/* Constructors */
Parser::Parser()
{

}

Parser::Parser(const std::string &configPath) : _path(configPath)
{
	try
	{
		CheckConfigFile();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	
}

Parser::Parser(const Parser &toCopy)
{

}

Parser &Parser::operator=(const Parser &rhs)
{
	return (*this);
}

Parser::~Parser()
{

}

/* Getters and setters */

/* Methods */
void Parser::CheckConfigFile()
{
	std::ifstream file;
	std::string line;
	std::string content = "";

	file.open(_path);
	if (!file.is_open())
		throw (Parser::FileNotOpenedException());

	size_t	semicolon;
	while (std::getline(file, line))
	{
		semicolon = line.find('\n');
		content += line.substr(0, semicolon - 1);
		std::cout << content << '\n' << std::endl;
	}
}

/* Exceptions */
const char	*Parser::FileNotOpenedException::what() const throw()
{
	return ("webserv: Can't open config file.");
}