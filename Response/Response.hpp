
#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "Libs.hpp"
# include "Logger.hpp"

class Response
{
private:
	Response(void);
	Response(const Response &copy);
	Response &operator=(const Response &other);
public:
	static void BadRequest400(int fd);
	static void	Forbidden403(int fd);
	static void NotFound404(int fd);
	static void MethodNotAllowed405(int fd);
	static void	RequestTimeout408(int fd);
	static void InternalServerError500(int fd);
	static void MethodNotImplemented501(int fd);
	static void BadGateway502(int fd);
	static void	ServiceUnavailable503(int fd);
	static void GatewayTimeout504(int fd);
	~Response(void);
};

#endif