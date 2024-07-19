#include "CheckConfig.hpp"

/* ########## Constructors ########## */
CheckConfig::CheckConfig() {}

CheckConfig::CheckConfig(const CheckConfig &toCopy)
{
	if (&toCopy != this)
		*this = toCopy;
}

CheckConfig &CheckConfig::operator=(const CheckConfig &rhs)
{
	if (&rhs != this)
		*this = rhs;
	return (*this);
}

CheckConfig::~CheckConfig() {}


/* ########## Member functions ########## */

/*
 *	@brief Check for the validity of a config file.
 *	@param path Path of the config file.
*/
void CheckConfig::CheckConfigFile(const std::string &path)
{
	std::ifstream	file;
	std::string		line;
	std::string		content = "";

	file.open(path);
	if (!file.is_open())
		throw (FileNotOpenedException());

	while (std::getline(file, line))
		content += line + '\n';

	if (content.empty() || !content.length())
		throw (EmptyFileException());

	if (!CheckBrackets(content))
		throw (UnclosedBracketsException());

	if (!CheckServerKeywords(content))
		throw (WrongKeywordException());

	CheckKeywords(content);

}

/*
 *	@brief Checks if all brackets are well closed in a string content.
 *	@param content The string content to check.
*	@return TRUE if brackets are well closed, FALSE if not.
*/
bool CheckConfig::CheckBrackets(std::string &content)
{
	std::stack<char>	tmp;

	for (size_t i = 0 ; i < content.length() ; i++)
	{
		if (tmp.size() > 2)
			return (false);

		if (content[i] == '{')
		{
			tmp.push(content[i]);
		}
		else if (content[i] == '}')
			tmp.pop();
	}

	if (tmp.size() > 0)
		return (false);
	return (true);
}


/*
 *	@brief Checks if a position of a string content is inside brackets.
 *	@param content The string content to check.
 *	@param pos The string content.
 *	@return TRUE if inside brackets, FALSE if not.
*/
bool CheckConfig::IsInsideBrackets(std::string &content, size_t pos)
{
	std::stack<char>	tmp;

	for (size_t i = 0 ; i < content.length() ; i++)
	{
		if (content[i] == '{')
		{
			tmp.push(content[i]);
		}
		else if (content[i] == '}')
			tmp.pop();
		if (i == pos)
			break ;
	}

	if (tmp.size() > 0)
		return (true);
	return (false);
}

/*
 *	@brief Checks for the 'server' keyword before opening brackets.
 *	@param content The string content to check.
 *	@return TRUE if the keyword is 'server', FALSE if not.
*/
bool CheckConfig::CheckServerKeywords(std::string &content)
{
	std::string keyword = "";
	size_t i = 0;

	while (content[i])
	{
		while (content[i] && !IsInsideBrackets(content, i))
		{
			if (!std::isspace(content[i]) && content[i] != '{' && content[i] != '}')
				keyword += content[i];
			i++;
		}
		if (keyword.length() == 0 && content[i] == '\0')
			break ;
		if (keyword.compare("server"))
			return (false);
		else
		{
			keyword = "";
			while (content[i] && IsInsideBrackets(content, i))
				i++;
		}
	}
	return (true);
}

/*
 *	@brief Checks the validity of keywords from a config file.
 *	@param content The string content to check.
 *	@return TRUE if the content is valid, FALSE if not.
*/
bool CheckConfig::CheckKeywords(std::string &content)
{
	std::vector<std::string>	serversContent = SplitServerContents(content);
	std::string					serverContent;
	std::vector<std::string>	parameters;

	while (serversContent.size() > 0)
	{
		serverContent = serversContent.back();
		if (serverContent.find("listen") == std::string::npos)
			throw (MissingParameterException("Port not found in config file."));
		if (serverContent.find("host") == std::string::npos)
			throw (MissingParameterException("Host not found in config file."));
		if (serverContent.find("location") == std::string::npos)
			throw (MissingParameterException("No location in config file. Need at least one."));

		
		parameters = SplitStr(serverContent, ";{}");
		size_t i = 0;
		for (std::vector<std::string>::iterator it = parameters.begin() ; it != parameters.end() ; it++)
		{
			std::cout << i++ << ": " << *it << std::endl;
		}
		std::cout << std::endl;
		serversContent.pop_back();
	}

	return (true);
}

/*
 *	@brief Separates the config file content according to servers.
 *	@param content The string content from the config file.
 *	@return A strings vector containing splited servers contents.
*/
std::vector<std::string> CheckConfig::SplitServerContents(std::string &content)
{
	std::vector<std::string>	serversContent;
	std::vector<size_t>			start;
	std::vector<size_t>			end;

	Parser::RemoveComments(content);
	Parser::RemoveWhiteSpaces(content);

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


/*
 *	@brief Split a string into substrings delimited by a character.
 *	@param str The string to split.
 *	@param c The delimiting character.
 *	@return The splited string.
*/
std::vector<std::string> CheckConfig::SplitStr(std::string &str, const char *charset)
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


/* ########## Exceptions ########## */
const char	*CheckConfig::FileNotOpenedException::what() const throw()
{
	return ("webserv: Error: Can't open config file.");
}

const char	*CheckConfig::UnclosedBracketsException::what() const throw()
{
	return ("webserv: Error: Unclosed brackets in config file.");
}

const char	*CheckConfig::EmptyFileException::what() const throw()
{
	return ("webserv: Error: Empty config file.");
}

const char	*CheckConfig::WrongKeywordException::what() const throw()
{
	return ("webserv: Error: Wrong keyword before brackets.");
}

CheckConfig::MissingParameterException::MissingParameterException(std::string message) throw()
{
	_msg = "webserv:Error: " + message;
}

const char *CheckConfig::MissingParameterException::what() const throw()
{
	return (_msg.c_str());
}