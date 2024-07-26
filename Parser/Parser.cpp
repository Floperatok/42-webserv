#include "Parser.hpp"

/* ########## Constructors ########## */
Parser::Parser()
{

}

Parser::Parser(const Parser &toCopy)
{
	if (this != &toCopy)
		*this = toCopy;
}

Parser &Parser::operator=(const Parser &rhs)
{
	if (this != &rhs)
		*this = rhs;
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
void Parser::_RemoveComments(std::string &content)
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
void Parser::_RemoveWhiteSpaces(std::string &content)
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

		while (set.find(str[i]) != std::string::npos && set.find(str[i + 1]) != std::string::npos)
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
	while (set.find_last_of(str[i]) != std::string::npos)
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

	_RemoveComments(content);
	_RemoveWhiteSpaces(content);

	for (size_t i = 0 ; i < content.length() ; i++)
	{
		if (!content.compare(i, 7, "server ") || !content.compare(i, 7, "server{")
			|| !content.compare(i, 7, "server\t"))
			start.push_back(content.find('{', i) + 1);
		if (!content.compare(i, 7, "server ") || !content.compare(i, 7, "server{")
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
 *	@brief Parse config file and replace matching attributes in master class.
 *	@param configPath Path of the config file.
 *	@param master The object from Master class to modify.
*/
void	Parser::ParseConfigFile(const std::string &configPath, Master &master)
{
	std::ifstream	file;
	std::string		line;
	std::string		content = "";

	file.open(configPath.c_str());
	if (!file.is_open())
		throw (CheckConfig::FileNotOpenedException());
	
	while (std::getline(file, line))
		content += line + '\n';
	(void) master;

	std::vector<std::string>	serversContent = Parser::SplitServerContents(content);
	std::vector<Server>			servers;

	while (serversContent.size() > 0)
	{
		std::vector<std::string>	parameters = Parser::SplitStr(serversContent.back(), ";{}");
		Server	server;

		Parser::_ParseServer(parameters, server);

		/************************************ DEBUG ************************************/
		server.printServerAttributes();
		/************************************ DEBUG ************************************/

		servers.push_back(server);
		serversContent.pop_back();
	}
	
	master.setServers(servers);

}

/*
 *	@brief Parse server informations in config file and replace matching attributes in server class.
 *	@param parameters Vector of content.
 *	@param server The server object to parse.
*/
void	Parser::_ParseServer(std::vector<std::string> &parameters, Server &server)
{
	std::vector<std::string>::iterator it = parameters.begin();
	while (it != parameters.end())
	{
		std::string	parameter = *it;
		Parser::TrimStr(parameter, " ");

		if (parameter.find("listen") != std::string::npos)
		{
			parameter.erase(0, 7);
			server.setPort(Utils::StrToint(parameter));
		}
		else if (parameter.find("server_name") != std::string::npos)
		{
			parameter.erase(0, 12);
			server.setServerName(parameter);
		}
		else if (parameter.find("host") != std::string::npos)
		{
			parameter.erase(0, 5);
			if (!parameter.compare("localhost"))
				parameter = "127.0.0.1";
			server.setHost(parameter);
		}
		else if (parameter.find("root") != std::string::npos)
		{
			parameter.erase(0, 5);
			if (parameter[parameter.length() - 1] != '/')
				parameter = parameter + '/';
			server.setRoot(parameter);
		}
		else if (parameter.find("index") != std::string::npos)
		{
			parameter.erase(0, 6);
			if (parameter[0] == '/')
				parameter.erase(0, 1);
			server.setIndex(parameter);
		}
		else if (parameter.find("error_page 404") != std::string::npos)
		{
			parameter.erase(0, 15);
			if (parameter[0] == '/')
				parameter.erase(0, 1);
			server.setErrorPage404(parameter);
		}
		else if (parameter.find("location") != std::string::npos)
			break ;
		it++;
	}

	for (std::vector<std::string>::iterator ite = parameters.begin() ; ite != it ; ite++)
		parameters.erase(ite);
}