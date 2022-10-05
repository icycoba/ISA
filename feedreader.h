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
#include <libxml/parser.h>
#include <openssl/rsa.h>
#include <sys/socket.h>

int argParse(std::vector<std::string>& args, std::vector<std::string>& feedURLs, std::vector<std::string>& certStrings);
std::string fileReader(std::ifstream& file);