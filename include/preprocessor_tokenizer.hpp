#ifndef PREPROCESSOR_TOKENIZER_CPP
#define PREPROCESSOR_TOKENIZER_CPP

#include <string>
#include <vector>
#include "types.hpp"

namespace cpp_parser
{
    /**
     * To tokenize a file or string for the purpose of preprocessor parsing generating
     * enough information for the generation of source code with macros expanded by the
     * preprocessor class.
     */
	class preprocessor_tokenizer
	{
        private:

	    //{Private static methods
	    /**
         * Checks if a given byte is an operator. Could be aritmethic,
         * logical, macro concatenation, assignment or bit operator.
         * @param byte The character to check
         * @return true if is one of the operators given on the description, false otherwise.
         */
		static bool is_operator(const char &byte);

        /**
         * Checks the type of operator on a given byte
         * @param byte The character to check
         * @return token_type
         */
		static token_type get_operator_type(const char &byte);

        /**
         * Checks the type of a given token
         * @param token The string to check for its type
         * @return token_type
         */
		static token_type get_identifier_type(const std::string &token);

        /**
         * Helper function for the tokenizer methods to correctly add a token to a vector of tokens
         * calculating correct column position when neccesary
         * @param token string representation of the token
         * @param line current line number where the token resides
         * @param column current column number where the token starts
         * @param type Type of token
         * @param tokens reference to the vector that will store the token
         */
		static void add_token(const std::string &token, unsigned int line, unsigned int column, token_type type, std::vector<preprocessor_token> &tokens);
		//}

	    public:

        //{Public statuc methods
        /**
         * Opens a file and tokenizes it (depends internally on tokenize_string method)
         * @param file_name The path of the file to tokenize
         * @return Vector that symbolyze lines with an array/vector of tokens
         */
		static std::vector< std::vector<preprocessor_token> > tokenize_file(const std::string &file_name);

        /**
         * Tokenizes a given string
         * @param cahracters The string to tokenize
         * @return Vector that symbolyze lines with an array/vector of tokens
         */
		static std::vector< std::vector<preprocessor_token> > tokenize_string(const std::string &characters);
		//}
	};
};

#endif
