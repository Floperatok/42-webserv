
#include "parsing.hpp"

std::string myStringTrim(const std::string &s, const char *charset)
{
	unsigned long first = s.find_first_not_of(charset);
	unsigned long last = s.find_last_not_of(charset);
	if (first == std::string::npos || last == std::string::npos)
		return (std::string());
	return (s.substr(first, last - first + 1));
}
