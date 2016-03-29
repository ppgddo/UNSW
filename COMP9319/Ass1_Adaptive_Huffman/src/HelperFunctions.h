#pragma once

#include <list>

namespace ah {
	namespace helpers {
		
		
		void ParseCmdLine(int argc, char ** argv, std::list<std::string>& outArray);
		bool SetDelimitMode(int argc, char ** argv);

	}
}