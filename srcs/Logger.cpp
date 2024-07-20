
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

void	Logger::debug(const char *msg, int tagColor, int textColor)
{
	if (VERBOSE)
	{
		std::cout 	<< "\033[" << tagColor << ";1m" << "debug\t" 
					<< " | "
					<< "\033[" << textColor << "m" << msg
					<< "\033[" << RESET << "m" << std::endl;
	}
}

void	Logger::info(const char *msg, int tagColor, int textColor)
{
	std::cout 	<< "\033[" << tagColor << ";1m" << "info\t" 
				<< " | "
				<< "\033[" << textColor << "m" << msg
				<< "\033[" << RESET << "m" << std::endl;
}

void	Logger::warning(const char *msg, int tagColor, int textColor)
{
	std::cout 	<< "\033[" << tagColor << ";1m" << "warning\t" 
				<< " | "
				<< "\033[" << textColor << "m" << msg
				<< "\033[" << RESET << "m" << std::endl;
}

void	Logger::error(const char *msg, int tagColor, int textColor)
{
	std::cout 	<< "\033[" << tagColor << ";1m" << "error\t" 
				<< " | "
				<< "\033[" << textColor << "m" << msg
				<< "\033[" << RESET << "m" << std::endl;
}


/* ########## Exception ########## */

Logger::FunctionError::FunctionError(const char *funcName, const int errnum)
{
	std::ostringstream oss;

	if (errnum < 0)
		oss << "Function " << funcName << " failed";
	else
		oss << "Function " << funcName << " failed with error: " << strerror(errnum);
	_message = oss.str();
}
Logger::FunctionError::~FunctionError(void) throw()
{
}
const char *Logger::FunctionError::what(void) const throw()
{
	return (_message.c_str());
}


/* ########## Non-member function ########## */

