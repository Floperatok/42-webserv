
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
#define ERROR_413	"www/error_pages/413.html"
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
	static int			_WritePage(int fd, const std::string &path, const std::string &type, \
									const std::string &status);
	static int			_HandlePost(int fd, const std::string &request, const std::string &root);
	static Server		_GetServer(std::vector<Server> &servers, const std::string &request);
	static std::string	_GetPath(Server &server, const std::string &subPath);
	static int			_GetPort(const std::string &request);
	static std::string	_GetContentType(const std::string &request, const std::string &path);
	static Location		_GetLocation(Server &server, const std::string &path);
	static bool			_IsMethodAllowed(const std::string &method, Location &location);
	static bool			_CheckAutoIndex(Server &server, Location &location);
	static std::string	_GetBoundary(const std::string &request);
	static std::string	_ExtractFilename(const std::string &body);
	static std::string	_ExtractFileData(const std::string &body, const std::string &boundary);
public:
	// Constructors
	~Response(void);

	// Methods
	static int	SendResponse(std::vector<Server> &servers, int fd, std::string request);
	// Error pages
	static int	BadRequest400(int fd, const std::string &path = ERROR_400);
	static int	Forbidden403(int fd, const std::string &path = ERROR_403);
	static int	NotFound404(int fd, const std::string &path = ERROR_404);
	static int	MethodNotAllowed405(int fd, const std::string &path = ERROR_405);
	static int	RequestTimeout408(int fd, const std::string &path = ERROR_408);
	static int	ContentTooLarge413(int fd, const std::string &path = ERROR_413);
	static int	InternalServerError500(int fd, const std::string &path = ERROR_500);
	static int	MethodNotImplemented501(int fd, const std::string &path = ERROR_501);
	static int	BadGateway502(int fd, const std::string &path = ERROR_502);
	static int	ServiceUnavailable503(int fd, const std::string &path = ERROR_503);
	static int	GatewayTimeout504(int fd, const std::string &path = ERROR_504);
};
