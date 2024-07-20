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

/*
 *	@brief Split a string into substrings delimited by a charset.
 *	@param str The string to split.
 *	@param charset The delimiting characters.
 *	@return The splited string.
*/
std::vector<std::string> Parser::SplitStr(std::string &str, const char *charset)
{
	std::vector<std::string>	splited;
	std::string					set = charset;

	for (size_t i = 0 ; i < str.length() ; i++)
	{
		size_t start = i;

		while (str[i] && set.find(str[i]) == std::string::npos)
			i++;
		
		splited.push_back(str.substr(start, i - start));

		while (set.find(str[i]) != std::string::npos)
			i++;
	}

	return (splited);
}

/*
 *	@brief Cuts the beginning and the end of a string according to a charset.
 *	@param str The string to trim.
 *	@param charset The delimiting characters.
*/
void Parser::TrimStr(std::string &str, const char *charset)
{
	size_t		i = 0;
	std::string	set = charset;

	if (str.length() == 0 || str.find_first_of(charset) == std::string::npos)
		return ;

	while (str[i] && set.find(str[i]) != std::string::npos)
		i++;
	str.erase(0, i);

	size_t		len = str.length() - 1;
	i = len;
	while (i >= 0 && set.find_last_of(str[i]) != std::string::npos)
		i--;
	str.erase(i + 1, len - i);
}

/*
 *	@brief Separates the config file content according to servers.
 *	@param content The string content from the config file.
 *	@return A strings vector containing splited servers contents.
*/
std::vector<std::string> Parser::SplitServerContents(std::string &content)
{
	std::vector<std::string>	serversContent;
	std::vector<size_t>			start;
	std::vector<size_t>			end;

	RemoveComments(content);
	RemoveWhiteSpaces(content);

	for (size_t i = 0 ; i < content.length() ; i++)
	{
		if (!content.compare(i, 7, "server ") || !content.compare(i, 7, "server{")
			|| !content.compare(i, 7, "server\t"))
			start.push_back(content.find('{', i) + 1);
		if (i > 0 && !content.compare(i, 7, "server ") || !content.compare(i, 7, "server{")
			|| !content.compare(i, 7, "server\t") || content[i + 1] == '\0')
			end.push_back(content.find_last_of('}', i) - 1);
	}
	
	while (start.size() > 0)
	{
		serversContent.push_back(content.substr(start.back(), end.back() - start.back()));
		start.pop_back();
		end.pop_back();
	}

	return (serversContent);
}

