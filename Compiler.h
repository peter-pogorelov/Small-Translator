#pragma once

#include <string>
#include <sstream>
#include <vector>

#include "ASTBuilder.h"
#include "Opers.h"

#define NL "\n"

namespace SmallTranslator
{
	class Compiler
	{
	public:
		Compiler(char* code);
		~Compiler();

		std::string Compile();
		std::string CompileBlock(Block* block);
		std::string CompileWhile(While* wh);
		std::string CompileIf(If* co);
		std::string CompileIn(In* incommand);
		std::string CompileOut(Out* outcommand);

		bool CompileExpression(std::string& asmc, Expression* expr, std::string& lastVar = std::string()); //lastVar is required for conditions.

		std::string GetNextExprToken(Expression* expr);

		void FreeVariableIfTemp(std::string& var);
		std::string GenerateNewVar();
		std::string GenerateMarker();

	private:
		AstBuilder ast;
		char* code;
		std::vector<bool> usingTempVars; //contains temp variables that is in use or not
		int locVarCount;
		int markerCount;
		std::string result;
	};
}