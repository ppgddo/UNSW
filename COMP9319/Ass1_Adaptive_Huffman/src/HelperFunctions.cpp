#include <stdio.h>
#include <iostream>
#include <string>
#include <list>

#include "HelperFunctions.h"

using namespace std;

namespace ah {
	namespace helpers {


		void ParseCmdLine(int argc, char ** argv, std::list<std::string>& outArray)
		{
			for (int i = 1; i < argc; i++)
			{
				cout << argv[i] << endl;
				outArray.push_back(string(argv[i]));
			}
		}

		bool SetDelimitMode(int argc, char ** argv)
		{
			std::list<std::string> cmdInputStrings;
			ParseCmdLine(argc, argv, cmdInputStrings);

			//const string& firstArg(cmdInputStrings.front());

			if ( !cmdInputStrings.empty() && cmdInputStrings.front().compare(string("-s") ) == 0 )
			{
				return true;
			}
			else
			{
				return false;
			}
		}

	}
}