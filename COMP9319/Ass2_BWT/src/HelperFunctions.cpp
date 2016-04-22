#include <stdio.h>
#include <iostream>
#include <string>
#include <list>
#include <cassert>
#include <cmath>

#include "HelperFunctions.h"

using namespace std;

namespace bwt {
	namespace helpers {

		// Parse the command line arguments from the main program
		void ParseCmdLine(int argc, char ** argv, std::list<std::string>& outArray)
		{
			for (int i = 1; i < argc; i++)
			{
				//cout << argv[i] << endl;
				outArray.push_back(string(argv[i]));
			}
		}
	}

}