# I am a comment, and I want to say that the variable CC will be
# the compiler to use.
CC=g++
# Hey!, I am comment number 2. I want to say that CFLAGS will be the
# options I'll pass to the compiler.
CFLAGS=-c -Wall -std=c++11

all: ahencode ahdecode

ahencode: AhEncodeMain.o HelperFunctions.o AhEncodeFunctions.o
	$(CC) AhEncodeMain.o HelperFunctions.o AhEncodeFunctions.o -o ahencode

HelperFunctions.o: HelperFunctions.cpp
	$(CC) $(CFLAGS) HelperFunctions.cpp

AhEncodeMain.o: AhEncodeMain.cpp
	$(CC) $(CFLAGS) AhEncodeMain.cpp
    
AhEncodeFunctions.o: AhEncodeFunctions.cpp
	$(CC) $(CFLAGS) AhEncodeFunctions.cpp
       
ahdecode: AhDecodeMain.o
	$(CC) AhDecodeMain.o -o ahdecode

AhDecodeMain.o: AhDecodeMain.cpp
	$(CC) $(CFLAGS) AhDecodeMain.cpp
    
    
#factorial.o: factorial.cpp
#	$(CC) $(CFLAGS) factorial.cpp

#ahencode.o: ahencode.cpp
#	$(CC) $(CFLAGS) ahencode.cpp

clean:
	rm *o ahencode ahdecode

