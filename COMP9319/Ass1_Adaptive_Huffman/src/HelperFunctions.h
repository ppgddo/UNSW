#pragma once

#include <list>
#include <string>


namespace ah {
	namespace helpers {
		
		
		void ParseCmdLine(int argc, char ** argv, std::list<std::string>& outArray);
		bool SetDelimitMode(int argc, char ** argv);
		bool SetDebugMode(int argc, char ** argv);

		
		void ConvertCharToBinaryString(std::string& output, char input);
		char ConvertBinaryStringToChar(const std::string& binaryStr);

	}
}