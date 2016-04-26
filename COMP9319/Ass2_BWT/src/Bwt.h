#pragma once

#include <string>
#include <fstream>
#include <unordered_map>

namespace bwt {

	enum SearchMode
	{
		_a,
		_n,
		_r
	};


	class BWT
	{
	public:
		static const unsigned int COUNT_DATA_ARRAY_SIZE = 128;	//We have been assured that the data will be < 128
		typedef unsigned int CountT[COUNT_DATA_ARRAY_SIZE];
		typedef unsigned int* RankT;

		BWT(const std::string bwtInputFilename, 
			const std::string indexFilename);

		~BWT();

		void Search(const SearchMode searchMode, std::string& searchString);

	private:
		// private functions
		unsigned int BackwardSearch(std::string& searchString, 
			const bool findIndex = false, const unsigned int startRow = 0);
		void ConstructCountAndRank();
		unsigned int RankOcc(const unsigned char c, const unsigned int row);
		unsigned char GetBwtCharacter(const unsigned int row);
		unsigned int GetRankFromIndexFile(const unsigned int arrayIndex);

	private:
		// Private data members
		SearchMode m_searchMode;
		const std::string m_bwtInputFilename;
		const std::string m_indexFilename;
		std::ifstream m_bwtFile;
		std::fstream m_indexFile;
		char* m_readBuffer = 0;
		bool m_readDataFromFiles = false;
		bool m_useIndexFile = false;
		bool m_createIndexFile = false;
		char* m_bwtLastColumn = 0;
		unsigned int m_bwtDataSize = 0;
		unsigned int m_indexFileSize = 0;
		unsigned int m_bwtIndexSize = 0;
		unsigned int m_rankArraySize = 0; //RANK_ARRAY_SIZE
		unsigned int m_occuranceIntervals = 0;  //10000; // = 5;
		CountT m_countOfThisChar = { 0 };
		CountT m_countOfNextChar = { 0 };
		RankT m_rankData = 0;
	};

}
	