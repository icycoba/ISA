/*
Autor:      Martin Hlinský (xhlins01)
Projekt:    Síťové aplikace a správa sítí - Čtečka novinek ve formátu Atom a RSS s podporou TLS
Soubor:     xmlretrieve.cc
*/

/**
 * OpenSSL:
 * 
 * Copyright (c) 1998-2019 The OpenSSL Project.  All rights reserved.
 * This product includes software developed by the OpenSSL Project
 * for use in the OpenSSL Toolkit (http://www.openssl.org/)
 * 
 * This product includes cryptographic software written by Eric Young
 * (eay@cryptsoft.com).  This product includes software written by Tim
 * Hudson (tjh@cryptsoft.com).
 * 
 * OpenSSL License available at: https://www.openssl.org/source/license-openssl-ssleay.txt
*/

/**
 * Libxml2:
 * 
 * Copyright (C) 1998-2012 Daniel Veillard. All Rights Reserved.
 * 
 * Copyright (C) 1998 Bjorn Reese and Daniel Stenberg.
 * 
 * Copyright (C) 2000 Bjorn Reese and Daniel Stenberg.
 * 
 * Copyright (C) 2000 Gary Pennington and Daniel Veillard.
 * 
 * Copyright (C) 2001 Bjorn Reese <breese@users.sourceforge.net>
 * 
 * Copyright (C) 2000,2012 Bjorn Reese and Daniel Veillard.
*/

#include "xmlretrieve.h"

/**
 * Pomocná funkce sloužící pro průchod elementu <entry> nebo <item>
 * 
 * Inspirováno příkladem:
 * 
 * Dostupné na: https://gnome.pages.gitlab.gnome.org/libxml2/examples/
 * Autor: Alfred Mickautsch
 * Příklad: tree1.c
 * Prohlášení: Části kódu byly pozměněny a funkčností je stejný pouze průchodem stromem.
 * 
 * @param a_node    Uzel, na kterém se momentálně nachází čtečka XML
 * @param output    Struktura, do které se ukládají jednotlivé informace (název článku, autor, URL, aktualizace)
*/
static void retrieveXMLEntryContent(xmlNode* a_node, struct xmlOutput *output){
    xmlNode* cur_node = NULL;

    for(cur_node = a_node; cur_node; cur_node = cur_node->next){
        if(cur_node->type == XML_ELEMENT_NODE && cur_node->children){
            if(xmlStrEqual(cur_node->name, (xmlChar *)"title")){
                output->title = cur_node->children->content;
            } else if(xmlStrEqual(cur_node->name, (xmlChar *)"author")){
                output->author = cur_node->children->content;
            } else if(xmlStrEqual(cur_node->name, (xmlChar *)"name")){
                output->author = cur_node->children->content;
            } else if(xmlStrEqual(cur_node->name, (xmlChar *)"email") && output->author == NULL){
                output->author = cur_node->children->content;
            } else if(xmlStrEqual(cur_node->name, (xmlChar *)"link")){
                output->link = cur_node->children->content;
            } else if(xmlStrEqual(cur_node->name, (xmlChar *)"lastBuildDate") || xmlStrEqual(cur_node->name, (xmlChar *)"updated")){
                output->update = cur_node->children->content;
            } else if(xmlStrEqual(cur_node->name, (xmlChar *)"pubDate") && output->update == NULL){
                output->update = cur_node->children->content;
            } else if(xmlStrEqual(cur_node->name, (xmlChar *)"guid")){
                xmlAttr* cur_node_attr = NULL;
                for(cur_node_attr = cur_node->properties; cur_node_attr; cur_node_attr = cur_node_attr->next){
                    if(xmlStrEqual(cur_node_attr->name, (xmlChar *)"isPermaLink")){
                        if(xmlStrEqual(cur_node_attr->children->content, (xmlChar *)"true")){
                            output->link = cur_node->children->content;
                        }
                    }
                }
            }
        } else if(cur_node->type == XML_ELEMENT_NODE){
            // Zpracování Atom odkazu
            if(xmlStrEqual(cur_node->name, (xmlChar *)"link")){
                xmlAttr* cur_node_attr = NULL;
                xmlChar* tempURL = NULL;
                bool relFlag = false;

                for(cur_node_attr = cur_node->properties; cur_node_attr; cur_node_attr = cur_node_attr->next){
                    if(xmlStrEqual(cur_node_attr->name, (xmlChar *)"href")){
                        if(!relFlag){
                            tempURL = cur_node_attr->children->content;
                        } else{
                            output->link = cur_node_attr->children->content;
                        }
                    }
                    if(xmlStrEqual(cur_node_attr->name, (xmlChar *)"rel")){
                        if(xmlStrEqual(cur_node_attr->children->content, (xmlChar *)"alternate")){
                            relFlag = true;
                            if(tempURL){
                                output->link = tempURL;
                            }
                        }
                    }
                }
            }
        }
        retrieveXMLEntryContent(cur_node->children, &(*output));
    }
}

/**
 * Pomocná funkce sloužící pro výpis XML v určitém formátu
 * 
 * Inspirováno příkladem:
 * 
 * Dostupné na: https://gnome.pages.gitlab.gnome.org/libxml2/examples/
 * Autor: Alfred Mickautsch
 * Příklad: tree1.c
 * Prohlášení: Části kódu byly pozměněny a funkčností je stejný pouze průchodem stromem.
 * 
 * @param a_node            Uzel, na kterém se momentálně nachází čtečka XML
 * @param titleFound        Pomocná proměnná sloužící k usnadnění rozlišení typu elementu <title> v dokumentu
 * @param globalAuthorFound Pomocná proměnná sloužící k usnadnění výpisu autora nad prvním <entry>
 * @param params            Struktura uchovávající hodnoty parametrů
*/
static void printFormattedXML(xmlNode* a_node, bool titleFound, bool globalAuthorFound, struct parameters *params){
    xmlNode* cur_node = NULL;
    xmlChar* globalAuthor = NULL;

    for(cur_node = a_node; cur_node; cur_node = cur_node->next){
        struct xmlOutput output;
        if(globalAuthor){
            output.author = globalAuthor;
        }
        if(cur_node->type == XML_ELEMENT_NODE && cur_node->children){
            if(!titleFound && xmlStrEqual(cur_node->name, (xmlChar *)"title")){
                printf("*** %s ***\n", cur_node->children->content);
                titleFound = true;
            } else if(!globalAuthorFound && xmlStrEqual(cur_node->name, (xmlChar *)"author")){
                // Pokud je globální autor, tzn. autor nad <entry> taggem,
                // uloží se do proměnné globalAuthor a použije se pro případ,
                // že se nenalezne autor uvnitř <entry>
                if(xmlStrEqual(cur_node->children->name, (xmlChar *)"name")){
                    globalAuthor = cur_node->children->content;
                } else if(xmlStrEqual(cur_node->children->name, (xmlChar *)"email") && output.author == NULL){
                    globalAuthor = cur_node->children->content;
                } else{
                    globalAuthor = cur_node->children->content;
                }
                output.author = globalAuthor;

                globalAuthorFound = true;
            } else if(xmlStrEqual(cur_node->name, (xmlChar *)"item") || xmlStrEqual(cur_node->name, (xmlChar *)"entry")){
                retrieveXMLEntryContent(cur_node->children, &output);
                printf("%s\n", output.title);
                if(params->aFlag)
                    printf("Autor: %s\n", (output.author == NULL)?(xmlChar*)"Nepodařilo se vyhledat autora článku":output.author);
                if(params->uFlag)
                    printf("URL: %s\n", (output.link == NULL)?(xmlChar*)"Nepodařilo se vyhledat odkaz k článku":output.link);
                if(params->TFlag)
                    printf("Aktualizace: %s\n", (output.update == NULL)?(xmlChar*)"Nepodařilo se vyhledat datum poslední aktualizace článku":output.update);
                printf("\n");
            }
        }
        printFormattedXML(cur_node->children, titleFound, globalAuthorFound, &(*params));
    }
}


/**
 * Pomocná funkce sloužící pro nastavení kontextu pro navázání připojení
 * 
 * Vypracováno podle příkladů:
 * 
 * Dostupný na: https://wiki.openssl.org/index.php/SSL/TLS_Client
 * Autor:       Viktor Dukhovni
 * Příklad:     openssl-bio-fetch.tar.gz
 * Prohlášení:  Část kódu je převzata z výše uvedené stránky
 *              z důvodu, že se jedná o inicializační funkce.
 *  
 * Dostupný na: https://developer.ibm.com/tutorials/l-openssl/
 * Autor:       Kenneth Ballard
 * Datum:       15. srpna 2018
 * Příklad:     Úseky kódu na zmíněné URL
 * Prohlášení:  Část kódu je převzata z výše uvedené stránky
 *              z důvodu, že se jedná o inicializační funkce.
 * 
 * @param ctx Struktura obsahující informace o SSL
*/
static void initCTX(SSL_CTX *ctx){
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);

    SSL_CTX_set_verify_depth(ctx, 4);

    const long flags = SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_COMPRESSION;
    SSL_CTX_set_options(ctx, flags);
}

/**
 * Pomocná funkce sloužící pro navázání připojení
 * 
 * Dostupný na: https://wiki.openssl.org/index.php/SSL/TLS_Client
 * Autor:       Viktor Dukhovni
 * Příklad:     openssl-bio-fetch.tar.gz
 * Prohlášení:  Část kódu je převzata z výše uvedené stránky
 *              z důvodu, že se jedná o inicializační funkce.
 *  
 * Dostupný na: https://developer.ibm.com/tutorials/l-openssl/
 * Autor:       Kenneth Ballard
 * Datum:       15. srpna 2018
 * Příklad:     Úseky kódu na zmíněné URL
 * Prohlášení:  Část kódu je převzata z výše uvedené stránky
 *              z důvodu, že se jedná o inicializační funkce.
 * 
 * @param bio       Ukazatel sloužící pro komunikaci
 * @param ssl       Ukazatel uchovávající informace o SSL
 * @param hostname  Hostitelské jméno vzdáleného serveru
 * @param bioURL    Adresa ve tvaru hostname:port
 * @param feedURL   Adresa ve tvaru protocol://hostname/path
 * 
 * @return 0 při úspěšném dokončení, 1 pokud je funkce neúspěšná
*/
static int setupBIO(BIO** bio, SSL **ssl, std::string hostname, const char* bioURL, std::string feedURL){
    long res = 1;

    BIO_get_ssl(*bio, ssl);
    if(! (ssl != NULL)){
        fprintf(stderr, "Funkce BIO_get_ssl() selhala!\n");
        return 1;
    }

    SSL_set_mode(*ssl, SSL_MODE_AUTO_RETRY);
    const char* const PREFERRED_CIPHERS = "HIGH:!aNULL:!kRSA:!PSK:!SRP:!MD5:!RC4";
    res = SSL_set_cipher_list(*ssl, PREFERRED_CIPHERS);
    if(!(1 == res)){
        fprintf(stderr, "Funkce SSL_set_cipher_list() selhala!\n");
        return 1;
    }

    res = SSL_set_tlsext_host_name(*ssl, hostname.c_str());
    if(!(1 == res)){
        fprintf(stderr, "Funkce SSL_set_tlsext_host_name() selhala!\n");
        return 1;
    }

    if(bio == NULL){
        fprintf(stderr, "Nepodařilo se vytvořit připojení!\n");
        return 1;
    }

    if(! BIO_set_conn_hostname(*bio, bioURL)){
        fprintf(stderr, "Funkce BIO_set_conn_hostname() selhala!\n");
        return 1;
    }

    if(BIO_do_connect(*bio) <= 0){
        BIO_reset(*bio);
        *bio = BIO_new_connect(bioURL);
        if(*bio == NULL){
            fprintf(stderr, "Nepodařilo se vytvořit připojení!\n");
            return 1;
        }
        if(BIO_do_connect(*bio) <= 0){
            fprintf(stderr, "Nepodařilo se připojit k odkazu %s!\n", feedURL.c_str());
            return 1;
        }
    }

    if(SSL_get_verify_result(*ssl) != X509_V_OK){
        fprintf(stderr, "Nepodařilo se ověřit platnost certifikátu!\n");
        return 1;
    }
    return 0;
}

/**
 * Funkce sloužící k navázání připojení a následnému zpracovávání XML z feedu
 * 
 * @param params Struktura uchovávající hodnoty parametrů
*/
void retrieveXMLDocs(struct parameters *params){
    // Pokud je certStrings(Folders) prázdné, pak jsou proměnné NULL, jinak se použije hodnota na 0. indexu
    const char* CAfile = (params->certStrings.empty()) ? NULL : params->certStrings[0].c_str();
    const char* CApath = (params->certFolders.empty()) ? NULL : params->certFolders[0].c_str();

    /**
     * Dostupný na: https://wiki.openssl.org/index.php/SSL/TLS_Client
     * Autor:       Viktor Dukhovni
     * Příklad:     openssl-bio-fetch.tar.gz
     * Prohlášení:  Část kódu v následujícím úseku je převzata z výše
     *              uvedené stránky z důvodu, že se jedná o inicializační funkce.
    */
    SSL_library_init();
    SSL_load_error_strings();
    ERR_load_BIO_strings();
    OpenSSL_add_all_algorithms();
    
    SSL_CTX *ctx =  SSL_CTX_new(SSLv23_client_method());
    BIO *bio =      NULL;
    SSL *ssl =      NULL;
    
    initCTX(ctx);

    if(!CAfile && !CApath){
        SSL_CTX_set_default_verify_paths(ctx);
    }else{
        if(! SSL_CTX_load_verify_locations(ctx, CAfile, CApath)){
            fprintf(stderr, "Nepodařilo se ověřit platnost certifikátu!\n");
            exit(EXIT_FAILURE);
        }
    }
    /**
     * Konec převzaté části kódu
    */

    for(auto feedURL : params->feedURLs){
        while(1){
            std::size_t foundIndex;
            std::string protocol;
            std::string path;
            std::string hostname;
            const char* bioURL;

            try{
                foundIndex = feedURL.find_first_of(":");
                protocol = feedURL.substr(0, foundIndex);
                feedURL = feedURL.substr(foundIndex+3);

                // Pokud se vyskytuje port v odkazu, pak jej použijeme místo protokolu
                if(feedURL.find(":") != std::string::npos){
                    protocol = feedURL.substr(feedURL.find(":")+1);
                    protocol = protocol.substr(0, protocol.find("/"));
                }

                foundIndex = feedURL.find_first_of("/");
                path = feedURL.substr(foundIndex, feedURL.size()-1);
                feedURL = feedURL.substr(0, foundIndex);
                hostname = feedURL;

                // Pokud je port nastavený, musíme upravit proměnnou feedURL
                if(hostname.find(":") != std::string::npos){
                    feedURL = feedURL.substr(0, feedURL.find(":"));
                }

                bioURL = feedURL.append(":").append(protocol).c_str();
                bio = BIO_new_ssl_connect(ctx);
            } catch(...){
                fprintf(stderr, "Chyba ve zpracování odkazu!\n");
                break;
            }

            if(setupBIO(&bio, &ssl, hostname, bioURL, feedURL)){
                BIO_free_all(bio);
                break;
            }

            /**
             * Poskládá požadavek ve tvaru:
             * 
             * GET <path> HTTP/1.0
             * Host: <hostname>
             * Connection: close
             * 
             * 
            */
            std::string requestParts = "GET ";
            requestParts.append(path).append(
                    " HTTP/1.0\r\nHost: ").append(hostname).append(
                    "\r\nConnection: close\r\n\r\n"
                );
            const char* request = requestParts.c_str();
            BIO_puts(bio, request);

            std::string response;
            int len = 0;

            // Čtení a ukládání odpovědi do proměnné response
            do{
                char buf[1024] = {};
                len = BIO_read(bio, buf, sizeof(buf)-1);

                if(len > 0){
                    response.append(buf);
                }

            } while (len > 0 || BIO_should_retry(bio));

            BIO_reset(bio);

            // Získání prvního řádku z odpovědi response (pro účely získání statusu)
            std::istringstream f(response);
            std::string responseHeader;
            std::getline(f, responseHeader);

            // Odpověď 301, 302, 307, 308 implikuje přesměrování a obsahuje vždy Location:
            // Odpověď 200 je OK, tudíž program pokračuje do zpracování XML
            // Jiné odpovědi nejsou programem podporovány
            if( responseHeader.find("301") != std::string::npos ||
                responseHeader.find("302") != std::string::npos ||
                responseHeader.find("307") != std::string::npos ||
                responseHeader.find("308") != std::string::npos){
                if(response.find("\nLocation") != std::string::npos){
                    std::string tempString = response;

                    size_t posIndex = response.find("\nLocation");
                    tempString = tempString.substr(posIndex+11);
                    tempString = tempString.substr(0, tempString.find("\n")-1);

                    feedURL = tempString;
                }
            } else if(responseHeader.find("200") != std::string::npos){
                BIO_free_all(bio);
                std::string xmlResponse = response;

                if(response.find("<?xml") != std::string::npos){
                    xmlResponse = response.substr(response.find("<?xml"));
                }

                xmlDoc *doc = NULL;
                xmlNode *root_element = NULL;

                doc = xmlReadDoc((const xmlChar*)xmlResponse.c_str(), NULL, NULL, 0); // 

                if(doc == NULL){
                    fprintf(stderr, "Nepodařilo se otevřít dokument XML!\n");
                    break;
                }

                root_element = xmlDocGetRootElement(doc);
                if(root_element == NULL){
                    fprintf(stderr, "Nepodařilo se načíst kořenový element dokumentu XML!\n");
                    break;
                }

                printFormattedXML(root_element, false, false, &(*params));

                xmlFreeDoc(doc);
                xmlCleanupParser();

                break;
            } else{
                fprintf(stderr, "Neplatná odpověď HTML! %s\n", responseHeader.c_str());
                break;
            }
            BIO_free_all(bio);
        }
    }
}