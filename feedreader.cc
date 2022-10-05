/*
Autor:      Martin Hlinský (xhlins01)
Projekt:    Síťové aplikace a správa sítí - Čtečka novinek ve formátu Atom a RSS s podporou TLS
Soubor:     feedreader.cc
*/

#include "feedreader.h"

int argParse(std::vector<std::string>& args,
             std::vector<std::string>& feedURLs,
             std::vector<std::string>& certStrings){
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

    for(size_t i = 1; i < args.size(); i++){
        if(args[i].find("-") != std::string::npos){
            foundFlag = false;
            if(args[i].find("-f") != std::string::npos){
                if(fFlag){
                    fprintf(stderr, "Každý parametr se může vyskytovat pouze jednou! 1\n");
                    exit(EXIT_FAILURE);
                }
                fFlag = true;
                foundFlag = true;

                i++;
                if(i >= args.size()){
                    fprintf(stderr, "Parametr -f musí následovat jméno souboru obsahující adresy!\n");
                    exit(EXIT_FAILURE);
                }
                //TODO idealne zamenit za regex, tady je sance, ze bude soubor jako feedfile.txt.pripona, coz neni dobre
                if(args[i].find(".txt") != std::string::npos){
                    try{
                        std::ifstream file(args[i]);
                        if (file.is_open()) {
                            std::string line;
                            while (std::getline(file, line)) {
                                // Remove whitespaces from the line
                                line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
                                
                                if(line[0] == '#' || line.empty()){
                                    continue;
                                } else{
                                    feedURLs.push_back(line);
                                }
                            }
                            file.close();
                            continue;
                        } else{
                            throw(21);
                        }
                    }
                    catch(int errNum) {
                        if(errNum == 21){
                            fprintf(stderr, "Soubor u parametru -f neexistuje nebo nebyl nalezen!\n");
                        }
                        exit(EXIT_FAILURE);
                    }
                } else{
                    fprintf(stderr, "Soubor u parametru -f musí mít koncovku .txt\n");
                    exit(EXIT_FAILURE);
                }
            }
            if(args[i].find("-c") != std::string::npos){
                if(cFlag){
                    fprintf(stderr, "Každý parametr se může vyskytovat pouze jednou! 2\n");
                    exit(EXIT_FAILURE);
                }
                cFlag = true;
                foundFlag = true;

                i++;
                if(i >= args.size()){
                    fprintf(stderr, "Parametr -c musí následovat jméno souboru obsahující certifikát!\n");
                    exit(EXIT_FAILURE);
                }

                try{
                    std::ifstream file(args[i]);
                    if (file.is_open()){
                        std::string outString = fileReader(file);
                        certStrings.push_back(outString);
                        file.close();
                    } else{
                        throw(21);
                    }
                    continue;
                }
                catch(int errNum) {
                    if(errNum == 21){
                        fprintf(stderr, "Soubor u parametru -f neexistuje nebo nebyl nalezen!\n");
                    }
                    exit(EXIT_FAILURE);
                }
            }
            if(args[i].find("-C") != std::string::npos){
                if(CFlag){
                    fprintf(stderr, "Každý parametr se může vyskytovat pouze jednou! 3\n");
                    exit(EXIT_FAILURE);
                } 
                CFlag = true;
                foundFlag = true;

                i++;
                if(i >= args.size()){
                    fprintf(stderr, "Parametr -C musí následovat jméno adresáře obsahující soubor s certifikátem!\n");
                    exit(EXIT_FAILURE);
                }

                std::string path = args[i];
                for(const auto& entry: std::filesystem::directory_iterator(path)){
                    try{
                        std::ifstream file(entry.path());
                        if(file.is_open()){
                            std::string outString = fileReader(file);
                            certStrings.push_back(outString);
                            file.close();
                        } else{
                            throw(21);
                        }
                    }
                    catch(int errNum) {
                        if(errNum == 21){
                            fprintf(stderr, "Soubor u parametru -f neexistuje nebo nebyl nalezen!\n");
                        }
                        exit(EXIT_FAILURE);
                    }
                }
                continue;
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
        if(std::regex_search(args[i], std::regex("^(http|https)://"))){
            if(fFlag){
                fprintf(stderr, "Každý parametr se může vyskytovat pouze jednou! 10\n");
                exit(EXIT_FAILURE);
            }
            fFlag = true;
            feedURLs.push_back(args[i]);
            continue;
        }
    }

    // TODO Debug printout flags
    //std::cout << "DEBUG: Flags:\n=======================================================\n" <<fFlag << cFlag << CFlag << TFlag << aFlag << uFlag << std::endl << "=======================================================" << std::endl;
    //if(fFlag){
    //    std::cout << "DEBUG: FEED URLs:\n=======================================================\n";
    //    for (auto feedURL: feedURLs){
    //        std::cout << feedURL << "\n";
    //    }
    //    std::cout << "=======================================================\n";
    //}
    //if(cFlag || CFlag){
    //    std::cout << "DEBUG: Cert Strings:\n=======================================================\n";
    //    for (auto certString: certStrings){
    //        std::cout << certString << "\n=======================================================";
    //    }
    //}
    //std::cout << std::endl;
    return 0;
}

int main(int argc, char **argv){
    std::vector<std::string> args(argv, argv+argc);

    std::vector<std::string> feedURLs;
    std::vector<std::string> certStrings;

    argParse(args, feedURLs, certStrings);
    return 0;
}

std::string fileReader(std::ifstream& file){
    std::string line;
    std::string outString;
    while (std::getline(file, line)) {
        outString.append(line);
    }
    return outString;
}