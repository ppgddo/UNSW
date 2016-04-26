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
static const bool ENABLE_ERROR_MSG = false;




using namespace std;
using namespace bwt::helpers;


namespace bwt {

	BWT::~BWT()
	{
		delete[] m_bwtLastColumn;
		delete[] m_readBuffer;
		delete[] m_rankData;
	}

	BWT::BWT(const std::string bwtInputFilename, 
			 const std::string indexFilename
			 )
		: m_bwtInputFilename(bwtInputFilename)
		, m_indexFilename(indexFilename)
		, m_bwtFile(bwtInputFilename)
		, m_readDataFromFiles(false)
	{
		m_bwtFile.seekg(0, ios::end);
		m_bwtDataSize = static_cast<unsigned int>(m_bwtFile.tellg());
		m_bwtFile.seekg(0, ios::beg);
		m_bwtLastColumn = new char[m_bwtDataSize];
		m_bwtFile.read(m_bwtLastColumn, m_bwtDataSize);


		if(0)	// (m_bwtDataSize > 30000) Not implemented yet
		{
			assert(0); // TODO this hasn't been tested or debugged!!

			m_readDataFromFiles = true;
			m_rankArraySize = (m_bwtDataSize / (COUNT_DATA_ARRAY_SIZE * sizeof(unsigned int)))
				* COUNT_DATA_ARRAY_SIZE;

			if (DEBUG_MODE)
				cout << "Reading Data From Files" << endl;
		}
		else 
		{
			// TODO this should be inversely proportional to the file size
			m_rankArraySize = COUNT_DATA_ARRAY_SIZE * (2000U / sizeof(unsigned int) );	//15000

			if (DEBUG_MODE)
			{
				m_rankArraySize = 2 * COUNT_DATA_ARRAY_SIZE;
				cout << "Reading Data from RAM" << endl;
			}
		}

		// TODO make m_occuranceIntervals a member variable
		double temp1 = static_cast<double>(m_rankArraySize) / 
			static_cast<double>(COUNT_DATA_ARRAY_SIZE);
		if (temp1 > 0)
			m_occuranceIntervals = static_cast<unsigned int> ( ceil (
				static_cast<double>(m_bwtDataSize) / temp1 ) );

		if (m_occuranceIntervals < 1)
			m_occuranceIntervals = 1;

		m_readBuffer = new char[m_occuranceIntervals + 1];
		m_rankData = new unsigned int[m_rankArraySize];


		// init all values to 0 by incrementing the m_rankData pointer
		for (unsigned int i = 0; i < m_rankArraySize; i++)
		{
			assert(i < m_rankArraySize);
			m_rankData[i] = 0;
		}


		if (DEBUG_MODE)
			cout << "File lenght = " << m_bwtDataSize << endl;

	}


	void BWT::Search(const SearchMode searchMode, std::string& searchString)
	{
		m_searchMode = searchMode;

		if (DEBUG_MODE)
			cout << "last bwt column = " << *m_bwtLastColumn << endl;

		// TODO what should I do when I can't store the whole column data??
		ConstructCountAndRank();

		BackwardSearch(searchString);
	}


	unsigned int BWT::BackwardSearch(std::string& searchString,
		const bool findIndex, const unsigned int startRow)
	{
		assert(!searchString.empty() );
		assert(m_bwtDataSize != 0);
		
		if (DEBUG_MODE)
			cout << "Searching for string: " << searchString << endl;


		// Note that the algorithm in the paper used arrays that started at index of 1,
		// but in this code we are using arrays that start at 0, so we need to subtract 1
		// from the psuedo code given in the lecture

		const unsigned int p = searchString.size() - 1;
		unsigned int i = p;
		unsigned char c = searchString.at(p);
		unsigned int first = 1;
		unsigned int last = 0;
		string indentifierStringTrace;
		bool startTrace = false;

		if (findIndex)
		{
			//i = startRow;
			c = GetBwtCharacter(startRow);
			first = startRow;
		}
		else //if (m_countOfThisChar.find(c) != m_countOfThisChar.end())
		{
			//const Letter& letterCount = m_countOfThisChar[c];
			first = m_countOfThisChar[c];
			last = m_countOfNextChar[c];	// Only decrement if not zero
			if(last != 0)
				last--;
		}

		while (findIndex || ( (first <= last) && (i >= 1) ) )
		{
			if (!findIndex)
			{
				c = searchString.at(i - 1);
			}
			else
			{
				c = GetBwtCharacter(first);
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
				assert(first != 0);
				first = RankOcc(c, (first - 1));
			}

			first += m_countOfThisChar[c];

			// Update the last position
			if (!findIndex)
			{
				assert(last != (static_cast<unsigned int> (-1)) );
				last = RankOcc(c, last);
				last += m_countOfThisChar[c] - 1;
			}

			i--;
		}

		if ( !findIndex && (m_searchMode != _n) )
		{
			// TODO loop from first to last
			set<unsigned int> identifies;
			for (auto occurance = first; occurance <= last; occurance++)
			{
				identifies.insert( BackwardSearch( searchString,
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


	unsigned int BWT::RankOcc(const unsigned char c, const unsigned int row)
	{
		assert(row <= m_bwtDataSize);

		if (ENABLE_ERROR_MSG && (row > m_bwtDataSize) )
		{
			cout << "Row = " << row << " and is bigger than the data size of " 
				<< m_bwtDataSize << endl;
			throw std::runtime_error("Read above error message!");
		}

		unsigned int startRow = 0;
		unsigned int occuranceCounter = 0;

		assert(m_occuranceIntervals > 0);
		if (row >= m_occuranceIntervals)
		{
			unsigned int relativeIndex = (row / m_occuranceIntervals) - 1;
			unsigned int arrayIndex = (relativeIndex*COUNT_DATA_ARRAY_SIZE) + c;
			assert(arrayIndex < m_rankArraySize);
			occuranceCounter = m_rankData[arrayIndex];

			startRow = (relativeIndex + 1) * m_occuranceIntervals;
		}
		
		m_bwtFile.seekg(startRow);
		m_bwtFile.read(m_readBuffer, (row - startRow + 1) );
		unsigned int i = 0;

		for (unsigned int rowCounter = startRow; rowCounter <= row; rowCounter++)
		{
			// The "m_bwtLastColumn" is a "fixed" sized unsigned char array and use this
			// for smaller files!
			if (c == m_readBuffer[i++] )
				occuranceCounter++;
		}

		return occuranceCounter;
	}


	unsigned char BWT::GetBwtCharacter(const unsigned int row)
	{
		// TODO FOr larger files read directly from the file!
		return m_bwtLastColumn[row];
	}


	void BWT::ConstructCountAndRank()
	{
		assert( (m_readDataFromFiles) || (m_bwtDataSize != 0) );

		// TODO find optimal ways of doing this for larger files
		// This will probably work better for smaller files

		// Calculate the Rank (aka Occurances) of each character for each row

		// Construct an index file that takes "snapshots" of the Rank at specified intervals
		// (i.e. one of the lecturers suggestions)

		CountT charCounter = { 0 };
		unsigned int rankArrayOffset = 0;

		unsigned int i = 0;


		//m_bwtFile.seekg(startRow);
		//m_bwtFile.read(readBlockBuffer, (row - startRow + 1));


		for (unsigned int charIterator = 0; charIterator < m_bwtDataSize; charIterator++)
		{
			unsigned char thisChar = m_bwtLastColumn[charIterator];
			assert(thisChar < COUNT_DATA_ARRAY_SIZE); //We were promised to only get ASCII values less than 128
			
			charCounter[thisChar]++;

			
			// TODO copying rank maps like this will use up a lot of memory!
			// Should store this data in the index file instead?
			if (m_occuranceIntervals == ++i)
			{
				
				for (unsigned int it = 0; it < COUNT_DATA_ARRAY_SIZE; it++)
				{
					unsigned int thisArrayIndex = rankArrayOffset + it;
					assert(thisArrayIndex < m_rankArraySize);
					m_rankData[thisArrayIndex] = charCounter[it];
				}

				rankArrayOffset += COUNT_DATA_ARRAY_SIZE;
				i = 0;
			}
		}

		// DO a second pass to fill in the "count data" (i.e. the number of characters
		// that come before it
		unsigned int runningTotal = 0;
		unsigned int currentValue = 0;
		for (unsigned int charIterator = 0; charIterator < COUNT_DATA_ARRAY_SIZE; charIterator++)
		{
			currentValue = charCounter[charIterator];
			if (currentValue == 0)
				continue;

			m_countOfThisChar[charIterator] = runningTotal;
			runningTotal += currentValue;
			m_countOfNextChar[charIterator] = runningTotal;
		}
	}



}