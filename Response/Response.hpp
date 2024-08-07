
#pragma once

#include "Libs.hpp"
#include "Utils.hpp"
#include "Parser.hpp"
#include "Logger.hpp"

#define	ERROR_CSS	"www/error_pages/error_pages.css"
#define ERROR_400	"www/error_pages/400.html"
#define ERROR_403	"www/error_pages/403.html"
#define ERROR_404	"www/error_pages/404.html"
#define ERROR_405	"www/error_pages/405.html"
#define ERROR_408	"www/error_pages/408.html"
#define ERROR_500	"www/error_pages/500.html"
#define ERROR_501	"www/error_pages/501.html"
#define ERROR_502	"www/error_pages/502.html"
#define ERROR_503	"www/error_pages/503.html"
#define ERROR_504	"www/error_pages/504.html"

class Response
{
private:
	// Constructors
	Response(void);
	Response(const Response &copy);
	Response &operator=(const Response &other);

	// Methods
	static int			_WritePage(int fd, std::string path, std::string type, std::string status);
	static Server		_GetServer(std::vector<Server> &servers, std::string request);
	static std::string	_GetPath(Server &server, std::string subPath);
	static int			_GetPort(std::string request);
	static std::string	_GetContentType(std::string request, std::string path);
public:
	// Constructors
	~Response(void);

	// Methods
	static int	SendResponse(std::vector<Server> &servers, int fd, std::string request);
	// Error pages
	static int	BadRequest400(int fd, std::string path = ERROR_400);
	static int	Forbidden403(int fd, std::string path = ERROR_403);
	static int	NotFound404(int fd, std::string path = ERROR_404);
	static int	MethodNotAllowed405(int fd, std::string path = ERROR_405);
	static int	RequestTimeout408(int fd, std::string path = ERROR_408);
	static int	InternalServerError500(int fd, std::string path = ERROR_500);
	static int	MethodNotImplemented501(int fd, std::string path = ERROR_501);
	static int	BadGateway502(int fd, std::string path = ERROR_502);
	static int	ServiceUnavailable503(int fd, std::string path = ERROR_503);
	static int	GatewayTimeout504(int fd, std::string path = ERROR_504);
};
