#include "Parser.hpp"

int main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cout << "Error: Need 1 argument" << std::endl;
		return (1);
	}
	Parser *parser = new Parser(av[1]);


	delete parser;
}