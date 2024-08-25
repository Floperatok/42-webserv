#pragma once

#include "Libs.hpp"
#include "Logger.hpp"

class Location
{
	public:
		// Constructors
		Location();
		Location(const Location &toCopy);
		Location	&operator=(const Location &rhs);
		~Location();

		// Getters and setters
		void							setLocation(const std::string &location);
		const std::string				&getLocation() const;
		void							setRoot(const std::string &root);
		const std::string				&getRoot() const;
		void							setIndex(const std::string &index);
		const std::string				&getIndex() const;
		void							setAutoIndex(bool autoIndex);
		bool							getAutoIndex() const;
		const std::string				&getRedirect() const;
		void							setRedirect(const std::string &redirect);
		void							setAllowMethods(const std::vector<std::string> &allowMethods);
		const std::vector<std::string>	&getAllowMethods() const;
		void							setCgiPath(const std::vector<std::string> &cgiPath);
		const std::vector<std::string>	&getCgiPath() const;
		void							setCgiExt(const std::vector<std::string> &cgiExt);
		const std::vector<std::string>	&getCgiExt() const;

	private:
		std::string						_location;
		std::string						_root;
		std::string						_index;
		bool							_autoIndex;
		std::string						_redirect;
		std::vector<std::string>		_allowMethods;
		std::vector<std::string>		_cgiPath;
		std::vector<std::string>		_cgiExt;
};