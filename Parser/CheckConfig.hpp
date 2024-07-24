#pragma once

#include "Libs.hpp"
#include "Logger.hpp"
#include "Parser.hpp"

class Parser;

class CheckConfig
{
	public:
		// Constructors
		~CheckConfig();

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

		// Methods
		static bool CheckBrackets(std::string &content);
		static bool IsInsideBrackets(std::string &content, size_t index);
		static bool CheckServerKeywords(std::string &content);
		static void CheckKeywords(std::string &content);
};