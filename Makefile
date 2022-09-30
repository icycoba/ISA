# Autor: Martin Hlinský (xhlins01)
# Projekt: Síťové aplikace a správa sítí - Čtečka novinek ve formátu Atom a RSS s podporou TLS

.PHONY: all clean

BIN=feedreader
OBJFILES=$(BIN).o

CXX=g++
CFLAGS=-o -Wall

%.o: %.cc
	$(CXX) -c $<

all: $(BIN)

$(BIN): $(OBJFILES)
	$(CXX) $(CFLAGS) $(OBJFILES) -o $@

clean:
	rm $(BIN) $(OBJFILES)