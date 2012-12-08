#include <cstdio>
#include <string>
#include "version.h"
#include "misc.hpp"

using namespace std;

namespace cpp_parser
{
	bool file_exists(const string &file)
	{
		FILE* fp = fopen(file.c_str(), "r");

		if(!fp)
		{
			return false;
		}

		return true;
	}

	unsigned int count_character(const char &character, string &source)
	{
	    unsigned int count = 0;
	    for(unsigned int i=0; i<source.size(); i++)
	    {
	        if(source[i] == character)
	        {
	            count++;
	        }
	    }

	    return count;
	}

	string version()
	{
	    string version_string;
	    char formatted_version[50];

	    sprintf(formatted_version, "%ld.%ld.%ld%s\n", AutoVersion::MAJOR, AutoVersion::MINOR, AutoVersion::BUILD, AutoVersion::STATUS_SHORT);

	    version_string = formatted_version;

	    version_string += "Copyright (C) 2010 Jefferson González.\n";
        version_string += "License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>.\n";
        version_string += "This is free software: you are free to change and redistribute it.\n";
        version_string += "There is NO WARRANTY, to the extent permitted by law.\n";

	    return version_string;
	}
}
