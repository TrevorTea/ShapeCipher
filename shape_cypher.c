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

// LSQVIJEPVJPSE
// LSQV IJEP VJPS E

// TODO Add safety to allocs.

uint * _calc_pad_posi(uint padlen, \
                      uint mismatch, \
                      uint chunk_size) {
    uint * posis = NULL;
    if (mismatch) {
        posis = (uint*) calloc(mismatch, sizeof(uint));
        uint * temp = (uint*) calloc(mismatch, sizeof(uint));
        for(uint i = 0; i < mismatch; i++) {
            temp[i] = padlen - (i * chunk_size) - 1;
        }
        for(uint i = 0; i < mismatch; i++) {
            posis[i] = temp[mismatch - i - 1];
        }
        free(temp);
    }
    return posis;
}

char * _pad_str(char* str,
                uint padlen,
                const char val) {
    char* res = (char*) calloc(strlen(str) + padlen + 1, sizeof(char));
    strcpy(res, str);
    for(uint i = strlen(str); i < strlen(str) + padlen; i++) {
        res[i] = val;
    }
    free(str);
    printf("%s\n", res);
    return res;
}

char * decrypt(int key, char* msg) {
    uint len = strlen(msg);
    uint pad_len = CEIL(((float) len) / key) * key;
    uint chunk_size = pad_len / key;
    char *res = (char*) malloc(sizeof(char) * (pad_len + 1));
    char *cpy = (char*) calloc(pad_len + 1, sizeof(char));
    
    strcpy(cpy, msg);

    cpy[pad_len] = '\0';

    for(uint i = 0; i < pad_len / key; i++) {
        for(int j = 0; j < key; j++) {
            res[i * key + j] = cpy[i + j * key];
        }
    }

    strcpy(cpy, res);

    uint j = 0;
    for(uint i = 0; i < pad_len; i++) {
        if (ISALPHA(cpy[i])) {
            res[j++] = cpy[i];
        }
    }
    for(uint i = len; i < pad_len; i++) {
        res[i] = '\0';
    }

    shift_caeserian(res, -key);

    free(cpy);
    return res;
}

char * encrypt(int key, char* msg) {
    uint len = strlen(msg);
    uint strp_len = 0;
    uint chunk_size;
    char** strs = (char **) malloc(sizeof(char*) * key);
    char* strp = (char *) calloc(len + 1, sizeof(char));
    
    // Input scrubbing.
    for (uint i = 0; i < len; i++) {
        if(ISALPHA(msg[i])) {
            strp[strp_len] = UPPER(msg[i]);
            strp_len++;
        }
    }
    printf("%d\n", strlen(strp));
    strp = _pad_str(strp, key - (strp_len % key), 'G');
    strp_len = strlen(strp);
    printf("%s", strp);
    printf("%d\n", strp_len);
    chunk_size = CEIL(strp_len / key);
    
    // Initializes substring array to appropriate size.
    for (int i = 0; i < key; i++) {
        strs[i] = (char*) calloc(chunk_size + 1, sizeof(char));
    }
    // Columnar encoding of substrings.
    for(uint i = 0; i < strp_len; i++) {
        strs[i%key][i/key] = strp[i];
    }
    // Combine substrings.
    // I hate strcat
    char* res = (char *) calloc(strp_len + 1, sizeof(char));
    for(uint i = 0; i < key; i++) {
        strcat(res, strs[i]);
    }

    // Apply caesarian shift.
    shift_caeserian(res, key);
    
    // Free substrs.
    for (int i = 0; i < key; i++) {
        free(strs[i]);
    }
    free(strs);

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

    if (strstr(mode, "encrypt") == NULL && strstr(mode, "decrypt") == NULL) {
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
    char * msg = NULL;
    if (strstr(path_or_msg, ".txt")) {
        FILE* inp = fopen(path_or_msg, "r");
        fseek(inp, 0, SEEK_END);
        uint sz = ftell(inp);
        if (sz > MAXFILESIZE) {
            fprintf(stderr, "Input file exceed max size.\n");
            return 1;
        }
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
        result = decrypt(key, msg);
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

///////////////////////////////////////////////////////////////////////////////
///                                  NOTES                                  ///
///////////////////////////////////////////////////////////////////////////////
/*
    Ex:     4   hellofromafar
            1. Transposition.
                homr       efa*       lrf*       loa*
            2. Caesar
                LSQV       IJE*       PVJ*       PSE*
            3. Output
                LSQVIJEPVJPSE
*/