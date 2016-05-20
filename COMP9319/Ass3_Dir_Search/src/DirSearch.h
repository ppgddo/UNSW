#pragma once

#include <string>
#include <fstream>
#include <unordered_map>

namespace dirsearch 
{


	class DirSearch
	{
	public:
		static const unsigned int PREFIX_ARRAY_DIM = 27;	//We have been assured that the data will be < 128
		static const unsigned int MAX_SEARCH_WORD_SIZE = 256;	//We have been assured that the data will be < 128

		//typedef unsigned char SuffixArrayT[PREFIX_ARRAY_DIM*m_prefixArrayDimension];
		typedef char* SuffixArrayT;
		typedef std::unordered_map<std::string, char> ExistingWordMapT;

		DirSearch(const std::string indexFilename, const int indexPercentage);

		~DirSearch();

		void Search(const std::vector<std::string>& searchStrings);

		void SearchFile(const char* const fileName, short fileArrayIndex);

	private:
		// private functions
		void InsertWord(std::string& prefixPath, std::string& suffixPath, unsigned int currentWordPosition);

	private:
		// Private data members
		std::fstream m_indexFile;
		SuffixArrayT m_prefixArray;
		int m_indexFileSize;
		bool m_useIndexFile = false;
		bool m_createIndexFile = false;
		const int m_indexPercentage = 20;
		std::string m_indexFilename;
		const int m_dirsearchDataSize;
		unsigned int m_maxFileSize = 0;
		char* m_readBuffer = 0;
		unsigned int m_readBufferSize = 256 * 256;	// TODO might need to increase this!
		char m_prefixArrayDimension = 4;			// This is the default unless the m_useIndexFile is set
		unsigned int m_maxExpectedWordSize = 256;	// This will increase if a larger word is discovered
		ExistingWordMapT m_tempExistingWordMap;

	};

}
	