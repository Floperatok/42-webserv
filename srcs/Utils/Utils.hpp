#pragma once

#include "Libs.hpp"
#include "Logger.hpp"

class Utils
{
	public:
		// Methods
		static int						StrToInt(const std::string &str);
		static size_t					StrToSizeT(const std::string &str);
		static std::string				IntToStr(int n);
		static std::vector<std::string> SplitStr(const std::string &str, const char *charset);
		static void 					TrimStr(std::string &str, const char *charset);
		static void						ExecCommand(const std::string &command, char **env);

	private:
		// Constructors
		Utils();
		~Utils();
		Utils(const Utils &toCopy);
		Utils &operator=(const Utils &rhs);
};