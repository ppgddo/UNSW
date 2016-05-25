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


static const bool DEBUG_MAIN_MODE = false;


int main(int argc, char ** argv)
{
	try
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
		DirSearch dirsearch(cmdStrings[0], indexPercentage);
		dirsearch.Search(searchStrings);

	}
	catch (std::exception e)
	{
		if (DEBUG_MAIN_MODE)
		{
			cerr << "DirSearch::CreateIndexForFile threw following exception! " << e.what() << endl;
			assert(0);
		}
	}
	catch (...)
	{
		if (DEBUG_MAIN_MODE )
		{
			cerr << "DirSearch::CreateIndexForFile threw unknown exception! " << endl;
			assert(0);
		}
	}

	return 0;
}

