#include "Opers.h"

namespace SmallTranslator
{
	Operators* Operators::self = nullptr;

	std::string Operators::ArithmeticalFunction(const char* oper, std::string& a, std::string& b, std::string& c)
	{
		std::string result;
		result += "mov\t" + a + "\t" + b + "\n";
		result += std::string(oper) + "\t" + a + "\t" + c + "\n";
		return result;
	}
	std::string Operators::ComparingFunction(const char* reg, std::string& a, std::string& b, std::string& c)
	{
		std::string result;
		result += "mov\t" + a + "\t" + b + "\n";
		result += "cmp\t" + a + "\t" + c + "\n";
		result += "mov\t" + a + "\t" + std::string(reg) + "\n";
		return result;
	}
	std::string Operators::AssignFunction(std::string& a, std::string& b, std::string& _no)
	{
		std::string result;
		result += "mov\t" + a + "\t" + b + "\n";
		return result;
	}

	Operators::Operators()
	{
		using namespace std::placeholders;
		
		dict["+"] = { Priority::PlusBin, OperatorType::Arithmetic };
		dict["+"].func = std::bind(ArithmeticalFunction, "add", _1, _2, _3);
		dict["-"] = { Priority::MinusBin, OperatorType::Arithmetic };
		dict["-"].func = std::bind(ArithmeticalFunction, "sub", _1, _2, _3);
		dict["*"] = { Priority::MulBin, OperatorType::Arithmetic };
		dict["*"].func = std::bind(ArithmeticalFunction, "mul", _1, _2, _3);
		dict["/"] = { Priority::DivBin, OperatorType::Arithmetic };
		dict["/"].func = std::bind(ArithmeticalFunction, "div", _1, _2, _3);
		dict["%"] = { Priority::ModBin, OperatorType::Arithmetic };
		dict["%"].func = std::bind(ArithmeticalFunction, "mod", _1, _2, _3);

		dict["="] = { Priority::Assign, OperatorType::Assign };
		dict["="].func = AssignFunction;

		dict["<"] = { Priority::LogicLess, OperatorType::Logic };
		dict["<"].func = std::bind(ComparingFunction, RegisterLess, _1, _2, _3);
		dict[">"] = { Priority::LogicGreather, OperatorType::Logic };
		dict[">"].func = std::bind(ComparingFunction, RegisterGreather, _1, _2, _3);
		dict["<="] = { Priority::LogicLessEqual, OperatorType::Logic };
		dict["<="].func = std::bind(ComparingFunction, RegisterLessEqual, _1, _2, _3);
		dict[">="] = { Priority::LogicGreatherEqual, OperatorType::Logic };
		dict[">="].func = std::bind(ComparingFunction, RegisterGrEqual, _1, _2, _3);
		dict["=="] = { Priority::LogicEqual, OperatorType::Logic };
		dict["=="].func = std::bind(ComparingFunction, RegisterEqual, _1, _2, _3);
		dict["!="] = { Priority::LogicNotEqual, OperatorType::Logic };
		dict["!="].func = std::bind(ComparingFunction, RegisterNotEqual, _1, _2, _3);

		dict["and"] = { Priority::ArithmAnd, OperatorType::Arithmetic };
		dict["and"].func = std::bind(ArithmeticalFunction, "and", _1, _2, _3);
		dict["or"] = { Priority::ArithmOr, OperatorType::Arithmetic };
		dict["or"].func = std::bind(ArithmeticalFunction, "or", _1, _2, _3);

		dict["("] = { Priority::Bracket, OperatorType::Arithmetic };
		dict[")"] = { Priority::Bracket, OperatorType::Arithmetic };
	}

	Operators* Operators::Inst()
	{
		if (self == nullptr)
			self = new Operators();

		return self;
	}

	void Operators::Release()
	{
		if (self)
		{
			delete self;
			self = nullptr;
		}
	}

	Operator* Operators::Get(std::string& op)
	{
		auto result = dict.find(op);

		if (result != dict.cend())
		{
			return &result->second;
		}

		return nullptr;
	}

	Operators::~Operators()
	{
	}
}