#include <algorithm>
#include <stack>
#include <TheLostGirl/scripts/Interpreter.h>
#include <TheLostGirl/scripts/scriptsFunctions.h>
#include <TheLostGirl/scripts/ScriptError.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/functions.h>

Interpreter::Interpreter():
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
				{"!", 1},
				{"=", 0}},
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
				{"=", {2, nullptr}},
				{"nearest foe", {1, nearestFoe}},
				{"distance from", {2, distanceFrom}},
				{"direction to", {2, directionTo}},
				{"direction of", {1, directionOf}},
				{"attack", {1, attack}},
				{"can move", {1, canMove}},
				{"move", {2, move}},
				{"stop", {1, stop}},
				{"can jump", {1, canJump}},
				{"jump", {1, jump}}},
	m_initialVars{{"left", static_cast<int>(Direction::Left)},
				{"right", static_cast<int>(Direction::Right)},
				{"top", static_cast<int>(Direction::Top)},
				{"bottom",static_cast<int>(Direction::Bottom)}},
	m_vars(m_initialVars),
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
	std::ifstream file(fileName);
	try
	{
		if(not file.is_open())
			throw std::runtime_error("invalid script file.");
		for(std::string line; std::getline(file, line);)
		{
			std::list<std::string> tokens = tokenize(line.begin(), line.end());
			m_expressions.push_back(convertTokens(tokens.cbegin(), tokens.cend()));
		}
	}
	catch(const std::runtime_error& e)
	{
		std::cerr << "Error while interpreting script \"" << fileName << "\": " << e.what() << std::endl;
		file.close();
		return false;
	}
	file.close();
	return true;
}

void Interpreter::interpret(entityx::Entity entity)
{
	m_vars["self"] = entity;
	interpretBlock(entity, m_expressions.begin(), m_expressions.end(), m_expressions.begin());
	//Reset all variables
	m_vars = m_initialVars;
}

inline std::string Interpreter::getStr(std::list<Expression::Ptr>::iterator it) const
{
	return boost::get<std::string>((*it)->getValue().first);
}

void Interpreter::interpretBlock(entityx::Entity entity, std::list<Expression::Ptr>::iterator from, std::list<Expression::Ptr>::iterator to, std::list<Expression::Ptr>::iterator begin)
{
	try
	{
		for(;from != to; ++from)
		{
			if(getStr(from) == "if" or getStr(from) == "else if")
			{
				//An if statement has only one child, the boolean expression to evaluate.
				Data conditionData = evaluateExpression((*from)->getChild(0));
				cast<bool>(conditionData);
				bool condition{boost::get<bool>(conditionData)}, skip{false};
				std::list<Expression::Ptr>::iterator beginIf(std::next(from));
				short int depth{0};
				for(std::list<Expression::Ptr>::iterator endIf(beginIf); endIf != to; ++endIf)
				{
					if(getStr(endIf) == "if")
						depth++;
					if(depth == 0 and not skip and (getStr(endIf) == "else if" or getStr(endIf) == "else" or getStr(endIf) == "endif"))
					{
						if(condition)
						{
							interpretBlock(entity, beginIf, endIf, begin);
							skip = true;
						}
						else
						{
							from = std::prev(endIf);
							break;
						}
					}
					if(getStr(endIf) == "endif")
					{
						if(depth == 0)
						{
							from = endIf;
							break;
						}
						else
							depth--;
					}
				}
			}
			else if(getStr(from) != "else" and getStr(from) != "endif")
				evaluateExpression(*from);
		}
	}
	catch(const ScriptError& e)
	{
		std::cerr << "unable to evaluate line " << std::distance(begin, from)+1 << " : " << e.what() << std::endl;
	}
}

Data Interpreter::evaluateExpression(const Expression::Ptr expression)
{
	if(expression->noChildren())
	{
		if(expression->getValue().second)
		{
			auto it = m_vars.find(boost::get<std::string>(expression->getValue().first));
			if(it == m_vars.end())
				throw ScriptError("unknow variable name: " + it->first);
			return it->second;
		}
		else
			return expression->getValue().first;
	}
	else
	{
		std::string value = boost::get<std::string>(expression->getValue().first);
		if(value == "=")
		{
			const Data rvalue = evaluateExpression(Expression::copy(expression->getChild(1)));
			m_vars[boost::get<std::string>(expression->getChild(0)->getValue().first)] = rvalue;
			return rvalue;
		}
		else
		{
			auto fn_it = m_functions.find(value);
			if(fn_it != m_functions.end())
			{
				auto& fn = fn_it->second;
				if(fn.numberOperands > -1 and
						(short int)expression->childrenNumber() != fn.numberOperands)
				{
					throw ScriptError("wrong number of operands "
							"(got "+std::to_string(expression->childrenNumber())+
							", expected "+std::to_string(fn.numberOperands)+"): "+
							boost::get<std::string>(expression->getValue().first));
				}
				std::vector<Data> args;
				for(size_t i{0}; i < expression->childrenNumber(); ++i)
					args.push_back(evaluateExpression(expression->getChild(i)));
				return fn.pointer(args);
			}
			else
			{
				throw ScriptError("unable to parse the following token: " +
						boost::get<std::string>(expression->getValue().first));
			}
		}
	}
}

std::list<std::string> Interpreter::tokenize(std::string::const_iterator from, std::string::const_iterator to) const
{
	//This function use boost and handmade algorithms in place of std::regex because
	//std:regex is very, very slow and totally kill the FPS.
	std::list<std::string> first, res;
	//First tokenization, match string literal
	//This step must be done before everything else because reserved words in string must not be matched.
	bool inString{false};
	auto beginOfToken(from);
	for(auto it(from); it != to; ++it)
	{
		if(*it == '\"' and not (it != from and *(it-1) != '\\'))
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
	first.emplace_back(beginOfToken, to);

	//Second tokenization, match reserved names.
	for(const auto& reservedName : m_reservedNames)
	{
		const auto reservedNameSize(static_cast<std::string::iterator::difference_type>(reservedName.size()));
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
		beginOfToken = submatch.cbegin();
		for(auto it(submatch.cbegin()); it != submatch.cend(); ++it)
		{
			if(m_operators.find(*it) != std::string::npos)
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
			res.emplace(it, multicharOp.front() + multicharOp.back());
			it = std::search(res.begin(), res.end(), multicharOp.cbegin(), multicharOp.cend());
		}
	}
	return res;
}

Expression::Ptr Interpreter::convertTokens(std::list<std::string>::const_iterator from, std::list<std::string>::const_iterator to) const
{
	Expression::Ptr res;
	if(*from == "if" or *from == "else if")
		res = Expression::create({*from, false}, {convertTokens(std::next(from), to)});
	else if(*from == "else" or *from == "endif")
	{
		if(std::next(from) != to)
		{
			std::string errorMessage{"unexpected tokens after " + *from + ": "};
			from++;
			for(; from != to; ++from)
				errorMessage += *from;
			throw ScriptError(errorMessage);
		}
		res = Expression::create({*from, false});
	}
	else
	{
		//First, convert the tokens to a list of Expression::Ptr, with the postfix notation.
		//So a list of tokens like ["a", "+", "c", "*", "2"]
		//will be converted to [{"a"}, {"c"}, {"2"}, {"*"}, {"+"}].
		std::list<Expression::Ptr> postfixeRes;
		std::stack<std::string> operatorsStack;
		for(auto it(from); it != to; ++it)
		{
			//If *it is a funtion name, nothing has to be pushed on the stack
			if(m_functions.find(*it) != m_functions.end() and std::next(it) != to and *(std::next(it)) == "(")
				;
			//If *it is a literal value token
			else if(isInt(*it))
				postfixeRes.push_back(Expression::create({std::stoi(*it), false}));
			else if(isFloat(*it))
				postfixeRes.push_back(Expression::create({std::stof(*it), false}));
			else if(isString(*it))
				postfixeRes.push_back(Expression::create({evaluateStringToken(*it), false}));
			else if(isBool(*it))
				postfixeRes.push_back(Expression::create({(*it) == "true", false}));
			//If *it is an operator
			else if(m_precedence.find(*it) != m_precedence.end())
			{
				while(not operatorsStack.empty() and m_precedence.at(operatorsStack.top()) >= m_precedence.at(*it))
				{
					postfixeRes.push_back(Expression::create({operatorsStack.top(), false}));
					operatorsStack.pop();
				}
				operatorsStack.push(*it);
			}
			//If *it is a variable name
			else if(isIdentifier(*it))
				postfixeRes.push_back(Expression::create({*it, true}));
			//If *it is a parenthesis token
			else if(*it == "(")
			{
				//Parenthesis for function call
				if(it != from and m_functions.find(*std::prev(it)) != m_functions.end())
				{
					Expression::Ptr functionTree = Expression::create({*std::prev(it), false});
					int depth{1};
					for(auto it2(std::next(it)), lastComma(it2); it2 != to and depth > 0; ++it2)
					{
						if(*it2 == "(")
							depth++;
						else if(*it2 == ")")
							depth--;
						//Add one argument in the arguments list.
						if(*it2 == "," and depth == 1)
						{
							functionTree->pushChild(convertTokens(lastComma, it2));
							lastComma = std::next(it2);
						}
						if(depth == 0)
						{
							functionTree->pushChild(convertTokens(lastComma, it2));
							it = it2;
						}
					}
					functionTree->resolveChildren(functionTree);
					postfixeRes.push_back(functionTree);
				}
				//Arithmetic parenthesis
				else
				{
					auto closingParenthesis(parenthesis(std::next(it), to));
					postfixeRes.push_back(convertTokens(std::next(it), closingParenthesis));
					it = closingParenthesis;//Go to the end of the parentheses expression
				}
			}
			else
				throw ScriptError("unrecognized token: " + *it);
		}
		while(not operatorsStack.empty())
		{
			postfixeRes.push_back(Expression::create({operatorsStack.top(), false}));
			operatorsStack.pop();
		}
		//New effectively convert the postfixe expression to a tree.
		std::stack<Expression::Ptr> operandsStack;
		for(auto token:postfixeRes)
		{
			Data& value = token->getValue().first;
			if(value.which() == 3 and m_precedence.find(boost::get<std::string>(value)) != m_precedence.end())
			{
				Expression::Ptr operand2 = operandsStack.top();
				operandsStack.pop();
				Expression::Ptr operand1 = operandsStack.top();
				operandsStack.pop();
				Expression::Ptr result = Expression::create(token->getValue(), {operand1, operand2});
				operandsStack.push(result);
			}
			else
				operandsStack.push(token);
		}
		res = operandsStack.top();
	}
	return res;
}

Data Interpreter::evaluateStringToken(const std::string& token) const
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

std::list<std::string>::const_iterator Interpreter::parenthesis(std::list<std::string>::const_iterator from, std::list<std::string>::const_iterator to) const
{
	for(int depth{1}; from != to and depth > 0; ++from)
		if(*from == "(")
			depth++;
		else if(*from == ")")
			depth--;
	return from;
}

inline bool Interpreter::isSpace(const std::string& str) const
{
	return strip(str) == "";
}

inline bool Interpreter::isFloat(const std::string& str) const
{
	bool foundDot{false};
	return std::all_of(str.cbegin(), str.cend(), [&foundDot](const char& ch)
						{
							bool res = std::isdigit(ch) or (ch == '.' and not foundDot);
							if(ch == '.')
								foundDot = true;
							return res;
						}) and foundDot;
}

inline bool Interpreter::isInt(const std::string& str) const
{
	return std::all_of(str.cbegin(), str.cend(), [](const char& ch){return std::isdigit(ch);});
}

bool Interpreter::isIdentifier(const std::string& str) const
{
	return str.size() > 0 and not std::isdigit(str[0])
		and std::all_of(str.cbegin(), str.cend(), [](const char& ch){return std::isalnum(ch) or std::isspace(ch);});
}

inline bool Interpreter::isBool(const std::string& str) const
{
	return str == "true" or str == "false";
}

inline bool Interpreter::isString(const std::string& str) const
{
	return str.size() >= 2 and *(str.cbegin()) == '\"' and *(str.cend()-1) == '\"';
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
