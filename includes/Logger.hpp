
#ifndef LOGGER_HPP
# define LOGGER_HPP

# define VERBOSE 1

enum	Color {
	BLACK	= 40,
	RED		= 41,
	GREEN	= 42,
	YELLOW	= 43,
	BLUE	= 44,
	MAGENTA	= 45,
	CYAN	= 46,
	WHITE	= 47,
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

	static const char	*_colorCode(const int color);
public:
	~Logger(void);

	static void	debug(const char *msg, int tagColor = RESET, int textColor = RESET);
	static void	info(const char *msg, int tagColor = CYAN, int textColor = RESET);
	static void	warning(const char *msg, int tagColor = YELLOW, int textColor = RESET);
	static void	error(const char *msg, int tagColor = RED, int textColor = RESET);
};

#endif
