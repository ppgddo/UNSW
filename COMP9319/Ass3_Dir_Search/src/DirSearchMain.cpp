// COMP9319_Ass01.cpp : Defines the entry point for the dirsearch console application.
//

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>

// Internal headers
#include "HelperFunctions.h"
#include "DirSearch.h"


using namespace std;
using namespace dirsearch;
using namespace dirsearch::helpers;




int main(int argc, char ** argv)
{
	std::vector<std::string> cmdStrings;
	ParseCmdLine(argc, argv, cmdStrings);

	const std::string& searchCmd = cmdStrings[1];
	int indexPercentage = 20;	// Default indexPercentage
	std::vector<std::string>::iterator cmdIterator = cmdStrings.begin();
	cmdIterator++;

	if (searchCmd.find("-s") != string::npos)
	{
		indexPercentage = std::stoi(cmdStrings[2]);
		cmdIterator++;
		cmdIterator++;
	}

	std::vector<std::string> searchStrings;
	for (; cmdIterator != cmdStrings.end(); ++cmdIterator)
	{
		searchStrings.push_back(*cmdIterator);
	}
	
	// No need to pass the "
	DirSearch dirsearch( cmdStrings[0], indexPercentage);
	dirsearch.Search(searchStrings);







	return 0;
}

