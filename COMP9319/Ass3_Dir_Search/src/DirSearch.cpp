#include <stdio.h>
#include <iostream>
#include <string>
#include <cassert>
#include <algorithm>
#include <set>
#include <locale>

#ifndef  _WIN32
// Linux only headers
#include <sys/types.h>
#include <dirent.h>
#endif // ! _WIN32



// Internal headers
#include "DirSearch.h"
#include "HelperFunctions.h"


static const bool DEBUG_MODE = false;
static const bool ENABLE_ERROR_MSG = true;
static const bool DISABLE_CHAR_COMPRESSION = true;	// Disablabe this is if you want to use the 0-26 char range




using namespace std;
using namespace dirsearch::helpers;


namespace dirsearch {

	std::vector<std::string> DirSearch::ConvertString(const std::vector<std::string>& searchStrings)
	{
		std::vector<std::string> convertedStrings;

		for (auto nextWord = searchStrings.begin();
			nextWord != searchStrings.end(); ++nextWord)
		{
			std::string newWord;

			for (auto nextChar = (*nextWord).begin();
				nextChar != (*nextWord).end(); ++nextChar)
			{
				if (isalpha(*nextChar))
				{
					if (DISABLE_CHAR_COMPRESSION)
					{
						newWord.push_back( std::tolower(*nextChar, m_toLowerLocale) );
					}
					else if (*nextChar < 'Z')
					{
						// If captical letter
						newWord.push_back((*nextChar) - 'A');
					}
					else
					{
						newWord.push_back((*nextChar) - 'a');
					}
				}
				else
				{
					newWord.push_back(*nextChar);
				}
			}

			convertedStrings.push_back(newWord);
		}

		return convertedStrings;
	}



	DirSearch::~DirSearch()
	{
		delete[] m_readBuffer;
		//delete[] m_prefixArray;

		if (DEBUG_MODE)
			cout << "TODO: Delete the pointers in the m_prefixMap!!!" << endl;
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
			//m_prefixArray = new char[PREFIX_ARRAY_DIM * m_prefixMapKeyMaxLength];
		}



#ifndef  _WIN32
		
		DIR *dpdf;
		struct dirent *epdf;
		std::string dirName = "../Test_Data/man/";
		//std::string dirName = "./";

		//dpdf = opendir("./");
		//dpdf = opendir("~cs9319/a3/books200m/");
		dpdf = opendir(dirName.c_str());

		std::list<std::string> filesInDir;
		if (dpdf != NULL) 
		{
			short i = 0;
			while (epdf = readdir(dpdf)) 
			{


				

				if( (string(epdf->d_name) != ".") && (string(epdf->d_name) != ".." ) )
				{
					std::string fullPathString = dirName + string(epdf->d_name);
					filesInDir.push_back(fullPathString);
					//if (DEBUG_MODE)
					std::cout << epdf->d_name << std::endl;
				}
				//const char* const fullpathName = dirName.c_str();
				//this->CreateIndexForFile(fullPathString.c_str(), i++);

			}

			closedir(dpdf);
		}
		else if (DEBUG_MODE)
			std::cout << "Couldn't open direcot: " << dirName << std::endl;
		
		

		short fileArrayIndex = 0;
                //string fileName = "README.txt";
		//auto fileName = filesInDir.begin();
		for (auto fileName = filesInDir.begin();
			fileName != filesInDir.end(); ++fileName)
		{
			
			//this->CreateIndexForFile(fileName.c_str(), fileArrayIndex++);
			this->CreateIndexForFile((*fileName).c_str(), fileArrayIndex++);
		}






#else

		// TODO read every file in the target directory!!!
		// test code
		const char* const fileName =
			//"DirSearch.cpp";
			"D:/Dropbox/TimDocs/NonGit/Training/UNSW/Courses/2016/COMP9319-Web_Compress/Ass/Ass3/Test_Data_Provided/legal1/07_1062.xml";
			//"D:/Dropbox/TimDocs/NonGit/Training/UNSW/Courses/2016/COMP9319-Web_Compress/Ass/Ass3/Test_Data_Provided/legal1/06_1243.xml";
			//"D:/Dropbox/TimDocs/NonGit/Training/UNSW/Courses/2016/COMP9319-Web_Compress/Ass/Ass3/Test_Data_Provided/README.txt";
		short fileArrayIndex = 0;
		if (DEBUG_MODE)
			cout << "TODO: remove hard-coded file for creating index" << fileName << endl;

		this->CreateIndexForFile(fileName, fileArrayIndex);

#endif // ! _WIN32


	}


	void DirSearch::Search(const std::vector<std::string>& searchStrings )
	{
		if (DEBUG_MODE)
		{
			cout << "Searching for terms..." << endl << endl;
		}

		SuffixFileDataListT* listOfTermFileData = nullptr;

		const std::vector<std::string> convertSearchString = ConvertString(searchStrings);


		// TODO remove this hard coded search !
		// test code
		for (auto searchTerm = convertSearchString.begin();
			searchTerm != convertSearchString.end(); ++searchTerm)
		{
			listOfTermFileData = this->SearchAllFiles(*searchTerm);

			if (listOfTermFileData != nullptr)
			{
				if (DEBUG_MODE)
				{
					for (auto fileDataForTerm = listOfTermFileData->begin();
						fileDataForTerm != listOfTermFileData->end(); ++fileDataForTerm)
					{
						cout << "Results for term: \'" << *searchTerm <<
							"\' from file index: " << (*fileDataForTerm)->m_fileIndex <<
							" has word count = " << (*fileDataForTerm)->m_wordCount << endl;
					}
				}
			}
		}


		if (DEBUG_MODE)
		{
			cout << "Finished search" << endl << endl;
		}
	}




	SuffixFileDataListT* DirSearch::SearchAllFiles(const std::string& searchTerm)
	{
		SuffixFileDataListT* listOfTermFileData = nullptr;

		short fileArrayIndex = 0;
		//if (DEBUG_MODE)
		//	cout << "TODO: remove hard-coded file file index for the search: " << fileArrayIndex << 
		//		" and the hardcoded search term: " << endl;


		// TODO test for "spaces" when doing the "phrase search!"


		if (searchTerm.length() <= static_cast<std::size_t>(m_prefixMapKeyMaxLength))
		{
			// if the whole word is small enough to be a key on the map, then
			// no suffix is required
			listOfTermFileData = SearchPreFixMap(searchTerm, "");
		}
		else
		{
			// else the whole word isn't small enough to be a key on the map
			// so need to store it's prefix on map and its suffix on the list
			// of suffixs for that map
			string prefix = searchTerm.substr(0, m_prefixMapKeyMaxLength);
			string suffix = searchTerm.substr(m_prefixMapKeyMaxLength);
			listOfTermFileData = SearchPreFixMap(prefix, suffix);
		}

		return listOfTermFileData;
	}




	enum SearchState
	{
		startSearch,
		traversingNonAlpha,
		traversingPrefixArray,
		traversingOversizedWord,
		readingSuffix
	};


	void DirSearch::CreateIndexForFile(const char* const fileName, short fileArrayIndex)
	{
		//SuffixArrayT m_prefixPath = new char[m_prefixMapKeyMaxLength];
		//SuffixArrayT m_suffixPath = new char[m_maxExpectedWordSize];

		try
		{
			std::ifstream inputFile(fileName);
			inputFile.seekg(0, ios::end);
			unsigned int fileSize = static_cast<unsigned int>(inputFile.tellg());
			inputFile.seekg(0, ios::beg);
			m_fileWordCount = 0;
			
			
			






			// TODO Need to varry this based on size!
			char minNumOfPatterBitsRequired = 2;
			m_initPitPatternList.clear();
			for (char i = 0; i < minNumOfPatterBitsRequired; i++)
			{
				m_initPitPatternList.push_back(0);
			}










			if (fileSize > m_maxFileSize)
			{
				m_maxFileSize = fileSize;
			}

			if (fileSize > m_readBufferSize)
			{
				// Will need to increase the size of the read buffer
				if(m_readBuffer)
					delete[] m_readBuffer;
				m_readBufferSize = fileSize;
				m_readBuffer = new char[m_readBufferSize + 1];
			}

			// Based on what Raymond says in this thread:
			// http://webapps.cse.unsw.edu.au/webcms2/messageboard/viewmessage.php?cid=2440&topicid=6272&threadid=11104
			// I need to read the file in smaller blocks, because a file could be up to 10MB (although not likely)
			if (DEBUG_MODE)
			{
				static bool oneShotFlag = false;
				if (!oneShotFlag)
				{
					cout << "TODO: Read buffer files in smaller blocks!!!" << endl;
					oneShotFlag = true;
				}
			}
			
			inputFile.read(m_readBuffer, fileSize);

			if (!inputFile )
			{
				if (DEBUG_MODE || ENABLE_ERROR_MSG)
					cerr << "Could not open input file: " << fileName << endl;
				return;
			}
			else if (DEBUG_MODE)
			{
				cout << "Successfully Opened file: " << fileName << ", of size: "
					 << fileSize << endl;
			}
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
			SearchState searchMode = startSearch;
			m_tempExistingWordMap.clear();

			for (unsigned int i = 0; i < fileSize; i++)
			{
				// TODO The "m_bwtLastColumn" is a "fixed" sized unsigned char array and use this
				// for smaller files!
				nextChar = m_readBuffer[i];

				if ( m_currentWordLength >(MAX_SEARCH_WORD_SIZE - 1) )
				{
					searchMode = traversingOversizedWord;
				}

				if( (isalpha(nextChar) == 0) )
				{
					// If not an alpha value

					if(searchMode != traversingOversizedWord)
					{
						// Put this in a function and call at end too
						this->InsertWord();
					}

					startNewWord = true;
					arrayIndexBase = 0;
					m_prefixPath.clear();
					m_suffixPath.clear();
					m_currentWordLength = 0;
					searchMode = startSearch;
				}
				// If this is an alpha value and not traversing oversized word
				else if (searchMode != traversingOversizedWord) 
				{
					if (DISABLE_CHAR_COMPRESSION)
					{
						convertedChar = std::tolower(nextChar, m_toLowerLocale);
					}
					else
					{
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
					}

					if (m_currentWordLength < m_prefixMapKeyMaxLength)
					{
						searchMode = traversingPrefixArray;
						m_prefixPath.push_back(convertedChar);
						// Continue "traversing" the prefix array to find the "leaf node" it represents
					}
					else
					{
						searchMode = readingSuffix;
						m_suffixPath.push_back(convertedChar);
					}
				}

				m_currentWordLength++;
			}



			// Tidy up the last word (if it ends with a alpha char)
			if( (searchMode == traversingPrefixArray) || (searchMode == readingSuffix) )
			{
				// Put this in a function and call at end too
				this->InsertWord();
			}


			// Now transfer the "m_tempExistingWordMap" onto the "m_prefixMap"
			for (auto nextWord = m_tempExistingWordMap.begin();
				nextWord != m_tempExistingWordMap.end(); ++nextWord)
			{
				const string& wholeWord = nextWord->first;
				int fileWordCount = nextWord->second;

				if ( wholeWord.length() <= static_cast<std::size_t>( m_prefixMapKeyMaxLength) )
				{
					// if the whole word is small enough to be a key on the map, then
					// no suffix is required
					InsertIntoPrefixMap(wholeWord, "", fileArrayIndex, fileWordCount);
				}
				else
				{
					// else the whole word isn't small enough to be a key on the map
					// so need to store it's prefix on map and its suffix on the list
					// of suffixs for that map
					string prefix = wholeWord.substr(0, m_prefixMapKeyMaxLength);
					string suffix = wholeWord.substr(m_prefixMapKeyMaxLength);
					InsertIntoPrefixMap(prefix, suffix, fileArrayIndex, fileWordCount);
				}
			}










			// Step 1: Construct all the "leaf" nodes first





			// Step 2: After complete "tree" (represented as an array) is constructed, then go through and
			// sort each "suffix" in the leaves lexigraphically, one leaf at a time stored in memory using STL.
			// Also store the "size" (number of suffixes) in each leaf
		}
		catch ( std::exception e)
		{
			assert(0);
			if (DEBUG_MODE || ENABLE_ERROR_MSG)
				cerr << "DirSearch::CreateIndexForFile threw following exception! " << e.what() << endl;
		}
		catch (...)
		{
			assert(0);
			if (DEBUG_MODE || ENABLE_ERROR_MSG)
				cerr << "DirSearch::CreateIndexForFile threw unknown exception! " << endl;
		}

		//delete[] m_suffixPath;
		//delete[] m_prefixPath;
	}


	void DirSearch::InsertIntoPrefixMap(const std::string & prefix, const std::string & suffix,
		const short fileArrayIndex, const int fileWordCount)
	{
		if (m_prefixMap.find(suffix) == m_prefixMap.end())
		{
			Suffix* newSuffix = new Suffix(prefix);
			newSuffix->InsertSuffix(suffix, new SuffixFileData(fileArrayIndex, fileWordCount));
			m_prefixMap.insert(std::pair<std::string, Suffix*>(prefix, newSuffix ) );
		}
		else
		{
			m_prefixMap[suffix]->InsertSuffix(
				suffix, new SuffixFileData(fileArrayIndex, fileWordCount));
		}
	}

	
	SuffixFileDataListT* DirSearch::SearchPreFixMap(const std::string& prefix, const std::string& suffix)
	{
		if (m_prefixMap.find(prefix) == m_prefixMap.end())
		{
			return nullptr;
		}
		else
		{
			return m_prefixMap[prefix]->GetFileData(suffix);
		}
	}




	void DirSearch::InsertWord()
	{
		if ( (m_currentWordLength < MIN_SEARCH_WORD_SIZE) || 
			(m_currentWordLength > (MAX_SEARCH_WORD_SIZE -1) ) )
		{
			// Not searching for words less than 3 char long or more than 256 char long,
			// so dont store these words
			return;
		}

		//else if (m_currentWordLength < m_prefixMapKeyMaxLength)
		//{
			// Only need to insert the "prefix" (i.e. the suffix can be empty)
			// Store a map of currently inserted words and init the bitpattern variables for the new words 
			// for this file
			//PrefixMapT


		// TODO (replace with "m_prefixPath" and suffix version with just one string for the whole word
		const string wholeWord = m_prefixPath + m_suffixPath;
			
		if ( m_tempExistingWordMap.find(wholeWord) == m_tempExistingWordMap.end())
		{
			m_tempExistingWordMap[wholeWord] = 1;// m_initPitPatternList; // TODO for when I use bit pattern
		}
		else
		{
			m_tempExistingWordMap[wholeWord]++;
		}

		//m_currentWordBlock.push_back(wholeWord);

		//}
		//else
		//{
		//	// Need to include the suffix as well
		//}

		// TODO this is how I would set the "bit patterns" for when I implement "Phrase search"
		//if (++m_fileWordCount > m_wordBlockSize)
		//{
		//	int tempWordCount = 0;
		//	// Once the current block has reached it's limit go through the list of 
		//	// words set their "bit patterns"
		//	for ( auto existingSuffixes = m_currentWordBlock.begin();
		//		existingSuffixes != m_currentWordBlock.end(); ++existingSuffixes)
		//	{
		//		// TODO For now don't worry about the order, just make sure the correct
		//		// number of bits are set
		//	}
		//}
	}







}
