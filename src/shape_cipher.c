///////////////////////////////////////////////////////////////////////////////
///                                INCLUDES                                 ///
///////////////////////////////////////////////////////////////////////////////

#include "shape_cipher.h"

///////////////////////////////////////////////////////////////////////////////
///                                 DEFINES                                 ///
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///                                 GLOBALS                                 ///
///////////////////////////////////////////////////////////////////////////////

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
    uint len;
    char *res;

    len = strlen(str);
    res = (char*) calloc(len + padlen + 1, sizeof(char));
    strcpy(res, str);
    for (uint i = len; i < len + padlen; i++) {
        res[i] = val;
    }

    free(str);
    return res;
}

char * decrypt(uint key, char* msg) {
    uint len;
    uint chunksize;
    char *res;

    len = strlen(msg);
    chunksize = len / key;
    res = (char*) calloc(len + 1, sizeof(char));

    for (uint i = 0; i < chunksize; i++) {
        for (uint j = 0; j < key; j++) {
            res[i * key + j] = msg[i + j * chunksize];
        }
    }

    shift_caeserian(res, -key);

    return res;
}

char * determine_msg(char* path_or_msg) {
        char * msg = NULL;
        if (strstr(path_or_msg, ".txt")) {
            FILE* inp;
            uint sz;
            
            inp = fopen(path_or_msg, "r");
            fseek(inp, 0, SEEK_END);
            sz = ftell(inp);
            if (sz > MAXFILESIZE) {
                fprintf(stderr, "Input file exceed max size.\n");
                exit(EXIT_FAILURE);
            }
            rewind(inp);
            msg = (char*) calloc(sz + 1, sizeof(char));
            msg = fgets(msg, sz, inp);
        }
        else {
            msg = (char*) calloc(strlen(path_or_msg), sizeof(char));
            strcpy(msg, path_or_msg);
        }
        return msg;
}

char * encrypt(uint key, char* msg) {
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
    for (uint i = 0; i < strplen; i++) {
        subs[i%key][i/key] = strp[i];
    }
    // Combine substrings.
    res = (char *) calloc(strplen + 1, sizeof(char));
    for (uint i = 0; i < key; i++) {
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

struct arglist * parse_args(int argc, char** argv) {
    // Arg parsing.
    if (argc < 4 || argc > 5) {
        fprintf(stderr, "Improper usage: cipher \"encrypt\"|\"decrypt\" key "
                        "msg|path [outPath].\n");
        exit(EXIT_FAILURE);
    }

    struct arglist* args = malloc(sizeof(struct args *));
    args->mode = argv[1];
    args->key = atoi(argv[2]);
    args->path_or_msg = argv[3];
    args->outpath = (argc == 5) ? argv[4] : NULL;

    if (strcmp(args->mode, "encrypt") != 0 && strcmp(args->mode, "decrypt") != 0) {
        fprintf(stderr, "Improper usage: mode must be \"encrypt|decrypt\"\n");
        exit(EXIT_FAILURE);
    }

    return args;
}

void shift_caeserian(char* msg, int shift) {
    for (uint i = 0; i < strlen(msg); i++) {
        msg[i] = ALPHAWRAP(msg[i] + shift);
    }
}

///////////////////////////////////////////////////////////////////////////////
///                                   MAIN                                  ///
///////////////////////////////////////////////////////////////////////////////

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