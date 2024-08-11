
#include "Response.hpp"


/* ########## Constructors ########## */

Response::Response(void) {}

Response::Response(const Response &copy)
{
	if (&copy != this)
		*this = copy;
}

Response::~Response(void) {}


/* ########## Operator overload ########## */

Response &Response::operator=(const Response &other)
{
	if (&other != this)
		*this = other;
	return (*this);
}


/* ########## Setters and getters ########## */




/* ########## Member functions ########## */

/*
 *	@brief	Sends response to the appropriate server according to the client's request.
 *	@param	servers	The vector's list of servers.
 *	@param	fd The socket's file descriptor.
 *	@param	request	The request.
 *	@return	The status code of the response sent.
*/
int	Response::SendResponse(std::vector<Server> &servers, int fd, std::string request)
{
	std::string					line = request.substr(0, request.find('\n'));
	std::vector<std::string>	req = Parser::SplitStr(line, " ");
	std::string					method = req[0];
	Server						server = _GetServer(servers, request);
	std::string					path = _GetPath(server, req[1]);
	Location					location = _GetLocation(server, path);

	if (!_CheckAutoIndex(server, location))
		return (Forbidden403(fd));

	if (method == "GET" && _IsMethodAllowed(method, location))
	{
		std::string	type = _GetContentType(request, path);
		return (_WritePage(fd, path, type, "200 OK"));
	}
	else if (method == "POST" && _IsMethodAllowed(method, location))
	{
		return (_HandlePost(fd, request, server.getRoot()));
	}
	else if (method == "PUT" && _IsMethodAllowed(method, location))
	{

	}
	else if (method == "HEAD" && _IsMethodAllowed(method, location))
	{

	}
	else if (method == "DELETE" && _IsMethodAllowed(method, location))
	{

	}
	else if (method == "OPTIONS" && _IsMethodAllowed(method, location))
	{

	}
	else
		return (MethodNotAllowed405(fd));
	return (200);
}

/*
 *	@brief	Writes the header and the content into the socket fd.
 *	@param	fd The socket's file descriptor.
 *	@param	path The path of the ressource we want to access to.
 *	@param	type The content-type.
 *	@param	status The string-formated status code.
 *	@param	closeConnection	(bool) Specifies if the connection must be closed (default: false).
 *	@return	The status code.
*/
int	Response::_WritePage(int fd, const std::string &path, const std::string &type, const std::string &status)
{
	std::ifstream		file(path.c_str(), std::ios::in | std::ios::binary);

	if (!file.is_open())
	{
		Logger::error(("Can't open file '" + path + "'.").c_str());
		return (NotFound404(fd));
	}
	
	std::vector<char> content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	
	file.close();

	std::string header = "HTTP/1.1 " + status + "\r\nContent-Type: " + type + "\r\nContent-Length: " \
			+ Utils::IntToStr(content.size()) + "\r\n\r\n";

	if (write(fd, header.c_str(), header.size()) < 0)
	{
		Logger::error("Failed to write header.");
		return (InternalServerError500(fd));
	}

	ssize_t bytesWritten = write(fd, content.data(), content.size());
	if (bytesWritten < 0)
	{
		Logger::error("Failed to write content.");
		return (InternalServerError500(fd));
	}
	else if (static_cast<size_t>(bytesWritten) != content.size())
	{
		Logger::error("Incomplete content written.");
		return InternalServerError500(fd);
	}

	Logger::debug(("Response sent: HTTP/1.1 " + status + " | Content-Type: " + type \
				+ " | Content-Length: " + Utils::IntToStr(content.size())).c_str());

	return (200);
}

/*
 *	@brief	Upload a file into the server, creates the file, and sends a success page in case of success.
 *	@param	fd	The socket's file descriptor.
 *	@param	request	The request.
 *	@param	root	The root directory of the website.
 *	@return	200 in case of success, 500 in case of error.
*/
int Response::_HandlePost(int fd, const std::string &request, const std::string &root)
{
    std::string		boundary = _GetBoundary(request);
    std::string		body = request.substr(request.find("\r\n\r\n") + 4);
    std::string		filename = _ExtractFilename(body);
    std::string		fileData = _ExtractFileData(body, boundary);
    std::string		uploadPath = root + "uploads/" + filename;

	if (boundary.empty() || filename.empty() || fileData.empty())
		return (InternalServerError500(fd));

    std::ofstream	outFile(uploadPath.c_str(), std::ios::binary);
    if (!outFile)
	{
		Logger::error("Failed to open file for writting.");
        return (InternalServerError500(fd));
	}
    
    outFile.write(fileData.c_str(), fileData.size());
    outFile.close();
    
	_WritePage(fd, "www/file_uploaded.html", "text/html", "200 OK");
    
    return (200);
}

/*
 *	@brief	Gets the request's boundary.
 *	@param	request	The request.
 *	@return	The boundary.
*/
std::string Response::_GetBoundary(const std::string &request)
{
    size_t pos = request.find("boundary=");
    if (pos != std::string::npos)
    {
        size_t end = request.find("\n", pos) - 1;
        return ("--" + request.substr(pos + 9, end - (pos + 9)));
    }
    return ("");
}

/*
 *	@brief	Extracts the file's name from the request's body.
 *	@param	body	The request's body.
 *	@return	The file's name.
*/
std::string Response::_ExtractFilename(const std::string &body)
{
    size_t pos = body.find("filename=\"");

    if (pos != std::string::npos)
    {
        size_t start = pos + 10;
        size_t end = body.find("\"", start);
        return (body.substr(start, end - start));
    }
    return ("uploaded_file");
}

/*
 *	@brief	Extracts a file's data from the request's body.
 *	@param	body	The request's body.
 *	@param	boundary	The request's boundary.
 *	@return	The file's content.
*/
std::string Response::_ExtractFileData(const std::string &body, const std::string &boundary)
{
	std::string	endBoundary = boundary + "--";

    size_t	start = body.find("\r\n\r\n") + 4;
    size_t end = body.find(endBoundary, start) - 2;

    return (body.substr(start, end - start));
}


/*
 *	@brief	Gets the server from which request is done.
 *	@param	servers	The vector's list of servers.
 *	@param	request	The request.
 *	@return	The corresponding server.
*/
Server	Response::_GetServer(std::vector<Server> &servers, const std::string &request)
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
std::string	Response::_GetPath(Server &server, const std::string &subPath)
{
	std::string	root = server.getRoot();
	if (root[root.length() - 1] == '/')
		root.erase(root.length() - 1, 1);

	std::string	path = root + subPath;
	if (!subPath.compare("/"))
		path = root + "/" + server.getIndex();

	return (path);
}

/*
 *	@brief	Gets the port of the server from which request is done.
 *	@param	request	The request.
 *	@return	The server's port.
*/
int	Response::_GetPort(const std::string &request)
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
std::string	Response::_GetContentType(const std::string &request, const std::string &path)
{
	if (path.find(".") == std::string::npos)
		return ("text/plain");
	
	if (!path.compare(path.length() - 12, 12, "/favicon.ico"))
		return ("image/x-icon");

	std::string					contentType;

	size_t						start = request.find("Accept:") + 8;
	size_t						end = request.find(";", start) - 1;
	std::string					line = request.substr(start, end - start);
	std::vector<std::string>	types = Parser::SplitStr(line, ",");


	std::string					ext = path.substr(path.find(".") + 1);
	if (ext == "jpg")
				ext = "jpeg";
	else if (ext == "js")
		ext = "javascript";
	
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

	return (contentType);
}

/*
 *	@brief	Gets the appropriate server's location according to the request's path.
 *	@param	server The server.
 *	@param	path The request's path.
 *	@return	The current location.
*/
Location		Response::_GetLocation(Server &server, const std::string &path)
{
	Location	location;

	std::vector<Location>		locations = server.getLocations();
	for (std::vector<Location>::iterator it = locations.begin() ; it != locations.end() ; it++)
		if (path.find((*it).getLocation()) != std::string::npos \
			&& (*it).getLocation().size() > location.getLocation().size())
			location = *it;
	
	return (location);
}

/*
 *	@brief	Checks if a method is allowed in a given location.
 *	@param	method	The method to check.
 *	@param	location	The location.
 *	@return	TRUE if the method is allowed, FALSE if not.
*/
bool	Response::_IsMethodAllowed(const std::string &method, Location &location)
{
	if (location.getLocation() == "")
		return (true);
	
	std::vector<std::string>	allowedMethods = location.getAllowMethods();
	if (allowedMethods.size() == 0)
		return (true);

	for (std::vector<std::string>::iterator it = allowedMethods.begin() ; it != allowedMethods.end() ; it++)
		if (*it == method)
			return (true);
	
	return (false);
}

/*
 *	@brief	Checks if a method is allowed in a given location.
 *	@param	method	The method to check.
 *	@param	location	The location.
 *	@return	TRUE if the method is allowed, FALSE if not.
*/
bool	Response::_CheckAutoIndex(Server &server, Location &location)
{
	bool	autoIndex = location.getAutoIndex();

	if (autoIndex == true)
			return (true);

	std::string	index = "/index.html";
	if (location.getIndex().size() > 0)
		index = location.getIndex();
	
	std::string	path = location.getLocation();
	if (index[0] != '/')
		index = "/" + index;
	if (path[path.size() - 1] == '/')
			path.erase(path.size() - 1);
	
	std::string	root = server.getRoot();
	if (root[root.length() - 1] == '/')
		root.erase(root.length() - 1, 1);

	std::ifstream	file((root + path + index).c_str());
	if (!file.good())
		return (false);
	return (true);
}

/*
 *	@brief	Displays the error 400 page.
 *	@param	fd	The socket's file descriptor.
 *	@param	path The path of the corresponding page (let blank to use the default page).
 *	@return	The corresponding status code.
*/
int	Response::BadRequest400(int fd, const std::string &path)
{
	if (_WritePage(fd, path, "text/html", "400 Bad Request") != 200)
		Logger::error("Failed to send 400 response.");

	return (400);
}

/*
 *	@brief	Displays the error 403 page.
 *	@param	fd	The socket's file descriptor.
 *	@param	path The path of the corresponding page (let blank to use the default page).
 *	@return	The corresponding status code.
*/
int	Response::Forbidden403(int fd, const std::string &path)
{
	if (_WritePage(fd, path, "text/html", "403 Forbidden") != 200)
		Logger::error("Failed to send 403 response.");
	
	return (403);
}

/*
 *	@brief	Displays the error 404 page.
 *	@param	fd	The socket's file descriptor.
 *	@param	path The path of the corresponding page (let blank to use the default page).
 *	@return	The corresponding status code.
*/
int	Response::NotFound404(int fd, const std::string &path)
{
	if (_WritePage(fd, path, "text/html", "404 Not Found") != 200)
		Logger::error("Failed to send 404 response.");

	return (404);
}

/*
 *	@brief	Displays the error 405 page.
 *	@param	fd	The socket's file descriptor.
 *	@param	path The path of the corresponding page (let blank to use the default page).
 *	@return	The corresponding status code.
*/
int	Response::MethodNotAllowed405(int fd, const std::string &path)
{
	if (_WritePage(fd, path, "text/html", "405 Method Not Allowed") != 200)
		Logger::error("Failed to send 405 response.");

	return (405);
}

/*
 *	@brief	Displays the error 408 page.
 *	@param	fd	The socket's file descriptor.
 *	@param	path The path of the corresponding page (let blank to use the default page).
 *	@return	The corresponding status code.
*/
int	Response::RequestTimeout408(int fd, const std::string &path)
{
	if (_WritePage(fd, path, "text/html", "408 Request Timeout") != 200)
		Logger::error("Failed to send 408 response.");

	return (408);
}

/*
 *	@brief	Displays the error 413 page.
 *	@param	fd	The socket's file descriptor.
 *	@param	path The path of the corresponding page (let blank to use the default page).
 *	@return	The corresponding status code.
*/
int	Response::ContentTooLarge413(int fd, const std::string &path)
{
	if (_WritePage(fd, path, "text/html", "413 Content Too Large") != 200)
		Logger::error("Failed to send 413 response.");

	return (413);
}

/*
 *	@brief	Displays the error 500 page.
 *	@param	fd	The socket's file descriptor.
 *	@param	path The path of the corresponding page (let blank to use the default page).
 *	@return	The corresponding status code.
*/
int	Response::InternalServerError500(int fd, const std::string &path)
{
	if (_WritePage(fd, path, "text/html", "500 Internal Server Error") != 200)
		Logger::error("Failed to send 500 response.");

	return (500);
}

/*
 *	@brief	Displays the error 501 page.
 *	@param	fd	The socket's file descriptor.
 *	@param	path The path of the corresponding page (let blank to use the default page).
 *	@return	The corresponding status code.
*/
int	Response::MethodNotImplemented501(int fd, const std::string &path)
{
	if (_WritePage(fd, path, "text/html", "501 Not Implemented") != 200)
		Logger::error("Failed to send 501 response.");

	return (501);
}

/*
 *	@brief	Displays the error 502 page.
 *	@param	fd	The socket's file descriptor.
 *	@param	path The path of the corresponding page (let blank to use the default page).
 *	@return	The corresponding status code.
*/
int	Response::BadGateway502(int fd, const std::string &path)
{
	if (_WritePage(fd, path, "text/html", "502 Bad Gateway") != 200)
		Logger::error("Failed to send 502 response.");

	return (502);
}

/*
 *	@brief	Displays the error 503 page.
 *	@param	fd	The socket's file descriptor.
 *	@param	path The path of the corresponding page (let blank to use the default page).
 *	@return	The corresponding status code.
*/
int	Response::ServiceUnavailable503(int fd, const std::string &path)
{
	if (_WritePage(fd, path, "text/html", "503 Service Unavailable") != 200)
		Logger::error("Failed to send 503 response.");

	return (503);
}

/*
 *	@brief	Displays the error 504 page.
 *	@param	fd	The socket's file descriptor.
 *	@param	path The path of the corresponding page (let blank to use the default page).
 *	@return	The corresponding status code.
*/
int	Response::GatewayTimeout504(int fd, const std::string &path)
{
	if (_WritePage(fd, path, "text/html", "504 Gateway Timeout") != 200)
		Logger::error("Failed to send 504 response.");

	return (504);
}




/* ########## Exception ########## */


/* ########## Non-member function ########## */
