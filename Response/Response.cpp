
#include "Response.hpp"



/* ################################## CONSTRUCTORS ################################## */

Response::Response(void) {}

Response::Response(const Response &copy)
{
	if (&copy != this)
		*this = copy;
}

Response::~Response(void) {}



/* ############################## OPERATOR'S OVERLOADS ############################## */

Response &Response::operator=(const Response &rhs)
{
	if (&rhs != this)
		*this = rhs;
	return (*this);
}



/* ################################ MEMBER FUNCTIONS ################################ */
/* ############# GENERAL FUNCTIONS ############# */

/*
 *	@brief Sends response to the appropriate server according to the client's request.
 *	@param servers	The vector's list of servers.
 *	@param fd The socket's file descriptor.
 *	@param request	The request.
 *	@return	The status code of the response sent.
*/
int	Response::SendResponse(std::vector<Server> &servers, int fd, std::string request, char **env)
{
	std::string					line = request.substr(0, request.find('\n'));
	std::vector<std::string>	req = Utils::SplitStr(line, " ");
	std::string					method = req[0];
	Server						server = _GetServer(servers, request);
	std::string					root = server.getRoot();
	std::string					path = _GetPath(server, req[1]);
	Location					location = _GetLocation(server, path);
	size_t						maxBodySize = server.getMaxBodySize();
	
	if (!_CheckAutoIndex(server, location))
	{
		Logger::error("Auto-index is OFF but no index was specified.");
		return (Forbidden403(fd, server));
	}

	if (!_CheckBodySize(request, maxBodySize))
	{
		Logger::error("The request's body size exceeds the body size limit.");
		return (ContentTooLarge413(fd, server));
	}

	if (method == "GET" && _IsMethodAllowed(method, location))
	{
		_GenerateCgiPage(root, req[1]);
		if (req[1].find("/cgi-bin/cgi_script?script=") != std::string::npos)
			return (_HandleCgi(fd, server, req[1], location, env));
		return (_WritePage(fd, server, path, _GetContentType(request, path), "200 OK"));
	}
	else if (method == "POST" && _IsMethodAllowed(method, location) && _GetMethod(request) == "")
		return (_HandlePost(fd, server, request, req[1], location));
	else if (method == "POST" && _GetMethod(request) == "DELETE" && _IsMethodAllowed("DELETE", location))
		return (_HandleDelete(fd, server, request, root));
		
	Logger::error("The request's method is not allowed.");
	return (MethodNotAllowed405(fd, server));
}

/*
 *	@brief Writes the header and the content into the socket fd.
 *	@param fd The socket's file descriptor.
 *	@param server The server to write on.
 *	@param path The path of the ressource we want to access to.
 *	@param type The content-type.
 *	@param status The string-formated status code.
 *	@param recursed Parameter to indicate if we are in a loop (default: false).
 *	@return	The status code.
*/
int	Response::_WritePage(int fd, const Server &server, const std::string &path, \
				const std::string &type, const std::string &status, bool recursed)
{
	std::ifstream		file(path.c_str(), std::ios::in | std::ios::binary);

	if (!file.is_open())
	{
		Logger::error("Can't open file '" + path + "'.");
		if (recursed)
			return (-1);
		return (NotFound404(fd, server));
	}
	
	std::vector<char> content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	
	file.close();

	std::string header = "HTTP/1.1 " + status + "\r\nContent-Type: " + type + "\r\nContent-Length: " \
			+ Utils::IntToStr(content.size()) + "\r\n\r\n";

	if (send(fd, header.c_str(), header.size(), MSG_NOSIGNAL) < 0)
	{
		Logger::error("Failed to write header.");
		if (recursed)
			return (-1);
		return (InternalServerError500(fd, server));
	}

	ssize_t bytesWritten = send(fd, content.data(), content.size(), MSG_NOSIGNAL);
	if (bytesWritten < 0)
	{
		Logger::error("Failed to write content.");
		if (recursed)
			return (-1);
		return (InternalServerError500(fd, server));
	}
	else if (static_cast<size_t>(bytesWritten) != content.size())
	{
		Logger::error("Incomplete content written.");
		if (recursed)
			return (-1);
		return InternalServerError500(fd, server);
	}

	Logger::debug("Response sent: HTTP/1.1 " + status + " | Content-Type: " + type \
				+ " | Content-Length: " + Utils::IntToStr(content.size()) + ".");

	return (200);
}


/* ############ POST METHOD HANDLER ############ */

/*
 *	@brief Upload a file into the server, creates the file, and sends a success page in case of success.
 *	@param fd The socket's file descriptor.
 *	@param server The server on which the file is uploaded.
 *	@param request The request.
 *	@param path The path from the request.
 *	@return	200 in case of success, 500 in case of error.
*/
int Response::_HandlePost(int fd, const Server &server, const std::string &request, \
					const std::string &path, const Location &location)
{
	std::string		root = server.getRoot();
    std::string		body = request.substr(request.find("\r\n\r\n") + 4);
    std::string		boundary = _GetBoundary(request);
    std::string		filename = _ExtractFilename(body);
    std::string		fileData = _ExtractFileData(body, boundary);

	if (path == "/cgi-bin" && !_CheckExtension(filename, location.getCgiExt()))
		return (Forbidden403(fd, server));

    std::string		uploadPath = root + "uploads/" + filename;
	if (path == "/cgi-bin")
		uploadPath = root + "cgi-bin/" + filename;

	if (boundary.empty() || filename.empty() || fileData.empty())
		return (InternalServerError500(fd, server));
	
    std::ofstream	outFile(uploadPath.c_str(), std::ios::binary);
    if (!outFile)
	{
		Logger::error("Failed to open '" + uploadPath + "' for writting.");
        return (InternalServerError500(fd, server));
	}
    
    outFile.write(fileData.c_str(), fileData.size());
    outFile.close();

	if (path == "/cgi-bin")
		_WritePage(fd, server, root + "script_uploaded.html", "text/html", "200 OK");
	else
	{
	_UpdateUploadsPage(root);
	_WritePage(fd, server, root + "file_uploaded.html", "text/html", "200 OK");
	}
    
    return (200);
}

/*
 *	@brief Gets the request's boundary.
 *	@param request	The request.
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
 *	@brief Extracts the file's name from the request's body.
 *	@param body	The request's body.
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
 *	@brief Checks if cgi extension is allowed.
 *	@param filename The file's name;
 *	@parem ext The allowed extensions from config file.
*/
bool	Response::_CheckExtension(const std::string &filename, const std::vector<std::string> &ext)
{
	std::string	name = filename;
	std::vector<std::string>	splittedFilename = Utils::SplitStr(name, ".");

	if (splittedFilename.size() > 2)
		return (false);
	else if (splittedFilename.size() < 2)
		return (true);

	std::string extension = "." + splittedFilename[1];
	
	for (size_t i = 0 ; i < ext.size() ; i++)
		if (extension == ext[i])
			return (true);
	
	return (false);
}

/*
 *	@brief Extracts a file's data from the request's body.
 *	@param body	The request's body.
 *	@param boundary	The request's boundary.
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
 *	@brief Generates an uploads.html page with links to uploaded files.
 *	@param root	The website's root.
 *	@return	The resulted file's content.
*/
std::string	Response::_GenerateUploadsPage(const std::string &root)
{
	std::string uploadDir = root + "uploads/";
	std::string pageContent = "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n";
	pageContent += "\t<meta charset=\"UTF-8\">\n";
	pageContent += "\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
	pageContent += "\t<title>Uploads</title>\n\t<link rel=\"stylesheet\" href=\"style.css\">\n</head>\n<body>\n";
	pageContent += "\t<header>\n\t\t<nav>\n\t\t\t<ul>\n";
	pageContent += "\t\t\t\t<li><a href=\"index.html\">Home</a></li>\n";
	pageContent += "\t\t\t\t<li><a href=\"post.html\">Upload a file</a></li>\n";
	pageContent += "\t\t\t\t<li><a href=\"cgi.html\">CGI Scripts</a></li>\n";
	pageContent += "\t\t\t\t<li><a href=\"nonexistent.html\">Error 404</a></li>\n";
	pageContent += "\t\t\t</ul>\n\t\t</nav>\n\t</header>\n\t<main>\n";
	pageContent += "\t\t<h1>Uploaded Files</h1>\n\t\t<ul>\n";

	DIR *dir = opendir(uploadDir.c_str());
	if (!dir)
		pageContent += "\t\t\t<li>Unable to open uploads directory.</li>\n";
	else
	{
		struct dirent *entry;
		while ((entry = readdir(dir)))
			if (entry->d_type == DT_REG)
			{
				std::string	fileName = entry->d_name;
				pageContent += "\t\t\t<li>";
				pageContent += "<a href=\"uploads/" + fileName + "\" class=\"uploaded-files-link\">" + fileName + "</a>";
				pageContent += " <form action=\"/delete_test\" method=\"POST\" style=\"display:inline;\">";
				pageContent += "<input type=\"hidden\" name=\"_method\" value=\"DELETE\">";
				pageContent += "<input type=\"hidden\" name=\"filePath\" value=\"" + fileName + "\">";
				pageContent += "<input type=\"submit\" value=\"Delete\" style=\"background-color:#e74c3c;color:#fff;border:none;padding:5px 10px;cursor:pointer;\">";
				pageContent += "</form>";
				pageContent += "</li>\n";
			}
		closedir(dir);
	}

	pageContent += "\t\t</ul>\n\t</main>\n\t<footer>\n\t\t<p>&copy; 2024, by drenassi and nsalles.</p>\n";
	pageContent += "\t</footer>\n</body>\n</html>";

	return pageContent;
}

/*
 *	@brief Update uploads.html page.
 *	@param root The website's root directory.
*/
void	Response::_UpdateUploadsPage(const std::string &root)
{
	std::string		uploadsPageContent = _GenerateUploadsPage(root);
	std::ofstream	uploadsFile((root + "uploads.html").c_str());

	if (uploadsFile.is_open())
	{
		uploadsFile << uploadsPageContent;
		uploadsFile.close();
	}
	else
		Logger::error("Failed to update uploads.html.");
}


/* ########### DELETE METHOD HANDLER ########### */

/*
 *	@brief Deletes a file from uploaded files.
 *	@param fd The socket's file descriptor.
 *	@param request The request.
 *	@param root The website's root directory.
 *	@return	200 in case of success.
 *	@return 500 in case of error.
*/
int Response::_HandleDelete(int fd, const Server &server, const std::string &request, const std::string &root)
{
    std::string path = root + "uploads/" + _GetFilePathToDelete(request);

    if (std::remove(path.c_str()))
    {
        Logger::error(("Failed to delete file: " + path).c_str());
        return (InternalServerError500(fd, server));
    }

	_UpdateUploadsPage(root);

    _WritePage(fd, server, "www/file_deleted.html", "text/html", "200 OK");

    return (200);
}

/*
 *	@brief Gets the path of a file to delete from a request with DELETE method.
 *	@param request The request.
 *	@return The path of the file to delete.
*/
std::string	Response::_GetFilePathToDelete(const std::string &request)
{
	std::string	pathToDelete;
	size_t		start = request.find("_method=DELETE&filePath=") + 24;

	if (start == std::string::npos)
		return ("");

	size_t		end = request.find("\r\n");

	pathToDelete = request.substr(start, end - start);

	return (pathToDelete);
}


/* ################ CGI HANDLER ################ */

/*
 *	@brief Handle CGI GET requests.
 *	@param fd The socket's file descriptor.
 *	@param root The website's root.
 *	@param path The path from the request.
 *	@param location The CGI location.
 *	@return The appropriate status code.
*/
int	Response::_HandleCgi(int fd, const Server &server, std::string &path, const Location &location, char **env)
{
	
	path.erase(9, 18);
	path.erase(0, 1);
	path = server.getRoot() + path;

	std::string	ext = _GetExtension(path);

	std::string	cgiPath;
	if (ext.empty())
		cgiPath = "";
	else
	{
		std::vector<std::string>	cgiPaths = location.getCgiPath();
		std::vector<std::string>	cgiExts = location.getCgiExt();

		for (size_t i = 0 ; i < cgiExts.size() ; i++)
			if (ext == cgiExts[i])
				cgiPath = cgiPaths[i];
		if (cgiPath.empty())
			return (InternalServerError500(fd, server));
	}

	std::string	content;
	if (Cgi::getContent(path, cgiPath, content, env) < 0)
		return (InternalServerError500(fd, server));
	
	std::string	contentType = "text/plain";

	if (content.find("Content-type: ") == 0)
	{
		size_t	start = content.find("Content-type: ") + 14;
		size_t	end = content.find("\r\n", start);
		if (end != std::string::npos)
		{
			contentType = content.substr(start, end - start);
			content = content.substr(end + 2);
		}
	}

	std::string headers = "HTTP/1.1 200 OK\r\n";
	headers += "Content-Length: " + Utils::IntToStr(content.size()) + "\r\n";
	headers += "Content-Type: " + contentType + "\r\n\r\n";

	if (send(fd, headers.c_str(), headers.size(), MSG_NOSIGNAL) < 0)
	{
		Logger::error("Failed to write content.");
		return (InternalServerError500(fd, server));
	}

	ssize_t	bytesWritten = send(fd, content.c_str(), content.size(), MSG_NOSIGNAL);
	if (bytesWritten < 0)
	{
		Logger::error("Failed to write CGI content.");
		return (InternalServerError500(fd, server));
	}
	else if (static_cast<size_t>(bytesWritten) != content.size())
	{
		Logger::error("Incomplete CGI content written.");
		return InternalServerError500(fd, server);
	}

	Logger::debug("Response sent: HTTP/1.1 200 OK | Content-Type: " + contentType \
				+ " | Content-Length: " + Utils::IntToStr(content.size()));

	return (200);
}

/*
 *	@brief Generates cgi.html page.
 *	@param root The server's root.
 *	@param path The path to the html page from the request.
*/
void	Response::_GenerateCgiPage(const std::string &root, const std::string &path)
{
	if (path == "/cgi.html")
	{
		std::string		cgiPageContent = Cgi::GenerateCgiPage();
		std::ofstream	outFile((root + "cgi.html").c_str(), std::ios::binary);
		if (!outFile)
			Logger::error("Failed to generate cgi.html page.");
		outFile.write(cgiPageContent.c_str(), cgiPageContent.size());
		outFile.close();
	}
}


/* ############# GENERAL FUNCTIONS ############# */

/*
 *	@brief Gets the extension from a file.
 *	@param path The file's path from request.
 *	@return The extension.
*/
std::string	Response::_GetExtension(const std::string &path)
{
	std::string					ext = path;
	std::vector<std::string>	splitted = Utils::SplitStr(ext, ".");

	if (splitted.size() == 1)
		return ("");
	
	ext = "." + splitted[1];

	return (ext);
}

/*
 *	@brief Gets the server from which request is done.
 *	@param servers	The vector's list of servers.
 *	@param request	The request.
 *	@return	The corresponding server.
*/
Server	Response::_GetServer(const std::vector<Server> &servers, const std::string &request)
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
 *	@brief Gets the port of the server from which request is done.
 *	@param request	The request.
 *	@return	The server's port.
*/
std::string	Response::_GetPath(const Server &server, const std::string &subPath)
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
 *	@brief Gets the port of the server from which request is done.
 *	@param request	The request.
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
 *	@brief Gets the header's content type from a request.
 *	@param request	The request.
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
	std::string					line = request.substr(start, end - start + 1);
	std::vector<std::string>	types = Utils::SplitStr(line, ",");

	std::string					ext = path.substr(path.find(".") + 1);
	if (ext == "jpg")
		ext = "jpeg";
	else if (ext == "js")
		ext = "javascript";

	for (std::vector<std::string>::iterator it = types.begin() ; it != types.end() ; it++)
	{
		std::vector<std::string>	type = Utils::SplitStr(*it, "/");
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
 *	@brief Gets the appropriate server's location according to the request's path.
 *	@param server The server.
 *	@param path The request's path.
 *	@return	The current location.
*/
Location		Response::_GetLocation(const Server &server, const std::string &path)
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
 *	@brief Checks if a method is allowed in a given location.
 *	@param method	The method to check.
 *	@param location	The location.
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
 *	@brief Checks if a method is allowed in a given location.
 *	@param method	The method to check.
 *	@param location	The location.
 *	@return	TRUE if the method is allowed, FALSE if not.
*/
bool	Response::_CheckAutoIndex(const Server &server, const Location &location)
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
 *	@brief Gets the hidden method from a request.
 *	@param request	The request.
 *	@return	The method, or an empty string if no hidden method was found.
*/
std::string	Response::_GetMethod(const std::string &request)
{
	std::string	method;
	size_t		start = request.find("_method=");

	if (start == std::string::npos)
		return ("");

	start += 8;

	size_t		end = request.find("&", start);
	method = request.substr(start, end - start);

	return (method);
}

/*
 *	@brief Checks if a request's body size is not too big.
 *	@request The request.
 *	@return TRUE if the request's body size is ok, FALSE if it's too large.
*/
bool	Response::_CheckBodySize(const std::string &request, size_t maxBodySize)
{
	size_t	start = request.find("Content-Length: ");
	if (start == std::string::npos)
		return (true);

	size_t	end = request.find("\r\n", start);

	size_t	contentLength = Utils::StrToSizeT(request.substr(start + 16, end - (start + 16)));
	if (maxBodySize && contentLength > maxBodySize)
		return (false);
	else if (!maxBodySize && contentLength > MAX_BODY_SIZE)
		return (false);
	
	return (true);
}


/* ################ ERROR PAGES ################ */

/*
 *	@brief Displays the error 400 page.
 *	@param fd	The socket's file descriptor.
 *	@param path The path of the corresponding page (put an empty string to use the default page).
 *	@return	The corresponding status code.
*/
int	Response::BadRequest400(int fd, const Server &server)
{
	std::string	root = server.getRoot();
	std::string	path = server.getErrorPage400();
	
	if (path.empty())
		path = ERROR_400;
	else
		path = root + path;

	if (_WritePage(fd, server, path, "text/html", "400 Bad Request", true) == -1)
		Logger::error("Failed to send 400 response.");

	return (400);
}

/*
 *	@brief Displays the error 403 page.
 *	@param fd	The socket's file descriptor.
 *	@param path The path of the corresponding page (put an empty string to use the default page).
 *	@return	The corresponding status code.
*/
int	Response::Forbidden403(int fd, const Server &server)
{
	std::string	root = server.getRoot();
	std::string	path = server.getErrorPage403();
	
	if (path.empty())
		path = ERROR_403;
	else
		path = root + path;

	if (_WritePage(fd, server, path, "text/html", "403 Forbidden", true) == -1)
		Logger::error("Failed to send 403 response.");
	
	return (403);
}

/*
 *	@brief Displays the error 404 page.
 *	@param fd	The socket's file descriptor.
 *	@param path The path of the corresponding page (put an empty string to use the default page).
 *	@return	The corresponding status code.
*/
int	Response::NotFound404(int fd, const Server &server)
{
	std::string	root = server.getRoot();
	std::string	path = server.getErrorPage404();
	
	if (path.empty())
		path = ERROR_404;
	else
		path = root + path;

	if (_WritePage(fd, server, path, "text/html", "404 Not Found", true) == -1)
		Logger::error("Failed to send 404 response.");

	return (404);
}

/*
 *	@brief Displays the error 405 page.
 *	@param fd	The socket's file descriptor.
 *	@param path The path of the corresponding page (put an empty string to use the default page).
 *	@return	The corresponding status code.
*/
int	Response::MethodNotAllowed405(int fd, const Server &server)
{
	std::string	root = server.getRoot();
	std::string	path = server.getErrorPage405();
	
	if (path.empty())
		path = ERROR_405;
	else
		path = root + path;

	if (_WritePage(fd, server, path, "text/html", "405 Method Not Allowed", true) == -1)
		Logger::error("Failed to send 405 response.");

	return (405);
}

/*
 *	@brief Displays the error 408 page.
 *	@param fd	The socket's file descriptor.
 *	@param path The path of the corresponding page (put an empty string to use the default page).
 *	@return	The corresponding status code.
*/
int	Response::RequestTimeout408(int fd, const Server &server)
{
	std::string	root = server.getRoot();
	std::string	path = server.getErrorPage408();
	
	if (path.empty())
		path = ERROR_408;
	else
		path = root + path;

	if (_WritePage(fd, server, path, "text/html", "408 Request Timeout", true) == -1)
		Logger::error("Failed to send 408 response.");

	return (408);
}

/*
 *	@brief Displays the error 413 page.
 *	@param fd	The socket's file descriptor.
 *	@param path The path of the corresponding page (put an empty string to use the default page).
 *	@return	The corresponding status code.
*/
int	Response::ContentTooLarge413(int fd, const Server &server)
{
	std::string	root = server.getRoot();
	std::string	path = server.getErrorPage413();
	
	if (path.empty())
		path = ERROR_413;
	else
		path = root + path;

	if (_WritePage(fd, server, path, "text/html", "413 Content Too Large", true) == -1)
		Logger::error("Failed to send 413 response.");

	return (413);
}

/*
 *	@brief Displays the error 500 page.
 *	@param fd	The socket's file descriptor.
 *	@param path The path of the corresponding page (put an empty string to use the default page).
 *	@return	The corresponding status code.
*/
int	Response::InternalServerError500(int fd, const Server &server)
{
	std::string	root = server.getRoot();
	std::string	path = server.getErrorPage500();
	
	if (path.empty())
		path = ERROR_500;
	else
		path = root + path;

	if (_WritePage(fd, server, path, "text/html", "500 Internal Server Error", true) == -1)
		Logger::error("Failed to send 500 response.");

	return (500);
}

/*
 *	@brief Displays the error 501 page.
 *	@param fd	The socket's file descriptor.
 *	@param path The path of the corresponding page (put an empty string to use the default page).
 *	@return	The corresponding status code.
*/
int	Response::MethodNotImplemented501(int fd, const Server &server)
{
	std::string	root = server.getRoot();
	std::string	path = server.getErrorPage501();
	
	if (path.empty())
		path = ERROR_501;
	else
		path = root + path;

	if (_WritePage(fd, server, path, "text/html", "501 Not Implemented", true) == -1)
		Logger::error("Failed to send 501 response.");

	return (501);
}

/*
 *	@brief Displays the error 502 page.
 *	@param fd	The socket's file descriptor.
 *	@param path The path of the corresponding page (put an empty string to use the default page).
 *	@return	The corresponding status code.
*/
int	Response::BadGateway502(int fd, const Server &server)
{
	std::string	root = server.getRoot();
	std::string	path = server.getErrorPage502();
	
	if (path.empty())
		path = ERROR_502;
	else
		path = root + path;

	if (_WritePage(fd, server, path, "text/html", "502 Bad Gateway", true) == -1)
		Logger::error("Failed to send 502 response.");

	return (502);
}

/*
 *	@brief Displays the error 503 page.
 *	@param fd	The socket's file descriptor.
 *	@param path The path of the corresponding page (put an empty string to use the default page).
 *	@return	The corresponding status code.
*/
int	Response::ServiceUnavailable503(int fd, const Server &server)
{
	std::string	root = server.getRoot();
	std::string	path = server.getErrorPage503();
	
	if (path.empty())
		path = ERROR_503;
	else
		path = root + path;

	if (_WritePage(fd, server, path, "text/html", "503 Service Unavailable", true) == -1)
		Logger::error("Failed to send 503 response.");

	return (503);
}

/*
 *	@brief Displays the error 504 page.
 *	@param fd	The socket's file descriptor.
 *	@param path The path of the corresponding page (put an empty string to use the default page).
 *	@return	The corresponding status code.
*/
int	Response::GatewayTimeout504(int fd, const Server &server)
{
	std::string	root = server.getRoot();
	std::string	path = server.getErrorPage504();
	
	if (path.empty())
		path = ERROR_504;
	else
		path = root + path;

	if (_WritePage(fd, server, path, "text/html", "504 Gateway Timeout", true) == -1)
		Logger::error("Failed to send 504 response.");

	return (504);
}
