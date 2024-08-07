
#include "Response.hpp"


/* ########## Constructor ########## */

Response::Response(void)
{
}

Response::Response(const Response &copy)
{
	if (&copy != this)
		*this = copy;
}

Response::~Response(void)
{
}


/* ########## Operator overload ########## */

Response &Response::operator=(const Response &other)
{
	if (&other != this)
	{
		
	}
	return (*this);
}


/* ########## Setter/Getter ########## */


/* ########## Member function ########## */
int	Response::SendResponse(std::vector<Server> &servers, int fd, std::string request)
{
	std::string					line = request.substr(0, request.find('\n'));
	std::vector<std::string>	req = Parser::SplitStr(line, " ");
	std::string					method = req[0];
	Server						server = _GetServer(servers, request);

	std::string					path = _GetPath(server, req[1]);

	if (method == "GET")
	{
		std::string	type = _GetContentType(request, path);
		return (_WritePage(fd, path, type, "200 OK"));
	}
	// else if (method == "POST")
	// {

	// }
	// else if (method == "PUT")
	// {

	// }
	// else if (method == "DELETE")
	// {

	// }
	// else if (method == "OPTIONS")
	// {

	// }
	else
		return (MethodNotAllowed405(fd));
	return (200);
}

/*
 *	@brief	Gets the server from which request is done.
 *	@param	servers	The vector's list of servers.
 *	@param	request	The request.
 *	@return	The corresponding server.
*/
Server	Response::_GetServer(std::vector<Server> &servers, std::string request)
{
	Server	server = servers[0];
	int 	port = _GetPort(request);

	for (size_t i = 0 ; i < servers.size() ; i++)
	{
		if (servers[i].getPort() == port)
		{
			server = servers[i];
			break ;
		}
	}

	return (server);
}

/*
 *	@brief	Gets the port of the server from which request is done.
 *	@param	request	The request.
 *	@return	The server's port.
*/
std::string	Response::_GetPath(Server &server, std::string subPath)
{
	std::string	root = server.getRoot();
	if (root[root.length() - 1] == '/')
		root.erase(root.length() - 1, 1);

	std::string	path = root + subPath;
	if (!subPath.compare("/"))
		path = root + "/" + server.getIndex();

	std::cout << "PATH = " << path << std::endl;
	return (path);
}

/*
 *	@brief	Gets the port of the server from which request is done.
 *	@param	request	The request.
 *	@return	The server's port.
*/
int	Response::_GetPort(std::string request)
{
	std::string	port;
	size_t		start = request.find("Host:") + 6;
	size_t		end = request.find("\n", start) - 1;
	std::string	line = request.substr(start, end - start);

	port = line.substr(line.find(":") + 1);

	return (Utils::StrToInt(port));
}

/*
 *	@brief	Gets the header's content type from a request.
 *	@param	request	The request.
 *	@return	The content type.
*/
std::string	Response::_GetContentType(std::string request, std::string path)
{
	if (!path.compare(path.length() - 12, 12, "/favicon.ico"))
		return ("image/x-icon");

	std::string					contentType;

	size_t						start = request.find("Accept:") + 8;
	size_t						end = request.find(";", start) - 1;
	std::string					line = request.substr(start, end - start);
	std::vector<std::string>	types = Parser::SplitStr(line, ",");

	if (path.find(".") == std::string::npos)
		return ("text/plain");

	std::string					ext = path.substr(path.find(".") + 1);
	if (ext == "jpg")
				ext = "jpeg";
	
	for (std::vector<std::string>::iterator it = types.begin() ; it != types.end() ; it++)
	{
		std::vector<std::string>	type = Parser::SplitStr(*it, "/");
		if (type[1] == ext)
		{
			contentType = *it;
			break ;
		}
		else if (type[1] == "*")
		{
			contentType = type[0] + "/" + ext;
			break ;
		}
	}

	std::cout << "CONTENT TYPE = " << contentType << std::endl;

	return (contentType);
}

/*
 *	@brief	Writes the header and the content into the socket fd.
 *	@param	fd The socket's file descriptor.
 *	@param	path The path of the ressource we want to access to.
 *	@param	type The content-type.
 *	@param	status The string-formated status code.
 *	@return	The status code.
*/
int	Response::_WritePage(int fd, std::string path, std::string type, std::string status)
{
	std::ifstream		file(path.c_str(), std::ios::in | std::ios::binary);

	if (!file.is_open())
		return (NotFound404(fd));
	
	std::vector<char> content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	std::string header = "HTTP/1.1 " + status + "\r\nContent-Type: " + type + "\r\nContent-Length: " \
			+ Utils::IntToStr(content.size()) + "\r\n\r\n";

	write(fd, header.c_str(), header.size());
	write(fd, content.data(), content.size());

	Logger::debug(("Response send: HTTP/1.1 " + status + " | Content-Type: " + type \
				+ " | Content-Length: " + Utils::IntToStr(content.size())).c_str());

	return (200);
}

/*
 *	@brief	Displays the error 400 page.
 *	@param	fd	The socket's file descriptor.
 *	@param	path The path of the corresponding page (let blank to use the default page).
 *	@return	The corresponding status code.
*/
int	Response::BadRequest400(int fd, std::string path)
{
	_WritePage(fd, path, "text/html", "400 Bad Request");

	return (400);
}

/*
 *	@brief	Displays the error 403 page.
 *	@param	fd	The socket's file descriptor.
 *	@param	path The path of the corresponding page (let blank to use the default page).
 *	@return	The corresponding status code.
*/
int	Response::Forbidden403(int fd, std::string path)
{
	_WritePage(fd, path, "text/html", "403 Forbidden");

	return (403);
}

/*
 *	@brief	Displays the error 404 page.
 *	@param	fd	The socket's file descriptor.
 *	@param	path The path of the corresponding page (let blank to use the default page).
 *	@return	The corresponding status code.
*/
int	Response::NotFound404(int fd, std::string path)
{
	_WritePage(fd, path, "text/html", "404 Not Found");

	return (404);
}

/*
 *	@brief	Displays the error 405 page.
 *	@param	fd	The socket's file descriptor.
 *	@param	path The path of the corresponding page (let blank to use the default page).
 *	@return	The corresponding status code.
*/
int	Response::MethodNotAllowed405(int fd, std::string path)
{
	_WritePage(fd, path, "text/html", "405 Method Not Allowed");

	return (405);
}

/*
 *	@brief	Displays the error 408 page.
 *	@param	fd	The socket's file descriptor.
 *	@param	path The path of the corresponding page (let blank to use the default page).
 *	@return	The corresponding status code.
*/
int	Response::RequestTimeout408(int fd, std::string path)
{
	_WritePage(fd, path, "text/html", "408 Request Timeout");

	return (408);
}

/*
 *	@brief	Displays the error 500 page.
 *	@param	fd	The socket's file descriptor.
 *	@param	path The path of the corresponding page (let blank to use the default page).
 *	@return	The corresponding status code.
*/
int	Response::InternalServerError500(int fd, std::string path)
{
	_WritePage(fd, path, "text/html", "500 Internal Server Error");

	return (500);
}

/*
 *	@brief	Displays the error 501 page.
 *	@param	fd	The socket's file descriptor.
 *	@param	path The path of the corresponding page (let blank to use the default page).
 *	@return	The corresponding status code.
*/
int	Response::MethodNotImplemented501(int fd, std::string path)
{
	_WritePage(fd, path, "text/html", "501 Not Implemented");

	return (501);
}

/*
 *	@brief	Displays the error 502 page.
 *	@param	fd	The socket's file descriptor.
 *	@param	path The path of the corresponding page (let blank to use the default page).
 *	@return	The corresponding status code.
*/
int	Response::BadGateway502(int fd, std::string path)
{
	_WritePage(fd, path, "text/html", "502 Bad Gateway");

	return (502);
}

/*
 *	@brief	Displays the error 503 page.
 *	@param	fd	The socket's file descriptor.
 *	@param	path The path of the corresponding page (let blank to use the default page).
 *	@return	The corresponding status code.
*/
int	Response::ServiceUnavailable503(int fd, std::string path)
{
	_WritePage(fd, path, "text/html", "503 Service Unavailable");

	return (503);
}

/*
 *	@brief	Displays the error 504 page.
 *	@param	fd	The socket's file descriptor.
 *	@param	path The path of the corresponding page (let blank to use the default page).
 *	@return	The corresponding status code.
*/
int	Response::GatewayTimeout504(int fd, std::string path)
{
	_WritePage(fd, path, "text/html", "504 Gateway Timeout");

	return (504);
}




/* ########## Exception ########## */


/* ########## Non-member function ########## */
