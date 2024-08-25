#include "Location.hpp"



/* ################################## CONSTRUCTORS ################################## */

Location::Location() : _location(""), _root(""), _index(""), _autoIndex(false), _redirect("") {}

Location::Location(const Location &toCopy)
{
	if (this != &toCopy)
	{
		_location = toCopy._location;
		_root = toCopy._root;
		_index = toCopy._index;
		_autoIndex = toCopy._autoIndex;
		_redirect = toCopy._redirect;
		_allowMethods = toCopy._allowMethods;
		_cgiPath = toCopy._cgiPath;
		_cgiExt = toCopy._cgiExt;
	}
}

Location	&Location::operator=(const Location &rhs)
{
	if (this != &rhs)
	{
		_location = rhs._location;
		_root = rhs._root;
		_index = rhs._index;
		_autoIndex = rhs._autoIndex;
		_redirect = rhs._redirect;
		_allowMethods = rhs._allowMethods;
		_cgiPath = rhs._cgiPath;
		_cgiExt = rhs._cgiExt;
	}
	return (*this);
}

Location::~Location() {}



/* ############################## GETTERS AND SETTERS ############################### */

void	Location::setLocation(const std::string &location) {	_location = location;	}
const std::string	&Location::getLocation() const {	return (_location);	}

void	Location::setRoot(const std::string &root) {	_root = root;	}
const std::string	&Location::getRoot() const {	return (_root);	}

void	Location::setIndex(const std::string &index) {	_index = index;	}
const std::string	&Location::getIndex() const {	return (_index);	}

void	Location::setAutoIndex(bool autoIndex) {	_autoIndex = autoIndex;	}
bool	Location::getAutoIndex() const {	return (_autoIndex);	}

void	Location::setRedirect(const std::string &redirect) {	_redirect = redirect;	}
const std::string	&Location::getRedirect() const {	return (_redirect);	}

void	Location::setAllowMethods(const std::vector<std::string> &allowMethods) {	_allowMethods = allowMethods;	}
const std::vector<std::string>	&Location::getAllowMethods() const {	return (_allowMethods);	}

void	Location::setCgiPath(const std::vector<std::string> &cgiPath) {	_cgiPath = cgiPath;	}
const std::vector<std::string>	&Location::getCgiPath() const {	return (_cgiPath);	}

void	Location::setCgiExt(const std::vector<std::string> &cgiExt) {	_cgiExt = cgiExt;	}
const std::vector<std::string>	&Location::getCgiExt() const {	return (_cgiExt);	}