#include "CheckConfig.hpp"



/* ################################## CONSTRUCTORS ################################## */

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



/* ################################ MEMBER FUNCTIONS ################################ */

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

	file.close();

	if (content.empty() || !content.length())
		throw (EmptyFileException());

	if (!_CheckBrackets(content))
		throw (UnclosedBracketsException());

	if (!_CheckServerKeywords(content))
		throw (WrongKeywordException());

	_CheckKeywords(content);

}

/*
 *	@brief Checks if all brackets are well closed in a string content.
 *	@param content The string content to check.
*	@return TRUE if brackets are well closed, FALSE if not.
*/
bool CheckConfig::_CheckBrackets(const std::string &content)
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
bool CheckConfig::_IsInsideBrackets(const std::string &content, size_t pos)
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
bool CheckConfig::_CheckServerKeywords(const std::string &content)
{
	std::string keyword = "";
	size_t i = 0;

	while (content[i])
	{
		while (content[i] && !_IsInsideBrackets(content, i))
		{
			if (!std::isspace(content[i]) && content[i] != '{' && content[i] != '}')
				keyword += content[i];
			i++;
		}
		if (keyword.length() == 0 && content[i] == '\0')
			break ;
		if (keyword != "server")
			return (false);
		else
		{
			keyword = "";
			while (content[i] && _IsInsideBrackets(content, i))
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
void CheckConfig::_CheckKeywords(const std::string &content)
{
	std::vector<std::string>	serversContent = Parser::SplitServerContents(content);
	std::string					serverContent;
	std::vector<std::string>	parameters;

	while (serversContent.size() > 0)
	{
		std::string 				root = "";
		std::string 				index = "";
		std::vector<std::string>	errorPagePaths;
		std::vector<std::string>	cgiPaths;
		std::vector<std::string>	cgiExts;
		
		serverContent = serversContent.back();
		if (serverContent.find("listen") == std::string::npos)
			throw (MissingParameterException("Port not found."));
		if (serverContent.find("host") == std::string::npos)
			throw (MissingParameterException("Host not found."));

		parameters = Utils::SplitStr(serverContent, ";{}");

		for (std::vector<std::string>::iterator it = parameters.begin() ; it != parameters.end() ; it++)
		{
			std::string	parameter = *it;
			Utils::TrimStr(parameter, " ");

			if (parameter.find("listen") == 0)
			{
				if (parameter[6] != ' ')
					throw (WrongParameterException("Invalid port '" + parameter + "'."));
				parameter.erase(0, 7);
				if (parameter.find_first_not_of("0123456789") != std::string::npos)
					throw (WrongParameterException("Invalid port '" + parameter + "'."));
			}
			else if (parameter.find("server_name") == 0)
			{
				if (parameter[11] != ' ')
					throw (WrongParameterException("Invalid server name '" + parameter + "'."));
				parameter.erase(0, 12);
				if (parameter.find(' ') != std::string::npos)
					throw (WrongParameterException("Invalid server name '" + parameter + "'."));
			}
			else if (parameter.find("host") == 0)
			{
				if (parameter[4] != ' ')
					throw (WrongParameterException("Invalid host '" + parameter + "'."));
				parameter.erase(0, 5);
				if (parameter.find(' ') != std::string::npos \
					|| (parameter.find("localhost") == std::string::npos \
					&& Utils::SplitStr(parameter, ".").size() != 4))
					throw (WrongParameterException("Invalid host '" + parameter + "'."));
			}
			else if (parameter.find("root") == 0)
			{
				if (parameter[4] != ' ')
					throw (WrongParameterException("Invalid root '" + parameter + "'."));
				parameter.erase(0, 5);
				if (parameter.find(' ') != std::string::npos)
					throw (WrongParameterException("Invalid root '" + parameter + "'."));
				if (root == "")
					root = parameter;
			}
			else if (parameter.find("index") == 0)
			{
				if (parameter[5] != ' ')
					throw (WrongParameterException("Invalid index '" + parameter + "'."));
				parameter.erase(0, 6);
				if (parameter.find(' ') != std::string::npos)
					throw (WrongParameterException("Invalid index '" + parameter + "'."));
				if (index == "")
					index = parameter;
			}
			else if (parameter.find("client_max_body_size") == 0)
			{
				if (parameter[20] != ' ')
					throw (WrongParameterException("Invalid client_max_body_size '" + parameter + "'."));
				parameter.erase(0, 21);
				if (parameter.find_first_not_of("0123456789") != std::string::npos)
					throw (WrongParameterException("Invalid client_max_body_size '" + parameter + "'."));
			}
			else if (parameter.find("error_page") == 0)
			{
				if (parameter[10] != ' ')
					throw (WrongParameterException("Invalid error page '" + parameter + "'."));
				parameter.erase(0, 11);
				std::vector<std::string>	errorPage = Utils::SplitStr(parameter, " ");
				if (errorPage.size() != 2 || errorPage[0].find_first_not_of("0123456789") != std::string::npos)
					throw (WrongParameterException("Invalid error page '" + parameter + "'."));
				errorPagePaths.push_back(errorPage[1]);
			}
			else if (parameter.find("autoindex") == 0)
			{
				if (parameter[9] != ' ')
					throw (WrongParameterException("Invalid autoindex '" + parameter + "'."));
				parameter.erase(0, 10);
				if (parameter != "on" && parameter != "off")
					throw (WrongParameterException("Invalid autoindex '" + parameter + "'."));
			}
			else if (parameter.find("return") == 0)
			{
				if (parameter[6] != ' ')
					throw (WrongParameterException("Invalid HTTP redirection '" + parameter + "'."));
				parameter.erase(0, 7);
				if (parameter.find(" ") == std::string::npos)
					throw (WrongParameterException("Invalid HTTP redirection '" + parameter + "'."));
				std::vector<std::string>	redirect = Utils::SplitStr(parameter, " ");
				int	value = Utils::StrToInt(redirect[0]);
				if (value != 301 && value != 302)
					throw (WrongParameterException("Invalid HTTP redirection '" + parameter + "'."));
			}
			else if (parameter.find("allow_methods") == 0)
			{
				if (parameter[13] != ' ')
					throw (WrongParameterException("Invalid allow_methods '" + parameter + "'."));
				parameter.erase(0, 14);
			}
			else if (parameter.find("cgi_path") == 0)
			{
				if (parameter[8] != ' ')
					throw (WrongParameterException("Invalid cgi_path '" + parameter + "'."));
				parameter.erase(0, 9);
				cgiPaths = Utils::SplitStr(parameter, " ");
			}
			else if (parameter.find("cgi_ext") == 0)
			{
				if (parameter[7] != ' ')
					throw (WrongParameterException("Invalid cgi_ext '" + parameter + "'."));
				parameter.erase(0, 8);
				cgiExts = Utils::SplitStr(parameter, " ");
			}
		}
		
		if (access(root.c_str(), F_OK) == -1)
			throw (WrongParameterException("Can't access root path '" + root + "'."));
		if (root[root.length() - 1] != '/')
				root = root + '/';
		if (index[0] == '/')
			index.erase(0, 1);
		if (access((root + index).c_str(), F_OK | R_OK) == -1)
			throw (WrongParameterException("Can't access index path '" + root + index + "'."));
		for (std::vector<std::string>::iterator it = errorPagePaths.begin() ; it != errorPagePaths.end() ; it++)
		{
			if ((*it)[0] == '/')
				(*it).erase(0, 1);
			if (access((root + *it).c_str(), F_OK | R_OK) == -1)
				throw (WrongParameterException("Can't access error page path '" + root + *it + "'."));
		}
		if (cgiExts.size() != cgiPaths.size())
			throw (WrongParameterException("Need one path for each CGI extension."));

		serversContent.pop_back();
	}
}



/* ################################### EXCEPTIONS ################################### */

/*
 *	@brief Exception for files who cannot open.
 *	@return The error message.
*/
const char	*CheckConfig::FileNotOpenedException::what() const throw()
{
	return ("Config file: Cannot open file.");
}

/*
 *	@brief Exception for unclosed brackets
 *	@return The error message.
*/
const char	*CheckConfig::UnclosedBracketsException::what() const throw()
{
	return ("Config file: Found unclosed brackets.");
}

/*
 *	@brief Exception for empty file.
 *	@return The error message.
*/
const char	*CheckConfig::EmptyFileException::what() const throw()
{
	return ("Config file: Empty file.");
}

/*
 *	@brief Exception for wrong keyword.
 *	@return The error message.
*/
const char	*CheckConfig::WrongKeywordException::what() const throw()
{
	return ("Config file: Wrong keyword before brackets.");
}

/*
 *	@brief Exception for missing parameter.
 *	@param message The error message.
*/
CheckConfig::MissingParameterException::MissingParameterException(std::string message) throw()
{
	_msg = "Config file: " + message;
}

/*
 *	@brief Exception for missing parameter.
 *	@return The error message.
*/
const char *CheckConfig::MissingParameterException::what() const throw()
{
	return (_msg.c_str());
}

/*
 *	@brief Exception for wrong parameter.
 *	@param message The error message.
*/
CheckConfig::WrongParameterException::WrongParameterException(std::string message) throw()
{
	_msg = "Config file: " + message;
}

/*
 *	@brief Exception for wrong parameter.
 *	@return The error message.
*/
const char *CheckConfig::WrongParameterException::what() const throw()
{
	return (_msg.c_str());
}
