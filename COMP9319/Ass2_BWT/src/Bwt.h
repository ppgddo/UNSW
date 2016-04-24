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


	struct Letter
	{
		Letter(std::string::size_type thisLetterCount, std::string::size_type nextLetterCount)
			: m_thisLetterCount(thisLetterCount)
			, m_nextLetterCount(nextLetterCount)
		{}

		Letter()
		{};
		
		std::string::size_type m_thisLetterCount;
		std::string::size_type m_nextLetterCount;
	};


	class BWT
	{
	public:
		typedef std::unordered_map<char, Letter> CountT;
		//typedef std::unordered_map<char, std::string::size_type> RankMapT;
		//typedef std::vector < RankMapT > RankT;

		BWT(const std::string bwtInputFilename, 
			const std::string indexFilename);

		void Search(const SearchMode searchMode, std::string& searchString);

	private:
		// private functions
		unsigned int BackwardSearch(std::string& searchString, 
			const bool findIndex = false, const std::string::size_type startRow = 0);
		void ConstructCountAndRank();
		std::string::size_type RankOcc(const char c, const std::string::size_type row);

	private:
		// Private data members
		SearchMode m_searchMode;
		const std::string m_bwtInputFilename;
		const std::string m_indexFilename;
		std::string m_bwtLastColumn;
		std::ifstream m_bwtFile;
		CountT m_countData;
		//RankT m_rankData;
	};

}
	