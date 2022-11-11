/*
Autor:      Martin Hlinský (xhlins01)
Projekt:    Síťové aplikace a správa sítí - Čtečka novinek ve formátu Atom a RSS s podporou TLS
Soubor:     feedreader.cc
*/

// Dokumentace
// https://wiki.openssl.org/index.php/SSL/TLS_Client
// https://developer.ibm.com/tutorials/l-openssl/

#include "feedreader.h"

int main(int argc, char **argv){
    std::vector<std::string> args(argv, argv+argc);

    struct parameters params;

    argParse(args, &params);
    retrieveXMLDocs(&params);
    return 0;
}