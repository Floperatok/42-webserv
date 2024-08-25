#include "Cookie.hpp"

/*
 *	@brief generate a id for working session.
 *	@param cookies The line about cookie in header.
 *	@param cookieName The name of cookie.
*/
std::string generateRandomSessionID()
{
    const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::string session_id;

    std::srand(std::time(0));

    for (size_t i = 0; i < 6; ++i) {
        int index = std::rand() % characters.size();
        session_id += characters[index];
    }

    return session_id;
}

/*
 *	@brief get the id of client.
 *	@param cookies The line about cookie in header.
 *	@param cookieName The name of cookie.
*/
std::string getCookieValue(const std::string &cookies, const std::string &cookieName)
{
    size_t pos = cookies.find(cookieName + "=");
    if (pos == std::string::npos) return "";
    pos += cookieName.length() + 1;
    size_t endPos = cookies.find(";", pos);
    if (endPos == std::string::npos) endPos = cookies.length();
    return cookies.substr(pos, endPos - pos);
}


/*
 *	@brief extract Cookie from request.
 *	@param request The content of request.
*/
std::string extractCookies(const std::string &request)
{
    size_t pos = request.find("Cookie: ");
    if (pos == std::string::npos) return "";
    pos += 8;
    size_t endPos = request.find("\r\n", pos);
    if (endPos == std::string::npos) endPos = request.length();
    return request.substr(pos, endPos - pos);
}