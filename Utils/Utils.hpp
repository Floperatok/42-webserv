#pragma once

#include "Libs.hpp"

class Utils
{
	public:
		static int	strToint(const std::string &str);

	private:
		Utils();
		~Utils();
		Utils(const Utils &toCopy);
		Utils &operator=(const Utils &rhs);
};