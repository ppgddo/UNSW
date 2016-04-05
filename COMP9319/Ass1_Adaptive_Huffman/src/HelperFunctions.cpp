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


		// Determines if the "-d" delimit option has been selected
		bool SetDebugMode(int argc, char ** argv)
		{
			std::list<std::string> cmdInputStrings;
			ParseCmdLine(argc, argv, cmdInputStrings);

			if (!cmdInputStrings.empty() && cmdInputStrings.front().compare(string("-d")) == 0)
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
		//
		// The -std=c++11 supports utf8 for the string of chars, but large values
		// are negative, which wasn't handled by the "ConvertCharToBinaryString()"
		// or "ConvertBinaryStringToChar()" functions.As a work around I convert
		// the char to an "unsigned char" for converting it into the string
		// representation of the binary value. This is done with a static cast.

		void ConvertCharToBinaryString(std::string& binaryStrOut, char inChar)
		{
			unsigned char unsignedChar = static_cast<unsigned char>(inChar);
			char binaryDigits[3] = "01";
			unsigned int remainder;

			if(binaryStrOut.size() < 8)
			{
				binaryStrOut.resize(8);
			}

			for (auto pos = 7; pos >= 0; pos--)
			{
				remainder = unsignedChar % 2;
				unsignedChar = unsignedChar / 2;
				binaryStrOut.at(pos) = binaryDigits[remainder];
			}
		}


		// It converts the ASCII character into its corresponding binary value 
		// as a string of '0' and '1'. It does this simply by dividing the input by the 2
		// which in the default case is 2.
		//
		// As mentioned in the "ConvertCharToBinaryString()" function comment above,
		// the binary representation is of an unsigned char, so I need to cast it to
		// a char for the cout to display it as a utf8 character properly.

		char ConvertBinaryStringToChar( const std::string& binaryStr )
		{
			unsigned char unsignedChar = 0;
			assert(binaryStr.size() == 8);

			int power = 7;
			for (const char& nextChar : binaryStr)
			{
				if (nextChar == '1')
				{
					unsignedChar += pow( 2, power);
				}
				power--;
			}

			return static_cast<char>(unsignedChar);
		}

	}
}