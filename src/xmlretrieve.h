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

/**
 * Pomocná struktura pro výpis formátovaného výstupu
*/
struct xmlOutput{
    xmlChar* title  = NULL; /**Titulek článku*/
    xmlChar* author = NULL; /**Autor článku (jméno nebo e-mail)*/
    xmlChar* link   = NULL; /**Odkaz na článek*/
    xmlChar* update = NULL; /**Poslední aktualizace článku*/
};

void retrieveXMLDocs(struct parameters *params);