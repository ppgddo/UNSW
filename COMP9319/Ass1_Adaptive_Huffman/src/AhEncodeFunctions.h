#pragma once

#include <string>


namespace ah {
	namespace encode {

		struct Node 
		{
		public:

			Node(int address, char symbol,  int weight = 0,
				Node* parent = 0, Node* leftChild = 0, Node* rightChild = 0)
				: m_address(address)
				, m_symbol(symbol)
				, m_weight( weight)
				, m_parent(parent)
				, m_leftChild( leftChild)
				, m_rightChild( rightChild )
			{}

			int m_address;
			char m_symbol;
			int m_weight;
			Node* m_parent;
			Node* m_leftChild;
			Node* m_rightChild;
		};



		void EncodeMsg(const std::string& msg, const bool delimitMode);

	}
}