#pragma once
#include <string>
#include <cstdlib>
#include <ctime>
std::string getCookieValue(const std::string &cookies, const std::string &cookieName);
std::string extractCookies(const std::string &request);
std::string generateRandomSessionID();