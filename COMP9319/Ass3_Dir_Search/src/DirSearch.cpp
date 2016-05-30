//#define _CRT_SECURE_NO_WARNINGS
#ifndef  _WIN32
// Linux only headers
#include <sys/types.h>
#include <dirent.h>
#else	// If win32
//# define  strerror strerror_s
//# define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <tchar.h> 
#include <atlstr.h>
#include <stdio.h>
#include <strsafe.h>
#pragma comment(lib, "User32.lib")
#endif // ! _WIN32

#include <stdio.h>
#include <iostream>
#include <string>
#include <cassert>
#include <algorithm>
#include <set>
#include <locale>


// Internal headers
#include "DirSearch.h"
#include "HelperFunctions.h"


static const bool DEBUG_MODE = false;
static const bool ENABLE_ERROR_MSG = false;
static const bool DISPLAY_TODO = false;

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
		//bool startNewPhrase = false;
		Phrase* phrase = nullptr;

		for (auto nextWord = searchStrings.begin();
			nextWord != searchStrings.end(); ++nextWord)
		{
			std::string newWord;

			previousConvertedChar = 0;
			compressDoubleLetter = false;
			unsigned short wordUncompressedSize = 0;

			for (auto nextChar = (*nextWord).begin();
			nextChar != (*nextWord).end(); ++nextChar)
			{
				if( isalpha(*nextChar) )
				{
					wordUncompressedSize++;
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
				else if ((*nextChar) == ' ')
				{
					// If a phrase is detected (i.e. search term with spaces)
					if (phrase == nullptr)
					{
						phrase = new Phrase;
						m_numberOfPhrases++;
						m_doPhraseSearch = true;
					}
					phrase->phrases.push_back(newWord);

					wordUncompressedSize++;
					//newWord.push_back(*nextChar);
					previousConvertedChar = 0;
					compressDoubleLetter = false;
					newWord.clear();
				}
			}

			if (phrase)
			{
				// Put on the last word of phrase
				if (newWord.size() > 0)
				{
					phrase->phrases.push_back(newWord);
				}
				m_searchPhrases[m_numberOfPhrases - 1] = phrase;
				phrase = nullptr;
			}
			// Min search term lengh is within the valid size range
			else if( (wordUncompressedSize >= 3) && (wordUncompressedSize <= MAX_SEARCH_WORD_SIZE) )
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

			for (unsigned int i = 0; i < m_numberOfPhrases; i++)
			{
				delete m_searchPhrases[i];
			}
		}

		//delete[] m_prefixArray;

		if (DISPLAY_TODO)
		{
			cout << "TODO: pass in the directory file path as an INPUT ARG!!!" << endl;
			cout << "Map size = " << m_wordMapSize << endl;
		}
	}


	DirSearch::DirSearch(const std::string& targetDirectory, const std::string& indexFilename, 
		const std::vector<std::string>& searchStrings,const unsigned int indexPercentage
		)
		: m_dirFullPath(targetDirectory + "/" )
		, m_indexFilename(indexFilename)
		, m_indexPercentage(indexPercentage)
		, m_dirsearchDataSize(-1)	// TODO where should this be cacluated?
		, m_convertSearchString( ConvertString(searchStrings) )
	{
		m_filesInDir.reserve(2000);

		if (DEBUG_MODE)
			cout << "File lenght = " << m_dirsearchDataSize << endl;

		if (m_dirsearchDataSize >= 0)	//> 60000000) // If more than 60MB, store everthing in the index file
		{
			// For now I will use index file every time
			m_useIndexFile = true;
		}

		if (DISPLAY_TODO)
		{
			cout << endl << "TODO: WARNING! Index file is disabled for now" <<
				endl << endl;
		}

		{
			// TODO remove this hardcoded test valeus
			m_useIndexFile = false;		// For now I'm hardcoding this mode
			m_createIndexFile = false;
			m_indexFile.open(m_indexFilename, ios::in | ios::binary);
		}
		
		
		
		
		if (m_useIndexFile)
		{
			m_useIndexFileMap = true;	// For now I'm hardcoding this mode, but it should only be set if m_useIndexFile is true

			m_indexFile.open(m_indexFilename, ios::in | ios::binary);

			m_indexFile.seekg(0, ios::end);
			m_indexFileSize = static_cast<unsigned int>(m_indexFile.tellg());
			m_indexFile.seekg(0, ios::beg);

			if ( m_indexFile && m_indexFile.good() && (m_indexFileSize > 0) )
			{
				m_createIndexFile = false;




				//m_indexFileSize = 2;	// TODO hardcoding this for now



				if (m_indexFileSize <= 2 )
					m_useIndexFile = false;
				else
					m_useIndexFile = true;

				// If the file already exists, then we can assume it's the correct one and use it
				// This is one of the contraints we were given
				assert(!DEBUG_MODE || (m_indexFileSize <= indexPercentage*m_dirsearchDataSize) );
			}
			else
			{
				// If it is empty, we must create one for the next runs
				m_indexFile.close();
				m_indexFileSize = 0;
				m_createIndexFile = true;
				m_useIndexFile = false;		// Don't use index file when it's getting contructed, 
											// just search files as it's being created
				m_indexFile.open(m_indexFilename, ios::out | ios::binary);
				m_indexFile.seekp(0);
			}
		}

#ifndef  _WIN32

		DIR *dpdf;
		struct dirent *epdf;
		//m_dirFullPath = "/import/kamen/1/cs9319/a3/books200m/";
		//m_dirFullPath = "/import/kamen/1/cs9319/a3/legal1/";
		//m_dirFullPath = "/import/kamen/1/cs9319/a3/man/";
		//m_dirFullPath = "../Test_Data/books200m/";
		//m_dirFullPath = "../Test_Data/legal1/";
		//m_dirFullPath = "../Test_Data/man/";

		dpdf = opendir(m_dirFullPath.c_str());

		if (dpdf != NULL)
		{
			short i = 0;
			while (epdf = readdir(dpdf))
			{
				if ((string(epdf->d_name) != ".") && (string(epdf->d_name) != ".."))
				{
					std::string filenameString = string(epdf->d_name);
					m_filesInDir.push_back(filenameString);
					if (DEBUG_MODE)
						std::cout << epdf->d_name << std::endl;
				}
			}

			closedir(dpdf);
		}
		else if (DEBUG_MODE)
			std::cout << "Couldn't open direcot: " << m_dirFullPath << std::endl;

#else	// If win32
		/*
		// test code
		string fileName1 =
			"D:/Dropbox/TimDocs/NonGit/Training/UNSW/Courses/2016/COMP9319-Web_Compress/Ass/Ass3/Test_Data/debug_data/README.txt";
		string fileName2 =
			"D:/Dropbox/TimDocs/NonGit/Training/UNSW/Courses/2016/COMP9319-Web_Compress/Ass/Ass3/Test_Data/debug_data/README-Copy.txt";

		if (DISPLAY_TODO)
			cout << "Note that windows is using remove hard-coded files for testing..." << endl;

		m_filesInDir.push_back(fileName1);
		m_filesInDir.push_back(fileName2);
		*/



		// from: https://msdn.microsoft.com/en-us/library/windows/desktop/aa365200%28v=vs.85%29.aspx

		WIN32_FIND_DATA ffd;
		LARGE_INTEGER filesize;
		TCHAR szDir[MAX_PATH];
		//size_t length_of_arg;
		HANDLE hFind = INVALID_HANDLE_VALUE;
		DWORD dwError = 0;

		//m_dirFullPath =
			//"D:/Dropbox/TimDocs/NonGit/Training/UNSW/Courses/2016/COMP9319-Web_Compress/Ass/Ass3/Test_Data/debug_data";
			//"D:/Dropbox/TimDocs/NonGit/Training/UNSW/Courses/2016/COMP9319-Web_Compress/Ass/Ass3/Test_Data/legal1";
			//"D:/Dropbox/TimDocs/NonGit/Training/UNSW/Courses/2016/COMP9319-Web_Compress/Ass/Ass3/Test_Data/man";
			//"D:/Dropbox/TimDocs/NonGit/Training/UNSW/Courses/2016/COMP9319-Web_Compress/Ass/Ass3/Test_Data/man_debug";
			//"D:/Dropbox/TimDocs/NonGit/Training/UNSW/Courses/2016/COMP9319-Web_Compress/Ass/Ass3/Test_Data/booknmail";

		TCHAR *param = new TCHAR[m_dirFullPath.size() + 1];
		param[m_dirFullPath.size()] = 0;
		//As much as we'd love to, we can't use memcpy() because
		//sizeof(TCHAR)==sizeof(char) may not be true:
		std::copy(m_dirFullPath.begin(), m_dirFullPath.end(), param);
		
		//const TCHAR *pathTCHAR = m_dirFullPath.c_str();

		StringCchCopy(szDir, MAX_PATH, param);
		StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

		hFind = FindFirstFile(szDir, &ffd);

		if (DEBUG_MODE && (INVALID_HANDLE_VALUE == hFind) )
		{
			cerr << (TEXT("FindFirstFile"));
		}


		do
		{
			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if(DEBUG_MODE)
					_tprintf(TEXT("  %s   <DIR>\n"), ffd.cFileName);
			}
			else
			{
				filesize.LowPart = ffd.nFileSizeLow;
				filesize.HighPart = ffd.nFileSizeHigh;
				if (DEBUG_MODE)
					_tprintf(TEXT("  %s   %ld bytes\n"), ffd.cFileName, filesize.QuadPart);

				//char* fileString = CW2A(L"LPWSTR STRING");
				//CA2T szr(ffd.cFileName);
				//LPCSTR fileCharPointer = CW2A(ffd.cFileName);
				//string fileString(fileCharPointer);

				//PTSTR pszFileName = ffd.cFileName;
				//string fileString(pszFileName);
				m_filesInDir.push_back(string(CW2A(ffd.cFileName)));
			}
		} while (FindNextFile(hFind, &ffd) != 0);

		dwError = GetLastError();
		if ( (DEBUG_MODE) && (dwError != ERROR_NO_MORE_FILES) )
		{
			cerr << (TEXT("FindFirstFile"));
		}

		FindClose(hFind);








#endif // ! _WIN32

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
		else
		{
			this->ReadAllFiles();
		}

	}

	void DirSearch::ReadAllFiles()
	{

		short fileArrayIndex = 0;
		//string fileName = "README.txt";
		//auto fileName = m_filesInDir.begin();
		for (auto fileName = m_filesInDir.begin();
		fileName != m_filesInDir.end(); ++fileName)
		{
			string fullFilePath = m_dirFullPath + (*fileName);
			this->ReadAndSearchFile(fullFilePath.c_str(), fileArrayIndex++);
		}

	}


	void DirSearch::Search()
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

		unsigned int fileSize = 0;

		if(m_useIndexFile && (!m_createIndexFile) )
		{
			// Only need to load the index file data if it wasn't just created
			// by this instance of the app

			m_indexFile.seekg(0, ios::end);
			fileSize = static_cast<unsigned int>(m_indexFile.tellg());
			m_indexFile.seekg(0, ios::beg);
			m_fileWordCount = 0;

			if (m_readBuffer)
			{
				delete[] m_readBuffer;
				m_readBuffer = nullptr;
			}


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
		}



		
		
		if (m_useIndexFile)
		{
			// Only need to read data from index file if it wasn't created 
			// in this instance of the app
			const char* movingPointerToBufferData = m_readBuffer;
			bool readAllKeyValues = false;
			bool indexedFileNotRead = true;
			//assert(m_wordMap.empty());
			//m_wordMap.clear();
			WordDataMapT searchTermDataMap;

			while (!readAllKeyValues)
			{

				if (indexedFileNotRead ||
					(static_cast<unsigned int>(movingPointerToBufferData - m_readBuffer) >=
						(m_readIndexBufferSize - 2 * MAX_SEARCH_WORD_SIZE)))
				{
					if (DISPLAY_TODO)
					{
						cout << "Need to read the index file in blocks!" << endl;
					}

					if (m_readIndexBufferSize < fileSize)
						m_readIndexBufferSize = fileSize;

					//assert(!DEBUG_MODE);
					//todo need set seekg to where the file is is up to!!!!

					indexedFileNotRead = false;
					m_readBuffer = new char[m_readIndexBufferSize + 1];
					m_indexFile.read(m_readBuffer, fileSize);
					movingPointerToBufferData = m_readBuffer;
				}

				if (GetNextMapStringFromIndexFile(movingPointerToBufferData))
				{
					readAllKeyValues = true;
					break;
				}

				for (auto thisWord = m_convertSearchString.begin();
				thisWord != m_convertSearchString.end(); ++thisWord)
				{
					if (m_nextMapStringFromIndexFile == *thisWord)
					{
						const unsigned int startPosition = *(reinterpret_cast<const unsigned int*>(movingPointerToBufferData));
						const unsigned short numberOfFileData = *(reinterpret_cast<const unsigned short*>(
							movingPointerToBufferData + sizeof(unsigned int)));

						MapData searchTermData;
						searchTermData.intVal = startPosition;
						searchTermData.shortVal = numberOfFileData;

						assert( !DEBUG_MODE || (searchTermDataMap.find(*thisWord) == searchTermDataMap.end()) );
						searchTermDataMap[*thisWord] = searchTermData;
					}
					else if (m_nextMapStringFromIndexFile.find(*thisWord) != std::string::npos)
					{
						// If an exact word isn't found, but it's insise another string, then
						// this is a substring search.
						// Note that substring search will just append the "filedata" to the
						// list that includes the "filedata" of all other substring matches.
						// Then the "word count" for each file is just the addition of all
						// the word counts for that file in the list.
					}
				}

				movingPointerToBufferData += SIZE_OF_FILE_DATA;
			}

			SearchWordsFromIndexFile(searchTermDataMap);
		}
		else if (!m_createIndexFile)
		{
			ResultFilenameDataT searchResultsWithFileNames;
			
			for (auto thisFile = m_resultMap.begin();
			thisFile != m_resultMap.end(); ++thisFile)
			{
				searchResultsWithFileNames[m_filesInDir.at((*thisFile).first)]
					= (*thisFile).second;
			}

			DisplaySearchResults(searchResultsWithFileNames);
		}
		else
		{
			if (DISPLAY_TODO)
			{
				cout << "TODO: Not implemented yet! When index file is created in this instance of app, " <<
					"I just need to read off the map!" << endl;
			}
			//else
			//	assert(!DEBUG_MODE);	// Not implemented yet


			for (auto thisWord = m_convertSearchString.begin();
			thisWord != m_convertSearchString.end(); ++thisWord)
			{
				// If the index file was created in this intance, then all the data
				// is still in memory (i.e. in the m_wordMap) so just read straight 
				// off the map. If a "direct" lookup from map doesn't return a value,
				// then iterate through map and do a "substring" search.
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



		/*
		// TODO remove this hard coded search ?
		// test code
		for (auto searchTerm = m_convertSearchString.begin();
			searchTerm != m_convertSearchString.end(); ++searchTerm)
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


	void DirSearch::SearchWordsFromIndexFile(const WordDataMapT& searchTermDataMap)
	{
		ResultListT resultWordCountMap;
		assert(!DEBUG_MODE || (m_readBuffer != nullptr) );

		if (m_createIndexFile)
		{
			// If we have just created the index file, then it's in "out" mode,
			// so close it and open it in "in" mode.
			m_indexFile.close();
			m_indexFile.open(m_indexFilename, ios::in | ios::binary);
		}
		else if (!m_useIndexFileMap && (!m_wordMap.empty() ) )
		{
			// If using the word map from the index file, free up the memory
			m_wordMap.clear();
		}

		// Iterate throught the "temp" map and count the data
		// Only search files that ALL search terms occur in
		// Then search for the file data for this term
		for (auto thisWord = searchTermDataMap.begin();
		thisWord != searchTermDataMap.end(); ++thisWord)
		{
			ResultFileDataT wordResultData;

			const string& word = (*thisWord).first;
			if (DEBUG_MODE)
				cout << "Searching for search term: " << (*thisWord).first << endl << endl;

			unsigned int debugCountWords = 0;
			unsigned short fileIndex = 0;

			const unsigned int& startPostion = (*thisWord).second.intVal;
			const unsigned short& numberOfFileData = (*thisWord).second.shortVal;
			//int debugVal = m_indexFile.tellg();
			m_indexFile.seekg(startPostion);
			const unsigned int numberOfBytesForFileData = numberOfFileData * SIZE_OF_FILE_DATA;
			assert(!DEBUG_MODE || ( (m_readBufferSize >= numberOfBytesForFileData) &&
				(m_readIndexBufferSize >= numberOfBytesForFileData) ) );
			m_indexFile.read(m_readBuffer, numberOfBytesForFileData);
			const char* movingPointerToBufferData = m_readBuffer;
			
			for (unsigned int i = 0; i < numberOfFileData; i++)
			{
				// Count how many time each word appears in each file
				fileIndex =
					*(reinterpret_cast<const unsigned short*>(movingPointerToBufferData));
				movingPointerToBufferData += sizeof(unsigned short);

				const unsigned int wordCount =
					*(reinterpret_cast<const unsigned int*>(movingPointerToBufferData));
				movingPointerToBufferData += sizeof(unsigned int);
				debugCountWords += wordCount;

				// Add this word count and file index to a map of the "word"
				// Note that the "substring search" might have multiple "word counts"
				// for a given file (i.e. for all the different substrings that match)
				if (wordResultData.find(fileIndex) == wordResultData.end())
				{
					wordResultData[fileIndex] = wordCount;
				}
				else
				{
					// This is mainly for "substring search" that could have many different
					// counts for each file
					wordResultData[fileIndex] += wordCount;
				}

				if (DEBUG_MODE ) //&& wordCount)
				{
					cout << (*thisWord).first << " found " << wordCount <<
						" times in file: " << m_filesInDir.at(fileIndex) << 
						" with file index value = " << fileIndex << endl;
				}
			}

			if (DEBUG_MODE)
			{
				cout << endl << (*thisWord).first << " total count in all files = " 
					<< debugCountWords << endl << endl;
			}

			resultWordCountMap.push_back(wordResultData);
		}

		ProcessIndedFileResults(resultWordCountMap);
	}

	
	void DirSearch::ProcessIndedFileResults( ResultListT& resultWordCountMap)
	{
		ResultFileDataT firstResult;
		ResultFilenameDataT finalResultData;

		if (!resultWordCountMap.empty())
		{
			firstResult = resultWordCountMap.front();
			resultWordCountMap.pop_front();
		}

		for (auto fileDataInFirstResult = firstResult.begin();
		fileDataInFirstResult != firstResult.end(); ++fileDataInFirstResult)
		{
			const unsigned short& fileIndex = (*fileDataInFirstResult).first;
			bool allSearchTermsHaveThisFile = true;
			unsigned int totalWordCount = (*fileDataInFirstResult).second;

			for (auto otherFileData = resultWordCountMap.begin();
			otherFileData != resultWordCountMap.end(); ++otherFileData)
			{
				if (otherFileData->find(fileIndex) == otherFileData->end())
				{
					allSearchTermsHaveThisFile = false;
					continue;
				}

				totalWordCount += (*otherFileData)[fileIndex];
			}

			if (!allSearchTermsHaveThisFile)
			{
				continue;
			}

			const string& fileName = m_filesInDir.at(fileIndex);
			assert(!DEBUG_MODE || (finalResultData.find(fileName) == finalResultData.end()));
			finalResultData[fileName] = totalWordCount;

			if (DEBUG_MODE)
				cout << "results for: " << fileName << " , index = " << fileIndex <<
					" , word count = " << totalWordCount << endl;
		}

		DisplaySearchResults(finalResultData);

	}


	void DirSearch::DisplaySearchResults(const ResultFilenameDataT& finalResultData)
	{
		// Sort the results
		// Modified from: http://stackoverflow.com/a/19528891
		std::vector<pair<string, unsigned long>> pairs;
		for (auto itr = finalResultData.begin(); itr != finalResultData.end(); ++itr)
			pairs.push_back(*itr);

		sort(pairs.begin(), pairs.end(), [=](pair<string, unsigned long>& a, pair<string, unsigned long>& b)
		{
			if (a.second == b.second)
				return a.first < b.first;
			else
				return a.second > b.second;
		}
		);

		if (!pairs.empty())
		{
			for (auto itr = pairs.begin(); itr != pairs.end(); ++itr)
			{
				if (DEBUG_MODE)
				{
					cout << "results file = " << (*itr).first <<
						", word count = " << (*itr).second << endl;
				}
				else
				{
					cout << (*itr).first << endl;
				}
			}
		}

	}


	bool DirSearch::GetNextMapStringFromIndexFile(const char* & movingPointerToBufferData)
	{
		// returns true when the "#" is found, indicating the end of map strings 
		// in the index file
		m_nextMapStringFromIndexFile.clear();
		const char* const startPointer = movingPointerToBufferData;

		unsigned int stringSize = 0;
		char thisChar = 0;
		while (true)
		{
			thisChar = *(movingPointerToBufferData);
			movingPointerToBufferData++;
			if (thisChar == ',')
				break;
			else if (thisChar == '#')
			{
				return true;
			}
			stringSize++;
		}

		m_nextMapStringFromIndexFile = string(startPointer, stringSize);

		return false;
	}

	/*
	WordFileDataListT* DirSearch::SearchAllFiles(const std::string& searchTerm)
	{
		WordFileDataListT* listOfTermFileData = nullptr;

		short fileArrayIndex = 0;
		//if (DEBUG_MODE)
		//	cout << "TODO: remove hard-coded file file index for the search: " << fileArrayIndex << 
		//		" and the hardcoded search term: " << endl;



		// TODO test for "spaces" when doing the "phrase search!"



		// if the whole word is small enough to be a key on the map, then
		// no word is required
		listOfTermFileData = SearchWordMap(searchTerm);

		return listOfTermFileData;
	}
	*/



	enum SearchState
	{
		startSearch,
		traversingNonAlpha,
		readingAlphaString,
		traversingOversizedWord
	};


	void DirSearch::ReadAndSearchFile(const char* const fileName, short fileArrayIndex)
	{
		try
		{
			std::ifstream inputFile(fileName);
			inputFile.seekg(0, ios::end);
			unsigned int fileSize = static_cast<unsigned int>(inputFile.tellg());
			inputFile.seekg(0, ios::beg);
			m_fileWordCount = 0;
			m_fileTotalSearchTermsFound = 0;
			m_allKeywordsFoundInFile.clear();

			if (m_doPhraseSearch)
			{
				for (unsigned int i = 0; i < m_numberOfPhrases; i++)
				{
					m_allPhrasesFoundInFile[i] = false;
				}
			}


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
			m_onlySpacesBetweenWords = true;
			m_skippedLongOrShortWord = false;

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
				{
					cerr << "Could not open input file: " << fileName << endl;
					//" because following error occured: " << strerror(errno) << endl;

					throw std::runtime_error("Couldn't open file mentioned above");
				}
						
				return;
			}
			else if (DEBUG_MODE)
			{
				cout << "Successfully Opened file: " << fileName << " with index = " <<
					fileArrayIndex << ", of size: "	<< fileSize << endl;
			}


			//if (DEBUG_MODE && (fileArrayIndex == 1744))
			//{
			//	int dummy = 0;
			//	dummy += dummy + 1;
			//}




			for (unsigned int i = 0; i < fileSize; i++)
			{
				// TODO The "m_bwtLastColumn" is a "fixed" sized unsigned char array and use this
				// for smaller files!
				nextChar = m_readBuffer[i];

				if ( m_currentWordLength >(MAX_SEARCH_WORD_SIZE) )
				{
					searchMode = traversingOversizedWord;
				}

				// If this is an alpha value and not traversing oversized word
				
				if( (nextChar > 64) && (isalpha(nextChar) != 0) )
				{
					// If it is alpha, only process it if it isn't an oversized word
					if (searchMode != traversingOversizedWord)
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
				}
				else
				{
					// If not an alpha value

					if(searchMode == readingAlphaString)
					{
						// Put this in a function and call at end too
						this->InsertWord();

						//if (DEBUG_MODE && (fileArrayIndex == 1744) && (m_wholeWord == "company"))
						//{
						//	int dummy = 0;
						//	dummy += dummy + 1;
						//}
						
						if (nextChar == ' ')
							m_onlySpacesBetweenWords = true;
						else
							m_onlySpacesBetweenWords = false;
					}
					
					if (searchMode != traversingNonAlpha)
					{
						startNewWord = true;
						arrayIndexBase = 0;
						m_wholeWord.clear();
						m_currentWordLength = 0;
						searchMode = traversingNonAlpha;
						previousConvertedChar = 0;
						compressDoubleLetter = false;
					}
					else
					{
						m_onlySpacesBetweenWords = false;
					}
					
					//if (m_doPhraseSearch && m_onlySpacesBetweenWords && (nextChar != ' ') )
					//	m_onlySpacesBetweenWords = false;
				}
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

			if (m_createIndexFile)
			{
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
			else
			{
				// If any key word is missing, we can return
				for (auto thisWord = m_convertSearchString.begin();
				thisWord != m_convertSearchString.end(); ++thisWord)
				{
					if (m_allKeywordsFoundInFile.find(*thisWord) ==
						m_allKeywordsFoundInFile.end())
					{
						return;
					}
				}

				// If there are phrases, check that they were all found
				if (m_doPhraseSearch)
				{
					for (unsigned int i = 0; i < m_numberOfPhrases; i++)
					{
						if (m_allPhrasesFoundInFile[i] == false)
							return;
					}
				}

				// If all words were found
				m_resultMap[fileArrayIndex] = m_fileTotalSearchTermsFound;
			}
		}
		catch ( std::exception e)
		{
			if (DEBUG_MODE || ENABLE_ERROR_MSG)
			{
				cerr << "DirSearch::ReadAndSearchFile threw following exception! " << e.what() << endl;
				assert(!DEBUG_MODE);
			}
		}
		catch (...)
		{
			if (DEBUG_MODE || ENABLE_ERROR_MSG)
			{
				cerr << "DirSearch::ReadAndSearchFile threw unknown exception! " << endl;
				assert(!DEBUG_MODE);
			}
		}
	}


	void DirSearch::InsertIntoWordMap(const std::string & wholeWord, 
		const short fileArrayIndex, const int fileWordCount)
	{
		assert(!DEBUG_MODE || ( (m_currentWordLength >= MIN_SEARCH_WORD_SIZE) &&
			(m_currentWordLength <= MAX_SEARCH_WORD_SIZE) ) );

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






	void DirSearch::InsertWord()
	{
		if ( (m_currentWordLength < MIN_SEARCH_WORD_SIZE) || 
			(m_currentWordLength > (MAX_SEARCH_WORD_SIZE) ) )
		{
			// Not searching for words less than 3 char long or more than 256 char long,
			// so dont store these words
			m_skippedLongOrShortWord = true;
			return;
		}
		
		// First pass is simply counting how many files each word is in
		if (m_createIndexFile)
		{
			if (m_indexConstructionState == CountNumFilesEachWordIsIn)
			{
				if (m_tempExistingWordMap.find(m_wholeWord) == m_tempExistingWordMap.end())
				{
					InsertIntoWordMap(m_wholeWord, 0, 1);
					m_tempExistingWordMap[m_wholeWord] = true;// m_initPitPatternList; // TODO for when I use bit pattern
				}
			}
			// Then second pass is counting how many time a given word appears in a given file
			else
			{
				m_tempExistingWordMap[m_wholeWord]++;
			}
		}
		else
		{
			bool subStringFound = false;

			// If not contructing the index file, then just search each term now
			for (auto thisWord = m_convertSearchString.begin();
			thisWord != m_convertSearchString.end(); ++thisWord)
			{
				if (m_wholeWord == *thisWord)
				{
					m_fileTotalSearchTermsFound++;
					m_allKeywordsFoundInFile[*thisWord] = true;
				}
				else if (m_doSubstringSearch && (m_wholeWord.find(*thisWord) != string::npos))
				{
					// TODO do substring search
					subStringFound = true;
					m_fileTotalSearchTermsFound++;
					m_allKeywordsFoundInFile[*thisWord] = true;
				}
			}
			
			if (m_doPhraseSearch ) // && (!subStringFound))
			{
				for (unsigned int i = 0; i < m_numberOfPhrases; i++)
				{
					unsigned short& phraseIndex = m_searchPhrases[i]->phraseIndex;
					const std::vector<std::string>& phrases = m_searchPhrases[i]->phrases;

					if (m_skippedLongOrShortWord)
					{
						// If just skipped a long or short word, reset the phrase index
						phraseIndex = 0;
					}

					if (phrases[phraseIndex] == m_wholeWord)
					{
						if( (!m_onlySpacesBetweenWords) && (phraseIndex != 0) )
						{
							phraseIndex = 0;
						}
						if ((++phraseIndex) == phrases.size())
						{
							// if full phrase has been found
							phraseIndex = 0;
							m_fileTotalSearchTermsFound++;
							m_allPhrasesFoundInFile[i] = true;
						}
					}
					else
					{
						// Reset the phrase index back to start
						phraseIndex = 0;
					}
				}
			}
		}

		m_skippedLongOrShortWord = false;
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

					//if (DEBUG_MODE && (word == "aPle,")) // && (fileArrayIndex == 5)
					//{
					//	cout << endl << "Index for word aPle in file index " << fileArrayIndex << " = " << indexForNextWord <<
					//		" , with word count = " << (*thisWord).second.shortVal << endl << endl;
					//}
					
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
					unsigned int writePosition = thisWordFileDataPosition + 
						(offset * (sizeof(unsigned short) + sizeof(unsigned int) ) );
					// Get the word count value for this word for this file
					const unsigned int fileWordCount = (*thisWord).second;

					// Then write the index number for this file ( range = 0-2000)
					const char*const writeFileIndexBuffer = reinterpret_cast<const char*const>(&fileArrayIndex);
					m_indexFile.seekp(writePosition);
					m_indexFile.write(writeFileIndexBuffer, sizeof(unsigned short));

					// Then write the word count
					writePosition += sizeof(unsigned short);
					const char*const writeCountBuffer = reinterpret_cast<const char*const>(&fileWordCount);
					m_indexFile.seekp(writePosition);
					m_indexFile.write(writeCountBuffer, sizeof(unsigned int));

					// Update the location of the "offset value"
					offset++;
				}
			}
		}

	}



}
