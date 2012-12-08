#ifndef TEST_HPP
#define TEST_HPP

//Has multiply macro
#include "misc.h"

#ifndef MAX_VALUE
	#define MAX_VALUE 10000
#endif

#ifndef MAX_VALUE
	#define MAX_VALUE ShouldNotOccurre
#endif

#if defined(MAX_VALUE)
	int testy;
#endif

typedef struct
{
	//The content of the struct in another file
	#include "object.h"
	//Should only be included once
	#include "object.h"
} Object;

namespace test
{
    int value = MAX_VALUE;
    int test = multiply;
	
    /**
     * Function one documentation
     * @return true otherwise false
     */
    bool function_one(const char &argument);

    /**
     * Function two documentation
     * @return The amount of characters found
     */
    unsigned int function_two(const char &argument);
};

//We undefine the MAX_VALUE macro
#undef MAX_VALUE

int value = MAX_VALUE;

#endif

