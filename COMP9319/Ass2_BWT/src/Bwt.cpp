#include <stdio.h>
#include <iostream>
#include <string>
#include <cassert>
#include <algorithm>
#include <set>

// Internal headers
#include "Bwt.h"
#include "HelperFunctions.h"


static const bool DEBUG_MODE = false;


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

			// TODO what should I do when I can't store the whole column data??
			ConstructCountAndRank(bwtLastColumn);

			BackwardSearch(bwtLastColumn, searchString);
		}
	}


	unsigned int BWT::BackwardSearch(std::string& bwtLastColumn, std::string& searchString,
		const bool findIndex, const std::string::size_type startRow)
	{
		assert(!searchString.empty() );

		if (DEBUG_MODE)
			cout << "Searching for string: " << searchString << endl;


		// Note that the algorithm in the paper used arrays that started at index of 1,
		// but in this code we are using arrays that start at 0, so we need to subtract 1
		// from the psuedo code given in the lecture

		const string::size_type p = searchString.size() - 1;
		string::size_type i = p;
		char c = searchString.at(p);
		string::size_type first = 1;
		string::size_type last = 0;
		string indentifierStringTrace;
		bool startTrace = false;

		if (findIndex)
		{
			//i = startRow;
			c = bwtLastColumn.at(startRow);
			first = startRow;
		}
		else if (m_countData.find(c) != m_countData.end())
		{
			const Letter& letterCount = m_countData[c];
			first = letterCount.m_thisLetterCount;
			last = letterCount.m_nextLetterCount - 1;
		}

		while (findIndex || ( (first <= last) && (i >= 1) ) )
		{
			if (!findIndex)
			{
				c = searchString.at(i - 1);
			}
			else
			{
				c = bwtLastColumn.at(first);
				if (c == '[')  // if (c == 'm')
				{
					std::reverse(indentifierStringTrace.begin(), indentifierStringTrace.end());
					return std::stoi( indentifierStringTrace );
				}
				else if (c == ']')
				{
					startTrace = true;
				}	
				else if (startTrace)
				{
					indentifierStringTrace.push_back(c);
				}
			}

			// Update the first position
			if (first != 0)
			{
				RankMapT firstLetterRank = m_rankData.at(first - 1);
				if (firstLetterRank.find(c) == firstLetterRank.end())
					first = 0;
				else
					first = firstLetterRank[c];
			}

			first += m_countData[c].m_thisLetterCount;

			// Update the last position
			if (!findIndex)
			{
				RankMapT lastLetterRank = m_rankData.at(last);
				if (lastLetterRank.find(c) == lastLetterRank.end())
					last = 0;
				else
					last = lastLetterRank[c];

				last += m_countData[c].m_thisLetterCount - 1;
			}

			i--;
		}

		if ( !findIndex && (m_searchMode != _n) )
		{
			// TODO loop from first to last
			set<unsigned int> identifies;
			for (auto occurance = first; occurance <= last; occurance++)
			{
				identifies.insert( BackwardSearch(bwtLastColumn, searchString,
					true, occurance) );
			}

			if (m_searchMode == _a)
			{
				for (auto id : identifies)
					cout << "[" << id << "]" << endl;
			}
			else
			{
				cout << identifies.size() << endl;;
			}
		}
		else if (m_searchMode == _n)
		{
			cout << (last - first + 1) << endl;
		}

		if (DEBUG_MODE && !findIndex)
		{
			if (last < first )
				cout << "string not found" << endl;
			else
				cout << (last - first + 1) << " occurances have been found" << endl;
		}

		return 0;
	}

	void BWT::ConstructCountAndRank(const std::string& bwtLastColumn)
	{
		// TODO find optimal ways of doing this for larger files
		// This will probably work better for smaller files

		// TODO don't copy bwtLastColumn for larger files
		string reverseBwtLastColumn(bwtLastColumn);

		RankMapT previousRank;
		m_rankData.reserve(reverseBwtLastColumn.size());
		m_countData.reserve(reverseBwtLastColumn.size());
		//string::size_type i = 0;

		for (char thisChar : reverseBwtLastColumn)
		{
			//RankMapT& currentRank = 
			if (previousRank.find(thisChar) == previousRank.end())
				previousRank[thisChar] = 1;
			else
				previousRank[thisChar]++;
			
			// TODO copying rank maps like this will use up a lot of memory!
			RankMapT thisRankMap(previousRank);
			m_rankData.push_back(thisRankMap);
		}

		// TODO Sort before counting and jump might take up too much memory for large files??
		// Ideas: could put the characters in an ordered set (will remove the duplicates)
		sort( reverseBwtLastColumn.begin(), reverseBwtLastColumn.end() );
		if (DEBUG_MODE)
		{
			cout << "sorted last column = " << reverseBwtLastColumn << endl;
		}

		std::string::size_type counter = 0;
		std::string::size_type prevCounter = 0;
		char prevChar = reverseBwtLastColumn.front();
		string::size_type i = 0;

		for (char thisChar : reverseBwtLastColumn)
		{
			if (thisChar == prevChar)
			{
				counter++;
				if (i == reverseBwtLastColumn.size() - 1)
				{
					// Make sure you update the last one in the list
					std::string::size_type nextLetterCount = (prevCounter + counter);
					m_countData[prevChar] = Letter(prevCounter, nextLetterCount);
				}
			}
			else
			{
				std::string::size_type nextLetterCount = (prevCounter + counter);
				m_countData[prevChar] = Letter(prevCounter, nextLetterCount);
				prevCounter = nextLetterCount;
				counter = 1;
			}

			prevChar = thisChar;
			i++;
		}
	}



}