#include <stdio.h>
#include <iostream>
#include <string>
#include <list>


#include "AhEncodeFunctions.h"


static const bool DEBUG_MODE = true;


using namespace std;

namespace ah {
	namespace encode {



		void EncodeMsg(const string& msg, const bool delimitMode)
		{
			if (DEBUG_MODE)
			{
				std::cout << "(Debug Mode ON) ='" << msg << ", delimit mode = " << delimitMode << "'\n";
			}
		}



	}
}