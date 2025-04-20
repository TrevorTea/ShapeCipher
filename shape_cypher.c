///////////////////////////////////////////////////////////////////////////////
///                                INCLUDES                                 ///
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shape_cypher.h"

///////////////////////////////////////////////////////////////////////////////
///                                 DEFINES                                 ///
///////////////////////////////////////////////////////////////////////////////

#define CEIL(x) ((x - (int)x) ? (int)(x + 1) : (int)(x))
#define MAXFILESIZE (1<<16)

///////////////////////////////////////////////////////////////////////////////
///                                  ENUMS                                  ///
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///                                 STRUCTS                                 ///
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///                                FUNCTIONS                                ///
///////////////////////////////////////////////////////////////////////////////

char * encrypt(int key, char* msg) {
    unsigned int len = strlen(msg);
    char** strs = (char **) malloc(sizeof(char*) * key);
    char* res = (char *) malloc(sizeof(char) * len);
    // TODO Add text scrubing to remove unknown chars and toupper.
    // Initializes substring array to appropriate size.
    for (unsigned int i = 0; i < key; i++) {
        strs[i] = (char*) malloc(sizeof(char) * (CEIL((float)len / key)));
    }
    // Columnar encoding of substrings.
    for(unsigned int i = 0; i < len; i++) {
        strs[i%key][i/key] = msg[i];
    }
    // Combine substrings.
    for(unsigned int i = 0; i < key; i++) {
        strcat(res, strs[i]);
    }
    // TODO Handle wrapping around the back of the alphabet.
    // Apply caesarian shift.
    for(unsigned int i = 0; i < len; i++) {
        res[i] = res[i] + key;
    }
    // Free substrs.
    for (unsigned int i = 0; i < key; i++) {
        free(strs[i]);
    }
    free(strs);

    return res;
}

///////////////////////////////////////////////////////////////////////////////
///                                   MAIN                                  ///
///////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
    // Arg parsing.
    if (argc < 4 || argc > 5) {
        fprintf(stderr, "Improper usage: cypher \"encode\"|\"decode\" key "
                        "msg|path [outPath].\n");
        return 1;
    }

    char* mode = argv[1];
    int key = atoi(argv[2]);
    char* path_or_msg = argv[3];
    char* outpath = (argc == 5) ? argv[4] : NULL;

    // Determine input message.
    char * msg = NULL;
    if (strstr(path_or_msg, ".txt")) {
        FILE* inp = fopen(path_or_msg, "r");
        fseek(inp, 0, SEEK_END);
        unsigned int sz = ftell(inp);
        rewind(inp);
        msg = (char*) malloc(sizeof(char) * (sz + 1));
        msg = fgets(msg, sz, inp);
        msg[sz] = '\0';
    }
    else {
        msg = path_or_msg;
    }

    // Determine whether to encrypt or decrypt and do.
    char* result = NULL;
    if (strcmp(mode, "encrypt") == 0) {
        result = encrypt(key, msg);
    }
    // TODO Add decryption.
    else if (strcmp(mode, "decrypt") == 0) {
        result = NULL;
    }
    
    // Determine output stream.
    FILE* out = NULL;
    if (argc == 5) {
        out = fopen(outpath, "w");
    }
    else out = stdout;

    // Output printing and resource cleanup.
    fprintf(out, "%s", result);
    fprintf(out, "\n");
    free(result);
    if (out) fclose(out);
    return 0;
}