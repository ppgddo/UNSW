# I am a comment, and I want to say that the variable CC will be
# the compiler to use.
CC=g++
# Hey!, I am comment number 2. I want to say that CFLAGS will be the
# options I'll pass to the compiler.
CFLAGS=-c -Wall -std=c++11

all: bwtsearch

HelperFunctions.o: HelperFunctions.cpp
	$(CC) $(CFLAGS) HelperFunctions.cpp

BwtMain.o: BwtMain.cpp
	$(CC) $(CFLAGS) BwtMain.cpp
    
Bwt.o: Bwt.cpp
	$(CC) $(CFLAGS) Bwt.cpp
       
bwtsearch: BwtMain.o HelperFunctions.o Bwt.o
	$(CC) BwtMain.o HelperFunctions.o Bwt.o -o bwtsearch

clean:
	rm *o bwtsearch

