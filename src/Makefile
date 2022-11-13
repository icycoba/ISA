# Autor: Martin Hlinský (xhlins01)
# Projekt: Síťové aplikace a správa sítí - Čtečka novinek ve formátu Atom a RSS s podporou TLS

.PHONY: all clean test pack

BIN=feedreader

CXX=g++
CXXFLAGS=-static-libstdc++ -I/usr/include/libxml2 -I/usr/local/include/libxml2 -std=c++17 -g -Wall 
LDLIBS=-L/usr/local/lib -lxml2 -lssl -lcrypto

all: feedreader.o argparse.o xmlretrieve.o
	$(CXX) $(CXXFLAGS) feedreader.o argparse.o xmlretrieve.o -o $(BIN) $(LDLIBS)

feedreader.o: feedreader.cc feedreader.h argparse.h xmlretrieve.h
	$(CXX) $(CXXFLAGS) -c feedreader.cc $(LDLIBS)

xmlretrieve.o: xmlretrieve.cc argparse.h xmlretrieve.h
	$(CXX) $(CXXFLAGS) -c xmlretrieve.cc $(LDLIBS)

argparse.o: argparse.cc argparse.h
	$(CXX) $(CXXFLAGS) -c argparse.cc $(LDLIBS)

pack: clean
	mkdir ../xhlins01
	tar -cvf ../xhlins01/xhlins01.tar ../src ../tests ../docs ../README

test: all
	./feedreadertests.sh

clean:
	rm -f *.tar *.o $(BIN)
	rm -rf ../xhlins01