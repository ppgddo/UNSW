#include <stdio.h>
#include <iostream>
#include <string>
#include <list>
#include <cassert>
#include <cmath>

#include "HelperFunctions.h"

using namespace std;

namespace ah {
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

		// Determines if the "-s" delimit option has been selected
		bool SetDelimitMode(int argc, char ** argv)
		{
			std::list<std::string> cmdInputStrings;
			ParseCmdLine(argc, argv, cmdInputStrings);

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
			int remainder;

			if(binaryStrOut.size() < 8)
			{
				binaryStrOut.resize(8);
			}

			for (auto pos = 7; pos >= 0; pos--)
			{
				remainder = charValue % 2;
				charValue = charValue / 2;
				binaryStrOut.at(pos) = binaryDigits[remainder];
			}
		}


		// It converts the ASCII character into its corresponding binary value 
		// as a string of '0' and '1'. It does this simply by dividing the input by the 2
		// which in the default case is 2.
		char ConvertBinaryStringToChar( const std::string& binaryStr )
		{
			char outChar = 0;
			assert(binaryStr.size() == 8);

			int power = 7;
			for (const char& nextChar : binaryStr)
			{
				if (nextChar == '1')
				{
					outChar += pow( 2, power);
				}
				power--;
			}

			return outChar;
		}

	}
}