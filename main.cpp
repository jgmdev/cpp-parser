#include <string>
#include <vector>
#include <iostream>
#include "misc.hpp"
#include "types.hpp"
#include "preprocessor.hpp"

using namespace std;
using namespace cpp_parser;

int main(int argc, char** argv)
{
    string argument;
    string file = "";
    string action = "";

    vector<string> global_includes;
    vector<string> local_includes;
    vector<define> global_defines;

    local_includes.push_back(argv[0]);

    if(argc > 1)
    {
        for(int i=1; i<argc; i++)
        {
            argument = argv[i];

            if(argument == "-Ig" || argument == "--include_global")
            {
                action = "Ig";
            }
            else if(argument == "-Il" || argument == "--include_local")
            {
                action = "Il";
            }
            else if(argument == "-D" || argument == "--define")
            {
                action = "D";
            }
            else if(argument == "-v" || argument == "--version")
            {
                cout << "cpp_parser " << version();

                return 0;
            }
            else if(argument == "-help" || argument == "--help")
            {
                cout << "Usage: cpp_parser [options] file\n"
                "Options:\n"
                "\t-Ig, --include_global\t\t"
                "Add path to search for header files enclosed in <>, example #include <string>\n"
                "\t-Il, --include_local\t\t"
                "Add path to search for header files enclosed in \"\", example #include \"file.h\"\n"
                "\t-v, --version\t\t"
                "Displays the cpp_parser libary version.\n"
                "\t-help, --help\t\t"
                "Prints out this help\n";

                return 0;
            }
            else
            {
                if(action == "Ig")
                {
                    global_includes.push_back(argument);
                }
                else if(action == "Il")
                {
                    local_includes.push_back(argument);
                }
                else if(action == "D")
                {

                }
                else
                {
                    file = argument;
                }

                action = "";
            }
        }
    }
    else
    {
        cerr << "cpp_parser: You need to specify an input file.\n";
        return 1;
    }

    if(file == "")
    {
        cerr << "cpp_parser: You need to specify an input file.\n";
        return 1;
    }
    else if(!file_exists(file))
    {
        cerr << "cpp_parser: No such file exists.\n";
        return 1;
    }

	cpp_parser::preprocessor parser;

	parser.set_local_includes(local_includes);
	parser.set_global_includes(global_includes);

    cout << parser.parse_file(file);

	/*for(unsigned int i = 0; i<parser.get_local_defines().size(); i++)
	{
	    cout << "File: " << parser.get_local_defines()[i].file << "\n";
	    cout << "Name: " << parser.get_local_defines()[i].name << "\n";
	    cout << "Value: " << parser.get_local_defines()[i].value << "\n";
	    cout << "Line: " << parser.get_local_defines()[i].line << "\n";
	    cout << "Column: " << parser.get_local_defines()[i].column << "\n";

	    for(unsigned int y=0; y<parser.get_local_defines()[i].parameters.size(); y++)
	    {
	        cout << "Parameter " << y << ": " << parser.get_local_defines()[i].parameters[y] << "\n";
	    }

	    cout << "\n";
	}*/

	return 0;
}
