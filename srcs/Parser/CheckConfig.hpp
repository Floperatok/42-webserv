#pragma once

#include "Libs.hpp"
#include "Logger.hpp"
#include "Parser.hpp"

class Parser;

class CheckConfig
{
	public:
		// Methods
		static void CheckConfigFile(const std::string &path);

		// Exceptions
		class FileNotOpenedException : public std::exception
		{
			public :
				virtual const char	*what() const throw();
		};
		class EmptyFileException : public std::exception
		{
			public :
				virtual const char	*what() const throw();
		};
		class UnclosedBracketsException : public std::exception
		{
			public :
				virtual const char	*what() const throw();
		};
		class WrongKeywordException : public std::exception
		{
			public :
				virtual const char	*what() const throw();
		};
		class MissingParameterException : public std::exception
		{
			public:
				MissingParameterException(std::string message) throw();
				virtual const char	*what() const throw();
				virtual ~MissingParameterException() throw() {}
			private:
				std::string _msg;
		};
		class WrongParameterException : public std::exception
		{
			public:
				WrongParameterException(std::string message) throw();
				virtual const char	*what() const throw();
				virtual ~WrongParameterException() throw() {}
			private:
				std::string _msg;
		};

	private:
		// Constructors
		CheckConfig();
		CheckConfig(const CheckConfig &toCopy);
		CheckConfig &operator=(const CheckConfig &rhs);
		~CheckConfig();

		// Methods
		static bool _CheckBrackets(const std::string &content);
		static bool _IsInsideBrackets(const std::string &content, size_t index);
		static bool _CheckServerKeywords(const std::string &content);
		static void _CheckKeywords(const std::string &content);
};