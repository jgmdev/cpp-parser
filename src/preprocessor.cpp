#include <map>
#include <cctype>
#include <iostream>
#include "misc.hpp"
#include "types.hpp"
#include "preprocessor.hpp"
#include "preprocessor_tokenizer.hpp"
#include "constexpr.hpp"

using namespace std;

namespace cpp_parser
{
	preprocessor::~preprocessor()
	{
	}

	vector<preprocessor_token> preprocessor::strip_macro_definition(const vector<preprocessor_token> &definition_declaration)
	{
	    vector<preprocessor_token> new_copy = definition_declaration;

	    new_copy.erase(new_copy.begin(), new_copy.begin() + 2);

	    return new_copy;
	}

	const define preprocessor::parse_define(const vector<preprocessor_token> &define_declaration)
	{
		int declaration_size = define_declaration.size();

		bool getting_name = true;
		bool parsing_parameters = false;
		bool getting_value = false;

		string name = "";
		string value = "";
		vector<string> parameters;

		for(int i=0; i<declaration_size; i++)
		{
			if(getting_name)
			{
                name = define_declaration[i].token;
                getting_name = false;

                if(define_declaration[i+1].token == "(")
                {
                    i++;
                    parsing_parameters = true;
                }
                else
                {
                    getting_value = true;
                }
			}
			else if(parsing_parameters)
			{
                if(define_declaration[i].token != "," && define_declaration[i].token != ")")
                {
                    parameters.push_back(define_declaration[i].token);
                }
                else if(define_declaration[i].token == ")")
                {
                    parsing_parameters = false;
                    getting_value = true;
                }
			}
			else if(getting_value)
			{
			    if(define_declaration[i].token == "\\")
			    {
			        value += "\n";
			    }
			    else
			    {
			        value += define_declaration[i].token;
			    }
			}
		}

		define define_structure;
		define_structure.name = name;
		define_structure.value = value;
		define_structure.parameters = parameters;

		if(parameters.size() > 0)
		{
			define_structure.type = function;
		}
		else
		{
			define_structure.type = declaration;
		}

		return define_structure;
	}

    // TODO (jgm#1#): Fully Implement this function
	const bool preprocessor::parse_expression(const vector<preprocessor_token> &expression)
	{
	    bool return_value = false;

	    const vector<Token> tokens_vector = expand_macro_expression(expression);

        Token tokens[tokens_vector.size()];

	    for(unsigned int i=0; i<tokens_vector.size(); i++)
        {
            tokens[i] = tokens_vector[i];
        }

        try
        {
            PCToken pcToken = &tokens[0];

            if(ConstExprEvaluator::eval(&pcToken) > 0)
            {
                return_value = true;
            }
        }
        catch (const PreprocessorError &prepError)
        {
            return_value = false;

            //TODO add this exception to m_error
            std::cerr << "Exception: " << prepError.getMessage() << "\n";
        }

        return return_value;
	}

	void preprocessor::add_error(const std::vector<preprocessor_token> &error_declaration, const string& file)
	{
        string error_message;
        unsigned int line;

        //Copy error message
        for(unsigned int i=2; i<error_declaration.size(); i++)
        {
            error_message += error_declaration[i].token + " ";
        }

        line = error_declaration[0].line;

        preprocessor_error error_struct = {error_message, file, line};

        m_errors.push_back(error_struct);
	}

	const vector<Token> preprocessor::expand_macro_expression(const vector<preprocessor_token> &expression)
	{
	    vector<Token> tokens;
	    Token space = { ttWhiteSpace, " " };

	    for(unsigned int i=0; i<expression.size(); i++)
        {
            Token token_to_add;

            if(expression[i].token == "defined")
            {
                //No need to manually evaluate so skip it
                continue;
            }
            else if(expression[i].type == identifier)
            {
                token_to_add.type = ttNumber;

                if(is_defined(expression[i].token))
                {
                    define macro = get_define(expression[i].token);

                    //For macro definitions
                    if(macro.parameters.size() <= 0)
                    {
                        if(macro.value == "")
                        {
                            //The macro is defined but without a predifined value for it so we default to 1
                            token_to_add.value = "1";
                        }
                        else
                        {
                            //Tha macro has a predifined value (We should check if it's a valid number)
                            token_to_add.value = macro.value;
                        }
                    }

                    //For macro functions, we need to parse the parameters and then evaluate with recursive function
                    else
                    {
                        //for now we just return 1 but this is totally wrong
                        token_to_add.value = "1";
                    }
                }
                else
                {
                    token_to_add.value = "0";
                }
            }

            //Adds any numbers found
            else if(expression[i].type == number)
            {
                token_to_add.type = ttNumber;
                token_to_add.value = expression[i].token;
            }

            //Handle any other operator found
            else
            {
                if(expression[i].token == "?")
                {
                    token_to_add.type = ttQuestion;
                    token_to_add.value = "?";
                }
                else if(expression[i].token == ":")
                {
                    token_to_add.type = ttColon;
                    token_to_add.value = ":";
                }
                else if(expression[i].token == "||")
                {
                    token_to_add.type = ttOr;
                    token_to_add.value = "||";
                }
                else if(expression[i].token == "&&")
                {
                    token_to_add.type = ttAnd;
                    token_to_add.value = "&&";
                }
                else if(expression[i].token == "|")
                {
                    token_to_add.type = ttBitOr;
                    token_to_add.value = "|";
                }
                else if(expression[i].token == "^")
                {
                    token_to_add.type = ttBitXOr;
                    token_to_add.value = "^";
                }
                else if(expression[i].token == "&")
                {
                    token_to_add.type = ttBitAnd;
                    token_to_add.value = "&";
                }
                else if(expression[i].token == "==")
                {
                    token_to_add.type = ttEqual;
                    token_to_add.value = "==";
                }
                else if(expression[i].token == "!=")
                {
                    token_to_add.type = ttNotEqual;
                    token_to_add.value = "!=";
                }
                else if(expression[i].token == "<")
                {
                    token_to_add.type = ttLess;
                    token_to_add.value = "<";
                }
                else if(expression[i].token == ">")
                {
                    token_to_add.type = ttGreater;
                    token_to_add.value = ">";
                }
                else if(expression[i].token == "<=")
                {
                    token_to_add.type = ttLessEqual;
                    token_to_add.value = "<=";
                }
                else if(expression[i].token == ">=")
                {
                    token_to_add.type = ttGreaterEqual;
                    token_to_add.value = ">=";
                }
                else if(expression[i].token == "<<")
                {
                    token_to_add.type = ttLShift;
                    token_to_add.value = "<<";
                }
                else if(expression[i].token == ">>")
                {
                    token_to_add.type = ttRShift;
                    token_to_add.value = ">>";
                }
                else if(expression[i].token == "+")
                {
                    token_to_add.type = ttPlus;
                    token_to_add.value = "+";
                }
                else if(expression[i].token == "-")
                {
                    token_to_add.type = ttMinus;
                    token_to_add.value = "-";
                }
                else if(expression[i].token == "*")
                {
                    token_to_add.type = ttTimes;
                    token_to_add.value = "*";
                }
                else if(expression[i].token == "/")
                {
                    token_to_add.type = ttDivide;
                    token_to_add.value = "/";
                }
                else if(expression[i].token == "%")
                {
                    token_to_add.type = ttModulo;
                    token_to_add.value = "%";
                }
                else if(expression[i].token == "!")
                {
                    token_to_add.type = ttNot;
                    token_to_add.value = "?";
                }
                else if(expression[i].token == "~")
                {
                    token_to_add.type = ttBitNeg;
                    token_to_add.value = "~";
                }
                else if(expression[i].token == "(")
                {
                    token_to_add.type = ttLParen;
                    token_to_add.value = "(";
                }
                else if(expression[i].token == ")")
                {
                    token_to_add.type = ttRParen;
                    token_to_add.value = ")";
                }
            }

            if(token_to_add.value != "")
                tokens.push_back(token_to_add);
        }

        Token endToken = {ttEndOfTokens, ""};
        tokens.push_back(endToken);


        return tokens;
	}

	const define preprocessor::get_define(const string &definition)
	{
		//First check in global definitions
		unsigned int global_definitions_count = m_global_defines.size();

		for(unsigned int i=0; i<global_definitions_count; i++)
		{
			if(definition == m_global_defines.at(i).name)
			{
				return m_global_defines.at(i);
			}
		}

		//Secondly check in definitions added while parsing
		unsigned int local_definitions_count = m_local_defines.size();

		for(unsigned int i=0; i<local_definitions_count; i++)
		{
			if(definition == m_local_defines.at(i).name)
			{
				return m_local_defines.at(i);
			}
		}

		return define();
	}

	const string preprocessor::parse_file(const string &file, file_scope scope)
	{
	    string full_file_path = file_path(file, scope);

	    if(!file_exists(full_file_path))
	    {
	        return string();
	    }

        unsigned int deepness = 0;
        map<unsigned int, bool> last_condition_return;
        string output;

        vector< vector<preprocessor_token> > lines = preprocessor_tokenizer::tokenize_file(full_file_path);

        for(unsigned int position=0; position<lines.size(); position++)
        {
            vector<preprocessor_token> tokens = lines[position];

            //Parse macro
            if(tokens[0].token == "#")
            {
                if(deepness == 0 || (deepness > 0 && last_condition_return[deepness]))
                {
                    if(tokens[1].token == "define")
                    {
                        define definition = parse_define(strip_macro_definition(tokens));
                        definition.file = file;
                        definition.line = tokens[2].line;
                        definition.column = tokens[2].column;
                        m_local_defines.push_back(definition);
                    }
                    if(tokens[1].token == "include")
                    {
                        string include_enclosure = tokens[2].token;
                        string include_file = "";
                        file_scope header_scope;

                        if(include_enclosure == "<")
                        {
                            for(unsigned int i=3; i<tokens.size(); i++)
                            {
                                if(tokens[i].token == ">")
                                {
                                    break;
                                }
                                else
                                {
                                    include_file += tokens[i].token;
                                }
                            }

                            m_headers_scope[include_file] = global;
                            header_scope = global;
                        }
                        else if(include_enclosure[0] == '"')
                        {
                            for(unsigned int i=1; i<tokens[2].token.size(); i++)
                            {
                                if(tokens[2].token.at(i) == '"')
                                {
                                    break;
                                }
                                else
                                {
                                    include_file += tokens[2].token.at(i);
                                }
                            }

                            m_headers_scope[include_file] = local;
                            header_scope = local;
                        }
                        else
                        {

                        }

                        if(!is_header_parsed(include_file))
                        {
                            output += parse_file(include_file, header_scope); //To output the processed headers code
                            //parse_file(include_file, header_scope);

                            m_headers.push_back(include_file);
                        }
                    }
                    else if(tokens[1].token == "undef")
                    {
                        remove_define(tokens[2].token);
                    }
                    else if(tokens[1].token == "ifdef")
                    {
                        deepness++;
                        if(is_defined(tokens[2].token))
                        {
                            last_condition_return[deepness] = true;
                        }
                        else
                        {
                            last_condition_return[deepness] = false;
                        }
                    }
                    else if(tokens[1].token == "ifndef")
                    {
                        deepness++;
                        if(!is_defined(tokens[2].token))
                        {
                            last_condition_return[deepness] = true;
                        }
                        else
                        {
                            last_condition_return[deepness] = false;
                        }
                    }
                    else if(tokens[1].token == "if")
                    {
                        deepness++;
                        last_condition_return[deepness] = parse_expression(strip_macro_definition(tokens));
                    }
                    else if(tokens[1].token == "error")
                    {
                        add_error(tokens, file);
                    }
                }

                if(deepness > 0 && (tokens[1].token == "elif" || tokens[1].token == "else" || tokens[1].token == "endif"))
                {
                    if(tokens[1].token == "elif" && last_condition_return[deepness] != true)
                    {
                        last_condition_return[deepness] = parse_expression(strip_macro_definition(tokens));
                    }
                    else if(tokens[1].token == "else" && last_condition_return[deepness] != true)
                    {
                        last_condition_return[deepness] = true;
                    }
                    else if(tokens[1].token == "endif")
                    {
                        last_condition_return.erase(last_condition_return.find(deepness));
                        deepness--;
                    }
                }
            }

            //Parse code
            else
            {
                if(deepness == 0 || (deepness > 0 && last_condition_return[deepness]))
                {
                    unsigned int column = 1;

                    for(unsigned int i=0; i<tokens.size(); i++)
                    {
                        unsigned int columns_to_jump = tokens[i].column - column;

                        if(tokens[i].column <= 0)
                        {
                            columns_to_jump = 0;
                        }
                        else if(tokens[i].column < column)
                        {
                            columns_to_jump = column - tokens[i].column;
                        }

                        for(unsigned int y=0; y<columns_to_jump; y++)
                        {
                            output += " ";
                        }

                        if(tokens[i].type == identifier && is_defined(tokens[i].token))
                        {
                            output += get_define(tokens[i].token).value;
                        }
                        else
                        {
                            output += tokens[i].token;
                        }

                        column = tokens[i].column + tokens[i].token.size();
                    }

                    output += "\n";
                }
            }
        }

		return output;
	}

	const bool preprocessor::is_defined(const string &definition)
	{
		//First check in global definitions
		unsigned int global_definitions_count = m_global_defines.size();

		for(unsigned int i=0; i<global_definitions_count; i++)
		{
			if(definition == m_global_defines.at(i).name)
			{
				return true;
			}
		}

		//Secondly check in definitions added while parsing
		unsigned int local_definitions_count = m_local_defines.size();

		for(unsigned int i=0; i<local_definitions_count; i++)
		{
			if(definition == m_local_defines.at(i).name)
			{
				return true;
			}
		}


		return false;
	}

	const bool preprocessor::remove_define(const string &definition)
	{
	    for(unsigned int i=0; i<m_global_defines.size(); i++)
	    {
	        if(definition == m_global_defines[i].name)
	        {
	            m_global_defines.erase(m_global_defines.begin() + i, (m_global_defines.begin() + i) + 1);
	            return true;
	        }
	    }

	    for(unsigned int i=0; i<m_local_defines.size(); i++)
	    {
	        if(definition == m_local_defines[i].name)
	        {
	            m_local_defines.erase(m_local_defines.begin() + i, (m_local_defines.begin() + i) + 1);
	            return true;
	        }
	    }

	    return false;
	}

	const string preprocessor::file_path(const string &file, file_scope scope)
	{
	    string file_path = "";
        if(scope == local)
        {
            for(unsigned int i=0; i<m_local_includes.size(); i++)
            {
                string temp_file_path = m_local_includes[i];
                if(temp_file_path.at(temp_file_path.size() -1) != '/')
                    temp_file_path += "/";

                temp_file_path += file;

                if(file_exists(temp_file_path))
                {
                    file_path = temp_file_path;
                    break;
                }
            }
        }
        else
        {
            for(unsigned int i=0; i<m_global_includes.size(); i++)
            {
                string temp_file_path = m_global_includes[i];
                if(temp_file_path.at(temp_file_path.size() -1) != '/')
                    temp_file_path += "/";

                temp_file_path += file;

                if(file_exists(temp_file_path))
                {
                    file_path = temp_file_path;
                    break;
                }
            }
        }

        return file_path;
	}
}
