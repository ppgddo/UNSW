#pragma once

#include <string>
#include <unordered_map>


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

		class EncodeOrDecode
		{
		public:
			typedef std::unordered_map< char, Node* > SymbolMapT;

			EncodeOrDecode(const std::string& msg, const bool delimitMode, const bool decode);
			
		private:
			// Member data
			// Initialise the variables.
			SymbolMapT m_existingSymbols;
			enum DecoderStates
			{
				decodingNewCharacter = 0,
				decodingTree
			};

			Node* m_rootNode = new Node(256, 0, 0, 0);	// Init NYT (null) is the root node
			Node* newNytNode = m_rootNode;				// Init the NYT node as the root
			int m_nextAddress = 255;
			int m_decoderCharCounter = 0;
			DecoderStates m_decoderState = decodingNewCharacter;
			std::string m_binaryString;
			std::string m_consoleOutput;
			std::string m_currentDecodingString;


			// Private member functions
			void ConstructSymbolFromTree(Node* startNode, std::string& outString);

			void ConstructEncodedMsg(
				const char& symbol,
				Node*const newNytNode, const bool symbolFound, const bool delimitMode);

			bool DecodeSymbolFromTree(char& returnChar);

			bool ConstructDecodedMsg(
				char& symbol,
				const bool delimitMode);
		};


	}
}