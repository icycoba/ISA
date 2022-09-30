/*
Autor:      Martin Hlinský (xhlins01)
Projekt:    Síťové aplikace a správa sítí - Čtečka novinek ve formátu Atom a RSS s podporou TLS
Soubor:     feedreader.cc
*/

#include "feedreader.h"

int main(int argc, char **argv){
    std::vector<std::string> args(argv, argv+argc);

    bool fFlag = false;
    bool cFlag = false;
    bool CFlag = false;
    bool TFlag = false;
    bool aFlag = false;
    bool uFlag = false;
    bool TTempFlag = false;
    bool aTempFlag = false;
    bool uTempFlag = false;
    bool foundFlag = false;
    std::string fFile;

    for(size_t i = 1; i < args.size(); i++){
        //TODO remove after basic argparse
        if(args[i].find("-") != std::string::npos){
            foundFlag = false;
            if(args[i].find("-f") != std::string::npos){
                if(fFlag){
                    fprintf(stderr, "Každý parametr se může vyskytovat pouze jednou! 1\n");
                    exit(EXIT_FAILURE);
                }
                fFlag = true;
                foundFlag = true;
            }
            if(args[i].find("-c") != std::string::npos){
                if(cFlag){
                    fprintf(stderr, "Každý parametr se může vyskytovat pouze jednou! 2\n");
                    exit(EXIT_FAILURE);
                }
                cFlag = true;
                foundFlag = true;
            }
            if(args[i].find("-C") != std::string::npos){
                if(CFlag){
                    fprintf(stderr, "Každý parametr se může vyskytovat pouze jednou! 3\n");
                    exit(EXIT_FAILURE);
                }
                CFlag = true;
                foundFlag = true;
            }
            if(std::regex_search(args[i], std::regex("-\\b(?!\\w*(\\w)\\w*\\1)[Tau]+\\b"))){
                // regex taken from https://stackoverflow.com/a/13546700
                if(args[i].find("T") != std::string::npos){
                    if(TFlag && !TTempFlag){
                        fprintf(stderr, "Každý parametr se může vyskytovat pouze jednou! 7\n");
                        exit(EXIT_FAILURE);
                    }
                    TFlag = true;
                }
                if(args[i].find("a") != std::string::npos){
                    if(aFlag && !aTempFlag){
                        fprintf(stderr, "Každý parametr se může vyskytovat pouze jednou! 8\n");
                        exit(EXIT_FAILURE);
                    }
                    aFlag = true;
                }
                if(args[i].find("u") != std::string::npos){
                    if(uFlag && !uTempFlag){
                        fprintf(stderr, "Každý parametr se může vyskytovat pouze jednou! 9\n");
                        exit(EXIT_FAILURE);
                    }
                    uFlag = true;
                }
                foundFlag = true;
            } else if(std::regex_search(args[i], std::regex("[Tau]+"))){
                foundFlag = false;
            }
            if(!foundFlag){
                char* err_char;
                err_char = &args[i][0];
                fprintf(stderr, "Neznámý argument %s\n", err_char);
                exit(EXIT_FAILURE);
            }
            TTempFlag = false;
            aTempFlag = false;
            uTempFlag = false;
        }
        // TODO Debug printout args
        //std::cout << args[i] << std::endl;
    }
    // TODO Debug printout flags
    std::cout << fFlag << cFlag << CFlag << TFlag << aFlag << uFlag << std::endl;
    return 0;
}