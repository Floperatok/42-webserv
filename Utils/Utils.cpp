#include "Utils.hpp"



/* ################################## CONSTRUCTORS ################################## */

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


/* ################################ MEMBER FUNCTIONS ################################ */

/*
 *	@brief Converts a string into an integer.
 *	@param str The string to convert.
 *	@return The converted integer.
*/
int	Utils::StrToInt(const std::string &str)
{
	std::stringstream	ss;
	int					res = 0;

	ss << str;
	ss >> res;

	return (res);
}

/*
 *	@brief Converts a string into an unsigned long (size_t).
 *	@param str The string to convert.
 *	@return The converted size_t.
*/
size_t	Utils::StrToSizeT(const std::string &str)
{
	std::stringstream	ss;
	size_t				res = 0;

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

/*
 *	@brief Split a string into substrings delimited by a charset.
 *	@param str The string to split.
 *	@param charset The delimiting characters.
 *	@return The splited string.
*/
std::vector<std::string>	Utils::SplitStr(const std::string &str, const char *charset)
{
	std::vector<std::string>	splited;
	std::string					set = charset;

	for (size_t i = 0 ; i < str.length() ; i++)
	{
		size_t start = i;

		while (str[i] && set.find(str[i]) == std::string::npos)
			i++;
		
		splited.push_back(str.substr(start, i - start));

		while (set.find(str[i]) != std::string::npos && set.find(str[i + 1]) != std::string::npos)
			i++;
	}

	return (splited);
}

/*
 *	@brief Cuts the beginning and the end of a string according to a charset.
 *	@param str The string to trim.
 *	@param charset The delimiting characters.
*/
void	Utils::TrimStr(std::string &str, const char *charset)
{
	int		i = 0;
	std::string	set = charset;

	if (str.length() == 0 || str.find_first_of(charset) == std::string::npos)
		return ;

	while (str[i] && set.find(str[i]) != std::string::npos)
		i++;
	str.erase(0, i);

	size_t		len = str.length() - 1;
	i = len;
	while (i >= 0 && set.find_last_of(str[i]) != std::string::npos)
		i--;
	str.erase(i + 1, len - i);
}
