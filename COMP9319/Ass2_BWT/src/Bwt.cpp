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
	{
		m_bwtFile.seekg(0, ios::end);
		m_bwtDataSize = static_cast<unsigned int>(m_bwtFile.tellg());
		m_bwtFile.seekg(0, ios::beg);
		
		if (m_bwtDataSize > 1000000)	// (0) if Not implemented yet
		{
			m_readDataFromFiles = true;

			if (DEBUG_MODE)
				cout << "Reading Data From Files" << endl;
		}
		
		if (m_bwtDataSize > 9000000)
		{
			m_useIndexFile = true;
		}
		else if(DEBUG_MODE && (m_bwtDataSize > 2) )
		{
			// TODO remove this debug code
			m_useIndexFile = true;
		}

		if (m_useIndexFile)
		{
			//unsigned int tempVar = static_cast<unsigned int> ( ceil(
			//	static_cast<double>(m_bwtDataSize /
			//	(COUNT_DATA_ARRAY_SIZE * sizeof(unsigned int) ) ) ) );
			unsigned int tempVar = ( (m_bwtDataSize - COUNT_DATA_ARRAY_SIZE) /
					(COUNT_DATA_ARRAY_SIZE * sizeof(unsigned int)));

			m_rankArraySize = tempVar * (COUNT_DATA_ARRAY_SIZE);  // *sizeof(unsigned int));

			assert(m_rankArraySize * sizeof(unsigned int) <= m_bwtDataSize);

			m_indexFile.open(m_indexFilename, ios::in | ios::binary);

			m_indexFile.seekg(0, ios::end);
			m_indexFileSize = static_cast<unsigned int>(m_indexFile.tellg());
			m_indexFile.seekg(0, ios::beg);

			if (m_indexFile.good() && (m_indexFileSize != 0) )
			{
				// If the file already exists, then we can assume it's the correct one and use it
				assert(m_indexFileSize <= m_bwtDataSize); // This is one of the contraints we were given
			}
			else
			{
				// If it is empty, we must create one for the next runs
				m_indexFile.close();
				m_indexFileSize = 0;
				m_createIndexFile = true;
				m_indexFile.open(m_indexFilename, ios::out | ios::binary);
			}
		}
		else 
		{
			// TODO this should be inversely proportional to the file size
			m_rankArraySize = COUNT_DATA_ARRAY_SIZE * (32000U / sizeof(unsigned int) );	//15000

			if (DEBUG_MODE)
			{
				m_rankArraySize = 2 * COUNT_DATA_ARRAY_SIZE;
				cout << "Reading Data from RAM" << endl;
			}

			m_rankData = new unsigned int[m_rankArraySize + 1];

			// init all values to 0 by incrementing the m_rankData pointer
			for (unsigned int i = 0; i < m_rankArraySize; i++)
			{
				assert(i < m_rankArraySize);
				m_rankData[i] = 0;
			}
		}

		double temp1 = static_cast<double>(m_rankArraySize) /
			static_cast<double>(COUNT_DATA_ARRAY_SIZE);
		if (temp1 > 0)
			m_occuranceIntervals = static_cast<unsigned int> (ceil(
				static_cast<double>(m_bwtDataSize) / temp1));

		if (m_occuranceIntervals < 1)
			m_occuranceIntervals = 1;

		if (!m_readDataFromFiles)
		{
			m_bwtLastColumn = new char[m_bwtDataSize + 1];
			m_bwtFile.read(m_bwtLastColumn, m_bwtDataSize);
		}

		if (DEBUG_MODE)
			cout << "File lenght = " << m_bwtDataSize << endl;

	}


	void BWT::Search(const SearchMode searchMode, std::string& searchString)
	{
		m_searchMode = searchMode;
		ConstructCountAndRank();

		m_readBuffer = new char[m_occuranceIntervals + 1];
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
			
			if (m_useIndexFile)
				occuranceCounter = GetRankFromIndexFile(arrayIndex);
			else
				occuranceCounter = m_rankData[arrayIndex];

			startRow = (relativeIndex + 1) * m_occuranceIntervals;
		}
		
		m_bwtFile.seekg(startRow);
		m_bwtFile.read(m_readBuffer, (row - startRow + 1) );
		unsigned int i = 0;

		for (unsigned int rowCounter = startRow; rowCounter <= row; rowCounter++)
		{
			// TODO The "m_bwtLastColumn" is a "fixed" sized unsigned char array and use this
			// for smaller files!
			if (c == m_readBuffer[i++] )
				occuranceCounter++;
		}

		return occuranceCounter;
	}


	unsigned int BWT::GetRankFromIndexFile(const unsigned int arrayIndex)
	{
		// If in file moode, don't use the m_rankData
		unsigned int filePosition = arrayIndex * sizeof(unsigned int);

		m_indexFile.seekg(filePosition);
		m_indexFile.read(m_readBuffer, sizeof(unsigned int) );
		
		return *(reinterpret_cast<unsigned int*>(m_readBuffer) );
	}


	unsigned char BWT::GetBwtCharacter(const unsigned int row)
	{
		assert(row < m_bwtDataSize);
		assert(m_readBuffer != 0);

		// TODO FOr larger files read directly from the file!
		if (m_readDataFromFiles)
		{
			// TODO should do this in blocks!
			m_bwtFile.seekg(row);
			m_bwtFile.read(m_readBuffer, 1);
			return m_readBuffer[0];
		}
		else
		{
			return m_bwtLastColumn[row];
		}
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
		unsigned int startRow = 0;
		// Ideally you want the block size to be a multiple of the "m_occuranceIntervals"
		const unsigned int optimalBlockSize = (1000000 / m_occuranceIntervals) * m_occuranceIntervals;
		const unsigned int blockSize = 
			( m_readDataFromFiles && (m_bwtDataSize > optimalBlockSize) ) ? optimalBlockSize : m_bwtDataSize;
		char* readBlockBuffer;
		
		if (m_readDataFromFiles)
			readBlockBuffer = new char[blockSize + 1];
		else
			readBlockBuffer = m_bwtLastColumn;

		if (m_createIndexFile)
		{
			assert(m_rankData == 0);
			unsigned int bufferSize = (blockSize / m_occuranceIntervals) * COUNT_DATA_ARRAY_SIZE * sizeof(unsigned int);
			m_rankData = new unsigned int[bufferSize];
		}


		while (startRow < m_bwtDataSize)
		{
			unsigned int readSize = m_bwtDataSize;

			if (m_readDataFromFiles)
			{
				m_bwtFile.seekg(startRow);
				readSize = blockSize;
				if ((startRow + readSize) > m_bwtDataSize)
					readSize = m_bwtDataSize - startRow;

				m_bwtFile.read(readBlockBuffer, readSize);
			}


			for (unsigned int charIterator = 0; charIterator < readSize; charIterator++)
			{
				unsigned char thisChar = readBlockBuffer[charIterator];
				assert(thisChar < COUNT_DATA_ARRAY_SIZE); //We were promised to only get ASCII values less than 128

				charCounter[thisChar]++;

				// Should store this data in the index file instead?
				if ( ( (!m_useIndexFile) || (m_createIndexFile)) && (m_occuranceIntervals == ++i) )
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

			startRow += readSize;

			if (m_createIndexFile)
			{
				char* writeBuffer = reinterpret_cast<char*>(m_rankData);
				m_indexFile.write(writeBuffer, rankArrayOffset*sizeof(unsigned int) );
				rankArrayOffset = 0;
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

		if (m_readDataFromFiles)
			delete[] readBlockBuffer;

		if (m_createIndexFile)
		{
			// If a index file was just created, update it's size and make sure it's within the
			// contraints given (i.e. isn't bigger than the input bwt data)
			m_indexFile.close();
			m_indexFile.open(m_indexFilename, ios::in | ios::binary);
			m_indexFile.seekg(0, ios::end);
			m_indexFileSize = static_cast<unsigned int>(m_indexFile.tellg());
			m_indexFile.seekg(0, ios::beg);
			assert(m_indexFileSize <= m_bwtDataSize); // This is one of the contraints we were given
		}
	}



}