#include <libxml/parser.h>
#include <libxml/tree.h>

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <sys/socket.h>
#include <netdb.h>

#include <vector>
#include <string>

#include "argparse.h"

struct xmlOutput{
    xmlChar* title  = NULL;
    xmlChar* author = NULL;
    xmlChar* link   = NULL;
    xmlChar* update = NULL;
};

void retrieveXMLDocs(std::vector<std::string>& xmlResponses, struct parameters *params);