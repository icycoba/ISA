/*
Autor:      Martin Hlinský (xhlins01)
Projekt:    Síťové aplikace a správa sítí - Čtečka novinek ve formátu Atom a RSS s podporou TLS
Soubor:     feedreader.h
*/

#include <iostream>
#include <cstring>
#include <vector>
#include <regex>
// #include <getopt.h> // NOPE, JUST CANT TODO REMOVEME
#include <libxml/parser.h>
#include <openssl/rsa.h>
#include <sys/socket.h>