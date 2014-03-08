#pragma once

#include <string>
#include <vector>

#define BITS_WHILE	1 << 0
#define BITS_EXPR	1 << 1
#define BITS_IF		1 << 2

#include "Types.h"

#define IF_STMT 1 << 0

#define ELSE_NODE 1 << 0

namespace SmallTranslator
{
	class AstBuilder
	{
	public:

		AstBuilder();
		AstBuilder(char* code);

		SymbolTable& GetTable();

		Block* BuildAST();
		Block* Build(int& params, int bits = 0);
		bool BuildDecl();
		While* BuildWhile();
		If* BuildIf();
		Expression* BuildExpression(unsigned char bits, std::string& prevToken = std::string());
		
		std::string GetNextToken();

	private:
		SymbolTable tbl;
		char * str;
		const char* keys = "+-*/%=<>!;";
	};
}