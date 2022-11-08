XMLLDFLAGS!=pkg-config --libs libxml-2.0
XMLLDFLAGS?=$(shell pkg-config --libs libxml-2.0)
XMLCFLAGS!=pkg-config --cflags libxml-2.0
XMLCFLAGS?=$(shell pkg-config --cflags libxml-2.0)

CXXFLAGS:=$(XMLCFLAGS) -std=c++17 -g -Wall
LDLIBS:=$(XMLLDFLAGS) -lssl -lcrypto
PRJ=feedreader

$(PRJ): feedreader.cc argparse.cc xmlretrieve.cc

test:
	./test.sh
clean:
	rm -f *.o $(PRJ)