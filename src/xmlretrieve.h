/*
Autor:      Martin Hlinský (xhlins01)
Projekt:    Síťové aplikace a správa sítí - Čtečka novinek ve formátu Atom a RSS s podporou TLS
Soubor:     xmlretrieve.h
*/

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <sys/socket.h>
#include <netdb.h>

#include <vector>
#include <string>

#include "argparse.h"

struct xmlOutput{
    xmlChar* title  = NULL;
    xmlChar* author = NULL;
    xmlChar* link   = NULL;
    xmlChar* update = NULL;
};

void retrieveXMLDocs(struct parameters *params);