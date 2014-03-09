#pragma once

#include <string>
#include <unordered_map>
#include <functional>

namespace SmallTranslator
{
	namespace Priority
	{
		enum
		{
			Assign = 0,

			PlusBin = 4,
			MinusBin = 4,
			MulBin = 5,
			DivBin = 5,
			ModBin = 5,
			
			ArithmAnd = 2,
			ArithmOr = 1,

			LogicGreather = 3,
			LogicLess = 3,
			LogicGreatherEqual = 3,
			LogicLessEqual =3,
			LogicEqual = 3,
			LogicNotEqual = 3,
			
			Bracket = 6
		};
	}

	namespace OperatorType
	{
		enum
		{
			Assign,
			Logic,
			Arithmetic
		};
	}

	struct Operator
	{
		int priority;
		int type;
		std::function<std::string(std::string&, std::string&, std::string&)> func;
	};

	class Operators
	{
	public:
		static Operators* Inst();
		static void Release();

		Operator* Get(std::string& op);
		~Operators();
	private:
		Operators();

		static std::string ArithmeticalFunction(const char* oper, std::string& a, std::string& b, std::string& c);
		static std::string ComparingFunction(const char* reg, std::string& a, std::string& b, std::string& c);
		static std::string AssignFunction(std::string& a, std::string& b, std::string& _no);

	private:
		const char* RegisterEqual = "req";
		const char* RegisterNotEqual = "rneq";
		const char* RegisterGreather = "reg";
		const char* RegisterGrEqual = "regeq";
		const char* RegisterLess = "rel";
		const char* RegisterLessEqual = "releq";
		static Operators* self;
		std::unordered_map<std::string, Operator> dict; //using unordored_map instead of map because of better perfomance
	};
}