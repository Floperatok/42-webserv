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

	file.open(path.c_str());
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
void CheckConfig::CheckKeywords(std::string &content)
{
	std::vector<std::string>	serversContent = Parser::SplitServerContents(content);
	std::string					serverContent;
	std::vector<std::string>	parameters;

	while (serversContent.size() > 0)
	{
		serverContent = serversContent.back();
		if (serverContent.find("listen") == std::string::npos)
			throw (MissingParameterException("Port not found."));
		if (serverContent.find("host") == std::string::npos)
			throw (MissingParameterException("Host not found."));
		if (serverContent.find("location") == std::string::npos)
			throw (MissingParameterException("No location found."));

		
		parameters = Parser::SplitStr(serverContent, ";{}");

		/************************************ DEBUG ************************************/
		// size_t i = 0;
		/************************************ DEBUG ************************************/

		for (std::vector<std::string>::iterator it = parameters.begin() ; it != parameters.end() ; it++)
		{
			std::string	parameter = *it;
			Parser::TrimStr(parameter, " ");

			/************************************ DEBUG ************************************/
			// std::cout << i++ << ": '" << parameter << "'" << std::endl;
			/************************************ DEBUG ************************************/

			if (parameter.find("listen") != std::string::npos)
			{
				if (parameter[6] != ' ')
					throw (WrongParameterException("Invalid port '" + parameter + "'."));
				parameter.erase(0, 7);
				if (parameter.find_first_not_of("0123456789") != std::string::npos)
					throw (WrongParameterException("Invalid port '" + parameter + "'."));
			}
			else if (parameter.find("server_name") != std::string::npos)
			{
				if (parameter[11] != ' ')
					throw (WrongParameterException("Invalid server name '" + parameter + "'."));
				parameter.erase(0, 12);
				if (parameter.find(' ') != std::string::npos)
					throw (WrongParameterException("Invalid server name '" + parameter + "'."));
			}
			else if (parameter.find("host") != std::string::npos)
			{
				if (parameter[4] != ' ')
					throw (WrongParameterException("Invalid host '" + parameter + "'."));
				parameter.erase(0, 5);
				if (parameter.find(' ') != std::string::npos || Parser::SplitStr(parameter, ".").size() != 4)
					throw (WrongParameterException("Invalid host '" + parameter + "'."));
			}
		}

		/************************************ DEBUG ************************************/
		// std::cout << std::endl;
		/************************************ DEBUG ************************************/
		
		serversContent.pop_back();
	}
}


/* ########## Exceptions ########## */
const char	*CheckConfig::FileNotOpenedException::what() const throw()
{
	return ("Config file: Cannot open file.");
}

const char	*CheckConfig::UnclosedBracketsException::what() const throw()
{
	return ("Config file: Found unclosed brackets.");
}

const char	*CheckConfig::EmptyFileException::what() const throw()
{
	return ("Config file: Empty file.");
}

const char	*CheckConfig::WrongKeywordException::what() const throw()
{
	return ("Config file: Wrong keyword before brackets.");
}

CheckConfig::MissingParameterException::MissingParameterException(std::string message) throw()
{
	_msg = "Config file: " + message;
}

const char *CheckConfig::MissingParameterException::what() const throw()
{
	return (_msg.c_str());
}

CheckConfig::WrongParameterException::WrongParameterException(std::string message) throw()
{
	_msg = "Config file: " + message;
}

const char *CheckConfig::WrongParameterException::what() const throw()
{
	return (_msg.c_str());
}