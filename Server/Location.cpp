#include "Location.hpp"

/* ########## Constructors ########## */
Location::Location() {}

Location::Location(const Location &toCopy)
{
	if (this != &toCopy)
		*this = toCopy;
}

Location	&Location::operator=(const Location &rhs)
{
	if (this != &rhs)
		*this = rhs;
	return (*this);
}

Location::~Location() {}


/* ########## Getters and setters ########## */
void	Location::setLocation(const std::string &location) {	_location = location;	}
const std::string	&Location::getLocation() const {	return (_location);	}

void	Location::setRoot(const std::string &root) {	_root = root;	}
const std::string	&Location::getRoot() const {	return (_root);	}

void	Location::setIndex(const std::string &index) {	_index = index;	}
const std::string	&Location::getIndex() const {	return (_index);	}

void	Location::setAutoIndex(bool &autoIndex) {	_autoIndex = autoIndex;	}
const bool	&Location::getAutoIndex() const {	return (_autoIndex);	}

void	Location::setAllowMethods(const std::vector<std::string> &allowMethods) {	_allowMethods = allowMethods;	}
const std::vector<std::string>	&Location::getAllowMethods() const {	return (_allowMethods);	}

void	Location::setCgiPath(const std::string &cgiPath) {	_cgiPath = cgiPath;	}
const std::string	&Location::getCgiPath() const {	return (_cgiPath);	}

void	Location::setCgiExt(const std::string &cgiExt) {	_cgiExt = cgiExt;	}
const std::string	&Location::getCgiExt() const {	return (_cgiExt);	}