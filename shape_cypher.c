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
    if (argc < 3 || argc > 4) {
        fprintf(stderr, "Improper usage, only one arg can be provided.\n");
    }

    char * msg = NULL;

    if (strstr(argv[2], ".txt")) {
        FILE* inp = fopen(argv[2], "r");
        fseek(inp, 0, SEEK_END);
        unsigned int sz = ftell(inp);
        rewind(inp);
        msg = (char*) malloc(sizeof(char) * (sz + 1));
        msg = fgets(msg, sz, inp);
        msg[sz] = '\0';
    }
    else {
        msg = argv[2];
    }

    char* encrypted = encrypt(atoi(argv[1]), msg);

    FILE* out = NULL;
    if (argc == 4) {
        out = fopen(argv[3], "w");
    }
    else out = stdout;

    fprintf(out, "%s", encrypted);
    fprintf(out, "\n");
    return 0;
}