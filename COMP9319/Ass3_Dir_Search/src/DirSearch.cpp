#include <stdio.h>
#include <iostream>
#include <string>
#include <cassert>
#include <algorithm>
#include <set>

// Internal headers
#include "DirSearch.h"
#include "HelperFunctions.h"


static const bool DEBUG_MODE = false;
static const bool ENABLE_ERROR_MSG = false;




using namespace std;
using namespace dirsearch::helpers;


namespace dirsearch {

	DirSearch::~DirSearch()
	{
		delete[] m_readBuffer;
		delete[] m_prefixArray;
	}

	DirSearch::DirSearch(const std::string indexFilename, const int indexPercentage
			 )
		: m_indexFilename(indexFilename)
		, m_indexPercentage(indexPercentage)
		, m_dirsearchDataSize(0)	// TODO where should this be cacluated?
	{
		m_readBuffer = new char[m_readBufferSize + 1];
		
		if (DEBUG_MODE)
			cout << "File lenght = " << m_dirsearchDataSize << endl;

		if (m_dirsearchDataSize > 60000000)
		{
			// If more than 60MB, store everthing in the index file
			m_useIndexFile = true;
		}
		else if(DEBUG_MODE && (m_dirsearchDataSize > 2) )
		{
			// TODO remove this debug code
			m_useIndexFile = true;
		}

		if (m_useIndexFile)
		{
		}
		else
		{
			m_prefixArray = new char[PREFIX_ARRAY_DIM * m_prefixArrayDimension];
		}
	}


	void DirSearch::Search(const std::vector<std::string>& searchStrings )
	{
	}



	enum SearchState
	{
		startSearch,
		traversingNonAlpha,
		traversingPrefixArray,
		readingSuffix
	};



	void DirSearch::SearchFile(const char* const fileName, short fileArrayIndex)
	{
		//SuffixArrayT prefixPath = new char[m_prefixArrayDimension];
		//SuffixArrayT suffixPath = new char[m_maxExpectedWordSize];
		string prefixPath;
		string suffixPath;

		try
		{
			std::ifstream inputFile(fileName);
			m_indexFile.seekg(0, ios::end);
			unsigned int fileSize = static_cast<unsigned int>(m_indexFile.tellg());
			m_indexFile.seekg(0, ios::beg);

			if (fileSize > m_maxFileSize)
			{
				m_maxFileSize = fileSize;
			}

			if (fileSize > m_readBufferSize)
			{
				// Will need to increase the size of the read buffer
				delete[] m_readBuffer;
				m_readBufferSize += m_readBufferSize;
				m_readBuffer = new char[m_readBufferSize + 1];
			}

			// TODO depending on what Raymond says in this thread:
			// http://webapps.cse.unsw.edu.au/webcms2/messageboard/viewmessage.php?cid=2440&topicid=6272&threadid=11104
			// I may need to read the file in smaller blocks
			inputFile.read(m_readBuffer, fileSize);

			// Get all of the "tokens"
			/*
			char* token = strtok(m_readBuffer, " ");
			while (token)
			{
				printf("token: %s\n", token);
				token = strtok(NULL, " ");
			}
			*/


			char nextChar = 0;
			char convertedChar = 0;
			bool startNewWord = true;
			bool wordInserted = false;
			unsigned int arrayIndexBase = 0;
			unsigned int currentWordPosition = 0;
			SearchState searchMode = startSearch;
			m_tempExistingWordMap.clear();

			for (unsigned int i = 0; i < fileSize; i++)
			{
				// TODO The "m_bwtLastColumn" is a "fixed" sized unsigned char array and use this
				// for smaller files!
				nextChar = m_readBuffer[i++];

				if (isalpha(nextChar) == 0)
				{
					// If not an alpha value

					if (searchMode == traversingPrefixArray)
					{
						// TODO this word is shorter than the m_prefixArrayDimension size
						// put it as the "empty string" value

						// Put this in a function and call at end too
						
						InsertWord(prefixPath, suffixPath, --currentWordPosition);
					}
					else if (searchMode == readingSuffix)
					{
						// TODO reached the end of the string, so enter it's data

						// Put this in a function and call at end too
						InsertWord(prefixPath, suffixPath, --currentWordPosition);
					}

					startNewWord = true;
					arrayIndexBase = 0;
					currentWordPosition = 0;
					prefixPath.clear();
					suffixPath.clear();
				}
				else
				{
					// If this is an alpha value

					if (currentWordPosition > m_maxExpectedWordSize)
					{
						// Need to increase the size of the array
						m_maxExpectedWordSize += m_maxExpectedWordSize;
					}


					// convert char to case insensitive offset value relative to 0 (i.e. value [0-25])
					if (nextChar < 'Z')
					{
						// If captical letter
						convertedChar = nextChar - 'A';
					}
					else
					{
						convertedChar = nextChar - 'a';
					}

					if (currentWordPosition < m_prefixArrayDimension)
					{
						searchMode = traversingPrefixArray;
						prefixPath.push_back(convertedChar);
						// Continue "traversing" the prefix array to find the "leaf node" it represents
					}
					else
					{
						searchMode = readingSuffix;
						suffixPath.push_back(convertedChar);
					}
				}


				currentWordPosition++;
			}

			// Tidy up the last word (if it ends with a alpha char)
			if (searchMode == traversingPrefixArray)
			{
				// TODO this word is shorter than the m_prefixArrayDimension size
				// put it as the "empty string" value
				InsertWord(prefixPath, suffixPath, --currentWordPosition);
			}
			else if (searchMode == readingSuffix)
			{
				// TODO reached the end of the string, so enter it's data
				InsertWord(prefixPath, suffixPath, --currentWordPosition);
			}




			// Step 1: Construct all the "leaf" nodes first





			// Step 2: After complete "tree" (represented as an array) is constructed, then go through and
			// sort each "suffix" in the leaves lexigraphically, one leaf at a time stored in memory using STL.
			// Also store the "size" (number of suffixes) in each leaf
		}
		catch ( std::exception e)
		{
			assert(0);
			if (DEBUG_MODE)
				cerr << "DirSearch::SearchFile threw following exception! " << e.what() << endl;
		}
		catch (...)
		{
			assert(0);
			if (DEBUG_MODE)
				cerr << "DirSearch::SearchFile threw unknown exception! " << endl;
		}

		//delete[] suffixPath;
		//delete[] prefixPath;
	}





	void DirSearch::InsertWord(std::string& prefixPath, std::string& suffixPath, unsigned int currentWordPosition)
	{
		if (currentWordPosition < m_prefixArrayDimension)
		{

			// Store a map of currently inserted words and init the bitpattern variables for the new words 
			// for this file

		}
	}







}