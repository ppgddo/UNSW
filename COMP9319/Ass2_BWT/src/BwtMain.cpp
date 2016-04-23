// COMP9319_Ass01.cpp : Defines the entry point for the bwt console application.
//

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>

// Internal headers
#include "HelperFunctions.h"
#include "Bwt.h"


using namespace std;
using namespace bwt;
using namespace bwt::helpers;




int main(int argc, char ** argv)
{
	std::vector<std::string> cmdStrings;
	ParseCmdLine(argc, argv, cmdStrings);

	const std::string& searchCmd = cmdStrings[0];
	SearchMode searchMode = _a;

	if (searchCmd.find("-n") != string::npos)
		searchMode = _n;
	else if (searchCmd.find("-a") != string::npos)
		searchMode = _a;
	else if (searchCmd.find("-r") != string::npos)
		searchMode = _r;

	BWT bwt( cmdStrings[1], cmdStrings[2]);
	bwt.Search(searchMode, cmdStrings[3]);

	return 0;
}

