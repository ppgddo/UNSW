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
		static const unsigned int COUNT_DATA_ARRAY_SIZE = 128;	//256;	//sizeof(unsigned char) ^ 8;
		//static const unsigned int RANK_ARRAY_SIZE = 710 * COUNT_DATA_ARRAY_SIZE; // must be a multiple of COUNT_DATA_ARRAY_SIZE!!!
		typedef unsigned int CountT[COUNT_DATA_ARRAY_SIZE];
		//typedef std::unordered_map<unsigned char, unsigned int> RankMapT;
		//typedef std::vector < RankMapT > RankT;
		typedef unsigned int* RankT;
		//typedef unsigned int RankT[RANK_ARRAY_SIZE];


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

	private:
		// Private data members
		SearchMode m_searchMode;
		const std::string m_bwtInputFilename;
		const std::string m_indexFilename;
		std::ifstream m_bwtFile;
		char* m_readBuffer = 0;
		bool m_readDataFromFiles = false;
		bool m_useIndexFile = false;
		char* m_bwtLastColumn = 0;
		unsigned int m_bwtDataSize;
		unsigned int m_rankArraySize; //RANK_ARRAY_SIZE
		unsigned int m_occuranceIntervals = 0;  //10000; // = 5;
		CountT m_countOfThisChar = { 0 };
		CountT m_countOfNextChar = { 0 };
		RankT m_rankData;
	};

}
	