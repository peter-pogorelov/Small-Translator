#include <iostream>
#include <functional>
#include <typeinfo>
#include <string>

#include "Compiler.h"

void main()
{
	SmallTranslator::Compiler comp(
		"decl a b c;"
		"if a == b then "
		"a = 1 < 2 and 2 > 1 or 3 < 1;"
		"else "
		"b = 10;"
		"end"
		);
	std::cout << comp.Compile();
	

	getchar();
	return;
}