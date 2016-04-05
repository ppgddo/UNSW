// COMP9319_Ass01.cpp : Defines the entry point for the ahencode console application.
//

//#include "stdafx.h"

#include <stdio.h>
#include <iostream>
#include <string>

// Internal headers
#include "HelperFunctions.h"
#include "AhEncodeFunctions.h"


using namespace std;
using namespace ah::helpers;
using namespace ah::encode;




int main(int argc, char ** argv)
{

	bool delimitMode = SetDelimitMode(argc, argv);

	std::string inputString;
	while (getline(cin, inputString) )
	{
		EncodeOrDecode encoder(inputString, delimitMode, false);
	}

	return 0;
}

