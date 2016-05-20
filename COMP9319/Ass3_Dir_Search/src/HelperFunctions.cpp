#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <cmath>

#include "HelperFunctions.h"

using namespace std;

namespace dirsearch {
	namespace helpers {

		// Parse the command line arguments from the main program
		void ParseCmdLine(int argc, char ** argv, std::vector<std::string>& outArray)
		{
			for (int i = 1; i < argc; i++)
			{
				//cout << argv[i] << endl;
				outArray.push_back(string(argv[i]));
			}
		}
	}

}