/*
Autor:      Martin Hlinský (xhlins01)
Projekt:    Síťové aplikace a správa sítí - Čtečka novinek ve formátu Atom a RSS s podporou TLS
Soubor:     argparse.h
*/

#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>
#include <regex>
#include <algorithm>
#include <filesystem>

struct parameters{
    std::vector<std::string> feedURLs;
    std::vector<std::string> certStrings;
    std::vector<std::string> certFolders;
    bool TFlag = false;
    bool aFlag = false;
    bool uFlag = false;
};

int argParse(std::vector<std::string>& args, struct parameters *params);