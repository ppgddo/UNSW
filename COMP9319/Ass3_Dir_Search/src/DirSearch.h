#pragma once

#include <string>
#include <fstream>
#include <unordered_map>
#include <list>
#include <cassert>
#include <locale>

namespace dirsearch 
{
	struct WordFileData
	{
		WordFileData( short fileIndex, unsigned int wordCount ) // std::list<char> & bitPatterns)
			: m_fileIndex(fileIndex)
			, m_wordCount(wordCount)
			//, m_bitPatterns(bitPatterns)
		{}

		unsigned short m_fileIndex;
		unsigned int m_wordCount;
		//std::list<char> m_bitPatterns;	// TODO use this instead when I implement "Phrase search"

	private:
		WordFileData(const WordFileData&) = delete;
		WordFileData& operator=(const WordFileData&);
	};


	//typedef std::list<WordFileData * > WordFileDataListT;

	//struct WordData
	//{
	//public:
	//	WordData(const char* const wordString)
	//		: m_wordString(wordString)
	//	{
	//		//m_wordFileData = new WordFileDataListT;
	//	}


	//	~WordData()
	//	{
	//		// TODO delete the pointers on the m_wordFileData as well!
	//		//delete m_wordFileData;
	//	}

	//	void InsertFileData(WordFileData* wordFileData)
	//	{
	//		// Update the file data
	//		//m_wordFileData->push_back(wordFileData);
	//	}


	//	WordFileDataListT * GetFileData()
	//	{
	//		return nullptr;
	//		//return m_wordFileData;
	//	}


	//	const char* const  m_wordString;
	//	// Put all data for each "word" for a given "prefix" 
	//	// i.e. search word = preffix (key of WordMap) + word
	//	//WordFileDataListT * m_wordFileData;

	//	/*
	//	Word(Word&& other) : m_singleWord(nullptr), m_wordList(nullptr)
	//	{
	//		// pilfer other’s resource
	//		m_singleWord = other.m_singleWord;
	//		m_wordList = other.m_wordList;
	//		// reset other
	//		other.m_singleWord = nullptr;
	//		other.m_wordList = nullptr;
	//	}
	//	*/


	//private:
	//	WordData(const WordData&) = delete;
	//	WordData& operator=(const WordData&);
	//};



	
	//class Word
	//{
	//public:
	//	//typedef std::list<WordData*> WordListT;

	//	Word()/*(const char* const wordString)
	//		: m_singleWord(new WordData(wordString))*/
	//	{}

	//	~Word()
	//	{
	//		// TODO delete the pointers on the m_wordList list too!
	//		delete m_singleWord;
	//		delete m_wordList;
	//	}

	//	bool HasOnlyOneWord()
	//	{
	//		return (m_wordList == nullptr);
	//	}


	//	WordFileDataListT* GetFileData(const char* const wordString)
	//	{
	//		if ((m_wordList != nullptr))
	//		{
	//			for (auto existingWordes = m_wordList->begin();
	//				existingWordes != m_wordList->end(); ++existingWordes)
	//			{
	//				if (std::string(wordString) == std::string((*existingWordes)->m_wordString) )
	//				{
	//					return (*existingWordes)->GetFileData();
	//				}
	//			}

	//			return nullptr;
	//		}
	//		else
	//		{
	//			// Just return the m_singleWord data
	//			assert(m_singleWord != nullptr);
	//			if ( (m_singleWord != nullptr) && 
	//				std::string(m_singleWord->m_wordString) != std::string(wordString) )
	//			{
	//				return m_singleWord->GetFileData();
	//			}
	//			else
	//			{
	//				return nullptr;
	//			}
	//		}
	//	}


	//	void InsertWord(const char* const word, WordFileData* wordFileData)
	//	{
	//		if ((m_wordList != nullptr))
	//		{
	//			for (auto existingWordes = m_wordList->begin();
	//			existingWordes != m_wordList->end(); ++existingWordes)
	//			{
	//				if ((*existingWordes)->m_wordString == word)
	//				{
	//					(*existingWordes)->InsertFileData(wordFileData);
	//					return;
	//				}
	//			}
	//			
	//			WordFileData* newWord = new WordFileData(word);
	//			newWord->InsertFileData(wordFileData);
	//			m_wordList->push_back(newWord );
	//		}
	//		else
	//		{
	//			assert(m_singleWord != nullptr);
	//			if (m_singleWord != nullptr)
	//			{
	//				if (m_singleWord->m_wordString != word)
	//				{
	//					// If new word string is inserted, delete the "single word
	//					// Stored in this class and transfer it to the m_wordList list
	//					m_wordList = new WordListT;
	//					m_wordList->push_back(m_singleWord);
	//					WordFileData* newWord = new WordFileData(word);
	//					newWord->InsertFileData(wordFileData);
	//					m_wordList->push_back(newWord);
	//				}
	//				else
	//				{
	//					m_singleWord->InsertFileData(wordFileData);
	//				}
	//			}
	//		}
	//	}

	//	Word(Word&& other) : m_singleWord(nullptr), m_wordList(nullptr)
	//	{
	//		// pilfer other’s resource
	//		m_singleWord = other.m_singleWord;
	//		m_wordList = other.m_wordList;
	//		// reset other
	//		other.m_singleWord = nullptr;
	//		other.m_wordList = nullptr;
	//	}

	//private:
	//	//unsigned int m_numberOfWords = 0;
	//	WordFileData * m_singleWord = nullptr;
	//	WordListT * m_wordList = nullptr;	// TODO should I use another map or would that use too much memory?

	//	Word(const Word&) = delete;
	//	Word& operator=(const Word&);
	//};
	
















	struct MapData
	{
		unsigned int intVal;
		unsigned short shortVal;
	};


	enum IndexConstructionState
	{
		CountNumFilesEachWordIsIn,
		WordCounting
	};



	class DirSearch
	{
	public:
		//static const unsigned int PREFIX_ARRAY_DIM = 27;	//We have been assured that the data will be < 128
		static const unsigned int MIN_SEARCH_WORD_SIZE = 3;
		static const unsigned int MAX_SEARCH_WORD_SIZE = 256;

		typedef std::list<WordFileData> WordListT;
		typedef std::unordered_map<std::string, MapData> WordDataMapT;
		typedef std::unordered_map<std::string, unsigned int > ExistingWordMapT;
		typedef std::unordered_map<std::string, WordListT > SearchWordMapT;
		typedef char* FilenameArrayT[2000];

		DirSearch(const std::string indexFilename, const unsigned int indexPercentage);

		~DirSearch();

		void Search(const std::vector<std::string>& searchStrings);



	private:
		// private functions
		void InsertWord();
		void CreateIndexForFile(const char* const fileName, short fileArrayIndex);

		void ReadAllFiles();
		void InsertIntoWordMap(const std::string & prefix, const short fileArrayIndex, const int fileWordCount);
		std::vector<std::string> ConvertString(const std::vector<std::string>& searchStrings);

		void ConstructIndexFile(const unsigned short fileArrayIndex);
		void CullWordMap();
		bool GetNextMapStringFromIndexFile(const char* & movingPointerToBufferData);

		void SearchWordsFromIndexFile(const WordDataMapT& searchTermDataMap);

	private:
		// Private data members
		std::fstream m_indexFile;
		unsigned int m_indexFilePosition;
		WordDataMapT m_wordMap;
		ExistingWordMapT m_tempExistingWordMap;
		unsigned int m_wordMapSize = 0;
		unsigned int m_indexFileSize;
		bool m_useIndexFile = false;
		bool m_createIndexFile = false;
		const unsigned int m_indexPercentage = 20;
		std::string m_indexFilename;
		const unsigned int m_dirsearchDataSize;
		unsigned int m_maxFileSize = 0;
		char* m_readBuffer = nullptr;
		unsigned int m_readBufferSize = 1000000;		// TODO will need to decrease this??
		unsigned int m_readIndexBufferSize = 6000000;	// Only used when the "m_wordMap" isn't used, so can be large
		//unsigned char m_wordMapKeyMaxLength = MAX_SEARCH_WORD_SIZE - 1;			// This is the default unless the m_useIndexFile is set
		//int m_maxExpectedWordSize = 256;	// This will increase if a larger word is discovered
		bool m_putMapWordKeysInIndexFile = true;
		IndexConstructionState m_indexConstructionState;
		std::string m_nextMapStringFromIndexFile;
		std::vector<std::string> m_filesInDir;
		std::string m_dirFullPath;
		
		// Used by the CreateIndexForFile() and InsertWord() functions
		std::string m_wholeWord;
		std::locale m_toLowerLocale;
		unsigned int m_currentWordLength;
		unsigned int m_fileWordCount;
		unsigned int m_wordBlockSize = 256;
		std::list<std::string> m_currentWordBlock;
	};

}
	
