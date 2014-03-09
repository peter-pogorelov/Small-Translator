#include <iostream>
#include <functional>
#include <typeinfo>
#include <string>

#include <fstream>

#include "Compiler.h"

std::string LoadFromFile(const char* path)
{
	std::string result;
	std::ifstream in(path, std::ios::in);

	if (in.is_open())
	{
		while (!in.eof())
		{
			static char tmp[128];
			in.getline(tmp, sizeof tmp);
			result += tmp;
			result += '\n';
		}
	}

	return result;
}

void main()
{
	std::string code = LoadFromFile("code.txt");

	if (!code.empty())
	{
		SmallTranslator::Compiler comp(const_cast<char*>(code.c_str()));
		std::cout << comp.Compile();
	}
	

	getchar();
	return;
}