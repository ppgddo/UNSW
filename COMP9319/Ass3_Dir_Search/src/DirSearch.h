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
		SuffixFileData( short fileIndex, int wordCount ) // std::list<char> & bitPatterns)
			: m_fileIndex(fileIndex)
			, m_wordCount(wordCount)
			//, m_bitPatterns(bitPatterns)
		{}

		short m_fileIndex;
		int m_wordCount;
		//std::list<char> m_bitPatterns;	// TODO use this instead when I implement "Phrase search"

	private:
		SuffixFileData(const SuffixFileData&) = delete;
		SuffixFileData& operator=(const SuffixFileData&);
	};


	typedef std::list<SuffixFileData * > SuffixFileDataListT;



	struct SuffixData
	{
	public:
		SuffixData(const std::string& suffixString)
			: m_suffixString(suffixString)
		{
			m_suffixFileData = new SuffixFileDataListT;
		}


		~SuffixData()
		{
			// TODO delete the pointers on the m_suffixFileData as well!
			delete m_suffixFileData;
		}

		void InsertFileData(SuffixFileData* suffixFileData)
		{
			// Update the file data
			m_suffixFileData->push_back(suffixFileData);
		}


		SuffixFileDataListT * GetFileData()
		{
			return m_suffixFileData;
		}


		const std::string m_suffixString;
		// Put all data for each "suffix" for a given "prefix" 
		// i.e. search word = preffix (key of PrefixMap) + suffix
		SuffixFileDataListT * m_suffixFileData;

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

		Suffix(const std::string& suffixString)
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


		SuffixFileDataListT* GetFileData(const std::string& suffixString)
		{
			if ((m_suffixList != nullptr))
			{
				for (auto existingSuffixes = m_suffixList->begin();
				existingSuffixes != m_suffixList->end(); ++existingSuffixes)
				{
					if (suffixString == (*existingSuffixes)->m_suffixString)
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
				if ((m_singleSuffix != nullptr) && (m_singleSuffix->m_suffixString != suffixString))
				{
					return m_singleSuffix->GetFileData();
				}
				else
				{
					return nullptr;
				}
			}
		}


		void InsertSuffix(const std::string& suffix, SuffixFileData* suffixFileData)
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


	class DirSearch
	{
	public:
		//static const unsigned int PREFIX_ARRAY_DIM = 27;	//We have been assured that the data will be < 128
		static const unsigned int MIN_SEARCH_WORD_SIZE = 3;
		static const unsigned int MAX_SEARCH_WORD_SIZE = 256;

		//typedef unsigned char SuffixArrayT[PREFIX_ARRAY_DIM*m_prefixMapKeyMaxLength];
		typedef std::unordered_map<std::string, Suffix* > PrefixMapT;
		//typedef std::unordered_map<std::string, std::list<char> > ExistingWordMapT;	// for "bit pattern" impl
		typedef std::unordered_map<std::string, int > ExistingWordMapT;
		typedef char* FilenameArrayT[2000];

		DirSearch(const std::string indexFilename, const int indexPercentage);

		~DirSearch();

		void Search(const std::vector<std::string>& searchStrings);



	private:
		// private functions
		void InsertWord();
		void CreateIndexForFile(const char* const fileName, short fileArrayIndex);

		SuffixFileDataListT* SearchAllFiles(const std::string& searchTerm);

		void InsertIntoPrefixMap(const std::string & prefix, const std::string & suffix, const short fileArrayIndex, const int fileWordCount);
		SuffixFileDataListT* SearchPreFixMap(const std::string & prefix, const std::string & suffix);

		std::vector<std::string> ConvertString(const std::vector<std::string>& searchStrings);

	private:
		// Private data members
		std::fstream m_indexFile;
		PrefixMapT m_prefixMap;
		int m_indexFileSize;
		bool m_useIndexFile = false;
		bool m_createIndexFile = false;
		const int m_indexPercentage = 20;
		std::string m_indexFilename;
		const int m_dirsearchDataSize;
		unsigned int m_maxFileSize = 0;
		char* m_readBuffer = nullptr;
		unsigned int m_readBufferSize = 256 * 256;	// TODO might need to increase this!
		char m_prefixMapKeyMaxLength = 5;			// This is the default unless the m_useIndexFile is set
		int m_maxExpectedWordSize = 256;	// This will increase if a larger word is discovered
		
		
		// Used by the CreateIndexForFile() and InsertWord() functions
		ExistingWordMapT m_tempExistingWordMap;
		std::string m_prefixPath;
		std::string m_suffixPath;
		std::locale m_toLowerLocale;
		int m_currentWordLength;
		int m_fileWordCount;
		int m_wordBlockSize = 256;
		std::list<std::string> m_currentWordBlock;
		std::list<char> m_initPitPatternList;
	};

}
	