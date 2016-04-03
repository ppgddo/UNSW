#include <stdio.h>
#include <iostream>
#include <string>
#include <list>
#include <queue>
#include <vector>
#include <unordered_map>
#include <map>
#include <cassert>

// Internal headers
#include "AhEncodeFunctions.h"
#include "HelperFunctions.h"


static const bool DEBUG_MODE = true;


using namespace std;
using namespace ah::helpers;


namespace ah {
	namespace encode {

		class Block
		{
		public:
			
			// Define a contrainer that defines the functions required that doesn't
			// depend on the container used
			template<class T>
			class BlockContainer
			{
			public:
				void push(const T& data)
				{
					m_blockQueue.push(data);
					m_blockMap[data] = data;
				}

				void pop()
				{
					m_blockMap.erase(m_blockQueue.top());
					m_blockQueue.pop();
				}

				const T& top()
				{
					return m_blockQueue.top();
				}

				Node* HighestNodeWithoutChildren(const T value, vector<Node*>& nodeVector)
				{
					Node* highestNodeWithoutChildren = 0;
					for (auto it = m_blockMap.begin(); it != m_blockMap.end(); ++it)
					{
						T address = it->second;
						if (address > value)
						{
							Node* higherNode = nodeVector.at(address);
							if ((higherNode->m_leftChild == 0) &&
								(higherNode->m_rightChild == 0))
							{
								highestNodeWithoutChildren = higherNode;
							}
						}
					}
					return highestNodeWithoutChildren;
				}

			private:
				priority_queue<T> m_blockQueue;
				map<T, T> m_blockMap;
			};

			typedef priority_queue< int > HigherAddressQueueT;
			typedef BlockContainer< int > BlockContainerT;
			typedef std::unordered_map< int, BlockContainerT > BlockMapT;

			Block::Block()
				: m_nodeAddresses(257, 0)
			{}

			void InsertNode( Node*const newNode )
			{
				BlockMapT::const_iterator blockFound = m_existingQueues.find(newNode->m_weight);
				if (blockFound == m_existingQueues.end())
				{
					// If the isn't a block with this weight, create a new queue for it
					BlockContainerT newBlock;
					newBlock.push(newNode->m_address);
					m_existingQueues[newNode->m_weight] = newBlock;
				}
				else
				{
					// Check if this is an internal node (i.e. has children) and if so, make sure it has
					// a higher address than any leaf nodes (i.e. have no children ) by swapping it
					// with any leaf node that has a higher address.

					BlockContainerT& tempQueue = m_existingQueues[newNode->m_weight];

					if ( newNode->m_leftChild || newNode->m_rightChild )
					{
						Node* highestNodeWithoutChildren =
							tempQueue.HighestNodeWithoutChildren(newNode->m_address, m_nodeAddresses);
						if (highestNodeWithoutChildren)
						{
							SwapNodes(highestNodeWithoutChildren, newNode);
						}
					}
					tempQueue.push(newNode->m_address);
				}

				m_nodeAddresses.at(newNode->m_address) = newNode;
			}

			void SwapNodes(Node* firstNode, Node* secondNode)
			{
				int oldFirstAddress = firstNode->m_address;
				Node* oldFirstNode = firstNode;
				Node* oldFirstParent = oldFirstNode->m_parent;
				m_nodeAddresses[oldFirstAddress] = secondNode;
				m_nodeAddresses[secondNode->m_address] = oldFirstNode;
				oldFirstNode->m_address = secondNode->m_address;
				secondNode->m_address = oldFirstAddress;

				// swap the parents and update their new children (the order this is done is critical!)
				oldFirstNode->m_parent = secondNode->m_parent;
				oldFirstNode->m_parent->m_rightChild = oldFirstNode;
				secondNode->m_parent = oldFirstParent;
				secondNode->m_parent->m_rightChild = secondNode;
				if (DEBUG_MODE)
				{
					cout << "Debug: swapped nodes: " << firstNode->m_symbol << " and "
						<< secondNode->m_symbol << endl;
				}
			}

			void SwitchThenIncrementNodeWeight( Node* currentNode)
			{
				int highestBlockAddress =
					this->getHighestAddress(currentNode->m_address, currentNode->m_weight);
				Node* currentHighestNode = m_nodeAddresses[highestBlockAddress];
				if( (highestBlockAddress != currentNode->m_address) 
					&& (currentHighestNode != currentNode->m_parent ) 
					&& (currentHighestNode != currentNode->m_rightChild) )
				{
					// If it's not currently the highest address in the block
					// and the highest addrss isn't it's parent (critical!), then swap it
					SwapNodes(currentHighestNode, currentNode);
				}

				// Before incrementing the weight of the node, remove it from it's current block
				BlockContainerT& addressQueue = m_existingQueues[currentNode->m_weight];
				addressQueue.pop();
				currentNode->m_weight++;

				// Then move it into it's new block for it's new weight
				this->InsertNode(currentNode);
			}


			int getHighestAddress(const int address, const int weight)
			{
				BlockMapT::const_iterator blockFound = m_existingQueues.find(weight);
				if (blockFound == m_existingQueues.end())
				{
					assert(0);
					throw std::runtime_error("Error: this block doesn't exist yet!");
				}

				BlockContainerT addressQueue = m_existingQueues[weight];
				return addressQueue.top();
			}

		private:
			BlockMapT m_existingQueues;
			vector<Node*> m_nodeAddresses;
		};




		void EncodeMsg(const string& msg, const bool delimitMode)
		{
			if (DEBUG_MODE)
			{
				std::string binaryString;
				ConvertBaseVersion(binaryString, char(msg.front()));

				std::cout << "(Debug Mode ON) ='" << msg << ", binary of first char = "
					<< binaryString << ", delimit mode = " << delimitMode << std::endl;
			}
						
			// Initialise the variables and create the NYT node as the root.
			typedef std::unordered_map< char, Node* > SymbolMapT;
			SymbolMapT existingSymbols;
			Block blocks;

			Node* rootNode = new Node( 256, 0, 0, 0);	// Init NYT (null) is the root node
			//existingSymbols[0] = rootNode;
			Node* newNytNode = rootNode;
			blocks.InsertNode(rootNode);
			int nextAddress = 255;
			//bool treeIsEmpty = true;

			// Process the msg string one char at a time
			for ( const char& symbol : msg) 
			{
				if (DEBUG_MODE)
					std::cout << "debug = " << symbol << endl;

				Node* currentNode = 0;

				SymbolMapT::const_iterator symbolFound = existingSymbols.find(symbol);
				if (symbolFound == existingSymbols.end())
				{
					// If this symbol is not yet transmitted (nyt), the nyt node will give "birth"
					// to new nyt node (as left child) and the new symbol (as right child).
					Node* oldNytNode = newNytNode;
					// Creat the two new child nodes and make the oldNytNode their parent
					Node* newSymbol = new Node(nextAddress--, symbol, 1, oldNytNode);
					newNytNode = new Node(nextAddress--, 0, 0, oldNytNode);
					//existingSymbols[0] = newNytNode;
					existingSymbols[symbol] = newSymbol;
					oldNytNode->m_rightChild = newSymbol;
					oldNytNode->m_leftChild = newNytNode;

					// Update the block and address info for the new nodes
					blocks.InsertNode(newSymbol);
					blocks.InsertNode(newNytNode);

					//Update the current node to be the old nyt node
					currentNode = oldNytNode;
				}
				else
				{
					currentNode = existingSymbols[symbol];
				}

				assert(currentNode != 0);

				while( (currentNode != 0) )	// || ( treeIsEmpty ) ) //|| (currentNode != rootNode) ) )
				{
					blocks.SwitchThenIncrementNodeWeight(currentNode);
					currentNode = currentNode->m_parent;
					//treeIsEmpty = false;
				}


				// TODO Iterate through existingSymbols map and delete all nodes to prevent memory leak!!!
				// Do this in the "Block" desctructor!

			}
		}



	}
}