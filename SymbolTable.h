#pragma once

#include <string>
#include <vector>

namespace SmallTranslator
{
	class SymbolTable
	{
	public:
		friend class Compiler;

		SymbolTable()
		{
		}

		~SymbolTable()
		{
		}

		void Add(std::string& var)
		{
			if (!Exist(var))
				vars.push_back(var);
		}

		bool Exist(std::string& var)
		{
			for (auto& i : vars)
			{
				if (var == i)
					return true;
			}

			return false;
		}

	public:
		std::vector<std::string> vars;
	};
}