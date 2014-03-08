#include "ASTBuilder.h"

namespace SmallTranslator
{
	AstBuilder::AstBuilder() :
		str(nullptr)
	{
	}

	AstBuilder::AstBuilder(char* code) :
		str(code)
	{
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

	Block* AstBuilder::Build(int &params, int bits)
	{
		params = 0;
		auto block = new Block();

		auto t = GetNextToken();
		bool b = bits & IF_STMT;
		while ((t != "end" && !b) || (b && t != "else"))
		{

			if (t == "decl")
			{
				BuildDecl();
			}
			else if (t == "while")
			{
				block->units.push_back(BuildWhile());
			}
			else if (t == "if")
			{
				block->units.push_back(BuildIf());
			}
			else if (t.empty())
			{
				return block;
			}
			else
			{
				block->units.push_back(BuildExpression(BITS_EXPR, t));
			}

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
			return false;
		}
	}

	While* AstBuilder::BuildWhile()
	{
		auto cycle = new While();
		int params;

		cycle->condition = BuildExpression(BITS_WHILE);
		cycle->block = Build(params);

		return cycle;
	}

	If* AstBuilder::BuildIf()
	{
		auto cond = new If();
		int params;

		cond->condition = BuildExpression(BITS_IF);
		cond->block = Build(params, IF_STMT);
		if (params & ELSE_NODE)
		{
			cond->elseNode = Build(params); //do not send IF_STMT because there should not be anything after else node.
		}

		return cond;
	}

	Expression* AstBuilder::BuildExpression(unsigned char bits, std::string& prevToken)
	{
		auto expr = new Expression();
		std::string t;

		if (!prevToken.empty())
			t = prevToken;
		else
			t = GetNextToken();

		while ((bits & BITS_WHILE && t != "do") || (bits & BITS_EXPR && t != ";") || (bits & BITS_IF && t != "then"))
		{
			expr->expression.push_back(t);
			t = GetNextToken();
		}

		return expr;
	}

	std::string AstBuilder::GetNextToken()
	{
		std::string res;
		while (isspace(*str)) ++str;
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