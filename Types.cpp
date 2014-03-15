#include "Types.h"

namespace SmallTranslator
{
	bool IsKeyWord(std::string& token)
	{
		if(token == "if")
			return true;
		if(token == "then")
			return true;
		if(token == "else")
			return true;
		if(token == "elif")
			return true;
		if(token == "while")
			return true;
		if(token == "do")
			return true;
		if(token == "end")
			return true;
		if(token == "and")
			return true;
		if(token == "or")
			return true;
		if(token == "in")
			return true;
		if(token == "out")
			return true;
		if(token == "void")
			return true;
		if(token == "func")
			return true;
		if(token == "return")
			return true;
		return false;
	}

	bool IsName(std::string& str)
	{
		if (isalpha(str[0]) && !IsKeyWord(str) || str[0] == '_')
		{
			return true;
		}

		return false;
	}

	bool IsNumber(std::string& str)
	{
		for (auto&c : str)
		{
			if (!isdigit(c))
				return false;
		}

		return true;
	}
}