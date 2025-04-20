///////////////////////////////////////////////////////////////////////////////
///                                INCLUDES                                 ///
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    for (unsigned int i = 0; i < key; i++) {
        strs[i] = (char*) malloc(sizeof(char) * (CEIL((float)len / key)));
    }
    for(unsigned int i = 0; i < len; i++) {
        strs[i%key][i/key] = msg[i];
    }
    for(unsigned int i = 0; i < key; i++) {
        strcat(res, strs[i]);
    }
    // TODO Handle wrapping around the back of the alphabet.
    for(unsigned int i = 0; i < len; i++) {
        res[i] = res[i] + key;
    }

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
    if (argc < 4 || argc > 5) {
        fprintf(stderr, "Improper usage: cypher \"encode\"|\"decode\" key "
                        "msg|path [outPath].\n");
        return 1;
    }

    char * msg = NULL;

    if (strstr(argv[3], ".txt")) {
        FILE* inp = fopen(argv[3], "r");
        fseek(inp, 0, SEEK_END);
        unsigned int sz = ftell(inp);
        rewind(inp);
        msg = (char*) malloc(sizeof(char) * (sz + 1));
        msg = fgets(msg, sz, inp);
        msg[sz] = '\0';
    }
    else {
        msg = argv[3];
    }

    char* result = NULL;
    if (strcmp(argv[1], "encrypt") == 0) {
        result = encrypt(atoi(argv[2]), msg);
    }
    // TODO Add decryption.
    else if (strcmp(argv[1], "decrypt") == 0) {
        result = NULL;
    }
    
    FILE* out = NULL;
    if (argc == 5) {
        out = fopen(argv[4], "w");
    }
    else out = stdout;

    fprintf(out, "%s", result);
    fprintf(out, "\n");
    free(result);
    if (out) fclose(out);
    return 0;
}