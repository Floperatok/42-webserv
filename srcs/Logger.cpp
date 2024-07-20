
#include "Logger.hpp"


/* ########## Constructor ########## */

Logger::Logger(void)
{
}

Logger::Logger(const Logger &copy)
{
	if (&copy != this)
		*this = copy;
}

Logger::~Logger(void)
{
}


/* ########## Operator overload ########## */

Logger &Logger::operator=(const Logger &other)
{
	if (&other != this)
	{
		
	}
	return (*this);
}


/* ########## Setter/Getter ########## */


/* ########## Member function ########## */

const char *Logger::_colorCode(const int color)
{
	std::ostringstream oss;

	oss << "\033[" << color << "m";
	return (oss.str().c_str());
}

void	Logger::debug(const char *msg, int tagColor = RESET, int textColor = RESET)
{
	if (VERBOSE)
	{
		std::cout 	<< _colorCode(tagColor) << "DEBUG" 
					<< _colorCode(RESET) << " | "
					<< _colorCode(textColor) << msg
					<< _colorCode(RESET) << std::endl;
	}
}

void	Logger::info(const char *msg, int tagColor = CYAN, int textColor = RESET)
{
	std::cout 	<< _colorCode(tagColor) << "INFO" 
				<< _colorCode(RESET) << " | "
				<< _colorCode(textColor) << msg
				<< _colorCode(RESET) << std::endl;
}

void	Logger::warning(const char *msg, int tagColor = YELLOW, int textColor = RESET)
{
	std::cout 	<< _colorCode(tagColor) << "WARNING" 
				<< _colorCode(RESET) << " | "
				<< _colorCode(textColor) << msg
				<< _colorCode(RESET) << std::endl;
}

void	Logger::error(const char *msg, int tagColor = RED, int textColor = RESET)
{
	std::cout 	<< _colorCode(tagColor) << "ERROR" 
				<< _colorCode(RESET) << " | "
				<< _colorCode(textColor) << msg
				<< _colorCode(RESET) << std::endl;
}


/* ########## Exception ########## */


/* ########## Non-member function ########## */

