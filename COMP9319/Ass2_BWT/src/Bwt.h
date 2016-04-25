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
		static const unsigned int COUNT_DATA_ARRAY_SIZE = 256;	//sizeof(unsigned char) ^ 8;
		typedef unsigned int CountT[COUNT_DATA_ARRAY_SIZE];
		typedef std::unordered_map<unsigned char, unsigned int> RankMapT;
		typedef std::vector < RankMapT > RankT;

		BWT(const std::string bwtInputFilename, 
			const std::string indexFilename);

		void Search(const SearchMode searchMode, std::string& searchString);

	private:
		// private functions
		unsigned int BackwardSearch(std::string& searchString, 
			const bool findIndex = false, const unsigned int startRow = 0);
		void ConstructCountAndRank();
		unsigned int RankOcc(const unsigned char c, const unsigned int row);

	private:
		// Private data members
		SearchMode m_searchMode;
		const std::string m_bwtInputFilename;
		const std::string m_indexFilename;
		std::ifstream m_bwtFile;
		char* m_readBuffer;
		bool m_readDataFromFiles;
		char* m_bwtLastColumn;
		unsigned int m_bwtDataSize;
		CountT m_countOfThisChar = { 0 };
		CountT m_countOfNextChar = { 0 };
		RankT m_rankData;
	};

}
	