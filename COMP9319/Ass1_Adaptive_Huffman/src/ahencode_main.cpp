// COMP9319_Ass01.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"

#include <stdio.h>
#include <iostream>
#include <string>

// Internal headers
#include "HelperFunctions.h"


using namespace std;
using namespace ah::helpers;


static const bool DEBUG_MODE = true;




// TODO: Put this into a seperate cpp file!


void EncodeMsg(const string& msg, const bool delimitMode)
{
	if (DEBUG_MODE)
	{
		std::cout << "(Debug Mode ON) ='" << msg << ", delimit mode = " << delimitMode << "'\n";
	}
}





int main(int argc, char ** argv)
{

	bool delimitMode = SetDelimitMode(argc, argv);

	std::string inputString;
	while (cin >> inputString)
	{
		EncodeMsg(inputString, delimitMode);
	}

	return 0;
}

