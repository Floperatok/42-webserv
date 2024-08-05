
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
void	Response::SendResponse(Server &server, int fd, std::string request)
{
	std::string					line = request.substr(0, request.find('\n'));
	std::vector<std::string>	req = Parser::SplitStr(line, " ");

	std::string					method = req[0];

	std::string					root = server.getRoot();
	// if (root[root.length() - 1] == '/')
	// 	root.erase(root.length() - 1, 1);

	std::cout << "ROOT = '" << server.getRoot() << "'" << std::endl;

	std::string					url = root + req[1];

	if (!method.compare("GET"))
	{
		_GetContentType(request);

		_WritePage(fd, url, "text/html", "200 OK");
	}
}

void	Response::_GetContentType(std::string request)
{
	size_t		start = request.find("Accept:") + 8;
	size_t		end = request.find(";", start) - 1;
	std::string	line = request.substr(start, end - start);

	std::vector<std::string>	types = Parser::SplitStr(line, ",");
	for (std::vector<std::string>::iterator it = types.begin() ; it != types.end() ; it++)
		std::cout << "\n TYPE: " << *it << std::endl;

}

void	Response::_WritePage(int fd, std::string path, std::string type, std::string status)
{
	std::ifstream		file(path.c_str());
	std::string			text;
	std::string			content;

	if (!file.is_open())
		Logger::error(("Can't open file '" + path + "'").c_str());
	
	getline(file, content, '\0');

	text = "HTTP/1.1 " + status + "\r\nContent-Type: " + type + "\r\nContent-Length: " \
			+ Utils::IntToStr(content.length()) + "\r\n\r\n";
	text += content;

	write(fd, text.c_str(), text.length());

	text = "Response send:\n" + text;
	Logger::info(content.c_str());
}

int	Response::BadRequest400(int fd, std::string path)
{
	_WritePage(fd, path, "text/html", "400 Bad Request");

	return (400);
}

int	Response::Forbidden403(int fd, std::string path)
{
	_WritePage(fd, path, "text/html", "403 Forbidden");

	return (403);
}

int	Response::NotFound404(int fd, std::string path)
{
	_WritePage(fd, path, "text/html", "404 Not Found");

	return (404);
}

int	Response::MethodNotAllowed405(int fd, std::string path)
{
	_WritePage(fd, path, "text/html", "405 Method Not Allowed");

	return (405);
}

int	Response::RequestTimeout408(int fd, std::string path)
{
	_WritePage(fd, path, "text/html", "408 Request Timeout");

	return (408);
}

int	Response::InternalServerError500(int fd, std::string path)
{
	_WritePage(fd, path, "text/html", "500 Internal Server Error");

	return (500);
}

int	Response::MethodNotImplemented501(int fd, std::string path)
{
	_WritePage(fd, path, "text/html", "501 Not Implemented");

	return (501);
}

int	Response::BadGateway502(int fd, std::string path)
{
	_WritePage(fd, path, "text/html", "502 Bad Gateway");

	return (502);
}

/*
 *	Send a "Service Unavailable" reponse 
*/
int	Response::ServiceUnavailable503(int fd, std::string path)
{
	_WritePage(fd, path, "text/html", "503 Service Unavailable");

	return (503);
}

int	Response::GatewayTimeout504(int fd, std::string path)
{
	_WritePage(fd, path, "text/html", "504 Gateway Timeout");

	return (504);
}




/* ########## Exception ########## */


/* ########## Non-member function ########## */
