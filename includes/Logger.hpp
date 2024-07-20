
#ifndef LOGGER_HPP
# define LOGGER_HPP

# define VERBOSE 1

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

# include <iostream>
# include <sstream>

class Logger
{
private:
	Logger(void);
	Logger(const Logger &copy);
	Logger &operator=(const Logger &other);

public:
	~Logger(void);

	static void	debug(const char *msg, int tagColor = RESET, int textColor = RESET);
	static void	info(const char *msg, int tagColor = CYAN, int textColor = RESET);
	static void	warning(const char *msg, int tagColor = YELLOW, int textColor = RESET);
	static void	error(const char *msg, int tagColor = RED, int textColor = RESET);
};

#endif
