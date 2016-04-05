#include <stdio.h>
#include <iostream>
#include <string>
#include <list>
#include <queue>
#include <vector>
#include <unordered_map>
#include <map>
#include <cassert>
#include <algorithm>

// Internal headers
#include "AhEncodeFunctions.h"
#include "HelperFunctions.h"


static const bool DEBUG_MODE = false;


using namespace std;
using namespace ah::helpers;


namespace ah {
	namespace encode {

		

		//*********************************************************************
		//	Helper Functions
		//*********************************************************************

		bool IsLeafNode(Node* node)
		{
			if (node->m_leftChild || node->m_rightChild)
			{
				return false;
			}
			else
			{
				return true;
			}
		}


		void SwapNodesHelper(Node* &firstNode, Node* &secondNode, vector<Node*>& nodeAddresses)
		{
			int oldFirstAddress = firstNode->m_address;
			Node* oldFirstNode = firstNode;
			Node* oldFirstParent = oldFirstNode->m_parent;

			// Update the parents first (hardest part!!)
			if (oldFirstParent->m_rightChild == oldFirstNode)
			{
				oldFirstParent->m_rightChild = secondNode;
			}
			else
			{
				oldFirstParent->m_leftChild = secondNode;
			}

			Node* oldSecondParent = secondNode->m_parent;
			if (oldSecondParent->m_rightChild == secondNode)
			{
				oldSecondParent->m_rightChild = oldFirstNode;
			}
			else
			{
				oldSecondParent->m_leftChild = oldFirstNode;
			}

			
			// Swap the addresses
			nodeAddresses[oldFirstAddress] = secondNode;
			nodeAddresses[secondNode->m_address] = oldFirstNode;
			oldFirstNode->m_address = secondNode->m_address;
			secondNode->m_address = oldFirstAddress;

			// swap the parents and update their new children and their
			oldFirstNode->m_parent = secondNode->m_parent;
			secondNode->m_parent = oldFirstParent;

			if (DEBUG_MODE)
			{
				cout << "Debug: swapped nodes: " << firstNode->m_symbol << " and "
					<< secondNode->m_symbol << endl;
			}
		}


		void SlideNodesHelper(Node* &pNode, vector<Node*>& nodeVector, list<Node*>& nodeBlock)
		{
			// The special case is if the block only has one internal node, then
			// it's just a direct swap
			if (nodeBlock.size() == 1)
			{
				Node* singleNode = nodeBlock.front();	//nodeVector.at( (nodeBlock.front())->m_address);
				SwapNodesHelper(pNode, singleNode, nodeVector);
			}
			else
			{
				for(auto rit = nodeBlock.rbegin(); rit != nodeBlock.rend(); ++rit)
				{
					// Iterate backwards to swap the target node starting from the 
					// lowest node in block to highest
					Node* nextLowestNode = (*rit); // nodeVector.at(address);
					SwapNodesHelper(pNode, nextLowestNode, nodeVector);
				}
			}
		}














		//*********************************************************************
		//	Block Class
		//*********************************************************************



		class Block
		{
		public:
			
			// Define a contrainer that defines the functions required that doesn't
			// depend on the type of container used
			template<class T>
			class BlockContainer
			{
			public:
				typedef list<T> ContainerT;

				void push(const T& data, bool isLeaf)
				{
					if (isLeaf)
					{
						InsertHelper(data, m_blockOfLeafNodes);
					}
					else
					{
						InsertHelper(data, m_blockOfInternalNodes);
					}
				}

				bool IsEmpty(bool isLeaf)
				{
					if (isLeaf)
					{
						return m_blockOfLeafNodes.empty();
					}
					else
					{
						return m_blockOfInternalNodes.empty();
					}
				}


				void RemoveNode(const T& node, bool isLeaf)
				{
					if (isLeaf && !m_blockOfLeafNodes.empty() )
					{
						m_blockOfLeafNodes.remove(node);
					}
					else if(!m_blockOfInternalNodes.empty() )
					{
						m_blockOfInternalNodes.remove(node);
					}
					else if (DEBUG_MODE)
					{
						assert(0);
						cout << "Error! Removing node when container is empty! isLeaf = "
							<< isLeaf << endl;
					}
				}


				Node* HighestNode(const T value, bool isLeaf)
				{
					if (isLeaf && !m_blockOfLeafNodes.empty())
					{
						return m_blockOfLeafNodes.front();
					}
					else if (!m_blockOfInternalNodes.empty())
					{
						return m_blockOfInternalNodes.front();
					}
					else if (DEBUG_MODE)
					{
						assert(0);
						cout << "Error! Getting highest node when container is empty! isLeaf = "
							<< isLeaf << endl;
					}

					return 0;
				}


				void SlideInternalNodesForLeaf(Node* &pNode, vector<Node*>& nodeVector)
				{
					SlideNodesHelper(pNode, nodeVector, m_blockOfInternalNodes);
				}


				void SlideLeavesForInternalNodes(Node* &pNode, vector<Node*>& nodeVector)
				{
					SlideNodesHelper(pNode, nodeVector, m_blockOfLeafNodes);
				}




			private:
				void InsertHelper(const T& data, ContainerT& container)
				{
					// Insert data so that the container invariant is maintained:
					// (i.e. It's ordered from highest address to lowest address)
					// Find the correct position and use "insert" to put it in the
					// correct position instead of sorting and reversing (linear time instead 
					// of Nlog(N) time!)

					if (container.empty())
					{
						container.push_back(data);
					}
					else
					{
						int address = data->m_address;
						if (address < (container.back() )->m_address )
						{
							// If the address is lower than the last address in container,
							// insert at end
							container.push_back(data);
						}
						else
						{
							// Otherwise find the correct position to insert
							for (auto it = container.begin(); it != container.end(); ++it)
							{
								int nextAddress = (*it)->m_address;
								if (nextAddress < address)
								{
									// if the next address is lower than the one we are inserting
									// then this is the position to insert it
									container.insert(it, data);
									break;
								}
							}
						}
					}
				}

				// Private member Data of internal BlockContainer class:
				ContainerT m_blockOfLeafNodes;
				ContainerT m_blockOfInternalNodes;

			};	// End of internal BlockContainer class definition


			typedef BlockContainer< Node* > BlockContainerT;
			typedef std::unordered_map< int, BlockContainerT > BlockMapT;

			Block()
				: m_nodeAddresses(257, 0)
			{}


			~Block()
			{
				// Delete all the nodes
				for (auto it = m_nodeAddresses.begin(); it != m_nodeAddresses.end(); ++it)
				{
					if (*it)
					{
						delete(*it);
					}
				}
			}

			void InsertNode( Node*const newNode )
			{
				BlockMapT::const_iterator blockFound = m_existingQueues.find(newNode->m_weight);
				if (blockFound == m_existingQueues.end())
				{
					// If the isn't a block with this weight, create a new queue for it
					BlockContainerT newBlock;
					newBlock.push(newNode, IsLeafNode(newNode) );
					m_existingQueues[newNode->m_weight] = newBlock;
				}
				else
				{
					BlockContainerT& tempQueue = m_existingQueues[newNode->m_weight];
					tempQueue.push(newNode, IsLeafNode(newNode) );
				}

				m_nodeAddresses.at(newNode->m_address) = newNode;
			}


			void MakeNodeInternal(Node* node)
			{
				// remove it from the list of leaf nodes
				BlockContainerT& addressQueue = m_existingQueues[node->m_weight];
				addressQueue.RemoveNode(node,true);

				// then insert it
				InsertNode(node);
			}

			void UpdateNodeList(Node* node)
			{
				// remove it from the list of its node type (leaf or internal)
				BlockContainerT& addressQueue = m_existingQueues[node->m_weight];
				addressQueue.RemoveNode(node, IsLeafNode(node) );

				// then insert it
				InsertNode(node);
			}


			void SwapNodes(Node* firstNode, Node* secondNode)
			{
				SwapNodesHelper(firstNode, secondNode, m_nodeAddresses);

				// After swapping their order, update their order
				UpdateNodeList(firstNode);
				UpdateNodeList(secondNode);
			}

			void SlideAndIncrement(Node* &pNode)
			{
				Node* previousParent = pNode->m_parent;
				bool isLeaf = IsLeafNode(pNode);
				BlockContainerT& thisBlockQueue = m_existingQueues[pNode->m_weight];

				BlockMapT::const_iterator findNextBlock =
					m_existingQueues.find(pNode->m_weight + 1);
				bool nextBlockOfLeavesExist = false;
				if (findNextBlock != m_existingQueues.end())
				{
					nextBlockOfLeavesExist =
						!(m_existingQueues[pNode->m_weight + 1].IsEmpty(true));
				}

				if (isLeaf && !(thisBlockQueue.IsEmpty(false)))
				{
					// Slide pNode in the tree ahead of the nodes in the block of 
					// internal nodes with the same weight
					thisBlockQueue.SlideInternalNodesForLeaf(pNode, m_nodeAddresses);
				}
				else if (!isLeaf && nextBlockOfLeavesExist)
				{
					// Slide pNode in the tree ahead of the nodes in the block of 
					// leaf nodes with of weight = weight + 1
					BlockContainerT& nextBlockQueue = m_existingQueues[pNode->m_weight + 1];
					nextBlockQueue.SlideLeavesForInternalNodes(pNode, m_nodeAddresses);
				}

				// Before incrementing the weight of the node, remove it from it's current block,
				BlockContainerT& addressQueue = m_existingQueues[pNode->m_weight];
				addressQueue.RemoveNode(pNode, isLeaf);

				pNode->m_weight++;

				// Then move it into it's new block for it's new weight
				this->InsertNode(pNode);

				// adjust parent accordingly
				if (isLeaf)
				{
					pNode = pNode->m_parent;
				}
				else
				{
					pNode = previousParent;
				}
			}


			void SwapWithLeadingLeafNode(Node* currentNode)
			{
				Node* highestLeafNode = this->GetHighestLeafNode(currentNode);

				if (highestLeafNode && (highestLeafNode != currentNode ) )
				{
					this->SwapNodes(currentNode, highestLeafNode);
				}
			}

			Node* GetHighestLeafNode(Node* currentNode)
			{
				BlockMapT::const_iterator blockFound = m_existingQueues.find(currentNode->m_weight);
				if (blockFound == m_existingQueues.end())
				{
					assert(0);
					throw std::runtime_error("Error: this block doesn't exist yet!");
				}

				BlockContainerT addressQueue = m_existingQueues[currentNode->m_weight];
				return addressQueue.HighestNode(currentNode, true);
			}



		private:
			BlockMapT m_existingQueues;
			vector<Node*> m_nodeAddresses;
		};











		//*********************************************************************
		//	EncodeOrDecode class
		//*********************************************************************

		void EncodeOrDecode::ConstructSymbolFromTree(Node* startNode, std::string& outString)
		{
			// Construct the huffman code from the current binary tree

			outString.clear();
			Node* parent = startNode->m_parent;

			if (parent == 0)
				return;

			Node* nextSymbol = startNode;
			while (parent != 0)
			{
				if (parent->m_rightChild == nextSymbol)
				{
					outString.push_back('1');
				}
				else
				{
					outString.push_back('0');
				}
				nextSymbol = parent;
				parent = parent->m_parent;
			}

			std::reverse(outString.begin(), outString.end());
		}


		void EncodeOrDecode::ConstructEncodedMsg(
			const char& symbol,
			Node*const newNytNode, const bool symbolFound, const bool delimitMode)
		{
			// Transmit the current tree before updating it and only send the
			// symbol if it hasn't already been sent
			Node* transmitNode = newNytNode;
			if (symbolFound)
			{
				transmitNode = m_existingSymbols[symbol];
			}

			string symbolFromTree;
			ConstructSymbolFromTree(transmitNode, symbolFromTree);
			m_consoleOutput.append(symbolFromTree);

			if (!symbolFound)
			{
				// If it is a new symbol transmit it too
				ConvertCharToBinaryString(m_binaryString, symbol);
				m_consoleOutput.append(m_binaryString);
			}

			if (delimitMode)
			{
				// If we are in "delimit mode" (i.e. -s command option), add in a space
				m_consoleOutput.push_back(' ');
			}
		}


		bool EncodeOrDecode::DecodeSymbolFromTree(char& returnChar)
		{
			returnChar = 0;
			bool returnValue = false;

			if ( m_rootNode == 0 ) 
			{
				assert(0);
				return false;
			}
			else if (m_currentDecodingString.empty())
			{
				returnChar = m_rootNode->m_symbol;
				return true;
			}

			Node* nextNode = m_rootNode;

			for (const char& digit : m_currentDecodingString)
			{
				if(digit == '1')
					nextNode = nextNode->m_rightChild;
				else
					nextNode = nextNode->m_leftChild;

				if (nextNode == 0)
					break;
				else
					returnChar = nextNode->m_symbol;
			}

			//if (nodeCounter == m_currentDecodingString.size() + 1)
			if (nextNode && IsLeafNode(nextNode) )
			{
				// If we have reached a leaf node, we are finished decoding the tree
				returnValue = true;
				returnChar = nextNode->m_symbol;
			}

			return returnValue;
		}


		bool EncodeOrDecode::ConstructDecodedMsg(
			char& symbol, const bool delimitMode)
		{
			if (!DecodeSymbolFromTree(symbol) )
				return false;

			if (symbol == 0)
			{
				// Need to decode the binary reperesentation of the symbol
				m_decoderState = decodingNewCharacter;
				m_decoderCharCounter = 0;
				m_currentDecodingString.clear();
				return false;
			}

			m_currentDecodingString.clear();

			return true;
		}

		EncodeOrDecode::EncodeOrDecode(const std::string& msg, const bool delimitMode, const bool decode)
		{
			Block blocks;
			blocks.InsertNode(m_rootNode);

			if (DEBUG_MODE)
			{
				ConvertCharToBinaryString(m_binaryString, char(msg.front()));

				std::cout << endl <<  "(Debug Mode ON) ='" << msg << ", binary of first char = "
					<< m_binaryString << ", delimit mode = " << delimitMode << std::endl;
			}
						
			bool skipNextChar = false;

			// Process the msg string one char at a time
			const std::size_t messageSize = msg.size();
			std::size_t messageCounter = 0;
			for ( const char& nextChar : msg) 
			{
				if (skipNextChar)
				{
					skipNextChar = false;
					continue;
				}

				if (DEBUG_MODE)
					std::cout << "debug = " << nextChar << endl;

				bool symbolFound = false;

				char decodedSymbol = nextChar;
				if (decode)
				{
					// If we are decoding message
					m_currentDecodingString.push_back(nextChar);

					if ( m_decoderState == decodingNewCharacter )
					{
						// If we are currently decoding a new character
						if (++m_decoderCharCounter >= 8)
						{
							decodedSymbol = ConvertBinaryStringToChar(m_currentDecodingString);
							m_consoleOutput.push_back(decodedSymbol);
							m_decoderCharCounter = 0;
							m_decoderState = decodingTree;
							m_currentDecodingString.clear();
							skipNextChar = delimitMode;
						}
						else
						{
							continue;
						}
					}
					else
					{
						// otherwise we are currently decoding the huffman tree
						bool messageDecoded = ConstructDecodedMsg(
							decodedSymbol, delimitMode);

						if ( !messageDecoded )
							continue;
						else if (decodedSymbol)
						{
							m_consoleOutput.push_back(decodedSymbol);
							symbolFound = true;
							skipNextChar = delimitMode;
						}
					}
				}
				else
				{
					// Otherwise we are encoding a message
					symbolFound = 
						m_existingSymbols.find(nextChar) != m_existingSymbols.end();

					// If this is the last character in message, don't put in a trailing character
					bool putInSpace = delimitMode;
					if (++messageCounter == messageSize)
						putInSpace = false;

					ConstructEncodedMsg(
						nextChar, newNytNode, symbolFound, putInSpace);
				}

				// Now update the tree for new symbol
				Node* qNode = 0;
				Node* leafToIncrement = 0;
				
				if ( m_existingSymbols.find(decodedSymbol) == m_existingSymbols.end() )
				{
					// If this symbol is not yet transmitted (nyt), the nyt node will give "birth"
					// to new nyt node (as left child) and the new symbol (as right child).
					Node* oldNytNode = newNytNode;
					// Creat the two new child nodes and make the oldNytNode their parent
					Node* newSymbol = new Node(m_nextAddress--, decodedSymbol, 0, oldNytNode);
					newNytNode = new Node(m_nextAddress--, 0, 0, oldNytNode);
					m_existingSymbols[decodedSymbol] = newSymbol;
					oldNytNode->m_rightChild = newSymbol;
					oldNytNode->m_leftChild = newNytNode;

					// Update the block and address info for the new nodes
					blocks.InsertNode(newSymbol);
					blocks.InsertNode(newNytNode);

					// need to take the oldNytNode off the list of leaf nodes and onto the list
					// of internal nodes
					blocks.MakeNodeInternal(oldNytNode);

					//Update the current node to be the old nyt node
					qNode = oldNytNode;
					leafToIncrement = newSymbol;
				}
				else
				{
					// Else, if the symbol already exists
					// Swap it with the leading LEAF NODE of its block
					qNode = m_existingSymbols[decodedSymbol];
					blocks.SwapWithLeadingLeafNode(qNode);

					if (qNode->m_parent->m_leftChild == newNytNode)
					{
						// "Special Case #2" from Vitter's paper
						leafToIncrement = qNode;
						qNode = qNode->m_parent;

						if (DEBUG_MODE)
						{
							cout << "Special Case # 2 with leafToIncrement = "
								<< leafToIncrement->m_symbol << " and qNode = "
								<< qNode->m_symbol << endl;
						}
					}
				}

				assert(qNode != 0);

				while( (qNode != 0) )
				{
					blocks.SlideAndIncrement(qNode);
				}

				if (leafToIncrement)
				{
					blocks.SlideAndIncrement(leafToIncrement);
				}
				leafToIncrement = 0;


			}

			cout << m_consoleOutput << endl;
		}



	}
}