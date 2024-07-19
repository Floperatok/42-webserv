#include "Parser.hpp"

/* ########## Constructors ########## */
Parser::Parser()
{

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

/* ########## Getters and setters ########## */

/* ########## Member functions ########## */

/*
 *	@brief Removes comments in a string content.
 *	@param content The string content.
*/
void Parser::RemoveComments(std::string &content)
{
	size_t	i = content.find('#');

	while (i != std::string::npos)
	{
		content.erase(i, content.find('\n', i) - i + 1);
		i = content.find('#');
	}
}

/*
 *	@brief Replaces white spaces and multiple spaces in a string content by one space.
 *	@param content The string content.
*/
void Parser::RemoveWhiteSpaces(std::string &content)
{
	for (size_t i = 0 ; i < content.length() ; i++)
	{
		if (std::isspace(content[i]))
			content[i] = ' ';
	}

	for (size_t i = 0 ; i < content.length() ; i++)
	{
		if (content[i] == ' ')
		{
			int j = 0;
			while (content[i + j] == ' ')
				j++;
			content.erase(i + 1, j - 1);
		}
	}
	if (content[0] == ' ')
		content.erase(0, 1);
	if (content[content.length() - 1] == ' ')
		content.erase(content.length() - 1, 1);
}

