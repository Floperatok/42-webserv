#include "Logger.hpp"



/* ################################## CONSTRUCTORS ################################## */

Logger::Logger(void) {}

Logger::Logger(const Logger &copy)
{
	if (&copy != this)
		*this = copy;
}

Logger::~Logger(void) {}



/* ############################## OPERATOR'S OVERLOADS ############################## */

Logger &Logger::operator=(const Logger &other)
{
	if (&other != this)
		*this = other;
	
	return (*this);
}



/* ################################ MEMBER FUNCTIONS ################################ */

/*
 *	@brief Prints a debug log into the console.
 *	@param msg The message to print.
 *	@param tagColor The color of the "Debug" text. Optional, defaults to RESET.
 *	@param textColor The color of the message text. Optional, defaults to RESET.
*/
void	Logger::debug(const std::string &msg, int tagColor, int textColor)
{
	if (VERBOSE)
	{
		std::cout 	<< "\033[" << tagColor << ";1m" << "Debug\t" 
					<< "\033[" << RESET << "m" << " |\t"
					<< "\033[" << textColor << "m" << msg
					<< "\033[" << RESET << "m" << std::endl;
	}
}

/*
 *	@brief Prints an info log into the console.
 *	@param msg The message to print.
 *	@param tagColor The color of the "Info" text. Optional, defaults to CYAN.
 *	@param textColor The color of the message text. Optional, defaults to RESET.
*/
void	Logger::info(const std::string &msg, int tagColor, int textColor)
{
	std::cout 	<< "\033[" << tagColor << ";1m" << "Info\t" 
				<< "\033[" << RESET << "m" << " |\t"
				<< "\033[" << textColor << "m" << msg
				<< "\033[" << RESET << "m" << std::endl;
}

/*
 *	@brief Prints a warning log in the console.
 *	@param msg The message to print.
 *	@param tagColor The color of the "Warning" text. Optional, defaults to YELLOW.
 *	@param textColor The color of the message text. Optional, defaults to RESET.
*/
void	Logger::warning(const std::string &msg, int tagColor, int textColor)
{
	std::cout 	<< "\033[" << tagColor << ";1m" << "Warning\t" 
				<< "\033[" << RESET << "m" << " |\t"
				<< "\033[" << textColor << "m" << msg
				<< "\033[" << RESET << "m" << std::endl;
}

/*
 *	@brief Prints an error log in the console (on standard error output).
 *	@param msg The message to print.
 *	@param tagColor The color of the "Error" text. Optional, defaults to RED.
 *	@param textColor The color of the message text. Optional, defaults to RESET.
*/
void	Logger::error(const std::string &msg, int tagColor, int textColor)
{
	std::cerr 	<< "\033[" << tagColor << ";1m" << "Error\t" 
				<< "\033[" << RESET << "m" << " |\t"
				<< "\033[" << textColor << "m" << msg
				<< "\033[" << RESET << "m" << std::endl;
}



/* ################################### EXCEPTIONS ################################### */

/*
 *	@brief Gets an error message with strerror.
 *	@param funcName The name of the function which returned an error.
 *	@param errnum The error's status code.
*/
Logger::FunctionError::FunctionError(const std::string &funcName, const int errnum)
{
	std::ostringstream oss;

	if (errnum < 0)
		oss << "Function " << funcName << " failed";
	else
		oss << "Function " << funcName << " failed with error: " << strerror(errnum);
	_message = oss.str();
}

Logger::FunctionError::~FunctionError(void) throw() {}

/*
 *	@brief Gets an error message with strerror.
 *	@return The error message.
*/
const char *Logger::FunctionError::what(void) const throw() {	return (_message.c_str());	}
