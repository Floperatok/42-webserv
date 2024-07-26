#pragma once

#include "Libs.hpp"

class Utils
{
	public:
		static int	StrToint(const std::string &str);
		static std::string	IntToStr(int n);

	private:
		Utils();
		~Utils();
		Utils(const Utils &toCopy);
		Utils &operator=(const Utils &rhs);
};