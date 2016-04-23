#include <stdio.h>
#include <iostream>
#include <string>
#include <cassert>
#include <algorithm>

// Internal headers
#include "Bwt.h"
#include "HelperFunctions.h"


static const bool DEBUG_MODE = true;


using namespace std;
using namespace bwt::helpers;


namespace bwt {


	BWT::BWT(const std::string bwtInputFilename, 
			 const std::string indexFilename
			 )
		: m_bwtInputFilename(bwtInputFilename)
		, m_indexFilename(indexFilename)
		, m_bwtFile(bwtInputFilename)
	{
	
	}


	void BWT::Search(const SearchMode searchMode, std::string& searchString)
	{
		m_searchMode = searchMode;

		std::string bwtLastColumn;
		while (std::getline(m_bwtFile, bwtLastColumn))
		{
			if (DEBUG_MODE)
				cout << "last bwt column = " << bwtLastColumn << endl;

			BackwardSearch(bwtLastColumn, searchString);
		}
	}


	void BWT::BackwardSearch(std::string& bwtLastColumn, std::string& searchString)
	{
		assert(!searchString.empty() );

		if (DEBUG_MODE)
			cout << "Searching for string: " << searchString << endl;

		CountT countData;
		RankT rankData;
		ConstructCountAndRank(bwtLastColumn, countData, rankData);

		// Note that the algorithm in the paper used arrays that started at index of 1,
		// but in this code we are using arrays that start at 0, so we need to subtract 1
		// from the psuedo code given in the lecture

		// need to reverse the search string for backward search
		reverse(searchString.begin(), searchString.end());

		const string::size_type p = searchString.size() - 1;
		string::size_type i = p;
		char c = searchString.at(p);
		string::size_type first = 1;
		string::size_type last = 0;

		if (countData.find(c) != countData.end())
		{
			const Letter& letterCount = countData[c];
			first = letterCount.m_thisLetterCount;
			last = letterCount.m_nextLetterCount - 1;
		}

		while ((first <= last) && (i >= 1))
		{
			c = searchString.at(i - 1);
			// Update the first position
			if (first != 0)
			{
				RankMapT firstLetterRank = rankData.at(first - 1);
				if (firstLetterRank.find(c) == firstLetterRank.end())
					first = 0;
				else
					first = firstLetterRank[c];
			}

			first += countData[c].m_thisLetterCount;

			// Update the last position
			RankMapT lastLetterRank = rankData.at(last);
			if (lastLetterRank.find(c) == lastLetterRank.end())
				last = 0;
			else
				last = lastLetterRank[c];

			last += countData[c].m_thisLetterCount - 1;

			i--;
		}

		if (DEBUG_MODE)
		{
			if (last < first )
				cout << "string not found" << endl;
			else
				cout << (last - first + 1) << " occurances have been found" << endl;
		}
	}


	void BWT::ConstructCountAndRank(std::string& bwtLastColumn, 
								    CountT& countData, RankT& rankData)
	{
		// TODO find optimal ways of doing this for larger files
		// This will probably work better for smaller files

		RankMapT previousRank;
		rankData.reserve(bwtLastColumn.size());
		countData.reserve(bwtLastColumn.size());
		//string::size_type i = 0;

		for (char thisChar : bwtLastColumn)
		{
			//RankMapT& currentRank = 
			if (previousRank.find(thisChar) == previousRank.end())
				previousRank[thisChar] = 1;
			else
				previousRank[thisChar]++;
			
			// TODO copying rank maps like this will use up a lot of memory!
			RankMapT thisRankMap(previousRank);
			rankData.push_back(thisRankMap);
		}

		// TODO Sort before counting and jump might take up too much memory for large files??
		// Ideas: could put the characters in an ordered set (will remove the duplicates)
		sort( bwtLastColumn.begin(), bwtLastColumn.end() );
		if (DEBUG_MODE)
		{
			cout << "sorted last column = " << bwtLastColumn << endl;
		}

		unsigned int counter = 0;
		unsigned int prevCounter = 0;
		char prevChar = bwtLastColumn.front();
		string::size_type i = 0;

		for (char thisChar : bwtLastColumn)
		{
			if (thisChar == prevChar)
			{
				counter++;
				if (i == bwtLastColumn.size() - 1)
				{
					// Make sure you update the last one in the list
					unsigned int nextLetterCount = (prevCounter + counter);
					countData[prevChar] = Letter(prevCounter, nextLetterCount);
				}
			}
			else
			{
				unsigned int nextLetterCount = (prevCounter + counter);
				countData[prevChar] = Letter(prevCounter, nextLetterCount);
				prevCounter = nextLetterCount;
				counter = 1;
			}

			prevChar = thisChar;
			i++;
		}
	}



}