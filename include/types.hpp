#ifndef TYPES_HPP
#define TYPES_HPP

#include <string>
#include <vector>

namespace cpp_parser
{
    //{Enumerations
    /**
     * To classify the scope of a header file
     */
	enum file_scope
	{
		global,     /*< for header files found like #include <header.h> */
		local       /*< for header files found like #include "header.h" */
	};

    /**
     * To classify a #define type as function or declaration/identifier
     */
	enum define_type
	{
		declaration,    /*< for macros like #define DEBUG 1 */
		function,       /*< for macros like #define add(x,y) x*y */
	};

    /**
     * To classify the type of a token.
     * Many of the types here maybe are not neccessary just for preprocessing.
     */
	enum token_type
	{
	    operator_symbol,        // +-/*% etc...
	    parenthesis_begin,      // (
	    parethesis_end,         // )
	    body_begin,             // {
	    body_end,               // }
	    scope_resolution,       // ::
	    identifier,             // Any variable, function, object ect
	    keyword,                // Predifined keyword defined by c/c++ itself
	    member_access_pointer,  // ->
	    member_access_object,   // .
	    array_begin,            // [
	    array_end,              // ]
	    strings,                // "" or ''
	    comment,                // //
	    multi_comment,          // /* */
	    number,                 // 0-9
	    new_line,               // \n
	    other                   // Anything else
	};
	//}

    //{Data structures
    /**
     * To hold data of include files found on #include statements
     */
	struct includes
	{
		file_scope scope;       /*< Scope of the include file */
		std::string path;       /*< full path of the include file */
		std::string file_name;  /*< name of the include file */
	};

    /**
     * To hold data of a #define statement
     */
	struct define
	{
	    std::string file;                       /*< Name of the header file where #define was found */
		define_type type;                       /*< Type of define */
		std::string name;                       /*< Name of macro */
		std::string value;                      /*< Value of the definition */
		unsigned int line;                      /*< Line position where the definition was found in the file */
		unsigned int column;                    /*< Column position where the definition starts */
		std::vector<std::string> parameters;    /*< Definition parameters in case of macro function */
	};

	/**
     * To hold data about a token
     */
    struct preprocessor_token
    {
        unsigned int column;    /*< column position */
        unsigned int line;      /*< line position */
        std::string token;      /*< token string */
        token_type type;        /*< type of token */
    };
	//}
};

#endif
