#include <algorithm>
#include <stack>

#include <TheLostGirl/scripts/Interpreter.h>
#include <TheLostGirl/scripts/scriptsFunctions.h>
#include <TheLostGirl/scripts/ScriptError.h>
#include <TheLostGirl/components.h>

Interpreter::Interpreter(std::ifstream& file, entityx::Entity entity, StateStack::Context context):
	m_file(file),
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
				{"attack", {2, attack}},
				{"can move", {1, canMove}},
				{"move", {2, move}},
				{"stop", {2, stop}},
				{"can jump", {1, canJump}},
				{"jump", {1, jump}}},
	m_vars{{"self", entity},//Add variables so that they will be available in the script
		{"left", static_cast<int>(Direction::Left)},
		{"right", static_cast<int>(Direction::Right)},
		{"top", static_cast<int>(Direction::Top)},
		{"bottom",static_cast<int>(Direction::Bottom)}},
	m_context{context},
	identifier("[[:alnum:]][\\w\\s]*\\w|\\w"),
	number_literal("\\d+|\\d*\\.\\d*"),
	boolean_literal("true|false"),
	string_literal("\"([^\"]|\\\\\")*\"|'([^']|\\\\')*'"),
	operators("<=|>=|==|!=|<|>|\\+|-|\\*|/|%|\\(|\\)"),
	other(":|=|!|,"),
	reserved_names("(if|else if|else|endif|event|and|or|not|is)"
			"[\\s\\b]+"),
	identifier_regex(identifier),
	number_literal_regex(number_literal),
	boolean_literal_regex(boolean_literal),
	string_literal_regex("("+string_literal+")"),
	reserved_names_regex(reserved_names),
	token_regex("("+number_literal+"|"+string_literal+"|"+boolean_literal+"|"+identifier+"|"+operators+"|"+other+")\\s*"),
	value_regex(identifier+"|"+string_literal+"|"+number_literal+"|"+boolean_literal),
	space_regex("[[:space:]]*")
{
}

void Interpreter::interpret()
{
	try
	{
		VecStr lines;
		for(std::string line; std::getline(m_file, line);)
			lines.push_back(line);
		interpretBlock(lines.begin(), lines.end(), lines.begin());
	}
	catch(const std::runtime_error& e)
	{
		std::cerr << "Error while interpreting script : " << e.what() << std::endl;
	}
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
				if(tokens[0] == "if" or tokens[0] == "else if")
				{
					Data conditionData = evaluateTree(convert(VecStr(tokens.begin()+1, tokens.end())));
					cast<bool>(conditionData);
					bool condition{boost::get<bool>(conditionData)}, skip{false};
					VecStr::iterator beginIf(from+1);
					short int depth{0};
					for(VecStr::iterator endIf(beginIf); endIf != to; endIf++)
					{
						VecStr ifTokens = tokenize(*endIf);
						if(ifTokens[0] == "if")
							depth++;
						if(depth == 0 and not skip and (ifTokens[0] == "else if" or ifTokens[0] == "else" or ifTokens[0] == "endif"))
						{
							if(condition)
							{
								interpretBlock(beginIf, endIf, begin);
								skip = true;
							}
							else
							{
								from = endIf-1;
								break;
							}
						}
						if(depth == 0 and ifTokens[0] == "endif")
						{
							from = endIf;
							break;
						}
						if(ifTokens[0] == "endif")
							depth--;
					}
				}
				else if(tokens[0] != "else" and tokens[0] != "endif")
					evaluateTree(convert(tokens));
			}
		}
		catch(const ScriptError& e)
		{
			throw std::runtime_error("unable to evaluate line " + std::to_string((from - begin)+1) + " : "
					+ std::string(e.what()));
		}
	}
}

VecStr Interpreter::tokenize(std::string line) const
{
	VecStr first, second, third, err;
	std::smatch m;
	//First tokenization, match string literal
	//This step must be done before everything else because
	//words in string must don't be matched.
	while(regex_search(line, m, string_literal_regex))
	{
		if(m.prefix().str() != "")
			first.push_back(m.prefix().str());
		first.push_back(m[1].str());
		line=m.suffix().str();
	}
	first.push_back(line);
	for(auto submatch:first)
	{
		if(submatch.size() > 0 and submatch[0] != '\"')
		{
			//Second tokenization, match reserved words
			while(regex_search(submatch, m, reserved_names_regex))
			{
				if(m.prefix().str() != "")
					second.push_back(m.prefix().str());
				second.push_back(m[1].str());
				submatch=m.suffix().str();
			}
		}
		second.push_back(submatch);
	}
	//Third tokenization of every submatch
	for(auto& tok:second)
	{
		copy(std::sregex_token_iterator(tok.begin(), tok.end(), token_regex, 1),
	    	 std::sregex_token_iterator(),
			 std::back_inserter(third));
		copy(std::sregex_token_iterator(tok.begin(), tok.end(), token_regex, -1),
	    	 std::sregex_token_iterator(),
			 std::back_inserter(err));
	}
	for(long int i{0}; i < static_cast<long int>(err.size()); ++i)
	{
		if(std::regex_match(err[i], space_regex))
		{
			err.erase(err.begin()+i);
			i--;
		}
	}
	if (not err.empty())
	{
		std::string errorMessage("unrecognized tokens while parsing:");
		for(auto& tok:err)
			errorMessage += " " + tok;
		throw ScriptError(errorMessage);
	}
	return third;
}

Tree<Data>::Ptr Interpreter::convert(const VecStr& tokens)
{
	Tree<Data>::Ptr res;
	std::stack<std::string> operatorsStack;
	std::stack<Tree<Data>::Ptr> operands;
	for(size_t i{0}; i < tokens.size(); ++i)
	{
		if(i+1 < tokens.size() and (tokens[i+1] == "=" or (m_precedence.find(tokens[i]) != m_precedence.end() and tokens[i+1] == "(")))
			operands.push(nullptr);
		else if(m_precedence.find(tokens[i]) != m_precedence.end())
		{
			while(not operatorsStack.empty() and
					m_precedence.at(operatorsStack.top())
					>= m_precedence.at(tokens[i]))
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
			operatorsStack.push(tokens[i]);
		}
		//Value token
		else if(std::regex_match(tokens[i], value_regex) and tokens[i] != "is")
			operands.push(Tree<Data>::create(evaluateToken(tokens[i])));
		//Parenthesis token
		else if(tokens[i] == "(")
		{
			//Parenthesis for function call
			if(i > 0 and m_functions.find(tokens[i-1]) != m_functions.end())
			{
				Tree<Data>::Ptr functionTree = Tree<Data>::create(tokens[i-1]);
				//The function has been pushed on the stack with the value 0, so pop it.
				operands.pop();
				int depth{1};
				for(size_t j{i+1}, lastComma{j}; j < tokens.size() and depth > 0; ++j)
				{
					if(tokens[j] == "(")
						depth++;
					else if(tokens[j] == ")")
						depth--;
					//Add one argument in the arguments list.
					if(tokens[j] == "," or depth == 0)
					{
						VecStr subExpression(tokens.begin()+lastComma, tokens.begin()+j);
						functionTree->pushChild(convert(subExpression));
						lastComma = j+1;
						if(depth == 0)
							i = j;
					}
					functionTree->resolveChildren(functionTree);
				}
				operands.push(functionTree);
			}
			//Arithmetic parenthesis
			else
			{
				VecStr subExpression(tokens.begin()+i+1, tokens.end());
				size_t offset(parenthesis(subExpression));
				subExpression.assign(subExpression.begin(), subExpression.begin()+offset);
				operands.push(convert(subExpression));
				i += offset+1;
			}
		}
		else if(tokens[i] == "=")
		{
			if(i < 1 or not std::regex_match(tokens[i-1], identifier_regex))
				throw ScriptError("Invalid identifier in assignation.");
			operands.pop();
			Tree<Data>::Ptr value = convert(VecStr(tokens.begin()+i+1, tokens.end()));
			m_vars[tokens[i-1]] = evaluateTree(value);
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
		std::string errorMessage("Following operands need an operator:");
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
	if(std::regex_match(token, number_literal_regex))
	{
		if(token.find(".") != std::string::npos)
			return stof(token);
		else
			return stoi(token);
	}
	//Boolean literal
	else if(std::regex_match(token, boolean_literal_regex))
		return token == "true";
	//String literal
	else if(std::regex_match(token, string_literal_regex))
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
	else if(std::regex_match(token, identifier_regex)
			and m_vars.find(token) != m_vars.end())
		return m_vars.at(token);
	//Function name or variable identifier in assignement
	else if(std::regex_match(token, identifier_regex)
			and m_functions.find(token) != m_functions.end())
		return 0;
	else
		throw ScriptError("Unrecognized token: " + token);
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
			return fn.pointer(args, m_context);
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
	int depth{1};
	size_t i(0);
	for(; i < tokens.size(); i++)
	{
		if(tokens[i] == "(")
			depth++;
		else if(tokens[i] == ")")
			depth--;
		if(depth == 0)
			break;
	}
	return i;
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

