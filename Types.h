#pragma once
#include <string>
#include <vector>

#include "SymbolTable.h"

namespace SmallTranslator
{
	enum class UnitType
	{
		Unknown,
		Declaration,
		Expression,
		Block,
		While,
		If,
		In,
		Out
	};

	bool IsKeyWord(std::string& token);
	bool IsName(std::string& str);
	bool IsNumber(std::string& str);

	class BasicUnit
	{
	public:
		BasicUnit(){}
		virtual ~BasicUnit(){}
		virtual UnitType GetType()
		{
			return UnitType::Unknown;
		}
	};

	class Expression : public BasicUnit
	{
	public:
		Expression(){}
		virtual ~Expression(){}
		virtual UnitType GetType()
		{
			return UnitType::Expression;
		}

	public:
		std::vector<std::string> expression;
	};

	class Block : public BasicUnit
	{
	public:
		Block(){}
		virtual ~Block()
		{
			for (auto&i : units)
			{
				delete i;
			}
		}

		virtual UnitType GetType()
		{
			return UnitType::Block;
		}

	public:
		std::vector<BasicUnit*> units;
	};


	class While : public BasicUnit
	{
	public:
		While(){}
		virtual ~While()
		{
			delete block;
			delete condition;
		}

		virtual UnitType GetType()
		{
			return UnitType::While;
		}

	public:
		Block* block;
		Expression* condition;
	};

	class If : public BasicUnit
	{
	public:
		If()
		{
			elseNode = nullptr;
		}

		virtual ~If()
		{
			delete block;
			delete condition;
			if(elseNode)
				delete elseNode;
		}

		virtual UnitType GetType()
		{
			return UnitType::If;
		}

	public:
		Block* block;
		Block* elseNode;
		Expression* condition;
	};

	class In : public BasicUnit
	{
	public:
		In(){}
		virtual ~In(){}
		virtual UnitType GetType()
		{
			return UnitType::In;
		}

	public:
		std::vector<Expression*> expressions;
	};

	class Out : public BasicUnit
	{
	public:
		Out(){}
		virtual ~Out(){}
		virtual UnitType GetType()
		{
			return UnitType::Out;
		}
	public:
		std::vector<Expression*> expressions;
	};
}