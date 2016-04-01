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


		// This was modified from the code from the following website
		// http://www.techworld.com.au/article/527677/how_convert_an_ascii_char_binary_string_representation_c/
		// It converts the ASCII character into its corresponding binary value 
		// as a string of '0' and '1'. It does this simply by dividing the input by the base
		// which in the default case is 2.
		void ConvertBaseVersion(std::string& output, char input, const int base, int digits)
		{
			int remainder;
			char digitsArray[17] = "0123456789ABCDEF";
			output.resize(digits);

			for (int i = digits; i > 0; i--)
			{
				remainder = input % base;
				input = input / base;
				output[i - 1] = digitsArray[remainder];
			}

			//output[digits] = '\0';
		}

	}
}