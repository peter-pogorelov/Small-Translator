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
		return false;
	}

	bool IsName(std::string& str)
	{
		if (isalpha(str[0]) && !IsKeyWord(str))
		{
			return true;
		}

		return false;
	}
}