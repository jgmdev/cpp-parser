#include <fstream>
#include <iostream>
#include "misc.hpp"
#include "preprocessor_tokenizer.hpp"

using namespace std;

namespace cpp_parser
{
    vector< vector<preprocessor_token> > preprocessor_tokenizer::tokenize_file(const string &file_name)
	{
	    //Open file
	    ifstream file_stream(file_name.c_str());

	    //To store the content of the file
	    string file_content = "";

        //store each byte of the opened file on the file_content variable
	    while(!file_stream.eof())
	    {
	        char byte;
	        file_stream.read(&byte, sizeof(char));
	        file_content += byte;
	    }

        //Tokenize the string and return the vector with tokens
	    return tokenize_string(file_content);
	}

	vector< vector<preprocessor_token> > preprocessor_tokenizer::tokenize_string(const string &characters)
	{
		char byte, byte_peek;
		std::string token = "";
		std::vector<preprocessor_token> tokens;
		std::vector< vector<preprocessor_token> > lines;

		bool inside_string = false;
		char string_enclosure;
		bool single_line_comment = false;
		bool multiple_line_comment = false;
		bool multiple_symbols_operator = false;
		bool is_identifier = false;
		bool is_number = false;
		bool multiline_preprocessor = false;
		bool line_ended = false;

        unsigned int line = 1;
		unsigned int column = 1;

		for(unsigned int byte_position=0; byte_position<characters.size(); byte_position++)
		{
			byte = characters[byte_position];

			if((byte_position + 1) < characters.size())
			{
			    byte_peek = characters[byte_position + 1];
			}
			else
			{
			    byte_peek = '\n';
			}

            if(is_number)
            {
                token += byte;

                if(!isalnum(byte_peek) && byte_peek != '.')
                {
                    is_number = false;

                    add_token(token, line, column, number, tokens);

                    token = "";
                }
            }
            else if(is_identifier)
            {
                token += byte;

                if((isspace(byte_peek) || !isalnum(byte_peek)) && (byte_peek != '_' && byte_peek != '$'))
                {
                    is_identifier = false;

                    add_token(token, line, column, identifier, tokens);

                    token = "";
                }
            }
            else if(multiple_symbols_operator)
            {
                token += byte;

                if(!is_operator(byte_peek))
                {
                    multiple_symbols_operator = false;

                    add_token(token, line, column, operator_symbol, tokens);

                    token = "";
                }
            }
            else if(multiple_line_comment)
            {
                token += byte;

                if(byte == '*' && byte_peek == '/') //End of multiline comment
                {
                    multiple_line_comment = false;

                    token += byte_peek;
                    byte_position++;

                    column++; //Since readed next character we need to increment column

                    add_token(token, line - count_character('\n', token), //Since multiple lines save the first line where started
                              column-2, multi_comment, tokens);

			        token = "";
                }

                if(byte == '\n')
                {
                    line++;
                    column = 1;
                    line_ended = true;
                }
            }
            else if(single_line_comment)
            {
                if(byte == '\n') //End of single line comment
                {
                    single_line_comment = false;

                    add_token(token, line, column, comment, tokens);

                    line++;
                    column = 1;

			        token = "";

			        lines.push_back(tokens);
                    tokens.clear();
                    line_ended = true;
                }
                else
                {
                    token += byte;
                }
            }
            else if(inside_string)
            {
                token += byte;

                if(byte == '\\') //Read escaped characters in case of \" \' to no detect end of string wrongly
                {
                    byte_position++;
                    column++; //Since readed next character we need to increment column
                    token += byte_peek;
                }
                else if(byte == '"' && string_enclosure == '"') //End of string was found
                {
                    inside_string = false;
                }
                else if(byte == '\'' && string_enclosure == '\'')
                {
                    inside_string = false;
                }

                if(!inside_string) //Finally save the full string token
                {
                    add_token(token, line, column, strings, tokens);
			        token = "";
                }
            }
			else if(!isspace(byte) && byte != '\n')
			{
			    if(byte == '/' && byte_peek == '*') //Check if comes multiple line commment
			    {
			        token += byte;
			        multiple_line_comment = true;
			    }
			    else if(byte == '/' && byte_peek == '/') //Check if comes single line comment
			    {
			        token += byte;
			        single_line_comment = true;
			    }
			    else if(byte == '"' || byte == '\'') //Check if entering string or character
			    {
			        token += byte;
			        inside_string = true;
			        string_enclosure = byte;
			    }
			    else if(isdigit(byte))
			    {
			        token += byte;

			        //Check if number is longer than 1 character
					if(isalnum(byte_peek) || byte_peek == '.')
					{
					    //to be able to parse numbers as part of identifier
                        is_number = true;
					}
					else
					{
					    add_token(token, line, column, number, tokens);
				        token = "";
					}
			    }
				else if(isalpha(byte) || byte == '_' || byte == '$') //Other normal keywords and identifiers
				{
					token += byte;

					//Check if identifer is more that 1 letter long
					if((!isspace(byte_peek) && isalnum(byte_peek)) || (byte_peek == '_' || byte_peek == '$'))
					{
					    //to be able to parse numbers as part of identifier
                        is_identifier = true;
					}
					else
					{
					    add_token(token, line, column, identifier, tokens);
				        token = "";
					}
				}
				else
				{
				    if(token != "") //Saves any identifier as token if available
				    {
				        add_token(token, line, column, get_identifier_type(token), tokens);
				    }

                    //Saves any other character like operators (should be only operators)
                    token = byte;
                    if(is_operator(byte) && is_operator(byte_peek)) //Check if multiple symbols operator
					{
					    multiple_symbols_operator = true;
					}
					else
					{
					    add_token(token, line, column, get_operator_type(byte), tokens);
                        token = "";
					}
				}
			}
			else if(isspace(byte) && byte != '\n') //Action after a space is found
			{
			    if(token != " " && token != "") //Save last token
			    {
			        add_token(token, line, column, get_identifier_type(token), tokens);
			        token = "";
			    }
			}
			else if(byte == '\n') //End of line
			{
                if(tokens.size() > 0) //Check if multiple lines macro definition
                {
                    if(tokens.at(0).token == "#" && tokens.at(tokens.size()-1).token == "\\")
                    {
                        //TODO: Reparar el problema de los defines con multiples lineas esto que hice no sirve con los nuevos cambios
                        //We are on a multiple lines macro definition so we continue parsing until the macro ends
                        token = "";
                    }
                    else
                    {
                        if(token != " ") //Just finish reading the line
                        {
                            add_token(token, line, column, new_line, tokens);
                        }
                    }
                }
			    else //Encountered an empty line
			    {
			        if(token != " ")
                    {
                        add_token(token, line, column, new_line, tokens);
                    }
			    }

			    line++;
			    column = 1;

			    lines.push_back(tokens);
                tokens.clear();
                line_ended = true;
			}

            if(!line_ended)
            {
                column++;
            }
            else
            {
                line_ended = false;
            }
		}

		return lines;
	}

	bool preprocessor_tokenizer::is_operator(const char& byte)
	{
	    switch(byte)
	    {
	        case '+':
	        case '-':
	        case '*':
	        case '/':
	        case '%':
	        case '^':
	        case '~':
	        case '=':
	        case '<':
	        case '>':
	        case '&':
	        case '|':
	        case '!':
	        case '.':
	        case ':':
	        case '?':
	        case '#':

                return true;
	    }

	    return false;
	}

	token_type preprocessor_tokenizer::get_operator_type(const char &byte)
	{
	    if(is_operator(byte))
            return operator_symbol;

	    switch(byte)
	    {
	        case '(':
                return parenthesis_begin;
            case ')':
                return parethesis_end;
            case '{':
                return body_begin;
            case '}':
                return body_end;
            case '[':
                return array_begin;
            case ']':
                return array_end;
	    }

	    return other;
	}

	token_type preprocessor_tokenizer::get_identifier_type(const string &token)
	{
	    static std::vector<std::string> cpp_keywords;

	    if(cpp_keywords.size() <= 0)
        {
            cpp_keywords.push_back("and");
            cpp_keywords.push_back("and_eq");
            cpp_keywords.push_back("asm");
            cpp_keywords.push_back("auto");
            cpp_keywords.push_back("bitand");
            cpp_keywords.push_back("bitor");
            cpp_keywords.push_back("bool");
            cpp_keywords.push_back("break");
            cpp_keywords.push_back("case");
            cpp_keywords.push_back("catch");
            cpp_keywords.push_back("char");
            cpp_keywords.push_back("class");
            cpp_keywords.push_back("compl");
            cpp_keywords.push_back("const");
            cpp_keywords.push_back("const_cast");
            cpp_keywords.push_back("continue");
            cpp_keywords.push_back("default");
            cpp_keywords.push_back("delete");
            cpp_keywords.push_back("do");
            cpp_keywords.push_back("double");
            cpp_keywords.push_back("dynamic_cast");
            cpp_keywords.push_back("else");
            cpp_keywords.push_back("enum");
            cpp_keywords.push_back("explicit");
            cpp_keywords.push_back("export");
            cpp_keywords.push_back("extern");
            cpp_keywords.push_back("false");
            cpp_keywords.push_back("float");
            cpp_keywords.push_back("for");
            cpp_keywords.push_back("friend");
            cpp_keywords.push_back("goto");
            cpp_keywords.push_back("if");
            cpp_keywords.push_back("inline");
            cpp_keywords.push_back("int");
            cpp_keywords.push_back("long");
            cpp_keywords.push_back("mutable");
            cpp_keywords.push_back("namespace");
            cpp_keywords.push_back("new");
            cpp_keywords.push_back("not");
            cpp_keywords.push_back("not_eq");
            cpp_keywords.push_back("operator");
            cpp_keywords.push_back("or");
            cpp_keywords.push_back("or_eq");
            cpp_keywords.push_back("private");
            cpp_keywords.push_back("protected");
            cpp_keywords.push_back("public");
            cpp_keywords.push_back("register");
            cpp_keywords.push_back("reintepret_cast");
            cpp_keywords.push_back("return");
            cpp_keywords.push_back("short");
            cpp_keywords.push_back("signed");
            cpp_keywords.push_back("sizeof");
            cpp_keywords.push_back("static");
            cpp_keywords.push_back("static_cast");
            cpp_keywords.push_back("struct");
            cpp_keywords.push_back("switch");
            cpp_keywords.push_back("template");
            cpp_keywords.push_back("this");
            cpp_keywords.push_back("throw");
            cpp_keywords.push_back("true");
            cpp_keywords.push_back("try");
            cpp_keywords.push_back("typedef");
            cpp_keywords.push_back("typeid");
            cpp_keywords.push_back("typename");
            cpp_keywords.push_back("union");
            cpp_keywords.push_back("unsigned");
            cpp_keywords.push_back("using");
            cpp_keywords.push_back("virtual");
            cpp_keywords.push_back("void");
            cpp_keywords.push_back("volatile");
            cpp_keywords.push_back("wchar_t");
            cpp_keywords.push_back("while");
            cpp_keywords.push_back("xor");
            cpp_keywords.push_back("xor_eq");
        }

	    for(unsigned int i=0; i<cpp_keywords.size(); i++)
	    {
	        if(cpp_keywords[i] == token)
	        {
	            return keyword;
	        }
	    }

	    return identifier;
	}

	void preprocessor_tokenizer::add_token(const string &token, unsigned int line, unsigned int column, token_type type, std::vector<preprocessor_token> &tokens)
	{
	    preprocessor_token token_struct;

	    token_struct.token = token;
        token_struct.line = line;

        if(column == 1 && token.size() == 0) //For empty lines
        {
            token_struct.column = column;
        }
        else if(column > token.size()) //Normal operation
        {
            token_struct.column = column - token.size() + 1;
        }
        else if(column == token.size()) //For lines that start with some identifier or word
        {
            token_struct.column = 1;
        }
        else // everything else
        {
            token_struct.column = column;
        }

        token_struct.type = type;

        tokens.push_back(token_struct);
	}
}
