# I am a comment, and I want to say that the variable CC will be
# the compiler to use.
CC=g++
# Hey!, I am comment number 2. I want to say that CFLAGS will be the
# options I'll pass to the compiler.
CFLAGS=-c -Wall

all: ahencode ahdecode

ahencode: ahencode_main.o HelperFunctions.o
	$(CC) ahencode_main.o HelperFunctions.o -o ahencode

HelperFunctions.o: HelperFunctions.cpp
	$(CC) $(CFLAGS) HelperFunctions.cpp

ahencode_main.o: ahencode_main.cpp
	$(CC) $(CFLAGS) ahencode_main.cpp

       
ahdecode: ahdecode_main.o
	$(CC) ahdecode_main.o -o ahdecode

ahdecode_main.o: ahdecode_main.cpp
	$(CC) $(CFLAGS) ahdecode_main.cpp
    
    
#factorial.o: factorial.cpp
#	$(CC) $(CFLAGS) factorial.cpp

#ahencode.o: ahencode.cpp
#	$(CC) $(CFLAGS) ahencode.cpp

clean:
	rm *o ahencode ahdecode

