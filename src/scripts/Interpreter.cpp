#include <iostream>
#include <stdexcept>
#include <regex>
#include <algorithm>

#include <entityx/System.h>

#include <TheLostGirl/scripts/scriptsFunctions.h>
#include <TheLostGirl/Parameters.h>
#include <TheLostGirl/systems/PendingChangesSystem.h>

#include <TheLostGirl/scripts/Interpreter.h>

using namespace std;

Interpreter::Interpreter(ifstream& file, entityx::Entity entity, StateStack::Context context):
    m_file(file),
    m_entity(entity),
    m_context(context)
{}

void Interpreter::interpret()
{
	try
	{
		vector<string> tokens;
		//Indicate if the branchement statement like else or else if
		//must be skipped when encountered.
		//So there will be a True when a if or else if condition is True,
		//and following else statement must be skipped.
		stack<bool> condition_match;
		for(string line; getline(m_file, line);)
		{
			tokens = tokenize(line);
//			cout << "Tokens: ";
//			for(auto& token:tokens)
//				cout << "[" << token << "]";
//			cout << "\n";
			if(tokens.size() > 0)
			{
				//Conditional branchement
				if((tokens[0] == "else" or tokens[0] == "else if") and condition_match.top())
					jump_to_endif();
				else if(tokens[0] == "if" or tokens[0] == "else if")
				{
					tokens.erase(tokens.begin());
					//If this is a else if statement, replace the flag
					if(tokens[0] == "else if")
						condition_match.pop();
					condition_match.push(convert<bool>(compute(tokens)));
					//If the condition is false
					if(not condition_match.top())
						//Jump to the next branchement
						jump_to_endif(true);
				}
				else if(tokens[0] == "endif")
					condition_match.pop();
				//Assignement
				else if(find(tokens.begin(), tokens.end(), "=") == tokens.begin()+1)
					m_vars[tokens[0]] = compute(vector<string>(tokens.begin()+2, tokens.end()));
				//Statement that does nothing
				else
					compute(tokens);
			}
		}
	}
	catch(runtime_error e)
	{
		cerr << "Unable to intepret byte " << m_file.tellg() << ": " << e.what() << "\n";
	}
}

vector<string> Interpreter::tokenize(string line)
{
	if(line.find_first_not_of("\n\t ") != std::string::npos)
		line = line.substr(line.find_first_not_of("\n\t "));//Remove trailing blank character
	string name("[[:alnum:]][\\w\\s]*\\w|\\w");
	string number("\\d+|\\d*\\.\\d*");
	string cmp("<=|>=|==|<|>");
	string arith("\\+|-|\\*|/|%|\\(|\\)");
	regex reserved_token("(if|else if|else|endif|event|and|or|not|is|true|false)[\\s\\b]+");
	regex token("("+number+"|"+name+"|"+cmp+"|"+arith+"|:|!|=|,)\\s*");
	vector<string> first_tokenization, second_tokenization, unrecognized_tokens;
	smatch m;
	//First tokenization, match reserved words
	while(regex_search(line, m, reserved_token))
	{
		if(m.prefix().str().length() > 0)
			first_tokenization.push_back(m.prefix().str());
		first_tokenization.push_back(m[1].str());
		line=m.suffix().str();
	}
	first_tokenization.push_back(line);
	//Second tokenization of every submatch
	for(auto& tok:first_tokenization)
	{
		copy(sregex_token_iterator(tok.begin(), tok.end(), token, 1),
	    	 sregex_token_iterator(),
			 back_inserter(second_tokenization));
		copy(sregex_token_iterator(tok.begin(), tok.end(), token, -1),
	    	 sregex_token_iterator(),
			 back_inserter(unrecognized_tokens));
	}
	if(unrecognized_tokens.size() > 0 and
		count(unrecognized_tokens.begin(), unrecognized_tokens.end(), "") != unrecognized_tokens.size())
    {
        string message("Lexical error:\n");
        for(auto& tok:unrecognized_tokens)
            message += "Unrecognized token: \"" + tok + "\"\n";
        throw runtime_error(message);
    }
	return second_tokenization;
}


Interpreter::Var Interpreter::compute(vector<string> tokens)
{
	if(tokens.size() < 1)
		throw runtime_error("expected value.");
	else if(tokens.size() == 1)
	{
		if(regex_match(tokens[0], regex("\\d+")))
			return stoi(tokens[0]);
		else if(regex_match(tokens[0], regex("\\d*.\\d*")))
			return stof(tokens[0]);
		else if(tokens[0] == "true")
			return true;
		else if(tokens[0] == "false")
			return false;
		else if(regex_match(tokens[0], regex("[[:alnum:]][\\w\\s]*\\w|\\w"))
				and m_vars.find(tokens[0]) != m_vars.end())
			return m_vars[tokens[0]];
		else
			throw runtime_error("Unrecognized token: "+tokens[0]);
	}
	//If the tokens match a function call with arguments
	else if(find(tokens.begin(), tokens.end(), "!") == tokens.end()-1//If there is a !
			and (find(tokens.begin(), tokens.end(), ":") == tokens.begin()+1 //and if there is a :
				or tokens.size() == 2))//or if there is only the identifier and !
	{
		vector<Var> arguments;
		//If there is some arguments
		if(find(tokens.begin(), tokens.end(), ":") == tokens.begin()+1)
		{
			//Get the list of the arguments
			vector<string> current_argument;
			for(unsigned int i{2}; i < tokens.size()-1; i++)
			{
				if(tokens[i] == ",")
				{
					arguments.push_back(compute(current_argument));
					current_argument.clear();
				}
				else
					current_argument.push_back(tokens[i]);
			}
			arguments.push_back(compute(current_argument));
		}
		auto check_args = [&](vector<Var> args, vector<int> types)
		{
			if(args.size() > types.size())
				throw runtime_error("too many arguments.");
			else if(args.size() < types.size())
				throw runtime_error("too few arguments.");
			else
				for(size_t i{0}; i < args.size(); ++i)
					if(args[i].which() != types[i])
						throw std::runtime_error("bad argument type.");
			return true;
		};
		try
		{
			if(tokens[0] == "nearest foe" && check_args(arguments, {}))
			{
				return nearestFoe(m_entity, m_context.parameters.pixelByMeter);
			}
			else if(tokens[0] == "attack" && check_args(arguments, {3}))
			{
				return attack(m_entity, boost::get<entityx::Entity>(arguments[0]));
			}
			else if(tokens[0] == "can jump" && check_args(arguments, {}))
			{
				return canJump(m_entity);
			}
			else if(tokens[0] == "jump" && check_args(arguments, {}))
			{
				return jump(m_entity, m_context.systemManager.system<PendingChangesSystem>()->commandQueue);
			}
			else if((tokens[0]=="print" or tokens[0]=="out"))
			{
				if(arguments.size() > 1)
					throw runtime_error("too many arguments.");
				else if(arguments.size() == 0)
					throw runtime_error("too few arguments.");
				if(arguments[0].which() == 0)
					return print(boost::get<int>(arguments[0]));
				else if(arguments[0].which() == 1)
					return print(boost::get<float>(arguments[0]));
				else if(arguments[0].which() == 2)
					print(boost::get<bool>(arguments[0]));
				else if(arguments[0].which() == 3)
					return print(boost::get<entityx::Entity>(arguments[0]));
			}
			else
				throw runtime_error("unrecognized function name.");
		}
		catch(runtime_error& e)
		{
			throw runtime_error("unable to execute the function " + tokens[0] +
					": " + e.what());
		}
	}
	else
	{
		vector<string> operators{"(", ")", "or", "and", "is", "==", "!=", "<",
				"<=",">", ">=", "+", "-", "*", "/", "%", "not"};
		vector<string>::iterator lowest_op(find_if(operators.begin(), operators.end(),
					[&](const string& str)
					{return find(tokens.begin(), tokens.end(), str) != tokens.end();}));
		if(lowest_op == operators.end())
		{
			string message("unable to compute following tokens: ");
			for(auto& token:tokens)
				message += "[" + token + "]";
			throw runtime_error(message);
		}
		else if(*lowest_op == ")")
			throw runtime_error("no matching parenthesis found.");
		else if(*lowest_op == "(")
		{
			vector<string>::iterator match_par(find(tokens.begin(), tokens.end(), ")"));
			if(match_par == tokens.end())
				throw runtime_error("no matching parenthesis found.");
		}
		lowest_op = find(tokens.begin(), tokens.end(), *lowest_op);
		Var left = compute(vector<string>(tokens.begin(), lowest_op));
		Var right = compute(vector<string>(lowest_op+1, tokens.end()));
		int highest_type = max(left.which(), right.which());
		int lowest_type = min(left.which(), right.which());
		if(highest_type == 3 and lowest_type < 2)
			throw runtime_error("operations between entities and artihmetics types are not allowed.");
		if(highest_type == 0)//Integers
		{
			int l{convert<int>(left)};
			int r{convert<int>(right)};
			if(*lowest_op == "or")
				return l or r;
			else if(*lowest_op == "and")
				return l and r;
			else if(*lowest_op == "is" or *lowest_op == "==")
				return l == r;
			else if(*lowest_op == "!=")
				return l != r;
			else if(*lowest_op == "<")
				return l < r;
			else if(*lowest_op == "<=")
				return l <= r;
			else if(*lowest_op == ">")
				return l > r;
			else if(*lowest_op == ">=")
				return l >= r;
			else if(*lowest_op == "+")
				return l + r;
			else if(*lowest_op == "-")
				return l - r;
			else if(*lowest_op == "*")
				return l * r;
			else if(*lowest_op == "/")
				return l / r;
			else if(*lowest_op == "%")
				return l % r;
			else if(*lowest_op == "not")
				return not r;
		}
		else if(highest_type == 1)//Floating-point
		{
			float l{convert<float>(left)};
			float r{convert<float>(right)};
			if(*lowest_op == "or")
				return l or r;
			else if(*lowest_op == "and")
				return l and r;
			else if(*lowest_op == "is" or *lowest_op == "==")
				return l == r;
			else if(*lowest_op == "!=")
				return l != r;
			else if(*lowest_op == "<")
				return l < r;
			else if(*lowest_op == "<=")
				return l <= r;
			else if(*lowest_op == ">")
				return l > r;
			else if(*lowest_op == ">=")
				return l >= r;
			else if(*lowest_op == "+")
				return l + r;
			else if(*lowest_op == "-")
				return l - r;
			else if(*lowest_op == "*")
				return l * r;
			else if(*lowest_op == "/")
				return l / r;
			else if(*lowest_op == "not")
				return not r;
		}
		else if(highest_type == 2 or (highest_type == 3 and lowest_type == 2))//Booleans
		{
			bool l{convert<bool>(left)};
			bool r{convert<bool>(right)};
			if(*lowest_op == "or")
				return l or r;
			else if(*lowest_op == "and")
				return l and r;
			else if(*lowest_op == "is" or *lowest_op == "==")
				return l == r;
			else if(*lowest_op == "!=")
				return l != r;
			else if(*lowest_op == "<")
				return l < r;
			else if(*lowest_op == "<=")
				return l <= r;
			else if(*lowest_op == ">")
				return l > r;
			else if(*lowest_op == ">=")
				return l >= r;
			else if(*lowest_op == "+")
				return l + r;
			else if(*lowest_op == "-")
				return l - r;
			else if(*lowest_op == "*")
				return l * r;
			else if(*lowest_op == "/")
				return l / r;
			else if(*lowest_op == "not")
				return not r;
		}
		else//Entities
		{
			//Here we are sure that the two variables are entities
			entityx::Entity l{boost::get<entityx::Entity>(left)};
			entityx::Entity r{boost::get<entityx::Entity>(right)};
			if(*lowest_op == "is" or *lowest_op == "==")
				return l == r;
			else if(*lowest_op == "!=")
				return l != r;
			else if(*lowest_op == "not")
				return not r;
			else
				throw runtime_error("operator [" + *lowest_op + "] not allowed on entity.");
		}
	}
	return false;
}

void Interpreter::jump_to_endif(bool jump_to_else)
{
	int before_branchement;
	int depth{0};
	string line;
	vector<string> tokens;
	do
	{
		if(tokens.size() > 0 and tokens[0] == "if")
			depth++;
		else if(tokens.size() > 0 and tokens[0] == "endif")
			depth--;
		before_branchement = m_file.tellg();
		getline(m_file, line);
		tokens = tokenize(line);
	}
	//While the first token is not any of the given tokens
	while(tokens.size() < 1
			or (jump_to_else and tokens[0] != "else"
				and tokens[0] != "else if" and tokens[0] != "endif")
			or (not jump_to_else and tokens[0] != "endif")
			or depth > 0);
	m_file.seekg(before_branchement);
}
