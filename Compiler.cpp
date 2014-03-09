#include "Compiler.h"

#define INTERCEPT_OPERAND(a) \
	if (a != "(")\
	{\
		if (IsName(a))\
		{\
			if (!ast.GetTable().Exist(a))\
				return false;\
		}\
		else if (!IsNumber(a))\
			return false;\
	}\
	else if (a == "(")\
	{\
		auto tempOp = op;\
		op = nullptr;\
		CompileExpression(asmc, expr, lastVar);\
		op = tempOp;\
	}\

namespace SmallTranslator
{
	Compiler::Compiler(char* code) :
		code(code)
	{
		result = std::string();
		ast = AstBuilder(code);
	}

	Compiler::~Compiler()
	{
		Operators::Inst()->Release();
	}

	std::string Compiler::Compile()
	{
		auto a = ast.BuildAST();
		auto code = CompileBlock(a);

		std::string body;
		SymbolTable &tbl = ast.GetTable();
		if (tbl.vars.size() > 0)
		{
			std::string marker = GenerateMarker();
			body += "jmp\t" + marker + "\n";
			for (auto&i : tbl.vars)
			{
				body += "alloc\t" + i + "\t0\n";
			}
			body += marker + ":\n";
		}

		body += code;
		delete a;
		return body;
	}

	std::string Compiler::CompileBlock(Block* block)
	{
		std::string allocs;
		std::string body;

		for (auto &i : block->units)
		{
			switch (i->GetType())
			{
			case UnitType::Expression:
			{
				std::string exprCompiled;
				CompileExpression(exprCompiled, (Expression*)i);
				body += exprCompiled;
				break;
			}
			case UnitType::While:
				body += NL + CompileWhile((While*)i);
				break;
			case UnitType::If:
				body += NL + CompileIf((If*)i);
				break;
			case UnitType::In:
				body += NL + CompileIn((In*)i);
				break;
			case UnitType::Out:
				body += NL + CompileOut((Out*)i);
				break;
			default:
				break;
			}
		}

		return body;
	}

	std::string Compiler::CompileWhile(While* wh)
	{
		std::string whileitself;
		std::string lastVar;
		std::string cond;
		std::string markerStart = GenerateMarker();
		std::string markerEnd = GenerateMarker();
		CompileExpression(cond, wh->condition, lastVar);
		//finally we got everything we want and we are ready to rembo.
		whileitself += markerStart + ":\n";
		whileitself += cond;
		whileitself += "cmp\t" + lastVar + "\t1\n";
		FreeVariableIfTemp(lastVar);//we can use it in body
		whileitself += "jnz\t" + markerEnd + "\n";
		whileitself += CompileBlock(wh->block);;
		whileitself += "jmp\t" + markerStart + "\n";
		whileitself += markerEnd + ":\n";

		return whileitself;
	}

	std::string Compiler::CompileIf(If* co)
	{
		std::string ifitself;
		std::string lastVar;
		std::string cond;
		std::string markerEnd = GenerateMarker();
		CompileExpression(cond, co->condition, lastVar);
		ifitself += cond;
		ifitself += "cmp\t" + lastVar + "\t1\n";
		FreeVariableIfTemp(lastVar);//it is not required anymore. we can use it in body.
		ifitself += "jnz\t" + markerEnd + "\n";
		ifitself += CompileBlock(co->block);
		if (co->elseNode != nullptr)
		{
			std::string markerElseEnd = GenerateMarker();
			ifitself += "jmp\t" + markerElseEnd + "\n";
			ifitself += markerEnd + ":\n";
			ifitself += CompileBlock(co->elseNode);
			ifitself += markerElseEnd + ":\n";
		}
		else
		{
			ifitself += markerEnd + ":\n";
		}

		return ifitself;
	}

	std::string Compiler::CompileIn(In* incommand)
	{
		std::string result;
		if (!incommand->expressions.empty())
		{
			for (auto&i : incommand->expressions)
			{
				std::string lastVar;
				std::string commandcode;
				if (CompileExpression(commandcode, i, lastVar))
				{
					result += commandcode;
					result += "in\t" + lastVar + "\n";
				}
			}
		}

		return result;
	}

	std::string Compiler::CompileOut(Out* outcommand)
	{
		std::string result;
		if (!outcommand->expressions.empty())
		{
			for (auto&i : outcommand->expressions)
			{
				std::string lastVar;
				std::string commandcode;
				if (CompileExpression(commandcode, i, lastVar))
				{
					result += commandcode;
					result += "in\t" + lastVar + "\n";
				}
			}
		}

		return result;
	}

	bool Compiler::CompileExpression(std::string& asmc, Expression* expr, std::string& lastVar) //lastVar is required for conditions.
	{
		std::string token;
		std::string tempVar;
		std::string a;
		static std::string b;
		static Operator* op = nullptr, *prevOp = nullptr, *nextOp = nullptr;

		if (!op)
		{
			a = GetNextExprToken(expr);
			INTERCEPT_OPERAND(a);
			op = Operators::Inst()->Get(GetNextExprToken(expr));

			if(a != "(")
				lastVar = a;
			else
				a = lastVar;
		}
		else
		{
			a = b;
		}

		b = GetNextExprToken(expr);
		INTERCEPT_OPERAND(b);

		token = GetNextExprToken(expr);
		nextOp = Operators::Inst()->Get(token);

		if (nextOp && token != ")")
		{
			if (op->priority < nextOp->priority)
			{
				prevOp = op;
				auto tempOp = op;
				op = nextOp;
				CompileExpression(asmc, expr, lastVar);
				op = tempOp;
			}
		}

		if (op)// need to check op because there are possibility that op can be nullptr after parsing brackets
		{
			switch (op->type)
			{
			case OperatorType::Arithmetic:
			{
				tempVar = GenerateNewVar();
				ast.GetTable().Add(tempVar);//same here

				asmc += op->func(tempVar, a, b);
				FreeVariableIfTemp(b);//if b is temp and we assigning left value to temp, then b wont be used anymore and we can redefine it.
				FreeVariableIfTemp(a);
				lastVar = tempVar;
				break;
			}
			case OperatorType::Assign:
			{
				asmc += op->func(a, b, std::string());
				FreeVariableIfTemp(b);//if b is temp and we assigning left value to temp, then b wont be used anymore and we can redefine it.
				lastVar = a;
				break;
			}
			case OperatorType::Logic:
			{
				tempVar = GenerateNewVar();
				ast.GetTable().Add(tempVar);//Do not need to check whenever temp value is already exists because it checks it itself

				asmc += op->func(tempVar, a, b);
				FreeVariableIfTemp(b);//if b is temp and we assigning left value to temp, then b wont be used anymore and we can redefine it.
				FreeVariableIfTemp(a);
				lastVar = tempVar;
				break;
			}
			}

			b = tempVar;
		}

		if (nextOp && token != ")")
		{
			if (!prevOp || prevOp && prevOp->priority < nextOp->priority)
			{
				op = nextOp;
				CompileExpression(asmc, expr, lastVar);
			}
		}

		prevOp = nullptr;
		op = nullptr;
		return true;
	}

	std::string Compiler::GetNextExprToken(Expression* expr)
	{
		static Expression* ex = nullptr;
		static size_t it = 0;

		if (ex != expr && expr != nullptr)
		{
			ex = expr;
			it = 0;
		}

		if (it < ex->expression.size())
			return ex->expression.at(it++);

		return "";
	}

	void Compiler::FreeVariableIfTemp(std::string& var)
	{
		int id;
		if (var.substr(0, 4) == "_tmp")
		{
			id = atoi(var.substr(4).c_str());

			if ((int)usingTempVars.size() > id && id >= 0)
			{
				usingTempVars.at(id) = false;
			}
		}
	}

	std::string Compiler::GenerateNewVar()
	{
		std::stringstream strm;
		int id = -1;
		for (size_t i = 0; i < usingTempVars.size(); ++i)
		{
			if (!usingTempVars.at(i))
			{
				id = i;
				usingTempVars.at(id) = true;
				break;
			}
		}

		if (id == -1)
		{
			id = locVarCount++;
			usingTempVars.push_back(true);
		}

		strm << "_tmp" << id;
		return strm.str();
	}

	std::string Compiler::GenerateMarker()
	{
		std::stringstream strm;
		strm << "_mrk" << markerCount++;
		return strm.str();
	}
}