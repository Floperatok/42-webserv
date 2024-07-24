
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

/*
 *	Send a "Service Unavailable" reponse 
*/
void	Response::serviceUnavailable(int fd)
{
	std::ostringstream	oss;

    oss << "HTTP/1.1 503 Service Unavailable\r\nContent-Type: text/plain\r\nContent-Length: 19\r\n\r\nServer too busy\r\n";
	write(fd, oss.str().c_str(), oss.str().size());
}


/* ########## Exception ########## */


/* ########## Non-member function ########## */
