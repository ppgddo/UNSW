#pragma once

#include <list>
#include <string>


namespace ah {
	namespace helpers {
		
		
		void ParseCmdLine(int argc, char ** argv, std::list<std::string>& outArray);
		bool SetDelimitMode(int argc, char ** argv);

		
		void ConvertBaseVersion(std::string& output, char input, const int base = 2, int digits = 8);

	}
}