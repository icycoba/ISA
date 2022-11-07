#include "xmlretrieve.h"

// https://gnome.pages.gitlab.gnome.org/libxml2/examples/
static void retrieveXMLEntryContent(xmlNode* a_node, struct xmlOutput *output){
    xmlNode* cur_node = NULL;

    for(cur_node = a_node; cur_node; cur_node = cur_node->next){
        if(cur_node->type == XML_ELEMENT_NODE && cur_node->children){
            if(xmlStrEqual(cur_node->name, (xmlChar *)"title")){
                output->title = cur_node->children->content;
            } else if(xmlStrEqual(cur_node->name, (xmlChar *)"name")){
                output->author = cur_node->children->content;
            } else if(xmlStrEqual(cur_node->name, (xmlChar *)"link")){
                output->link = cur_node->children->content;
            } else if(xmlStrEqual(cur_node->name, (xmlChar *)"pubDate") || xmlStrEqual(cur_node->name, (xmlChar *)"updated")){
                output->update = cur_node->children->content;
            }
        } else if(cur_node->type == XML_ELEMENT_NODE){
            // POKUD JE LINK V HREF A NE JAKO CONTENT ELEMENTU
            if(xmlStrEqual(cur_node->name, (xmlChar *)"link")){
                xmlAttr* cur_node_attr = NULL;
                for(cur_node_attr = cur_node->properties; cur_node_attr; cur_node_attr = cur_node_attr->next){
                    if(xmlStrEqual(cur_node_attr->name, (xmlChar *)"href")){
                        output->link = cur_node_attr->children->content;
                    }
                }
            }
        }
        retrieveXMLEntryContent(cur_node->children, &(*output));
    }
}

static void printFormattedXML(xmlNode* a_node, bool titleFound, struct parameters *params){
    xmlNode* cur_node = NULL;

    for(cur_node = a_node; cur_node; cur_node = cur_node->next){
        struct xmlOutput output;
        if(cur_node->type == XML_ELEMENT_NODE && cur_node->children){
            if(!titleFound && xmlStrEqual(cur_node->name, (xmlChar *)"title")){
                printf("*** %s ***\n", cur_node->children->content);
                titleFound = true;
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
        printFormattedXML(cur_node->children, titleFound, &(*params));
    }
}

void retrieveXMLDocs(std::vector<std::string>& xmlResponses, struct parameters *params){
    const char* CAfile = (params->certStrings.empty()) ? NULL : params->certStrings[0].c_str();
    const char* CApath = (params->certFolders.empty()) ? NULL : params->certFolders[0].c_str();

    long res = 1;

    SSL_CTX *ctx =  SSL_CTX_new(SSLv23_client_method());
    BIO *bio =      NULL;
    SSL *ssl =      NULL;

    SSL_load_error_strings();
    ERR_load_BIO_strings();
    OpenSSL_add_all_algorithms();
    SSL_library_init();

    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);

    SSL_CTX_set_verify_depth(ctx, 4);

    const long flags = SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_COMPRESSION;
    SSL_CTX_set_options(ctx, flags);

    if(! SSL_CTX_load_verify_locations(ctx, CAfile, CApath)){
        fprintf(stderr, "Nepodařilo se ověřit platnost certifikátu!\n");
        exit(EXIT_FAILURE);
    }

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

                    //if(feedURL.substr(foundIndex+1).find(":") != std::string::npos){
                    //    std::cout << feedURL.substr(foundIndex+1).find_first_of(":") << std::endl;
                    //}

                feedURL = feedURL.substr(foundIndex+3);
                foundIndex = feedURL.find_first_of("/");
                path = feedURL.substr(foundIndex, feedURL.size()-1);
                feedURL = feedURL.substr(0, foundIndex);
                hostname = feedURL;

                // TODO IF CUSTOM PORT
                bioURL = feedURL.append(":").append(protocol).c_str();
                bio = BIO_new_ssl_connect(ctx);
            } catch(...){
                fprintf(stderr, "Chyba v odakzu lmao\n");
                exit(EXIT_FAILURE);
            }

            BIO_get_ssl(bio, &ssl);
            if(! (ssl != NULL)){
                fprintf(stderr, "bio_get_ssl failed\n");
                exit(EXIT_FAILURE);
            }

            SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);

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

            if(! BIO_set_conn_hostname(bio, bioURL)){
                fprintf(stderr, "bio_set_conn_hostname failed\n");
                exit(EXIT_FAILURE);
            };

            if(BIO_do_connect(bio) <= 0){
                BIO_reset(bio);
                bio = BIO_new_connect(bioURL);
                if(bio == NULL){
                    fprintf(stderr, "\033[31mNepodařilo se vytvořit připojení!\033[0m\n");
                    BIO_free_all(bio);
                    exit(EXIT_FAILURE);
                }
                if(BIO_do_connect(bio) <= 0){
                    fprintf(stderr, "\033[31mNepodařilo se připojit k odkazu %s!\033[0m\n", feedURL.c_str());
                    BIO_free_all(bio);
                    exit(EXIT_FAILURE);
                }
            }

            if(SSL_get_verify_result(ssl) != X509_V_OK){
                fprintf(stderr, "Nepodařilo se ověřit platnost certifikátu!\n");
                exit(EXIT_FAILURE);
            }
            std::string requestParts = "GET ";
            requestParts.append(path).append(
                    " HTTP/1.0\r\nHost: ").append(hostname).append(
                    "\r\nConnection: close\r\n\r\n"
                );
            const char* request = requestParts.c_str();
            BIO_puts(bio, request);

            std::string response;
            int len = 0;
            do{
                char buf[1024] = {};
                len = BIO_read(bio, buf, sizeof(buf)-1);

                if(len > 0){
                    response.append(buf);
                }

            } while (len > 0 || BIO_should_retry(bio));

            BIO_reset(bio);

            std::istringstream f(response);
            std::string responseHeader;
            std::getline(f, responseHeader);

            if(responseHeader.find("301") != std::string::npos){
                if(response.find("\nLocation") != std::string::npos){
                    std::string tempString = response;

                    size_t posIndex = response.find("\nLocation");
                    tempString = tempString.substr(posIndex+11);
                    tempString = tempString.substr(0, tempString.find("\n")-1);

                    feedURL = tempString;
                }
            } else{
                BIO_free_all(bio);
                std::string xmlResponse = response;

                if(response.find("<?xml") != std::string::npos){
                    xmlResponse = response.substr(response.find("<?xml"));
                }
                xmlResponses.push_back(xmlResponse);

                xmlDoc *doc = NULL;
                xmlNode *root_element = NULL;

                doc = xmlReadDoc((const xmlChar*)xmlResponse.c_str(), NULL, NULL, 0); // 

                if(doc == NULL){
                    fprintf(stderr, "Nepodařilo se otevřít dokument XML.\n");
                    exit(EXIT_FAILURE);
                }

                root_element = xmlDocGetRootElement(doc);
                if(root_element == NULL){
                    fprintf(stderr, "Nepodařilo se načíst kořenový element dokumentu XML.\n");
                    exit(EXIT_FAILURE);
                }

                printFormattedXML(root_element, false, &(*params));

                xmlFreeDoc(doc);
                xmlCleanupParser();

                break;
            }
            BIO_free_all(bio);
        }
    }
}