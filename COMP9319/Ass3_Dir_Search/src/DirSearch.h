#pragma once

#include <string>
#include <fstream>
#include <unordered_map>
#include <list>
#include <cassert>
#include <locale>

namespace dirsearch 
{
	struct SuffixFileData
	{
		SuffixFileData( short fileIndex, unsigned int wordCount ) // std::list<char> & bitPatterns)
			: m_fileIndex(fileIndex)
			, m_wordCount(wordCount)
			//, m_bitPatterns(bitPatterns)
		{}

		unsigned short m_fileIndex;
		unsigned int m_wordCount;
		//std::list<char> m_bitPatterns;	// TODO use this instead when I implement "Phrase search"

	private:
		SuffixFileData(const SuffixFileData&) = delete;
		SuffixFileData& operator=(const SuffixFileData&);
	};


	typedef std::list<SuffixFileData * > SuffixFileDataListT;



	struct SuffixData
	{
	public:
		SuffixData(const char* const suffixString)
			: m_suffixString(suffixString)
		{
			//m_suffixFileData = new SuffixFileDataListT;
		}


		~SuffixData()
		{
			// TODO delete the pointers on the m_suffixFileData as well!
			//delete m_suffixFileData;
		}

		void InsertFileData(SuffixFileData* suffixFileData)
		{
			// Update the file data
			//m_suffixFileData->push_back(suffixFileData);
		}


		SuffixFileDataListT * GetFileData()
		{
			return nullptr;
			//return m_suffixFileData;
		}


		const char* const  m_suffixString;
		// Put all data for each "suffix" for a given "prefix" 
		// i.e. search word = preffix (key of WordMap) + suffix
		//SuffixFileDataListT * m_suffixFileData;

		/*
		Suffix(Suffix&& other) : m_singleSuffix(nullptr), m_suffixList(nullptr)
		{
			// pilfer other’s resource
			m_singleSuffix = other.m_singleSuffix;
			m_suffixList = other.m_suffixList;
			// reset other
			other.m_singleSuffix = nullptr;
			other.m_suffixList = nullptr;
		}
		*/


	private:
		SuffixData(const SuffixData&) = delete;
		SuffixData& operator=(const SuffixData&);
	};




	class Suffix
	{
	public:
		typedef std::list<SuffixData*> SuffixListT;

		Suffix(const char* const suffixString)
			: m_singleSuffix(new SuffixData(suffixString))
		{}

		~Suffix()
		{
			// TODO delete the pointers on the m_suffixList list too!
			delete m_singleSuffix;
			delete m_suffixList;
		}

		bool HasOnlyOneSuffix()
		{
			return (m_suffixList == nullptr);
		}


		SuffixFileDataListT* GetFileData(const char* const suffixString)
		{
			if ((m_suffixList != nullptr))
			{
				for (auto existingSuffixes = m_suffixList->begin();
					existingSuffixes != m_suffixList->end(); ++existingSuffixes)
				{
					if (std::string(suffixString) == std::string((*existingSuffixes)->m_suffixString) )
					{
						return (*existingSuffixes)->GetFileData();
					}
				}

				return nullptr;
			}
			else
			{
				// Just return the m_singleSuffix data
				assert(m_singleSuffix != nullptr);
				if ( (m_singleSuffix != nullptr) && 
					std::string(m_singleSuffix->m_suffixString) != std::string(suffixString) )
				{
					return m_singleSuffix->GetFileData();
				}
				else
				{
					return nullptr;
				}
			}
		}


		void InsertSuffix(const char* const suffix, SuffixFileData* suffixFileData)
		{
			if ((m_suffixList != nullptr))
			{
				//for( auto SuffixData data : m_suffixList)
				for (auto existingSuffixes = m_suffixList->begin();
				existingSuffixes != m_suffixList->end(); ++existingSuffixes)
				{
					if ((*existingSuffixes)->m_suffixString == suffix)
					{
						(*existingSuffixes)->InsertFileData(suffixFileData);
						return;
					}
				}
				
				SuffixData* newSuffix = new SuffixData(suffix);
				newSuffix->InsertFileData(suffixFileData);
				m_suffixList->push_back(newSuffix );
			}
			else
			{
				assert(m_singleSuffix != nullptr);
				if (m_singleSuffix != nullptr)
				{
					if (m_singleSuffix->m_suffixString != suffix)
					{
						// If new suffix string is inserted, delete the "single suffix
						// Stored in this class and transfer it to the m_suffixList list
						m_suffixList = new SuffixListT;
						m_suffixList->push_back(m_singleSuffix);
						SuffixData* newSuffix = new SuffixData(suffix);
						newSuffix->InsertFileData(suffixFileData);
						m_suffixList->push_back(newSuffix);
					}
					else
					{
						m_singleSuffix->InsertFileData(suffixFileData);
					}
				}
			}
		}

		Suffix(Suffix&& other) : m_singleSuffix(nullptr), m_suffixList(nullptr)
		{
			// pilfer other’s resource
			m_singleSuffix = other.m_singleSuffix;
			m_suffixList = other.m_suffixList;
			// reset other
			other.m_singleSuffix = nullptr;
			other.m_suffixList = nullptr;
		}

	private:
		//unsigned int m_numberOfSuffixs = 0;
		SuffixData * m_singleSuffix = nullptr;
		SuffixListT * m_suffixList = nullptr;	// TODO should I use another map or would that use too much memory?

		Suffix(const Suffix&) = delete;
		Suffix& operator=(const Suffix&);
	};






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

		//typedef unsigned char SuffixArrayT[PREFIX_ARRAY_DIM*m_wordMapKeyMaxLength];
		//typedef std::unordered_map<std::string, Suffix* > WordMapT;
		//typedef std::unordered_map<std::string, unsigned int > WordDataMapT;
		typedef std::unordered_map<std::string, MapData> WordDataMapT;
		//typedef std::unordered_map<std::string, std::list<char> > ExistingWordMapT;	// for "bit pattern" impl
		typedef std::unordered_map<std::string, bool > ExistingWordMapT;
		typedef char* FilenameArrayT[2000];

		DirSearch(const std::string indexFilename, const unsigned int indexPercentage);

		~DirSearch();

		void Search(const std::vector<std::string>& searchStrings);



	private:
		// private functions
		void InsertWord();
		void CreateIndexForFile(const char* const fileName, short fileArrayIndex);

		SuffixFileDataListT* SearchAllFiles(const std::string& searchTerm);

		void InsertIntoWordMap(const std::string & prefix, const short fileArrayIndex, const int fileWordCount);
		SuffixFileDataListT* SearchWordMap(const std::string & prefix);

		std::vector<std::string> ConvertString(const std::vector<std::string>& searchStrings);

		void ConstructIndexFile();
		void CullWordMap();

	private:
		// Private data members
		std::fstream m_indexFile;
		WordDataMapT m_wordMap;
		unsigned int m_wordMapSize = 0;
		unsigned int m_indexFileSize;
		bool m_useIndexFile = false;
		bool m_createIndexFile = false;
		const unsigned int m_indexPercentage = 20;
		std::string m_indexFilename;
		const unsigned int m_dirsearchDataSize;
		unsigned int m_maxFileSize = 0;
		char* m_readBuffer = nullptr;
		unsigned int m_readBufferSize = 1000000;	// 256 * 256;	// TODO will need to increase this!
		//unsigned char m_wordMapKeyMaxLength = MAX_SEARCH_WORD_SIZE - 1;			// This is the default unless the m_useIndexFile is set
		//int m_maxExpectedWordSize = 256;	// This will increase if a larger word is discovered
		bool m_putMapWordKeysInIndexFile = true;
		IndexConstructionState m_indexConstructionState = CountNumFilesEachWordIsIn;
		
		// Used by the CreateIndexForFile() and InsertWord() functions
		ExistingWordMapT m_tempExistingWordMap;
		std::string m_wholeWord;
		std::locale m_toLowerLocale;
		unsigned int m_currentWordLength;
		unsigned int m_fileWordCount;
		unsigned int m_wordBlockSize = 256;
		std::list<std::string> m_currentWordBlock;
		std::list<char> m_initPitPatternList;
	};

}
	
