
#pragma once

#include "Libs.hpp"
#include "Utils.hpp"
#include "Logger.hpp"

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
	static void	_SendResponse(int fd, std::string path);
public:
	// Constructors
	~Response(void);

	// Methods
	static void BadRequest400(int fd, std::string path = ERROR_400);
	static void	Forbidden403(int fd, std::string path = ERROR_403);
	static void NotFound404(int fd, std::string path = ERROR_404);
	static void MethodNotAllowed405(int fd, std::string path = ERROR_405);
	static void	RequestTimeout408(int fd, std::string path = ERROR_408);
	static void InternalServerError500(int fd, std::string path = ERROR_500);
	static void MethodNotImplemented501(int fd, std::string path = ERROR_501);
	static void BadGateway502(int fd, std::string path = ERROR_502);
	static void	ServiceUnavailable503(int fd, std::string path = ERROR_503);
	static void GatewayTimeout504(int fd, std::string path = ERROR_504);
};
