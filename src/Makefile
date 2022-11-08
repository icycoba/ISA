# Autor: Martin Hlinský (xhlins01)
# Projekt: Síťové aplikace a správa sítí - Čtečka novinek ve formátu Atom a RSS s podporou TLS

.PHONY: all clean test

BIN=feedreader

CXX=g++
CXXFLAGS= -I/usr/include/libxml2 -I/usr/local/include/libxml2 -std=c++17 -g -Wall 
LDLIBS= -L/usr/local/lib -lxml2 -lssl -lcrypto

all: $(BIN)

$(BIN): feedreader.cc argparse.cc xmlretrieve.cc

test:
	./feedreadertest.sh

clean:
	rm -f $(BIN)