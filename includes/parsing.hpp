
#ifndef PARSING_HPP
# define PARSING_HPP

#include <string>

std::string myStringTrim(const std::string &s, const char *charset);
std::pair<std::string, std::string>	parseLine(std::string &line);

#endif