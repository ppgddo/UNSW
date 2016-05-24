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

static const unsigned int SIZE_OF_UNSIGNED_SHORT = sizeof(unsigned short);
static const unsigned int SIZE_OF_UNSIGNED_INT = sizeof(unsigned int);




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
			cout << "TODO: Delete the pointers in the m_wordMap!!!" << endl;

		cout << "TODO: For the search terms replace double letters with capitals to be consistant!!!!!!!" << endl;
		cout << "Map size = " << m_wordMapSize << endl;
	}


	DirSearch::DirSearch(const std::string indexFilename, const unsigned int indexPercentage
			 )
		: m_indexFilename(indexFilename)
		, m_indexPercentage(indexPercentage)
		, m_dirsearchDataSize(-1)	// TODO where should this be cacluated?
	{
		m_readBuffer = new char[m_readBufferSize + 1];
		
		if (DEBUG_MODE)
			cout << "File lenght = " << m_dirsearchDataSize << endl;

		if (m_dirsearchDataSize	>= 0)	//> 60000000) // If more than 60MB, store everthing in the index file
		{
			// For now I will use index file every time
			m_useIndexFile = true;
		}
		else if(DEBUG_MODE && (m_dirsearchDataSize > 2) )
		{
			// TODO remove this debug code
			m_useIndexFile = true;
		}

		if (m_useIndexFile)
		{
			m_indexFile.open(m_indexFilename, ios::in | ios::binary);

			m_indexFile.seekg(0, ios::end);
			m_indexFileSize = static_cast<unsigned int>(m_indexFile.tellg());
			m_indexFile.seekg(0, ios::beg);

			if (m_indexFile.good() && (m_indexFileSize > 0))
			{
				// If the file already exists, then we can assume it's the correct one and use it
				// This is one of the contraints we were given
				assert(m_indexFileSize <= indexPercentage*m_dirsearchDataSize); 
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
			//m_prefixArray = new char[PREFIX_ARRAY_DIM * m_wordMapKeyMaxLength];
		}



#ifndef  _WIN32
		
		DIR *dpdf;
		struct dirent *epdf;
		//std::string dirName = "/import/kamen/1/cs9319/a3/books200m/";
		std::string dirName = "../Test_Data/books200m/";
		//std::string dirName = "../Test_Data/man/";
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
			//"D:/Dropbox/TimDocs/NonGit/Training/UNSW/Courses/2016/COMP9319-Web_Compress/Ass/Ass3/Test_Data/legal1/07_1062.xml";
			//"D:/Dropbox/TimDocs/NonGit/Training/UNSW/Courses/2016/COMP9319-Web_Compress/Ass/Ass3/Test_Data/legal1/06_1243.xml";
			"D:/Dropbox/TimDocs/NonGit/Training/UNSW/Courses/2016/COMP9319-Web_Compress/Ass/Ass3/Test_Data/README.txt";
		short fileArrayIndex = 0;
		if (DEBUG_MODE)
			cout << "TODO: remove hard-coded file for creating index" << fileName << endl;

		this->CreateIndexForFile(fileName, fileArrayIndex);

#endif // ! _WIN32

		if (m_createIndexFile)
		{
			this->ConstructIndexFile();
			m_indexConstructionState = WordCounting;
		}
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



		// if the whole word is small enough to be a key on the map, then
		// no suffix is required
		listOfTermFileData = SearchWordMap(searchTerm);

		return listOfTermFileData;
	}




	enum SearchState
	{
		startSearch,
		traversingNonAlpha,
		readingAlphaString,
		traversingOversizedWord
	};


	void DirSearch::CreateIndexForFile(const char* const fileName, short fileArrayIndex)
	{

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
			char previousConvertedChar = 0;
			bool compressDoubleLetter = false;
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
					m_wholeWord.clear();
					m_currentWordLength = 0;
					searchMode = startSearch;
					previousConvertedChar = 0;
					compressDoubleLetter = false;
				}
				// If this is an alpha value and not traversing oversized word
				else if (searchMode != traversingOversizedWord) 
				{
					if (DISABLE_CHAR_COMPRESSION)
					{
						convertedChar = std::tolower(nextChar, m_toLowerLocale);
						if (convertedChar == previousConvertedChar)
						{
							// I will encode "double letters as their capital equivalent
							convertedChar = std::toupper(convertedChar, m_toLowerLocale);
							compressDoubleLetter = true;
							previousConvertedChar = 0;
						}
						else
						{
							previousConvertedChar = convertedChar;
						}
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

					searchMode = readingAlphaString;
					if (compressDoubleLetter)
						m_wholeWord.pop_back();

					m_wholeWord.push_back(convertedChar);
				}

				m_currentWordLength++;
				compressDoubleLetter = false;
			}



			// Tidy up the last word (if it ends with a alpha char)
			if (searchMode == readingAlphaString) 
			{
				// Put this in a function and call at end too
				this->InsertWord();
			}


			// Now transfer the "m_tempExistingWordMap" onto the "m_wordMap"
			/*
			for (auto nextWord = m_tempExistingWordMap.begin();
				nextWord != m_tempExistingWordMap.end(); ++nextWord)
			{
				const string& m_wholeWord = nextWord->first;
				int fileWordCount = nextWord->second;

				if ( m_wholeWord.length() <= static_cast<std::size_t>( m_wordMapKeyMaxLength) )
				{
					// if the whole word is small enough to be a key on the map, then
					// no suffix is required
					InsertIntoWordMap(m_wholeWord, nullptr, fileArrayIndex, fileWordCount);
				}
				else
				{
					// else the whole word isn't small enough to be a key on the map
					// so need to store it's wholeWord on map and its suffix on the list
					// of suffixs for that map
					string wholeWord = m_wholeWord.substr(0, m_wordMapKeyMaxLength);
					string* suffix = new string( m_wholeWord.substr(m_wordMapKeyMaxLength));
					InsertIntoWordMap(wholeWord, suffix->c_str(), fileArrayIndex, fileWordCount);
				}
			}
			*/









			// Step 1: Construct all the "leaf" nodes first





			// Step 2: After complete "tree" (represented as an array) is constructed, then go through and
			// sort each "suffix" in the leaves lexigraphically, one leaf at a time stored in memory using STL.
			// Also store the "size" (number of suffixes) in each leaf
		}
		catch ( std::exception e)
		{
			if (DEBUG_MODE || ENABLE_ERROR_MSG)
			{
				cerr << "DirSearch::CreateIndexForFile threw following exception! " << e.what() << endl;
				assert(0);
			}
		}
		catch (...)
		{
			if (DEBUG_MODE || ENABLE_ERROR_MSG)
			{
				cerr << "DirSearch::CreateIndexForFile threw unknown exception! " << endl;
				assert(0);
			}
		}
	}


	void DirSearch::InsertIntoWordMap(const std::string & wholeWord, 
		const short fileArrayIndex, const int fileWordCount)
	{
		if (m_wordMap.find(wholeWord) == m_wordMap.end())
		{
			MapData newTemp;
			newTemp.intVal = 1;
			newTemp.shortVal = 1;
			m_wordMap.insert(std::pair<std::string, MapData>(wholeWord, newTemp) );
			m_wordMapSize++;
		}
		else
		{
			m_wordMap[wholeWord].intVal++;
			m_wordMap[wholeWord].shortVal++;
		}
	}







	void DirSearch::CullWordMap()
	{
		// If the map is getting to big, cull until it gets about 10% smaller

	}






	
	SuffixFileDataListT* DirSearch::SearchWordMap(const std::string& wholeWord)
	{
		if (m_wordMap.find(wholeWord) == m_wordMap.end())
		{
			return nullptr;
		}
		else
		{
			assert(0);	// This isn't implemented yet
			return nullptr;
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

		//else if (m_currentWordLength < m_wordMapKeyMaxLength)
		//{
			// Only need to insert the "wholeWord" (i.e. the suffix can be empty)
			// Store a map of currently inserted words and init the bitpattern variables for the new words 
			// for this file
			//WordMapT
		
		//if (m_indexConstructionState == WordCounting)
		{
			if (m_tempExistingWordMap.find(m_wholeWord) == m_tempExistingWordMap.end())
			{
				InsertIntoWordMap(m_wholeWord, 0, 1);
				m_tempExistingWordMap[m_wholeWord] = true;// m_initPitPatternList; // TODO for when I use bit pattern
			}
			else if (DEBUG_MODE)
			{
				InsertIntoWordMap(m_wholeWord, 0, 1);
			}
		}
		//else
		//{
		//	m_tempExistingWordMap[m_wholeWord]++;
		//}

		//m_currentWordBlock.push_back(m_wholeWord);

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






	void DirSearch::ConstructIndexFile()
	{
		if (m_putMapWordKeysInIndexFile)
		{
			// Store the words from the map keys
			if (m_indexConstructionState == CountNumFilesEachWordIsIn)
			{
				for (auto nextWord = m_wordMap.begin();
					nextWord != m_wordMap.end(); ++nextWord)
				{
					string nextString = (*nextWord).first.c_str();
					m_indexFile.write(nextString.c_str(), nextString.size());
					long pos = m_indexFile.tellp();
					pos += (SIZE_OF_UNSIGNED_SHORT + SIZE_OF_UNSIGNED_INT);
					m_indexFile.seekp(pos);
				}
			}
			else
			{
				// Put in the count of each word for each file in respective place
			}
		}

		// Store the file data for each word


	}



}
