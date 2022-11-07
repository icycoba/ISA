#include "argparse.h"

int argParse(std::vector<std::string>& args,
             struct parameters *params){
    bool fFlag = false;
    bool cFlag = false;
    bool CFlag = false;
    bool TFlag = false;
    bool aFlag = false;
    bool uFlag = false;
    bool foundFlag = false;

    if(args.size() <= 1){
        fprintf(stderr, "Použití:\n");
        fprintf(stderr, "feedreader <URL | -f <feedfile>> [-c <certfile>] [-C <certaddr>] [-T] [-a] [-u]\n");
        fprintf(stderr, "<URL | -f <feedfile>>\n\tZdroj ve formátu Atom nebo RSS\n\tPovinný parametr\n");
        fprintf(stderr, "[-c <certfile>]\n\tSoubor s certifikáty, který se použije pro ověřeni platnosti certifikátu SSL/TLS předloženého serverem\n\tNepovinný parametr\n");
        fprintf(stderr, "[-C <certaddr>]\n\tAdresář, ve kterém se mají vyhledávat certifikáty, které se použijí pro ověření platnosti certifikátu předloženého serverem\n\tNepovinný parametr\n");
        fprintf(stderr, "[-T]\n\tZobrazí navíc informace o čase změny záznamu či vytvoření záznamu\n\tNepovinný parametr\n");
        fprintf(stderr, "[-a]\n\tZobrazí jméno autora či jeho e-mailovou adresu\n\tNepovinný parametr\n");
        fprintf(stderr, "[-u]\n\tZobrazí asociované URL\n\tNepovinný parametr\n");
        exit(EXIT_FAILURE);
    }

    for(size_t i = 1; i < args.size(); i++){
        if(args[i][0] == '-'){
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
                                    params->feedURLs.push_back(line);
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
                    params->certStrings.push_back(args[i]);
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
                params->certFolders.push_back(path);
                continue;
            }
            if(std::regex_search(args[i], std::regex("-\\b(?!\\w*(\\w)\\w*\\1)[Tau]+\\b"))){
                // regex taken from https://stackoverflow.com/a/13546700
                if(args[i].find("T") != std::string::npos){
                    if(TFlag){
                        fprintf(stderr, "Každý parametr se může vyskytovat pouze jednou! 7\n");
                        exit(EXIT_FAILURE);
                    }
                    TFlag = true;
                }
                if(args[i].find("a") != std::string::npos){
                    if(aFlag){
                        fprintf(stderr, "Každý parametr se může vyskytovat pouze jednou! 8\n");
                        exit(EXIT_FAILURE);
                    }
                    aFlag = true;
                }
                if(args[i].find("u") != std::string::npos){
                    if(uFlag){
                        fprintf(stderr, "Každý parametr se může vyskytovat pouze jednou! 9\n");
                        exit(EXIT_FAILURE);
                    }
                    uFlag = true;
                }
                foundFlag = true;
                continue;
            } else if(std::regex_search(args[i], std::regex("[Tau]+"))){
                foundFlag = false;
            }
            if(!foundFlag){
                char* err_char;
                err_char = &args[i][0];
                fprintf(stderr, "Neznámý argument %s\n", err_char);
                exit(EXIT_FAILURE);
            }
        }
        if(std::regex_search(args[i], std::regex("^(http|https)://"))){
            if(fFlag){
                fprintf(stderr, "Každý parametr se může vyskytovat pouze jednou! 10\n");
                exit(EXIT_FAILURE);
            }
            fFlag = true;
            params->feedURLs.push_back(args[i]);
            continue;
        } else{
            fprintf(stderr, "Zadaná adresa (%s) je neplatná!\n", args[i].c_str());
            exit(EXIT_FAILURE);
        }
    }

    params->aFlag = aFlag;
    params->TFlag = TFlag;
    params->uFlag = uFlag;

    // TODO Debug printout flags
    /*
    std::cout << "DEBUG: Flags:\n=======================================================\n" <<fFlag << cFlag << CFlag << TFlag << aFlag << uFlag << std::endl << "=======================================================" << std::endl;
    if(fFlag){
        std::cout << "DEBUG: FEED URLs:\n=======================================================\n";
        for (auto feedURL: params->feedURLs){
            std::cout << feedURL << "\n";
        }
        std::cout << "=======================================================\n";
    }
    if(cFlag || CFlag){
        std::cout << "DEBUG: Cert Strings:\n=======================================================\n";
        for (auto certString: params->certStrings){
            std::cout << certString << "\n=======================================================";
        }
        for (auto certFolder: params->certFolders){
            std::cout << certFolder << "\n=======================================================";
        }
    }
    std::cout << std::endl;
    */
    return 0;
}