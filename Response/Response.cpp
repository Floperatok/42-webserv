
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

void	Response::BadRequest400(int fd)
{
	std::ostringstream	oss;

    oss << "HTTP/1.1 Error 400: Bad Request.\n";
	write(fd, oss.str().c_str(), oss.str().size());
}

void	Response::Forbidden403(int fd)
{
	std::ostringstream	oss;

    oss << "HTTP/1.1 Error 403: Forbidden.\n";
	write(fd, oss.str().c_str(), oss.str().size());
}

void	Response::NotFound404(int fd)
{
	std::ostringstream	oss;

    oss << "HTTP/1.1 Error 404: Not found.\n";
	write(fd, oss.str().c_str(), oss.str().size());
}

void	Response::MethodNotAllowed405(int fd)
{
	std::ostringstream	oss;

    oss << "HTTP/1.1 Error 405: Method not allowed.\n";
	write(fd, oss.str().c_str(), oss.str().size());
}

void	Response::RequestTimeout408(int fd)
{
	std::ostringstream	oss;

    oss << "HTTP/1.1 Error 408: Request timeout.\n";
	write(fd, oss.str().c_str(), oss.str().size());
}

void	Response::InternalServerError500(int fd)
{
	std::ostringstream	oss;

    oss << "HTTP/1.1 Error 500: Internal server error.\n";
	write(fd, oss.str().c_str(), oss.str().size());
}

void	Response::MethodNotImplemented501(int fd)
{
	std::ostringstream	oss;

    oss << "HTTP/1.1 Error 501: Method not implemented.\n";
	write(fd, oss.str().c_str(), oss.str().size());
}

void	Response::BadGateway502(int fd)
{
	std::ostringstream	oss;

    oss << "HTTP/1.1 Error 502: Bad gateway.\n";
	write(fd, oss.str().c_str(), oss.str().size());
}

/*
 *	Send a "Service Unavailable" reponse 
*/
void	Response::ServiceUnavailable503(int fd)
{
	std::ostringstream	oss;

    oss << "HTTP/1.1 Error 503: Service Unavailable\r\n"
		<< "Content-Type: text/plain\r\nContent-Length: 19\r\n\r\nServer too busy\r\n";
	write(fd, oss.str().c_str(), oss.str().size());
}

void	Response::GatewayTimeout504(int fd)
{
	std::ostringstream	oss;

    oss << "HTTP/1.1 Error 504: Gateway timeout.\n";
	write(fd, oss.str().c_str(), oss.str().size());
}




/* ########## Exception ########## */


/* ########## Non-member function ########## */
