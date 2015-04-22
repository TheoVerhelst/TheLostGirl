#include <algorithm>
#include <stack>

#include <TheLostGirl/scripts/Interpreter.h>
#include <TheLostGirl/scripts/scriptsFunctions.h>
#include <TheLostGirl/scripts/ScriptError.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/functions.h>

Interpreter::Interpreter():
	m_file(),
	m_precedence{{"or", 1},
				{"and", 2},
				{"is", 3},
				{"==", 3},
				{"!=", 3},
				{"<", 4},
				{"<=", 4},
				{">", 4},
				{">=", 4},
				{"+", 5},
				{"-", 5},
				{"*", 6},
				{"/", 6},
				{"%", 6},
				{"!", 1}},
	m_functions{{"print", {-1, print}},
				{"<", {2, lowerThanOp}},
				{">", {2, greaterThanOp}},
				{"<=", {2, lowerEqualOp}},
				{">=", {2, greaterEqualOp}},
				{"==", {2, equalOp}},
				{"is", {2, equalOp}},
				{"!=", {2, notEqualOp}},
				{"and", {2, andOp}},
				{"or", {2, orOp}},
				{"+", {2, addOp}},
				{"-", {2, substractOp}},
				{"*", {2, multiplyOp}},
				{"/", {2, divideOp}},
				{"%", {2, moduloOp}},
				{"!", {1, notOp}},
				{"nearest foe", {1, nearestFoe}},
				{"distance from", {2, distanceFrom}},
				{"direction to", {2, directionTo}},
				{"direction of", {1, directionOf}},
				{"attack", {2, attack}},
				{"can move", {1, canMove}},
				{"move", {2, move}},
				{"stop", {1, stop}},
				{"can jump", {1, canJump}},
				{"jump", {1, jump}}},
	m_initialVars{{"left", static_cast<int>(Direction::Left)},
				{"right", static_cast<int>(Direction::Right)},
				{"top", static_cast<int>(Direction::Top)},
				{"bottom",static_cast<int>(Direction::Bottom)}},
	m_vars{},
	m_context(nullptr),
	m_operators("!:,=<>+-*/%()"),
	m_multichar0perators{{"<", "="}, {">", "="}, {"=", "="}, {"!", "="}},
	m_reservedNames{"else if", "if", "else", "endif", "and", "or", "not", "event", "is"}
{
}

Interpreter::~Interpreter()
{
	if(m_file.is_open())
		m_file.close();
}

bool Interpreter::loadFromFile(const std::string& fileName)
{
	m_file.open(fileName);
	return m_file.good();
}

void Interpreter::interpret(entityx::Entity entity, StateStack::Context context)
{
	if(m_file.is_open())
	{
		try
		{
			m_file.clear();
			m_file.seekg(0);
			m_context = &context;
			m_vars["self"] = entity;
			VecStr lines;
			for(std::string line; std::getline(m_file, line);)
				lines.push_back(line);
			interpretBlock(lines.begin(), lines.end(), lines.begin());
			m_context = nullptr;
			//Reset all variables
			m_vars = m_initialVars;
		}
		catch(const std::runtime_error& e)
		{
			std::cerr << "Error while interpreting script : " << e.what() << std::endl;
		}
	}
	else
		throw std::runtime_error("Interpreter::interpret() method called with an invalid script file.");
}

void Interpreter::interpretBlock(VecStr::iterator from, VecStr::iterator to, VecStr::iterator begin)
{
	for(;from != to; from++)
	{
		try
		{
			VecStr tokens = tokenize(*from);
			/*std::cout << "Tokens:";
			for(auto& tok:tokens)
				std::cout << "[" << tok << "]";
			std::cout << std::endl;*/
			if(tokens.size() > 0)
			{
				if(tokens.front() == "if" or tokens.front() == "else if")
				{
					Data conditionData = evaluateTree(convert(VecStr(std::next(tokens.begin()), tokens.end())));
					cast<bool>(conditionData);
					bool condition{boost::get<bool>(conditionData)}, skip{false};
					VecStr::iterator beginIf(std::next(from));
					short int depth{0};
					for(VecStr::iterator endIf(beginIf); endIf != to; endIf++)
					{
						VecStr ifTokens = tokenize(*endIf);
						if(ifTokens.front() == "if")
							depth++;
						if(depth == 0 and not skip and
								(ifTokens.front() == "else if" or ifTokens.front() == "else" or ifTokens.front() == "endif"))
						{
							if(condition)
							{
								interpretBlock(beginIf, endIf, begin);
								skip = true;
							}
							else
							{
								from = std::prev(endIf);
								break;
							}
						}
						if(depth == 0 and ifTokens.front() == "endif")
						{
							from = endIf;
							break;
						}
						if(ifTokens.front() == "endif")
							depth--;
					}
				}
				else if(tokens.front() != "else" and tokens.front() != "endif")
					evaluateTree(convert(tokens));
			}
		}
		catch(const ScriptError& e)
		{
			throw std::runtime_error("unable to evaluate line " + std::to_string(std::distance(begin, from)+1) + " : "
					+ std::string(e.what()));
		}
	}
}

VecStr Interpreter::tokenize(const std::string& line) const
{
	//This function use boost and handmade algorithms in place of std::regex because
	//std:regex is very, very slow and totally kill the FPS.
	VecStr first, res;
	//First tokenization, match string literal
	//This step must be done before everything else because words in string must don't be matched.
	bool inString{false};
	auto beginOfToken(line.cbegin());
	for(auto it(line.cbegin()); it != line.cend(); ++it)
	{
		if(*it == '\"' and not (it != line.cbegin() and *(it-1) != '\\'))
		{
			if(inString)
				it++;
			first.emplace_back(beginOfToken, it);
			beginOfToken = it;
			if(inString)
				it--;
			inString = not inString;
		}
	}
	first.emplace_back(beginOfToken, line.cend());

	//Second tokenization, match reserved names.
	for(const auto& reservedName : m_reservedNames)
	{
		const size_t reservedNameSize{reservedName.size()};
		//For each submatch
		for(auto it(first.begin()); it != first.end(); ++it)
		{
			const std::string submatch(*it);
			//Skip string literals
			if(submatch.size() > 0 and submatch[0] != '\"')
			{
				it = first.erase(it);
				//Search the current reserved name in the submatch
				auto found = std::search(submatch.cbegin(), submatch.cend(), reservedName.begin(), reservedName.end());
				//The match is valid only if the preceding char is not a letter and the following one is a space.
				if(found != submatch.cend()
					and not (found != submatch.cbegin() and std::isalpha(*(found-1)))
					and (found+reservedNameSize == submatch.cend() or std::isspace(*(found+reservedNameSize))))
				{
					it = first.emplace(it, found+reservedNameSize, submatch.cend());
					it = first.emplace(it, submatch.cbegin(), found);
					it = first.emplace(it, found, found+reservedNameSize);
					++it;
				}
				else
					it = first.insert(it, submatch);
			}
		}
	}
	//Third tokenization, separates everything according to the operators.
	for(const auto& submatch:first)
	{
		auto beginOfToken(submatch.cbegin());
		for(auto it(submatch.cbegin()); it != submatch.cend(); ++it)
		{
			if(*it == '!' or *it == ':' or *it == ',' or *it == '=' or *it == '<' or *it == '>' or *it == '+'
					or *it == '-' or *it == '*' or *it == '/' or *it == '%' or *it == '(' or *it == ')')
			{
				const std::string strippedToken{strip(std::string(beginOfToken, it))};
				if(strippedToken != "")
					res.emplace_back(strippedToken);
				res.emplace_back(it, std::next(it));
				beginOfToken = std::next(it);
			}
		}
		const std::string strippedToken{strip(std::string(beginOfToken, submatch.cend()))};
		if(strippedToken != "")
			res.emplace_back(strippedToken);
	}
	//Join operators that have been separated like <= or !=
	for(const auto& multicharOp : m_multichar0perators)
	{
		std::list<std::string>::iterator it = std::search(res.begin(), res.end(), multicharOp.cbegin(), multicharOp.cend());
		while(it != res.end())
		{
			it = res.erase(it, std::next(it, 2));
			res.emplace(it, multicharOp[0] + multicharOp[1]);
			it = std::search(res.begin(), res.end(), multicharOp.cbegin(), multicharOp.cend());
		}
	}
	return res;
}

Tree<Data>::Ptr Interpreter::convert(const VecStr& tokens)
{
	Tree<Data>::Ptr res;
	std::stack<std::string> operatorsStack;
	std::stack<Tree<Data>::Ptr> operands;
	for(auto it(tokens.begin()); it != tokens.end(); ++it)
	{
		if(std::next(it) != tokens.end() and (*(std::next(it)) == "="
				or (m_precedence.find(*it) != m_precedence.end() and *(std::next(it)) == "(")))
			operands.push(nullptr);
		else if(m_precedence.find(*it) != m_precedence.end())
		{
			while(not operatorsStack.empty() and
					m_precedence.at(operatorsStack.top())
					>= m_precedence.at(*it))
			{
				Tree<Data>::Ptr left, right;
				left = res;
				right = operands.top();
				operands.pop();
				if(not res)
				{
					left = operands.top();
					operands.pop();
				}
				res = Tree<Data>::create(operatorsStack.top(),{left, right});
				operatorsStack.pop();
				res->resolveChildren(res);
			}
			operatorsStack.push(*it);
		}
		//Value token
		else if(isValue(*it) and *it != "is")
			operands.push(Tree<Data>::create(evaluateToken(*it)));
		//Parenthesis token
		else if(*it == "(")
		{
			//Parenthesis for function call
			if(it != tokens.begin() and m_functions.find(*std::prev(it)) != m_functions.end())
			{
				Tree<Data>::Ptr functionTree = Tree<Data>::create(*std::prev(it));
				//The function has been pushed on the stack with the value 0, so pop it.
				operands.pop();
				int depth{1};
				for(auto it2(std::next(it)), lastComma(it2); it2 != tokens.end() and depth > 0; ++it2)
				{
					if(*it2 == "(")
						depth++;
					else if(*it2 == ")")
						depth--;
					//Add one argument in the arguments list.
					if(*it2 == "," or depth == 0)
					{
						VecStr subExpression(lastComma, it2);
						functionTree->pushChild(convert(subExpression));
						lastComma = std::next(it2);
						if(depth == 0)
							it = it2;
					}
					functionTree->resolveChildren(functionTree);
				}
				operands.push(functionTree);
			}
			//Arithmetic parenthesis
			else
			{
				VecStr subExpression(std::next(it), tokens.end());
				const size_t offset(parenthesis(subExpression));
				subExpression.assign(subExpression.begin(), std::next(subExpression.begin(), offset));
				operands.push(convert(subExpression));
				std::advance(it, offset+1);
			}
		}
		else if(*it == "=")
		{
			if(it == tokens.begin() or not isIdentifier(*std::prev(it)))
				throw ScriptError("invalid identifier in assignation.");
			operands.pop();
			Tree<Data>::Ptr value = convert(VecStr(std::next(it), tokens.end()));
			m_vars[*std::prev(it)] = evaluateTree(value);
			//Assignement return assigned value, allowing chained assigements.
			return value;
		}
	}
	while(not operatorsStack.empty())
	{
		Tree<Data>::Ptr left, right;
		left = res;
		if(operands.empty())
			throw ScriptError("too few operands for operator " + operatorsStack.top());
		right = operands.top();
		operands.pop();
		if(not res)
		{
			if(operands.empty())
				throw ScriptError("too few operands for operator " + operatorsStack.top());
			left = operands.top();
			operands.pop();
		}
		res = Tree<Data>::create(operatorsStack.top(), {left, right});
		operatorsStack.pop();
		res->resolveChildren(res);
	}
	//If the line was only one value token.
	if(not res)
	{
		res = operands.top();
		operands.pop();
	}
	if(not operands.empty())
	{
		std::string errorMessage("following operands need an operator:");
		while(not operands.empty())
		{
			Data operand{operands.top()->getValue()};
			operands.pop();
			cast<std::string>(operand);
			errorMessage += " " + boost::get<std::string>(operand);
		}
		throw ScriptError(errorMessage);
	}
	return res;
}

Data Interpreter::evaluateToken(const std::string& token) const
{
	//Number literal
	if(isNumber(token))
	{
		if(token.find(".") != std::string::npos)
			return stof(token);
		else
			return stoi(token);
	}
	//Boolean literal
	else if(isBool(token))
		return token == "true";
	//String literal
	else if(isString(token))
	{
		std::string res(token.begin()+1, token.end()-1);
		//Parse string and replace escaped characters
		for(size_t i{0}; i < res.size(); ++i)
		{
			if(res[i] == '\\')
			{
				const char escaped{res[i+1]};
				res.erase(i, 2);
				switch(escaped)
				{
					case 'a':
						res.insert(i, 1, '\a');
						break;
					case 'b':
						res.insert(i, 1, '\b');
						break;
					case 'f':
						res.insert(i, 1, '\f');
						break;
					case 'n':
						res.insert(i, 1, '\n');
						break;
					case 'r':
						res.insert(i, 1, '\r');
						break;
					case 't':
						res.insert(i, 1, '\t');
						break;
					case 'v':
						res.insert(i, 1, '\v');
						break;
					default:
						res.insert(i, 1, escaped);
						break;
				}
			}
		}
		return res;
	}
	//Variable
	else if(isIdentifier(token) and m_vars.find(token) != m_vars.end())
		return m_vars.at(token);
	//Function name or variable identifier in assignement
	else if(isIdentifier(token) and m_functions.find(token) != m_functions.end())
		return 0;
	else
		throw ScriptError("unrecognized token: " + token);
}

Data Interpreter::evaluateTree(const Tree<Data>::Ptr expression) const
{
	if(not expression->noChildren())
	{
		/*
		std::cout << "Evaluate " << expression->getValue()
		<< " with children:" << std::endl;
		for(size_t i{0}; i < expression->childrenNumber(); ++i)
			std::cout << "\t(" << i << ") "
			<< expression->getChild(i)->getValue() << std::endl;
		*/
		auto fn_it = m_functions.find(boost::get<std::string>(expression->getValue()));
		if(fn_it != m_functions.end())
		{
			auto fn = fn_it->second;
			if(fn.numberOperands > -1 and
					(short int)expression->childrenNumber() != fn.numberOperands)
			{
				throw ScriptError("wrong number of operands "
						"(got "+std::to_string(expression->childrenNumber())+
						", expected "+std::to_string(fn.numberOperands)+") :"+
						boost::get<std::string>(expression->getValue()));
			}
			std::vector<Data> args;
			for(size_t i{0}; i < expression->childrenNumber(); ++i)
				args.push_back(evaluateTree(expression->getChild(i)));
			return fn.pointer(args, *m_context);
		}
		else
		{
			throw ScriptError("unable to parse the following token: " +
					boost::get<std::string>(expression->getValue()));
		}
	}
	return expression->getValue();
}

size_t Interpreter::parenthesis(const VecStr& tokens) const
{
	int depth{1}, i{0};
	for(auto& token : tokens)
	{
		if(token == "(")
			depth++;
		else if(token == ")")
			depth--;
		if(depth == 0)
			break;
		i++;
	}
	return i;
}

inline bool Interpreter::isSpace(const std::string& str) const
{
	return strip(str) == "";
}

inline bool Interpreter::isNumber(const std::string& str) const
{
	bool foundDot{false};
	return std::all_of(str.begin(), str.end(), [&foundDot](const char& ch)
						{
							bool res = std::isdigit(ch) or (ch == '.' and not foundDot);
							if(ch == '.')
								foundDot = true;
							return res;
						});
}

bool Interpreter::isIdentifier(const std::string& str) const
{
	return std::all_of(str.begin(), str.end(), [](const char& ch){return std::isalnum(ch) or std::isspace(ch);});
}

inline bool Interpreter::isBool(const std::string& str) const
{
	return str == "true" or str == "false";
}

inline bool Interpreter::isString(const std::string& str) const
{
	return str.size() >= 2 and *(str.begin()) == '\"' and *(str.end()-1) == '\"';
}

inline bool Interpreter::isValue(const std::string& str) const
{
	return isBool(str) or isNumber(str) or isIdentifier(str) or isString(str);
}

std::string Interpreter::strip(const std::string& str) const
{
	std::string res{str};
	while(res.size() > 0 and std::isspace(res[0]))
		res.erase(res.begin());
	while(res.size() > 0 and std::isspace(res[res.size()-1]))
		res.erase(res.end()-1);
	return res;
}

template <>
void cast<bool>(Data& var)
{
	if(var.which() == 4)//Convert entity to bool
		var = boost::get<entityx::Entity>(var).valid();
	else if(var.which() == 3)//Convert string to bool
		var = boost::get<std::string>(var).size() > 0;
	else if(var.which() == 1)//Convert int to bool
		var = static_cast<bool>(boost::get<int>(var));
	else if(var.which() == 2)//Convert float to bool
		var = static_cast<bool>(boost::get<float>(var));
}

template <>
void cast<int>(Data& var)
{
	if(var.which() == 4)//Convert entity to int
		throw ScriptError("Conversion from entity to integer is not allowed.");
	else if(var.which() == 3)//Convert string to int
		var = std::stoi(boost::get<std::string>(var));
	else if(var.which() == 0)//Convert bool to int
		var = static_cast<int>(boost::get<bool>(var));
	else if(var.which() == 2)//Convert float to int
		var = static_cast<int>(boost::get<float>(var));
}

template <>
void cast<float>(Data& var)
{
	if(var.which() == 4)//Convert entity to float
		throw ScriptError("Conversion from entity to floating point is not allowed.");
	else if(var.which() == 3)//Convert string to float
		var = std::stof(boost::get<std::string>(var));
	else if(var.which() == 0)//Convert bool to float
		var = static_cast<float>(boost::get<bool>(var));
	else if(var.which() == 1)//Convert int to float
		var = static_cast<float>(boost::get<int>(var));
}

template <>
void cast<std::string>(Data& var)
{
	if(var.which() == 1)//Convert int to string
		var = std::to_string(boost::get<int>(var));
	else if(var.which() == 0)//Convert bool to string
	{
		if(boost::get<bool>(var))
			var = "true";
		else
			var = "false";
	}
	else if(var.which() == 2)//Convert float to string
		var = std::to_string(boost::get<float>(var));
	else if(var.which() == 4)//Convert entity to string
		throw ScriptError("Conversion from entity to string is not allowed.");
}

template <>
void cast<entityx::Entity>(Data& var)
{
	if(var.which() != 4)
		throw ScriptError("Conversion from any type to entity is not allowed.");
}

