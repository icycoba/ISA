/*
Autor:      Martin Hlinský (xhlins01)
Projekt:    Síťové aplikace a správa sítí - Čtečka novinek ve formátu Atom a RSS s podporou TLS
Soubor:     feedreader.h
*/

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <regex>
#include <algorithm>
#include <filesystem>
// #include <getopt.h> // NOPE, JUST CANT TODO REMOVEME
#include <libxml/parser.h>
#include <openssl/rsa.h>
#include <sys/socket.h>

namespace fs = std::filesystem;

std::string fileReader(std::ifstream& file);