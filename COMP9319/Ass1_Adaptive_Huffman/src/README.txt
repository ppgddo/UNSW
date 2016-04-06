Assignement 1 - Tim Hale (z2273160)

I have put in a fair amount of comments in my source code. Some of it 
is in the "HelperFunctions.cpp" source file, but most of which is in the 
"AhEncodeFunctions.cpp" file, which has the bulk of the classes in it.
I'm following the pseudo code from Vitter's paper that was supplied 
quite closely, except I use a normal "binary tree" instead of the "floating 
tree". In order to do this, I'm implemented a "shifting" mechanism
(similar to bubble sort) to do the "slide and increment" and maintained
the parents and siblings of each node as they swap two at a time from
left to right (which shifts the target node to the right in the final 
destination and effectively slides each other node one spot to the 
left as required).
See the "SlideNodesHelper()", "SwapNodesHelper()" and 
"SlideAndIncrement()" functions for more details.

As the paper suggested, I keep a separate linked list for each
block, which groups different nodes of the same weight and type
together. I actually use a "std::unordered_map" to keep track
of all the different blocks. In particular, I have a "Block"
class which maintains a "std::unordered_map" of the 
"BlockContainer" class and using the weight as the key to access a
Block of the same weight, but internally the "BlockContainer" class 
which has two separate lists for the leaf and
internal node pointers. Each list is ordered, starting at the node with the
highest address and ending at the node with the lowest address. This order
is maintained throughout, which makes it easier to get the "highest node in
block" when the switching is required. All of the code that maintains
this data structure is in the "Block" and it's internal "BlockContainer"
class.

I should note that the vector of nodes ("m_nodeAddresses") is mostly 
obsolete, but I only kept it to quickly delete all of the node pointers
in the "Block" destructor to free the memory.

The updated binary tree is used to encode and decode the messages. The 
"EncodeOrDecode" class can do both, because the code is very similar
for each operation and a flag is passed to the constructor to indicate
which mode it is running in. This class reads each character and has
member functions for encoding and decoding the message based on the 
updated state of the tree to produce the output. It mainly calls the
"Block::InsertNode()" and "Block::SwapWithLeadingLeafNode()" as required 
based on what action is required.

In the words of the great Dr Wong, all this ensures that I maintain Vitter's
two core invariants to generate a dynamic Huffman code:

* implicit numbering (weights increase from bottom to top, left to right)

* For nodes with the same weights, leaves precede internal nodes.
