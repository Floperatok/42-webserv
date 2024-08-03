
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

void	Response::BadRequest400(int fd, std::string path)
{
	std::ifstream		file(path.c_str());
	std::string			text;
	std::string			content;

	file >> content;
	text = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " \
			+ Utils::IntToStr(content.length()) + "\r\n\r\n";
	text += content;
	write(fd, text.c_str(), text.length());

	text = "Response send:\n" + text;
	Logger::info(content.c_str());
}

void	Response::Forbidden403(int fd, std::string path)
{
	std::ifstream		file(path.c_str());
	std::string			content;

	file >> content;
	write(fd, content.c_str(), content.length());
}

void	Response::NotFound404(int fd, std::string path)
{
	std::ifstream		file(path.c_str());
	std::string			content;

	file >> content;
	write(fd, content.c_str(), content.length());
}

void	Response::MethodNotAllowed405(int fd, std::string path)
{
	std::ifstream		file(path.c_str());
	std::string			content;

	file >> content;
	write(fd, content.c_str(), content.length());
}

void	Response::RequestTimeout408(int fd, std::string path)
{
	std::ifstream		file(path.c_str());
	std::string			content;

	file >> content;
	write(fd, content.c_str(), content.length());
}

void	Response::InternalServerError500(int fd, std::string path)
{
	std::ifstream		file(path.c_str());
	std::string			content;

	file >> content;
	write(fd, content.c_str(), content.length());
}

void	Response::MethodNotImplemented501(int fd, std::string path)
{
	std::ifstream		file(path.c_str());
	std::string			content;

	file >> content;
	write(fd, content.c_str(), content.length());
}

void	Response::BadGateway502(int fd, std::string path)
{
	std::ifstream		file(path.c_str());
	std::string			content;

	file >> content;
	write(fd, content.c_str(), content.length());
}

/*
 *	Send a "Service Unavailable" reponse 
*/
void	Response::ServiceUnavailable503(int fd, std::string path)
{
	std::ifstream		file(path.c_str());
	std::string			content;

	file >> content;
	write(fd, content.c_str(), content.length());
}

void	Response::GatewayTimeout504(int fd, std::string path)
{
	std::ifstream		file(path.c_str());
	std::string			content;

	file >> content;
	write(fd, content.c_str(), content.length());
}




/* ########## Exception ########## */


/* ########## Non-member function ########## */
