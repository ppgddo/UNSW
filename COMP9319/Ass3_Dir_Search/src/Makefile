# I am a comment, and I want to say that the variable CC will be
# the compiler to use.
CC=g++
# Hey!, I am comment number 2. I want to say that CFLAGS will be the
# options I'll pass to the compiler.
CFLAGS=-c -m32 -Wall -std=c++11 

all: dirsearchsearch

HelperFunctions.o: HelperFunctions.cpp
	$(CC) $(CFLAGS) HelperFunctions.cpp

DirSearchMain.o: DirSearchMain.cpp
	$(CC) $(CFLAGS) DirSearchMain.cpp
    
DirSearch.o: DirSearch.cpp
	$(CC) $(CFLAGS) DirSearch.cpp
       
dirsearchsearch: DirSearchMain.o HelperFunctions.o DirSearch.o
	$(CC) DirSearchMain.o HelperFunctions.o DirSearch.o -o dirsearchsearch

clean:
	rm *o dirsearchsearch

