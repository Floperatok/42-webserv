#pragma once

#include "Libs.hpp"

#define VERBOSE 1

enum	Color {
	BLACK	= 30,
	RED		= 31,
	GREEN	= 32,
	YELLOW	= 33,
	BLUE	= 34,
	MAGENTA	= 35,
	CYAN	= 36,
	WHITE	= 37,
	RESET	= 0
};

class Logger
{
private:
	// Constructors
	Logger(void);
	Logger(const Logger &copy);
	Logger &operator=(const Logger &other);
	~Logger(void);

public:
	// Members
	static void	debug(const char *msg, int tagColor = RESET, int textColor = RESET);
	static void	info(const char *msg, int tagColor = CYAN, int textColor = RESET);
	static void	warning(const char *msg, int tagColor = YELLOW, int textColor = RESET);
	static void	error(const char *msg, int tagColor = RED, int textColor = RESET);

	// Exceptions
	class FunctionError : public std::exception
	{
	private:
		std::string			_message;
	public:
		FunctionError(const char *funcName, const int errnum);
		virtual ~FunctionError(void) throw();
		virtual const char	*what(void) const throw();
	};
};
