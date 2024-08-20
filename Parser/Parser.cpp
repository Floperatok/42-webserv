#include "Parser.hpp"



/* ################################## CONSTRUCTORS ################################## */
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



/* ################################ MEMBER FUNCTIONS ################################ */

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
 *	@brief Separates the config file content according to servers.
 *	@param content The string content from the config file.
 *	@return A strings vector containing splited servers contents.
*/
std::vector<std::string> Parser::SplitServerContents(const std::string &content)
{
	std::string					configFileContent = content;
	std::vector<std::string>	serversContent;
	std::vector<size_t>			start;
	std::vector<size_t>			end;

	_RemoveComments(configFileContent);
	_RemoveWhiteSpaces(configFileContent);

	for (size_t i = 0 ; i < configFileContent.length() ; i++)
	{
		if (!configFileContent.compare(i, 7, "server ") || !configFileContent.compare(i, 7, "server{")
			|| !configFileContent.compare(i, 7, "server\t"))
			start.push_back(configFileContent.find('{', i) + 1);
		if (!configFileContent.compare(i, 7, "server ") || !configFileContent.compare(i, 7, "server{")
			|| !configFileContent.compare(i, 7, "server\t") || configFileContent[i + 1] == '\0')
			end.push_back(configFileContent.find_last_of('}', i) - 1);
	}
	
	while (start.size() > 0)
	{
		serversContent.push_back(configFileContent.substr(start.back(), end.back() - start.back()));
		start.pop_back();
		end.pop_back();
	}

	return (serversContent);
}

/*
 *	@brief Parse config file and replace matching attributes in master class.
 *	@param configPath Path of the config file.
 *	@param master The object from Master class to modify.
 *	@param env The environnement to use to execute commands.
*/
void	Parser::ParseConfigFile(const std::string &configPath, Master &master, char **env)
{
	std::ifstream	file;
	std::string		line;
	std::string		content = "";

	file.open(configPath.c_str());
	if (!file.is_open())
		throw (CheckConfig::FileNotOpenedException());
	
	while (std::getline(file, line))
		content += line + '\n';

	file.close();

	std::vector<std::string>	serversContent = Parser::SplitServerContents(content);
	std::vector<Server>			servers;

	while (serversContent.size() > 0)
	{
		std::vector<std::string>	parameters = Utils::SplitStr(serversContent.back(), ";{}");
		Server	server;

		Parser::_ParseServer(parameters, server);
		Parser::_ParseLocations(parameters, server);
		Parser::_CreateUploadsDir(server.getRoot(), env);

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
		Utils::TrimStr(parameter, " ");

		if (parameter.find("listen") == 0)
		{
			parameter.erase(0, 7);
			server.setPort(Utils::StrToInt(parameter));
		}
		else if (parameter.find("server_name") == 0)
		{
			parameter.erase(0, 12);
			server.setServerName(parameter);
		}
		else if (parameter.find("host") == 0)
		{
			parameter.erase(0, 5);
			if (parameter == "localhost")
				parameter = "127.0.0.1";
			server.setHost(parameter);
		}
		else if (parameter.find("root") == 0)
		{
			parameter.erase(0, 5);
			if (parameter[parameter.length() - 1] != '/')
				parameter = parameter + '/';
			server.setRoot(parameter);
		}
		else if (parameter.find("index") == 0)
		{
			parameter.erase(0, 6);
			if (parameter[0] == '/')
				parameter.erase(0, 1);
			server.setIndex(parameter);
		}
		else if (parameter.find("client_max_body_size") == 0)
		{
			parameter.erase(0, 21);
			server.setMaxBodySize(Utils::StrToInt(parameter));
		}
		else if (parameter.find("error_page 400 ") == 0)
		{
			parameter.erase(0, 15);
			if (parameter[0] == '/')
				parameter.erase(0, 1);
			server.setErrorPage400(parameter);
		}
		else if (parameter.find("error_page 403 ") == 0)
		{
			parameter.erase(0, 15);
			if (parameter[0] == '/')
				parameter.erase(0, 1);
			server.setErrorPage403(parameter);
		}
		else if (parameter.find("error_page 404 ") == 0)
		{
			parameter.erase(0, 15);
			if (parameter[0] == '/')
				parameter.erase(0, 1);
			server.setErrorPage404(parameter);
		}
		else if (parameter.find("error_page 405 ") == 0)
		{
			parameter.erase(0, 15);
			if (parameter[0] == '/')
				parameter.erase(0, 1);
			server.setErrorPage405(parameter);
		}
		else if (parameter.find("error_page 408 ") == 0)
		{
			parameter.erase(0, 15);
			if (parameter[0] == '/')
				parameter.erase(0, 1);
			server.setErrorPage408(parameter);
		}
		else if (parameter.find("error_page 413 ") == 0)
		{
			parameter.erase(0, 15);
			if (parameter[0] == '/')
				parameter.erase(0, 1);
			server.setErrorPage413(parameter);
		}
		else if (parameter.find("error_page 500 ") == 0)
		{
			parameter.erase(0, 15);
			if (parameter[0] == '/')
				parameter.erase(0, 1);
			server.setErrorPage500(parameter);
		}
		else if (parameter.find("error_page 501 ") == 0)
		{
			parameter.erase(0, 15);
			if (parameter[0] == '/')
				parameter.erase(0, 1);
			server.setErrorPage501(parameter);
		}
		else if (parameter.find("error_page 502 ") == 0)
		{
			parameter.erase(0, 15);
			if (parameter[0] == '/')
				parameter.erase(0, 1);
			server.setErrorPage502(parameter);
		}
		else if (parameter.find("error_page 503 ") == 0)
		{
			parameter.erase(0, 15);
			if (parameter[0] == '/')
				parameter.erase(0, 1);
			server.setErrorPage503(parameter);
		}
		else if (parameter.find("error_page 504 ") == 0)
		{
			parameter.erase(0, 15);
			if (parameter[0] == '/')
				parameter.erase(0, 1);
			server.setErrorPage504(parameter);
		}
		else if (parameter.find("location") == 0)
			break ;
		it++;
	}

	while (--it != parameters.begin())
		parameters.erase(it);
	parameters.erase(it);
}

/*
 *	@brief Parse locations informations in config file and replace matching attributes in server class.
 *	@param parameters Vector of content.
 *	@param server The server object to parse.
*/
void	Parser::_ParseLocations(std::vector<std::string> &parameters, Server &server)
{
	std::vector<Location>	locations;

	for (std::vector<std::string>::iterator it = parameters.begin() ; it != parameters.end() ; it++)
	{
		std::string	parameter = *it;
		Utils::TrimStr(parameter, " ");
		Location	location;

		if (it != parameters.end() && parameter.find("location") == 0)
		{
			parameter.erase(0, 9);
			location.setLocation(parameter);

			Utils::TrimStr(parameter = *(++it), " ");
			while (it != parameters.end() && parameter.find("location") == std::string::npos)
			{
				Utils::TrimStr(parameter = *it, " ");

				if (parameter.find("root") == 0)
				{
					parameter.erase(0, 5);
					location.setRoot(parameter);
				}
				else if (parameter.find("index") == 0 \
						 && parameter.find("autoindex") == std::string::npos)
				{
					parameter.erase(0, 6);
					location.setIndex(parameter);
				}
				else if (parameter.find("autoindex") == 0)
				{
					parameter.erase(0, 10);
					if (parameter.find("on") == 0)
						location.setAutoIndex(true);
					else if (parameter.find("off") == 0)
						location.setAutoIndex(false);
				}
				else if (parameter.find("return") == 0)
				{
					parameter.erase(0, 7);
					location.setRedirect(parameter);
				}
				else if (parameter.find("allow_methods") == 0)
				{
					parameter.erase(0, 14);
					location.setAllowMethods(Utils::SplitStr(parameter, " "));
				}
				else if (parameter.find("cgi_path") == 0)
				{
					parameter.erase(0, 9);
					location.setCgiPath(Utils::SplitStr(parameter, " "));
				}
				else if (parameter.find("cgi_ext") == 0)
				{
					parameter.erase(0, 8);
					location.setCgiExt(Utils::SplitStr(parameter, " "));
				}

				if (it + 1 != parameters.end())
					parameter = *(++it);
				else
					break ;
			}
			locations.push_back(location);
			--it;
		}
	}
	server.setLocations(locations);
}

/*
 *	@brief Checks if uploads and cgi-bin directories exist and creates it if not.
 *	@param root The server's root.
 *	@param env The environnement to use to execute commands.
*/
void	Parser::_CreateUploadsDir(const std::string &root, char **env)
{
	const char	*dir = (root + "uploads").c_str();
	struct stat	sb;

	if (stat(dir, &sb))
		Utils::ExecCommand("mkdir " + root + "uploads", env);
	dir = (root + "cgi-bin").c_str();
	if (stat(dir, &sb))
		Utils::ExecCommand("mkdir " + root + "cgi-bin", env);
}