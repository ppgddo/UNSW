#include <stdio.h>
#include <iostream>
#include <string>
#include <list>
#include <queue>
#include <vector>
#include <unordered_map>
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
			typedef std::priority_queue< int > BlockQueueT;
			typedef std::unordered_map< int, BlockQueueT > BlockMapT;

			Block::Block()
				: m_nodeAddresses(257, 0)
			{}

			void InsertNode( Node*const newNode )
			{
				BlockMapT::const_iterator blockFound = m_existingQueues.find(newNode->m_weight);
				if (blockFound == m_existingQueues.end())
				{
					// If the isn't a block with this weight, create a new queue for it
					BlockQueueT newBlock;
					newBlock.push(newNode->m_address);
					m_existingQueues[newNode->m_weight] = newBlock;
				}
				else
				{
					m_existingQueues[newNode->m_weight].push(newNode->m_address);
				}

				m_nodeAddresses.at(newNode->m_address) = newNode;
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
					Node* oldHighestNode = currentHighestNode;
					Node* oldHighestParent = oldHighestNode->m_parent;
					m_nodeAddresses[highestBlockAddress] = currentNode;
					m_nodeAddresses[currentNode->m_address] = oldHighestNode;
					oldHighestNode->m_address = currentNode->m_address;
					currentNode->m_address = highestBlockAddress;

					// swap the parents and update their new children (the order this is done is critical!)
					oldHighestNode->m_parent = currentNode->m_parent;
					oldHighestNode->m_parent->m_rightChild = oldHighestNode;
					currentNode->m_parent = oldHighestParent;
					currentNode->m_parent->m_rightChild = currentNode;
					if (DEBUG_MODE)
					{
						cout << "Debug: parents swapped!" << endl;
					}
				}

				// Before incrementing the weight of the node, remove it from it's current block
				BlockQueueT& addresssQueue = m_existingQueues[currentNode->m_weight];
				addresssQueue.pop();
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

				BlockQueueT& addresssQueue = m_existingQueues[weight];
				return addresssQueue.top();
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