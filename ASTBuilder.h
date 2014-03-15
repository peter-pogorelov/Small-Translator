#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>

#define BITS_WHILE	1 << 0
#define BITS_EXPR	1 << 1
#define BITS_IF		1 << 2
#define BITS_IO		1 << 3

#include "Types.h"

#define IF_STMT 1 << 0
#define ELSE_NODE 1 << 0

#define LOG_ERROR(error)\
	errorstrm << "Line: " << lineCount << " " << error

namespace SmallTranslator
{
	class AstBuilder
	{
	public:

		AstBuilder();
		AstBuilder(char* code);

		~AstBuilder();

		SymbolTable& GetTable();
		std::string GetLastError();

		Block* BuildAST();
		bool IsFunction(std::string& name);
	private:
		Block* Build(int& params, unsigned int bits = 0);

		bool BuildFunction(); //reads function and moves it to funcs dictionary.
		Return* BuildReturn();

		bool BuildDecl();
		In* BuildIn();
		Out* BuildOut();
		While* BuildWhile();
		If* BuildIf();
		Expression* BuildExpression(unsigned char bits, bool &enumend /*required to intercept ',' in i/o*/, std::string& prevToken = std::string());
		
		std::string GetNextToken();

		std::string GenerateArgument();
		void ResetGenerator();

	public:
		std::unordered_map<std::string, Function*> funcs;

	private:
		Function* ownerFunc;

		int lineCount;
		std::stringstream errorstrm;
		int argCount;
		SymbolTable tbl;
		char * str;
		const char* keys = "+-*/%=<>!;(),[]";
	};
}