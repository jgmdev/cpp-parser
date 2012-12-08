#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <map>
#include <string>
#include <vector>
#include <exception>
#include "types.hpp"
#include "constexpr.hpp"

namespace cpp_parser
{
    //{Forward declarations
    struct preprocessor_token;
    //}

    struct preprocessor_error
    {
        std::string message;
        std::string file;
        unsigned int line;
    };

    /**
     * To preprocess macros in a file and produce source code ready for normal parsing
     */
	class preprocessor
	{
        private:

	    //{Private properties/members
		std::vector<includes> m_includes;
		std::vector<define> m_local_defines;
		std::vector<define> m_global_defines;
		std::vector<std::string> m_local_includes;
		std::vector<std::string> m_global_includes;
		std::vector<std::string> m_headers;
		std::map<std::string, file_scope> m_headers_scope;
		std::vector<preprocessor_error> m_errors;
		std::string m_file;
		//}

        //{Private Methods
        /**
         * Removes the # and macro type (ex: include, define, if, etc) from the tokens vector (first 2 elements)
         * @param definition_declaration vector/array of tokens part of a macro
         * @return vector of tokens with the first 2 elements stripped out (# and macro type)
         */
		std::vector<preprocessor_token> strip_macro_definition(const std::vector<preprocessor_token> &definition_declaration);

        /**
         * Checks if a given header file is already parsed/preprocessed
         * @param file The name of the header file to check
         * @return true if already parsed false otherwise
         */
		const bool is_header_parsed(const std::string &file)
		{
		    for(unsigned int i=0; i<m_headers.size(); i++)
		    {
		        if(m_headers[i] == file)
		        {
		            return true;
		        }
		    }

		    return false;
		}

		/**
         * Parses a macro definition getting the type (function or identifier), name/identifier,
         * value and parameters if it's a function macro.
         * @param define_declaration vector/array of tokens
         */
		const define parse_define(const std::vector<preprocessor_token> &define_declaration);

        /**
         * Evaluates a macro expression/condition
         * @param define_declaration A macro object
         * @return true if condition is true (duh!) false otherwise
         */
		const bool parse_expression(const std::vector<preprocessor_token> &define_declaration);

		/**
		 * Helper function to add all errors (#error) encountered while preprocessing
		 * @param error_declaration An error macro tokens
		 * @param file The name of the file where the error was found
		 */
		void add_error(const std::vector<preprocessor_token> &error_declaration, const std::string& file);

        /**
         * Converts an expression from a #if, #else, etc to an array of elements with macros expanded
         * @return Vector with tokens that can be used to evalulate the expression by the ConstExprEvaluator class.
         */
		const std::vector<Token> expand_macro_expression(const std::vector<preprocessor_token> &expression);
		//}

		public:

        //{Constructor and Destructor
		preprocessor(){}

		~preprocessor();
		//}

		//{Setters
		/**
		 * To set the path of global includes used when searching for files on #include statements
		 * @param global_includes array/vector of paths
		 */
		void set_global_includes(const std::vector<std::string> &global_includes){ m_global_includes = global_includes; }

        /**
		 * To set the path of local includes used when searching for files on #include statements
		 * @param local_includes array/vector of paths
		 */
		void set_local_includes(const std::vector<std::string> &local_includes){ m_local_includes = local_includes; }

        /**
         * To pass a list of predefined macro definitions to take into account when preprocessing source files
         * @param global_defines array/vector of denifitions
         */
		void set_global_defines(const std::vector<define> &global_defines){ m_global_defines = global_defines; }
		//}

		//{Getters
        /**
         * Gets a macro/definition by searching for it's identifier globally or locally
         * @param definition The identifier or name of the macro
         * @return A define object with the details of the macro
         */
		const define get_define(const std::string &definition);

        /**
         * The whole list of macro/defines stored as global (not found while parsing and manually passed by the user)
         */
		const std::vector<define>& get_global_defines(){ return m_global_defines; }

        /**
         * The whole list of macro/defines stored as local (found while parsing/preprocessing the source)
         */
		const std::vector<define>& get_local_defines(){ return m_local_defines; }

        /**
         * Complete list of header files that where parsed
         */
		const std::vector<std::string>& get_headers(){ return m_headers; }

        /**
         * Checks where a header file was found, globally or local
         * @param file name of header file
         * @return global or local
         */
		const file_scope get_header_scope(const std::string &file){ return m_headers_scope[file]; };

		/**
		 *To get the list of #error encountered while preprocessing
		 */
		const std::vector<preprocessor_error>& get_errors(){ return m_errors; }
		//}

		//{Methods
		/**
		 * Pre-process a c/c++ source file
		 * @param file the name of the file to preprocess
		 * @param scope the scope of the file (global or local) to know which paths to search on
		 * @return The original source file but with macros expanded (preprocessed)
		 */
		const std::string parse_file(const std::string &file = "", file_scope scope = local);

        /**
         * Check if a macro definition is already declared (useful for #ifdef)
         * @param definition The string/identifier of the macro
         */
		const bool is_defined(const std::string &definition);

        /**
         * Removes a macro definition (useful for #undef)
         * @param definition The string/identifier of the macro
         */
		const bool remove_define(const std::string &definition);

        /**
         * Checks where a header file declaration (ex: #include "file.h") resides
         * @param file The name of the header file
         * @param scope To know where to check for the existance of the file
         * @return full path of the header file on the system
         */
		const std::string file_path(const std::string &file, file_scope scope = local);
		//}
	};
};

#endif
