#include <stdio.h>
#include <iostream>
#include <string>
#include <list>

#include "HelperFunctions.h"

using namespace std;

namespace ah {
	namespace helpers {

		// Parse the command line arguments from the main program
		void ParseCmdLine(int argc, char ** argv, std::list<std::string>& outArray)
		{
			for (int i = 1; i < argc; i++)
			{
				cout << argv[i] << endl;
				outArray.push_back(string(argv[i]));
			}
		}

		// Determines if the "-s" delimit option has been selected
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


		// It converts the ASCII character into its corresponding binary value 
		// as a string of '0' and '1'. It does this simply by dividing the input by the 2
		// which in the default case is 2.
		void ConvertCharToBinaryString(std::string& binaryStrOut, char charValue)
		{
			char binaryDigits[3] = "01";
			binaryStrOut.resize(8);
			int remainder;

			for (auto i = 8; i > 0; i--)
			{
				remainder = charValue % 2;
				charValue = charValue / 2;
				binaryStrOut.at(i - 1) = binaryDigits[remainder];
			}
		}

	}
}