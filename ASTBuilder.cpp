#include "ASTBuilder.h"

namespace SmallTranslator
{
	AstBuilder::AstBuilder()
	{
		ownerFunc = nullptr;
		str = nullptr;
		argCount = 0;
		lineCount = 1;
		errorstrm = std::stringstream();
	}

	AstBuilder::AstBuilder(char* code)
	{
		ownerFunc = nullptr;
		str = code;
		argCount = 0;
		lineCount = 1;
		errorstrm = std::stringstream();
	}

	AstBuilder::~AstBuilder()
	{
		for (auto&i : funcs)
		{
			delete i.second;
		}
	}

	bool AstBuilder::IsFunction(std::string& name)
	{
		return funcs.find(name) != funcs.end();
	}

	SymbolTable& AstBuilder::GetTable()
	{
		return tbl;
	}

	Block* AstBuilder::BuildAST()
	{
		int params;
		return Build(params);
	}

	Block* AstBuilder::Build(int &params, unsigned int bits)
	{
		params = 0;
		auto block = new Block();

		auto t = GetNextToken();
		bool bi = bits & IF_STMT;
		BasicUnit* unit = nullptr;
		while (!(t == "end" || bi && t == "else"))
		{
			if (t == "decl")
			{
				BuildDecl();
			}
			else if (t == "while")
			{
				unit = BuildWhile();
			}
			else if (t == "if")
			{
				unit = BuildIf();
			}
			else if (t == "in")
			{
				unit = BuildIn();
			}
			else if (t == "out")
			{
				unit = BuildOut();
			}
			else if (t == "func")
			{
				BuildFunction();
			}
			else if (t == "return")
			{
				unit = BuildReturn();
			}
			else if (t.empty()) // error
			{
				LOG_ERROR("Unexpected block ending.");
			}
			else
			{
				bool notused;
				block->units.push_back(BuildExpression(BITS_EXPR, notused, t));
			}
			
			if (!errorstrm.str().empty())
			{
				delete block;
				return nullptr;
			}

			if (unit)
			{
				block->units.push_back(unit);
			}

			unit = nullptr;
			t = GetNextToken();
		}

		if (t == "else")
		{
			params |= ELSE_NODE;
		}

		return block;
	}

	bool AstBuilder::BuildDecl()
	{
		auto t = GetNextToken();
		while (IsName(t))
		{
			if (!tbl.Exist(t))
				tbl.Add(t);
			t = GetNextToken();
		}

		if (t == ";") //if we match semicolon that's mean that we've reached the end of line
		{
			return true;
		}
		else
		{
			LOG_ERROR("Invalid decl using.");
			return false;
		}
	}

	bool AstBuilder::BuildFunction()
	{
		int notused;
		auto func = new Function();
		auto token = GetNextToken();

		if (token == "void")
		{
			func->noreturn = true;
			token = GetNextToken();
		}
		else
		{
			func->noreturn = false;
		}

		if (!IsName(token))
		{
			LOG_ERROR("Invalid function name.");
			delete func;
			return false;
		}

		func->name = token;

		if (GetNextToken() == "[")
		{
			token = GetNextToken();
			while (token != "]")
			{
				if (IsName(token))
				{
					func->renames[token] = GenerateArgument();
					token = GetNextToken();
				}
				else
				{
					LOG_ERROR("Unexpected stuff in argument list.");
					delete func;
					return false;
				}
			}

			ResetGenerator();
		}
		else
		{
			LOG_ERROR("Invalid signature of function '" << func->name << "'.");
			delete func;
			return false;
		}

		auto temp = ownerFunc;//save old value (there may be recursion in function definitions.
		ownerFunc = func;
		func->block = Build(notused);
		funcs[func->name] = func;
		ownerFunc = temp;//ending of parsing function
		return true;
	}

	Return* AstBuilder::BuildReturn()
	{
		if (ownerFunc && !ownerFunc->noreturn)
		{
			auto ret = new Return();
			bool notused;
			Expression* expr = BuildExpression(BITS_EXPR, notused);
			if (expr->expression.size() != 0)
			{
				ret->expr = expr;
			}
			else
			{
				LOG_ERROR("Empty 'return' command.");
				delete ret;
				return nullptr;
			}

			return ret;
		}
		else
		{
			LOG_ERROR("Unexpected 'return' command.");
		}
	}

	In* AstBuilder::BuildIn()
	{
		bool endofenum;
		auto incommand = new In();
		Expression* next = BuildExpression(BITS_IO, endofenum);

		if (next->expression.size() != 0)
		{
			incommand->expressions.push_back(next);

			while (!endofenum)
			{
				next = BuildExpression(BITS_IO, endofenum);
				incommand->expressions.push_back(next);
			}
		}
		else
		{
			LOG_ERROR("Empty 'out' command.");
			delete incommand;
			return nullptr;
		}

		return incommand;
	}

	Out* AstBuilder::BuildOut()
	{
		bool endofenum;
		auto outcommand = new Out();
		Expression* next = BuildExpression(BITS_IO, endofenum);

		if (next->expression.size() != 0)
		{
			outcommand->expressions.push_back(next);

			while (!endofenum)
			{
				next = BuildExpression(BITS_IO, endofenum);
				outcommand->expressions.push_back(next);
			}
		}
		else
		{
			LOG_ERROR("Empty 'out' command.");
			delete outcommand;
			return nullptr;
		}

		return outcommand;
	}

	While* AstBuilder::BuildWhile()
	{
		auto cycle = new While();
		int params;
		bool notused;

		cycle->condition = BuildExpression(BITS_WHILE, notused);
		cycle->block = Build(params);

		return cycle;
	}

	If* AstBuilder::BuildIf()
	{
		auto cond = new If();
		int params;
		bool notused;

		cond->condition = BuildExpression(BITS_IF, notused);
		cond->block = Build(params, IF_STMT);
		if (params & ELSE_NODE)
		{
			cond->elseNode = Build(params); //do not send IF_STMT because there should not be anything after else node.
		}

		return cond;
	}

	Expression* AstBuilder::BuildExpression(unsigned char bits, bool& enumend, std::string& prevToken)
	{
		auto expr = new Expression();
		std::string t;

		if (!prevToken.empty())
			t = prevToken;
		else
			t = GetNextToken();

		while ((bits & BITS_WHILE && t != "do") || (bits & BITS_EXPR && t != ";") || (bits & BITS_IF && t != "then") || (bits & BITS_IO && t != "," && t != ";"))
		{
			if (IsName(t) && ownerFunc != nullptr) //replace name of function argument used in body to a special generated name
			{
				auto& renames = ownerFunc->renames;
				if(renames.find(t) != renames.end())
					t = renames[t];
			}
			expr->expression.push_back(t);
			t = GetNextToken();
		}

		enumend = bits & BITS_IO && t == ";";

		return expr;
	}

	std::string AstBuilder::GenerateArgument()
	{
		std::stringstream strm;
		strm << "_arg" << argCount++;
		return strm.str();
	}

	void AstBuilder::ResetGenerator()
	{
		argCount = 0;
	}

	std::string AstBuilder::GetLastError()
	{
		return errorstrm.str();
	}

	std::string AstBuilder::GetNextToken()
	{
		std::string res;
		while (isspace(*str))
		{
			if(*str == '\n')
				lineCount++;

			++str;
		}

		if (isdigit(*str))
		{
			while (isdigit(*str))
				res += *str++;

			return res;
		}

		if (isalpha(*str))
		{
			while (isalpha(*str) || isdigit(*str))
				res += *str++;

			return res;
		}

		if (strchr(keys, *str) && *str != '\0')
		{
			if (*str == '=' || *str == '<' || *str == '>' || *str == '!')
			{
				if (*(str + 1) == '=')
				{
					res = *str;
					res += '=';
					str += 2;
					return res;
				}
			}

			return res = *str++;
		}

		return res;
	}
}