
#pragma once

#include "Libs.hpp"
#include "Utils.hpp"
#include "Parser.hpp"
#include "Logger.hpp"
#include "Cgi.hpp"
#include "Client.hpp"

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

class Client;

class Response
{
	private:
		// Constructors
		Response(void);
		Response(const Response &copy);
		Response &operator=(const Response &rhs);

		// Send Response Methods
		static std::string	_GetPath(const Server &server, const std::string &subPath);
		static int			_GetPort(const std::string &request);
		static std::string	_GetContentType(const std::string &request, const std::string &path);
		static Location		_GetLocation(const Server &server, const std::string &path);
		static bool			_IsMethodAllowed(const std::string &method, Location &location);
		static std::string	_GetMethod(const std::string &request);
		static bool			_CheckBodySize(const std::string &request, size_t maxBodySize);
		static void			_ReplaceRootInLocation(const Server &server, Location &location, std::string &path);

		// Auto-Index
		static bool			_CheckAutoIndex(const Server &server, const Location &location, \
								std::string &path, const std::string &requestPath);
		static bool			_GenerateIndexPage(const std::string &path, const std::string &requestPath);

		// Redirection
		static void			_CheckRedirection(const Server &server, Location &location, \
									std::string &path, std::string &status);

		// GET Methods
		static int			_WritePage(int fd, const Server &server, const std::string &path, \
						const std::string &type, const std::string &status, \
						const std::vector<std::string> &cookies, bool recursed = false);
		
		// POST Methods
		static int			_HandlePost(int fd, const Server &server, const std::string &request, \
										const std::string &path, const Location &location, \
										const std::vector<std::string> &cookies);
		static std::string	_GetBoundary(const std::string &request);
		static std::string	_ExtractFilename(const std::string &body);
		static bool			_CheckExtension(const std::string &filename, const std::vector<std::string> &ext);
		static std::string	_ExtractFileData(const std::string &body, const std::string &boundary);
		static std::string	_GenerateUploadsPage(const std::string &root);
		static void			_UpdateUploadsPage(const std::string &root);

		// DELETE Methods
		static int			_HandleDelete(int fd, const Server &server, const std::string &request, \
										const std::string &root, const std::vector<std::string> &cookies);
		static std::string	_GetFilePathToDelete(const std::string &request);

		// CGI
		static int			_HandleCgi(int fd, const Server &server, std::string &path, \
										const Location &location, char **env, \
										const std::vector<std::string> &cookies);
		static void			_GenerateCgiPage(const std::string &root, const std::string &path);
		static std::string	_GetExtension(const std::string &path);

	public:
		// Constructors
		~Response(void);

		// Methods
		static int			SendResponse(Client &client, char **env, std::string sessionId);

		// Error pages Methods
		static int			BadRequest400(int fd, const Server &server, const std::vector<std::string> &cookies);
		static int			Forbidden403(int fd, const Server &server, const std::vector<std::string> &cookies);
		static int			NotFound404(int fd, const Server &server, const std::vector<std::string> &cookies);
		static int			MethodNotAllowed405(int fd, const Server &server, const std::vector<std::string> &cookies);
		static int			RequestTimeout408(int fd, const Server &server, const std::vector<std::string> &cookies);
		static int			ContentTooLarge413(int fd, const Server &server, const std::vector<std::string> &cookies);
		static int			InternalServerError500(int fd, const Server &server, const std::vector<std::string> &cookies);
		static int			MethodNotImplemented501(int fd, const Server &server, const std::vector<std::string> &cookies);
		static int			BadGateway502(int fd, const Server &server, const std::vector<std::string> &cookies);
		static int			ServiceUnavailable503(int fd, const Server &server, const std::vector<std::string> &cookies);
		static int			GatewayTimeout504(int fd, const Server &server, const std::vector<std::string> &cookies);
};
