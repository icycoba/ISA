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

/**
 * Pomocná struktura pro uložení hodnot parametrů
*/
struct parameters{
    std::vector<std::string> feedURLs;      /**Pole URL, které se bude zpracovávat*/
    std::vector<std::string> certStrings;   /**Pole souborů s certifikáty (použije se pouze první)*/
    std::vector<std::string> certFolders;   /**Pole složek, které se budou používat pro hledání certifikátl (použije se pouze první)*/
    bool TFlag = false;                     /**Příznak časové stopy*/
    bool aFlag = false;                     /**Příznak autora*/
    bool uFlag = false;                     /**Příznak URL*/
};

int argParse(std::vector<std::string>& args, struct parameters *params);