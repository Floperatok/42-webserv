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
int	Utils::strToint(const std::string &str)
{
	std::stringstream	ss;
	int					res = 0;

	ss << str;
	ss >> res;

	return (res);
}