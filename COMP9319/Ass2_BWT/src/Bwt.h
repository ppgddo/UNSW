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
		Letter(unsigned int thisLetterCount, unsigned int nextLetterCount)
			: m_thisLetterCount(thisLetterCount)
			, m_nextLetterCount(nextLetterCount)
		{}

		Letter()
		{};
		
		unsigned int m_thisLetterCount;
		unsigned int m_nextLetterCount;
	};


	class BWT
	{
	public:
		typedef std::unordered_map<char, Letter> CountT;
		typedef std::unordered_map<char, unsigned int> RankMapT;
		typedef std::vector < RankMapT > RankT;

		BWT(
			const std::string bwtInputFilename, 
			const std::string indexFilename
			);

		void Search(const SearchMode searchMode, std::string& searchString);

	private:
		// private functions
		void BackwardSearch(std::string& bwtLastColumn, std::string& searchString);
		void ConstructCountAndRank(std::string& bwtLastColumn, 
								   CountT& countData, RankT& rankData );


	private:
		// Private data members
		SearchMode m_searchMode;
		const std::string m_bwtInputFilename;
		const std::string m_indexFilename;
		std::ifstream m_bwtFile;
	};

}
	