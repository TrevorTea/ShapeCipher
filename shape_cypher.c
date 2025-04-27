///////////////////////////////////////////////////////////////////////////////
///                                INCLUDES                                 ///
///////////////////////////////////////////////////////////////////////////////

#include "shape_cypher.h"

///////////////////////////////////////////////////////////////////////////////
///                                 DEFINES                                 ///
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///                                 GLOBALS                                 ///
///////////////////////////////////////////////////////////////////////////////

// Static globals for arg parsing.
static int key;
static char* mode;
static char* path_or_msg;
static char* outpath;

///////////////////////////////////////////////////////////////////////////////
///                                  ENUMS                                  ///
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///                                 STRUCTS                                 ///
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///                                FUNCTIONS                                ///
///////////////////////////////////////////////////////////////////////////////

// TODO Add safety to allocs.

char * _pad_str(char* str,
                uint padlen,
                const char val) {
    char* res = (char*) calloc(strlen(str) + padlen + 1, sizeof(char));
    strcpy(res, str);
    for(uint i = strlen(str); i < strlen(str) + padlen; i++) {
        res[i] = val;
    }
    free(str);
    return res;
}

char * decrypt(int key, char* msg) {
    uint len;
    uint chunksize;
    char *res;

    len = strlen(msg);
    chunksize = len / key;
    res = (char*) calloc(len + 1, sizeof(char));

    for(uint i = 0; i < chunksize; i++) {
        for(uint j = 0; j < key; j++) {
            res[i * key + j] = msg[i + j * chunksize];
        }
    }

    shift_caeserian(res, -key);

    return res;
}

char * determine_msg(char* path_or_msg) {
        char * msg = NULL;
        if (strstr(path_or_msg, ".txt")) {
            FILE* inp = fopen(path_or_msg, "r");
            fseek(inp, 0, SEEK_END);
            uint sz = ftell(inp);
            if (sz > MAXFILESIZE) {
                fprintf(stderr, "Input file exceed max size.\n");
                exit(EXIT_FAILURE);
            }
            rewind(inp);
            msg = (char*) calloc(sz + 1, sizeof(char));
            msg = fgets(msg, sz, inp);
        }
        else {
            msg = path_or_msg;
        }
        return msg;
}

char * encrypt(int key, char* msg) {
    uint len;
    uint strplen;
    uint chunksize;
    char** subs;
    char* strp;
    char* res;

    len = strlen(msg);
    strplen = 0;
    subs = (char **) malloc(sizeof(char*) * key);
    strp = (char *) calloc(len + 1, sizeof(char));
    
    // Input scrubbing.
    for (uint i = 0; i < len; i++) {
        if(ISALPHA(msg[i])) {
            strp[strplen] = UPPER(msg[i]);
            strplen++;
        }
    }

    // TODO Add dynamic padding that can be intelligently shredded later.

    if (strplen % key) strp = _pad_str(strp, key - (strplen % key), 'G');
    strplen = strlen(strp);
    chunksize = strplen / key;
    
    // Initializes substring array to appropriate size.
    for (uint i = 0; i < key; i++) {
        subs[i] = (char*) calloc(chunksize + 1, sizeof(char));
    }
    // Columnar encoding of substrings.
    for(uint i = 0; i < strplen; i++) {
        subs[i%key][i/key] = strp[i];
    }
    // Combine substrings.
    res = (char *) calloc(strplen + 1, sizeof(char));
    for(uint i = 0; i < key; i++) {
        strcat(res, subs[i]);
    }
    // Apply caesarian shift.
    shift_caeserian(res, key);
    
    // Free substrs.
    for (uint i = 0; i < key; i++) {
        free(subs[i]);
    }
    free(subs);

    return res;
}

void parse_args(int argc, char** argv) {
    // Arg parsing.
    if (argc < 4 || argc > 5) {
        fprintf(stderr, "Improper usage: cypher \"encode\"|\"decode\" key "
                        "msg|path [outPath].\n");
        exit(EXIT_FAILURE);
    }

    mode = argv[1];
    key = atoi(argv[2]);
    path_or_msg = argv[3];
    outpath = (argc == 5) ? argv[4] : NULL;

    if (strcmp(mode, "encrypt") != 0 && strcmp(mode, "decrypt") != 0) {
        fprintf(stderr, "Improper usage: mode must be \"encrypt|decrypt\"\n");
        exit(EXIT_FAILURE);
    }
}

void shift_caeserian(char* msg, int shift) {
    for(uint i = 0; i < strlen(msg); i++) {
        msg[i] = ALPHAWRAP(msg[i] + shift);
    }
}

///////////////////////////////////////////////////////////////////////////////
///                                   MAIN                                  ///
///////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
    parse_args(argc, argv);

    // Determine input message.
    char * msg = determine_msg(path_or_msg);

    // Determine whether to encrypt or decrypt and do.
    char* result = NULL;
    if (strcmp(mode, "encrypt") == 0) {
        result = encrypt(key, msg);
    }
    else if (strcmp(mode, "decrypt") == 0) {
        result = decrypt(key, msg);
    }
    
    // Determine output stream.
    FILE* out = NULL;
    if (outpath) {
        out = fopen(outpath, "w");
    }
    else out = stdout;

    // Output printing and resource cleanup.
    fprintf(out, "%s\n", result);
    free(result);
    fclose(out);
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
///                                  NOTES                                  ///
///////////////////////////////////////////////////////////////////////////////
/*
    EX1:    INPUT: 4   hellofromafar
            1. Transposition.
                homr       efa*       lrf*       loa*
            2. Caesar
                LSQV       IJE*       PVJ*       PSE*
            3. Output
                LSQVIJEPVJPSE

    EX2     INPUT: 3 hellofromafar
            1. Transposition
                hlrar       eoof*       lfma*
            2. Caesar
                KOUDU       HRRI*       OIPD*
            3. Output
                KOUDUHRRIOIPD
*/