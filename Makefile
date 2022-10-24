# Autor: Martin Hlinský (xhlins01)
# Projekt: Síťové aplikace a správa sítí - Čtečka novinek ve formátu Atom a RSS s podporou TLS

.PHONY: all clean

BIN=feedreader
OBJFILES=$(BIN).o

CXX=g++
CFLAGS= -Wall

all: $(BIN)

$(BIN):
	g++ -std=c++17 -o feedreader feedreader.cc -lssl -lcrypto

clean: $(BIN)
	rm $(BIN)