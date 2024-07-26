#include "Utils.hpp"

/* ########## Constructors ########## */
Utils::Utils() {}

Utils::~Utils() {}

Utils::Utils(const Utils &toCopy)
{
	if (this != &toCopy)
		*this = toCopy;
}
Utils &Utils::operator=(const Utils &rhs)
{
	if (this != &rhs)
		*this = rhs;
	return (*this);
}


/* ########## Members functions ########## */

/*
 *	@brief Converts a string into an integer.
 *	@param str The string to convert.
 *	@return The converted integer.
*/
int	Utils::StrToint(const std::string &str)
{
	std::stringstream	ss;
	int					res = 0;

	ss << str;
	ss >> res;

	return (res);
}

/*
 *	@brief Converts an integer into a string.
 *	@param n The integer to convert.
 *	@return The converted string.
*/
std::string	Utils::IntToStr(int n)
{
	std::stringstream	ss;

	ss << n;

	return (ss.str());
}