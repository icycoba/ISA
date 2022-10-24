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
    std::cout << "\033[32mZačátek běhu programu\033[0m\n";
    std::vector<std::string> args(argv, argv+argc);

    std::vector<std::string> feedURLs;
    std::vector<std::string> certStrings;
    std::vector<std::string> certFolders;

    argParse(args, feedURLs, certStrings, certFolders);

    const char* CAfile = (certStrings.empty()) ? NULL : certStrings[0].c_str();
    const char* CApath = (certFolders.empty()) ? NULL : certFolders[0].c_str();

    long res = 1;

    SSL_CTX *ctx =  SSL_CTX_new(SSLv23_client_method());
    BIO *bio =      NULL;
    BIO *out =      NULL;
    SSL *ssl =      NULL;

    SSL_load_error_strings();
    ERR_load_BIO_strings();
    OpenSSL_add_all_algorithms();
    SSL_library_init();

    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);

    SSL_CTX_set_verify_depth(ctx, 4);

    const long flags = SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_COMPRESSION;
    SSL_CTX_set_options(ctx, flags);

    if(! SSL_CTX_load_verify_locations(ctx, CAfile, CApath))
    {
        fprintf(stderr, "susi husi\n");
        std::cout << CAfile << std::endl << CApath << std::endl;
        exit(EXIT_FAILURE);
    }

    for(auto feedURL : feedURLs){
        std::size_t foundIndex = feedURL.find_first_of(":");
        std::string protocol = feedURL.substr(0, foundIndex);

        feedURL = feedURL.substr(foundIndex+3);
        foundIndex = feedURL.find_first_of("/");
        std::string path = feedURL.substr(foundIndex, feedURL.size()-1);
        feedURL = feedURL.substr(0, foundIndex);
        std::string hostname = feedURL;

        // TODO IF CUSTOM PORT
        const char* bioURL = feedURL.append(":").append(protocol).c_str();
        std::cout << bioURL << std::endl;
        std::cout << bio << std::endl;
        bio = BIO_new_ssl_connect(ctx);

        std::cout << bioURL << std::endl;

        if(! BIO_set_conn_hostname(bio, bioURL)){
            fprintf(stderr, "bio_set_conn_hostname failed\n");
            exit(EXIT_FAILURE);
        };

        BIO_get_ssl(bio, &ssl);
        if(! (ssl != NULL)){
            fprintf(stderr, "bio_get_ssl failed\n");
            exit(EXIT_FAILURE);
        }

        const char* const PREFERRED_CIPHERS = "HIGH:!aNULL:!kRSA:!PSK:!SRP:!MD5:!RC4";
        res = SSL_set_cipher_list(ssl, PREFERRED_CIPHERS);
        if(!(1 == res)) fprintf(stderr, "ssl_set_cipher_list failed\n");

        res = SSL_set_tlsext_host_name(ssl, hostname.c_str());
        if(!(1 == res)) fprintf(stderr, "ssl_set_tlsext_host_name failed\n");
        
        if(bio == NULL){
            fprintf(stderr, "\033[31mNepodařilo se vytvořit připojení!\033[0m\n");
            BIO_free_all(bio);
            exit(EXIT_FAILURE);
        }

        std::cout << bio << std::endl;
        if(BIO_do_connect(bio) <= 0){
            fprintf(stderr, "\033[31mNepodařilo se připojit k odkazu %s!\033[0m\n", feedURL.c_str());
            BIO_free_all(bio);
            exit(EXIT_FAILURE);
        }

        std::cout << "\033[32mPřipojeno\033[0m\n";

        
        /*if(SSL_do_handshake(ssl) <= 0){
            fprintf(stderr, "\033[31mNepodařilo se dokončit handshake\033[0m\n");
            BIO_free_all(bio);
            exit(EXIT_FAILURE);
        }*/
        std::cout << "DEBUG\n";
//        int x = BIO_read(bio, buf, len);
//        if(x == 0){
//            /* HANDLE CLOSED CONNECTION */
//        } else if(x < 0){
//            if(!BIO_should_retry(bio)){
//                /* HANDLE FAILED READ HERE */
//            }
//
//            /* DO SOMETHING TO HANDLE THE RETRY */
//        }
        
        std::string requestParts = "GET ";
        requestParts.append(path).append(
                " HTTP/1.0\r\nHost: ").append(hostname).append(
                "\r\nConnection: close\r\n\r\n"
            );
        const char* request = requestParts.c_str();
        //BIO_puts(bio, request);

        out = BIO_new_fp(stdout, BIO_NOCLOSE);
        BIO_puts(bio, request);
        BIO_puts(out, "\n");

        int len = 0;
        do{
            char buf[1024] = {};
            len = BIO_read(bio, buf, sizeof(buf));

            if(len > 0){
                BIO_write(out, buf, len);
                std::cout << buf << std::endl;
            }
        } while (len > 0 || BIO_should_retry(bio));

        BIO_reset(bio);
        BIO_reset(out);
        std::cout << "\033[32mResetováno\033[0m\n";
    }
    BIO_free_all(bio);
    std::cout << "\033[32mKonec běhu programu\033[0m\n";
    return 0;
}

int argParse(std::vector<std::string>& args,
             std::vector<std::string>& feedURLs,
             std::vector<std::string>& certStrings,
             std::vector<std::string>& certFolders){
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
                    certStrings.push_back(args[i]);
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
                //for(const auto& entry: std::filesystem::directory_iterator(path)){
                //    try{
                //        certStrings.push_back(entry.path());
                //    }
                //    catch(int errNum) {
                //        if(errNum == 21){
                //            fprintf(stderr, "Soubor u parametru -f neexistuje nebo nebyl nalezen!\n");
                //        }
                //        exit(EXIT_FAILURE);
                //    }
                //}
                certFolders.push_back(path);
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
        } else{
            fprintf(stderr, "Zadaná adresa (%s) je neplatná!\n", args[i].c_str());
            exit(EXIT_FAILURE);
        }
    }

    // TODO Debug printout flags
    std::cout << "DEBUG: Flags:\n=======================================================\n" <<fFlag << cFlag << CFlag << TFlag << aFlag << uFlag << std::endl << "=======================================================" << std::endl;
    if(fFlag){
        std::cout << "DEBUG: FEED URLs:\n=======================================================\n";
        for (auto feedURL: feedURLs){
            std::cout << feedURL << "\n";
        }
        std::cout << "=======================================================\n";
    }
    if(cFlag || CFlag){
        std::cout << "DEBUG: Cert Strings:\n=======================================================\n";
        for (auto certString: certStrings){
            std::cout << certString << "\n=======================================================";
        }
        for (auto certFolder: certFolders){
            std::cout << certFolder << "\n=======================================================";
        }
    }
    std::cout << std::endl;
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