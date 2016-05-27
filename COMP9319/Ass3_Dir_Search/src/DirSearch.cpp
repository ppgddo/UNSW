//#define _CRT_SECURE_NO_WARNINGS

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
static const bool DISPLAY_TODO = true;

static const unsigned int SIZE_OF_UNSIGNED_SHORT = sizeof(unsigned short);
static const unsigned int SIZE_OF_UNSIGNED_INT = sizeof(unsigned int);
static const unsigned int SIZE_OF_FILE_DATA = (SIZE_OF_UNSIGNED_SHORT + SIZE_OF_UNSIGNED_INT);



using namespace std;
using namespace dirsearch::helpers;


namespace dirsearch {

	std::vector<std::string> DirSearch::ConvertString(const std::vector<std::string>& searchStrings)
	{
		std::vector<std::string> convertedStrings;
		char previousConvertedChar = 0;
		bool compressDoubleLetter = false;

		for (auto nextWord = searchStrings.begin();
			nextWord != searchStrings.end(); ++nextWord)
		{
			std::string newWord;

			previousConvertedChar = 0;
			compressDoubleLetter = false;

			for (auto nextChar = (*nextWord).begin();
				nextChar != (*nextWord).end(); ++nextChar)
			{
				if (isalpha(*nextChar))
				{
					char convertedChar = std::tolower(*nextChar, m_toLowerLocale);
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

					if (compressDoubleLetter)
						newWord.pop_back();

					compressDoubleLetter = false;
					newWord.push_back(convertedChar);
				}
				else
				{
					newWord.push_back(*nextChar);
					previousConvertedChar = 0;
					compressDoubleLetter = false;
				}
			}

			convertedStrings.push_back(newWord);
		}

		return convertedStrings;
	}



	DirSearch::~DirSearch()
	{
		if (m_readBuffer)
		{
			delete[] m_readBuffer;
			m_readBuffer = nullptr;
		}

		//delete[] m_prefixArray;

		if (DISPLAY_TODO)
		{
			cout << "TODO: Delete the pointers in the m_wordMap!!!" << endl;
			cout << "TODO: For the search terms replace double letters with capitals to be consistant!!!!!!!" << endl;
			cout << "Map size = " << m_wordMapSize << endl;
		}
	}


	DirSearch::DirSearch(const std::string indexFilename, const unsigned int indexPercentage
		)
		: m_indexFilename(indexFilename)
		, m_indexPercentage(indexPercentage)
		, m_dirsearchDataSize(-1)	// TODO where should this be cacluated?
	{
		if (DEBUG_MODE)
			cout << "File lenght = " << m_dirsearchDataSize << endl;

		if (m_dirsearchDataSize >= 0)	//> 60000000) // If more than 60MB, store everthing in the index file
		{
			// For now I will use index file every time
			m_useIndexFile = true;
		}

		if (m_useIndexFile)
		{
			m_indexFile.open(m_indexFilename, ios::in | ios::binary);

			m_indexFile.seekg(0, ios::end);
			m_indexFileSize = static_cast<unsigned int>(m_indexFile.tellg());
			m_indexFile.seekg(0, ios::beg);

			if (!DEBUG_MODE && m_indexFile && m_indexFile.good() && (m_indexFileSize > 0) )
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
				m_indexFile.seekp(0);
			}
		}
		else
		{
			//m_prefixArray = new char[PREFIX_ARRAY_DIM * m_wordMapKeyMaxLength];
		}

		if (m_createIndexFile)
		{
			// Read files for the first pass
			m_indexConstructionState = CountNumFilesEachWordIsIn;
			this->ReadAllFiles();
			this->ConstructIndexFile(0);
			
			// Change the state and then Read files for the second pass to fill
			// in the remaining data.
			m_indexConstructionState = WordCounting;
			// Note that the "ReadAllFiles" function will call the "ConstructIndexFile"
			// for each file it reads to write all of the corresponding "file data".
			this->ReadAllFiles();
		}

	}

	void DirSearch::ReadAllFiles()
	{
		std::list<std::string> filesInDir;


#ifndef  _WIN32
		
		DIR *dpdf;
		struct dirent *epdf;
		//std::string dirName = "/import/kamen/1/cs9319/a3/books200m/";
		std::string dirName = "/import/kamen/1/cs9319/a3/legal1/";
		//std::string dirName = "/import/kamen/1/cs9319/a3/man/";
		//std::string dirName = "../Test_Data/books200m/";
		//std::string dirName = "../Test_Data/legal1/";
		//std::string dirName = "../Test_Data/man/";
		//std::string dirName = "./";

		//dpdf = opendir("./");
		//dpdf = opendir("~cs9319/a3/books200m/");
		dpdf = opendir(dirName.c_str());

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
		
		


#else	// If win32

		// TODO read every file in the target directory!!!
		// test code
		string fileName1 =
			//"DirSearch.cpp";
			//"D:/Dropbox/TimDocs/NonGit/Training/UNSW/Courses/2016/COMP9319-Web_Compress/Ass/Ass3/Test_Data/legal1/07_1062.xml";
			//"D:/Dropbox/TimDocs/NonGit/Training/UNSW/Courses/2016/COMP9319-Web_Compress/Ass/Ass3/Test_Data/legal1/06_1243.xml";
			"D:/Dropbox/TimDocs/NonGit/Training/UNSW/Courses/2016/COMP9319-Web_Compress/Ass/Ass3/Test_Data/README.txt";
		//short fileArrayIndex = 0;
		string fileName2 =
			//"DirSearch.cpp";
			"D:/Dropbox/TimDocs/NonGit/Training/UNSW/Courses/2016/COMP9319-Web_Compress/Ass/Ass3/Test_Data/man/basename.1.txt";

		if (DISPLAY_TODO)
			cout << "Note that windows is using remove hard-coded files for testing..." << endl;


		filesInDir.push_back(fileName1);
		//filesInDir.push_back(fileName2);

		//this->CreateIndexForFile(fileName, fileArrayIndex);

#endif // ! _WIN32

		short fileArrayIndex = 0;
		//string fileName = "README.txt";
		//auto fileName = filesInDir.begin();
		for (auto fileName = filesInDir.begin();
		fileName != filesInDir.end(); ++fileName)
		{
			this->CreateIndexForFile((*fileName).c_str(), fileArrayIndex++);
		}

	}


	void DirSearch::Search(const std::vector<std::string>& searchStrings )
	{
		if (DISPLAY_TODO)
		{
			cout << "TODO: need to check for spaces in the searchStrings for phrase search" << endl;
			cout << "TODO: Read the index file in smaller blocks!" << endl;
		}

		if (DEBUG_MODE)
		{
			cout << "Searching for terms..." << endl << endl;
		}

		const std::vector<std::string> convertSearchString = ConvertString(searchStrings);

		if (!m_createIndexFile)
		{
			// Only need to load the index file if it wasn't just created
			// by this instance of the app

			m_indexFile.seekg(0, ios::end);
			unsigned int fileSize = static_cast<unsigned int>(m_indexFile.tellg());
			m_indexFile.seekg(0, ios::beg);
			m_fileWordCount = 0;

			if (m_readBuffer)
			{
				delete[] m_readBuffer;
				m_readBuffer = nullptr;
			}

			m_readBuffer = new char[fileSize + 1];
			m_indexFile.read(m_readBuffer, fileSize);

			if (DISPLAY_TODO)
			{
				static bool oneShotFlag = false;
				if (!oneShotFlag)
				{
					cout << "TODO: Read buffer files in smaller blocks!!!" << endl;
					oneShotFlag = true;
				}
			}

			if (!m_indexFile)
			{
				if (DEBUG_MODE || ENABLE_ERROR_MSG)
					cerr << "Could not open input file: " << m_indexFilename << endl;
				return;
			}
			else if (DEBUG_MODE)
			{
				cout << "Successfully Opened file: " << m_indexFilename << ", of size: "
					<< fileSize << endl;
			}




			const char* tempPointer = m_readBuffer;
			bool readAllKeyValues = false;
			while (!readAllKeyValues)
			{
				// Note, if I find a word that is a "substring" of another word,
				// then I know I'm doing the "substring" search

				if (GetNextMapStringFromIndexFile(tempPointer))
				{
					readAllKeyValues = true;
					break;
				}

				//tempPointer += m_nextMapStringFromIndexFile.size();

				for (auto thisWord = convertSearchString.begin();
				thisWord != convertSearchString.end(); ++thisWord)
				{
					if (m_nextMapStringFromIndexFile.find(*thisWord) != std::string::npos)
					{
						// Then search for this term
						const unsigned int startPosition = *(reinterpret_cast<const unsigned int*>(tempPointer));
						const unsigned short numberOfFileData = *(reinterpret_cast<const unsigned short*>(
							tempPointer + sizeof(unsigned int) ) );
					}
				}

				tempPointer += SIZE_OF_FILE_DATA;
			}

		}
		else
		{
			if (DISPLAY_TODO)
			{
				cout << "When index file is created in this instance of app, " <<
					"I just need to read off the map!" << endl;
			}
		}



		// Search stage 1: 
		// If no spaces in the search term, search for key words
		// as "complete" keys of the "m_wordMap"

		// If there is a space in a search term, search for "phrases"
		// as "complete" keys, then search the common file (i.e. the 
		// files that have all words of phrase) and search for the phrase

		// Search stage 2 (substring search): 
		// If one of the search terms from step 1 don't produce any results
		// then repeat it but seach every word in the "m_wordMap" and then
		// search each word that it is a substring of

		// Search stage 3 
		// If still no results, then search in every input file

		SuffixFileDataListT* listOfTermFileData = nullptr;


		/*
		// TODO remove this hard coded search ?
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
		*/

		if (DEBUG_MODE)
		{
			cout << "Finished search" << endl << endl;
		}

		if (m_readBuffer)
		{
			delete[] m_readBuffer;
			m_readBuffer = nullptr;
		}
	}


	bool DirSearch::GetNextMapStringFromIndexFile(const char* & tempPointer)
	{
		// returns true when the "#" is found, indicating the end of map strings 
		// in the index file
		m_nextMapStringFromIndexFile.clear();
		const char* const startPointer = tempPointer;

		bool endOfMapStrings = false;
		unsigned int stringSize = 0;
		char thisChar = 0;
		while (true)
		{
			thisChar = *(tempPointer);
			tempPointer++;
			if (thisChar == ',')
				break;
			else if (thisChar == '#')
			{
				endOfMapStrings = true;
				break;
			}
			stringSize++;
		}

		m_nextMapStringFromIndexFile = string(startPointer, stringSize);

		return endOfMapStrings;
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


			if (fileSize > m_maxFileSize)
			{
				m_maxFileSize = fileSize;
			}

			if (fileSize > m_readBufferSize)
			{
				// Will need to increase the size of the read buffer
				if (m_readBuffer)
				{
					delete[] m_readBuffer;
					m_readBuffer = nullptr;
				}
				m_readBufferSize = fileSize;
				m_readBuffer = new char[m_readBufferSize + 1];
			}
			else if (m_readBuffer == nullptr)
			{
				m_readBuffer = new char[m_readBufferSize + 1];
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
			m_currentWordLength = 0;
			m_wholeWord.clear();


			// TODO to read in smaller chunks, I just need to put the code below
			// in a for loop for each chunck, so I won't need to change the logic
			// (i.e. the code below won't notice the difference).
			inputFile.read(m_readBuffer, fileSize);



			if (DISPLAY_TODO)
			{
				static bool oneShotFlag = false;
				if (!oneShotFlag)
				{
					cout << "TODO: Read buffer files in smaller blocks!!!" << endl;
					oneShotFlag = true;
				}
			}

			if (!inputFile)
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



			for (unsigned int i = 0; i < fileSize; i++)
			{
				// TODO The "m_bwtLastColumn" is a "fixed" sized unsigned char array and use this
				// for smaller files!
				nextChar = m_readBuffer[i];

				if ( m_currentWordLength >(MAX_SEARCH_WORD_SIZE) )
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

					searchMode = readingAlphaString;
					if (compressDoubleLetter)
						m_wholeWord.pop_back();

					compressDoubleLetter = false;
					m_wholeWord.push_back(convertedChar);
					m_currentWordLength++;
				}
				
				compressDoubleLetter = false;
			}







			// IMPORTANT!!!!
			// When I implement the "block reading" mode, the code below should be 
			// executed after all blocks from the file is read!



			// Tidy up the last word (if it ends with a alpha char)
			if (searchMode == readingAlphaString) 
			{
				// Put this in a function and call at end too
				this->InsertWord();
			}

			if (m_indexConstructionState == CountNumFilesEachWordIsIn)
			{
				for (auto thisWord = m_tempExistingWordMap.begin();
				thisWord != m_tempExistingWordMap.end(); ++thisWord)
				{
					const string& word = (*thisWord).first;
					m_wordMap[word].shortVal++;
				}
			}
			else  // i.e. (m_indexConstructionState == WordCounting)
			{
				// Now transfer the "m_tempExistingWordMap" onto the "m_wordMap"
				ConstructIndexFile(fileArrayIndex);
			}
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
		assert( (m_currentWordLength >= MIN_SEARCH_WORD_SIZE) &&
			(m_currentWordLength <= MAX_SEARCH_WORD_SIZE) );

		if (m_wordMap.find(wholeWord) == m_wordMap.end())
		{
			MapData newTemp;
			newTemp.intVal = 1;
			newTemp.shortVal = 0;
			m_wordMap.insert(std::pair<std::string, MapData>(wholeWord, newTemp) );

			// Do prevent "double counting", only count the number of words
			// in map on first pass
			if (m_indexConstructionState == CountNumFilesEachWordIsIn)
				m_wordMapSize++;
		}
		else
		{
			m_wordMap[wholeWord].intVal++;
			//m_wordMap[wholeWord].shortVal++;
		}
	}







	void DirSearch::CullWordMap()
	{
		// If the map is getting to big, cull the least fequent ones until 
		// it gets about 10% smaller by incrementing the "cull frequency threshold".
		// e.g cull words that only have 2 words, then 3 etc until you cull enough
		// to get rid of about 10%.

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
			(m_currentWordLength > (MAX_SEARCH_WORD_SIZE) ) )
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
		
		// First pass is simply counting how many files each word is in
		if (m_indexConstructionState == CountNumFilesEachWordIsIn)
		{
			if (m_tempExistingWordMap.find(m_wholeWord) == m_tempExistingWordMap.end())
			{
				InsertIntoWordMap(m_wholeWord, 0, 1);
				m_tempExistingWordMap[m_wholeWord] = true;// m_initPitPatternList; // TODO for when I use bit pattern
			}
			//else if (DEBUG_MODE)
			//{
			//	InsertIntoWordMap(m_wholeWord, 0, 1);
			//}
		}
		// Then second pass is counting how many time a given word appears in a given file
		else
		{
			m_tempExistingWordMap[m_wholeWord]++;
		}

	}






	void DirSearch::ConstructIndexFile(const unsigned short fileArrayIndex)
	{
		if (m_putMapWordKeysInIndexFile)
		{
			// Store the words from the map keys with "place holder" spaces
			// for the indexs, which will be put in on the second pass
			if (m_indexConstructionState == CountNumFilesEachWordIsIn)
			{
				if (m_readBuffer)
				{
					delete[] m_readBuffer;
					m_readBuffer = nullptr;
				}

				std::string writeBuffer;
				writeBuffer.reserve(m_readBufferSize);

				// TODO write th
				if (DISPLAY_TODO)
					cout << "TODO: write buffer in much larger chuncks!" << endl;


				m_indexFilePosition = static_cast<unsigned int>(m_indexFile.tellp());

				// Calculate where the end of the "map string key" will be in the index file
				unsigned int mapKeysEndPosition = 0;
				for (auto thisWord = m_wordMap.begin();
				thisWord != m_wordMap.end(); ++thisWord)
				{
					mapKeysEndPosition += (*thisWord).first.size() + 1 + SIZE_OF_FILE_DATA;
				}

				mapKeysEndPosition++;	// This is for the "#" put at the end
				unsigned int indexForNextWord = mapKeysEndPosition;

				for (auto thisWord = m_wordMap.begin();
				thisWord != m_wordMap.end(); ++thisWord)
				{
					string word = (*thisWord).first;
					word.push_back(',');	// Append a comma for the word delimiter
					//m_indexFile.write(word.c_str(), word.size());
					writeBuffer.append(word);
					m_indexFilePosition += word.size();
					
					// Calc the file data "start index" for this word
					const char*const fileDataStartIndex = reinterpret_cast<const char*const>(&indexForNextWord);
					writeBuffer.append(string(fileDataStartIndex, sizeof(unsigned int)));

					// The "end index" is a relative value which is just how many files it was in
					const unsigned short numberOfFilesThisWordIsIn = (*thisWord).second.shortVal;
					(*thisWord).second.intVal = indexForNextWord;

					indexForNextWord += numberOfFilesThisWordIsIn *
						static_cast<unsigned int>(sizeof(unsigned int) + sizeof(unsigned short));

					const char*const fileDataEndRelativePosition =
						reinterpret_cast<const char*const>(&numberOfFilesThisWordIsIn);
					writeBuffer.append(string(fileDataEndRelativePosition, sizeof(unsigned short)));

					// The "shortVal" will now be used to indicate the end of the "file data" for
					// this word, which starts at 0 (it's a relative value) and get incremented
					// as the data gets inserted in the next pass (i.e. in the "WordCounting" state)
					(*thisWord).second.shortVal = 0;

					if (writeBuffer.size() >= m_readBufferSize)
					{
						m_indexFile.write(writeBuffer.c_str(), writeBuffer.size());
						writeBuffer.clear();
					}
				}

				if (writeBuffer.size() > 0)
				{
					// Write any remaining elements on the buffer
					m_indexFile.write(writeBuffer.c_str(), writeBuffer.size());
					writeBuffer.clear();
				}

				unsigned long currentFilePosition = static_cast<unsigned long>(m_indexFile.tellp());
				//assert(indexForNextWord == currentFilePosition);
				//m_indexFile.seekp(m_indexFilePosition);

				// Use a different delimiter to indicate the end of the list of "word" map keys
				string word = "#";
				m_indexFile.write(word.c_str(), word.size());
				m_indexFilePosition = static_cast<unsigned int>(m_indexFile.tellp());















				if (DISPLAY_TODO)
					cout << "TODO: I dont think the for loop below can be written to the map instead of the index file!"
					<< endl;

				/*

				// The "file data" postion for the first word will be
				// the final m_indexFilePosition position after writing all of the map keys.
				unsigned int indexForNextWord = static_cast<unsigned int>
				(m_indexFilePosition);

				// Put in the count of each word for each file in respective place
				for (auto thisWord = m_wordMap.begin();
				thisWord != m_wordMap.end(); ++thisWord)
				{
					unsigned int& thisWordIntVal = (*thisWord).second.intVal;

					// First write the "position or file data" value for this map key string
					// in the index file
					const char*const writeBuffer = reinterpret_cast<const char*const>(&indexForNextWord);
					m_indexFile.seekp(thisWordIntVal);
					m_indexFile.write(writeBuffer, sizeof(unsigned int));

					unsigned short numberOfFilesThisWordIsIn = (*thisWord).second.shortVal;
					thisWordIntVal = indexForNextWord;

					indexForNextWord += numberOfFilesThisWordIsIn *
						static_cast<unsigned int>(sizeof(unsigned int) + sizeof(unsigned short));

					// The "shortVal" will now be used to indicate the end of the "file data" for
					// this word, which starts at 0 (it's a relative value) and get incremented
					// as the data gets inserted in the next pass (i.e. in the "WordCounting" state)
					(*thisWord).second.shortVal = 0;
				}
				*/
			}
			else
			{
				// Iterate through the "m_tempExistingWordMap" and Put in the count of 
				// each word for each file in respective place for that file index
				for (auto thisWord = m_tempExistingWordMap.begin();
				thisWord != m_tempExistingWordMap.end(); ++thisWord)
				{
					// Extract the data onstructed in this function in the first pass 
					// (i.e. in the "CountNumFilesEachWordIsIn" state)
					const string& word = (*thisWord).first;
					const unsigned int thisWordFileDataPosition = m_wordMap[word].intVal;
					unsigned short& offset = m_wordMap[word].shortVal;
					unsigned int writePosition = thisWordFileDataPosition + offset;
					// Get the word count value for this word for this file
					const unsigned int fileWordCount = (*thisWord).second;

					// Then write the index number for this file ( range = 0-2000)
					const char*const writeFileIndexBuffer = reinterpret_cast<const char*const>(&fileWordCount);
					m_indexFile.seekp(writePosition);
					m_indexFile.write(writeFileIndexBuffer, sizeof(unsigned short));

					// Then write the word count
					writePosition += sizeof(unsigned short);
					const char*const writeCountBuffer = reinterpret_cast<const char*const>(&fileWordCount);
					m_indexFile.seekp(writePosition);
					m_indexFile.write(writeCountBuffer, sizeof(unsigned int));

					// Update the location of the "offset value"
					offset = sizeof(unsigned short) + sizeof(unsigned int);
				}
			}
		}

	}



}
