#pragma once

#include <vector>
#include <string>


namespace dirsearch {
	namespace helpers {
		
		
		void ParseCmdLine(int argc, char ** argv, std::vector<std::string>& outArray);

	}
}