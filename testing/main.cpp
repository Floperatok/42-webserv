
#include <string>
#include <iostream>

int main(void)
{
	std::string a("salut");

	try
	{
		std::cout << a.at(7) << std::endl;
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
}