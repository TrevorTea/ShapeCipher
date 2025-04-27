///////////////////////////////////////////////////////////////////////////////
///                                INCLUDES                                 ///
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////////
///                                 DEFINES                                 ///
///////////////////////////////////////////////////////////////////////////////

#define ALPHAWRAP(x) (x - 'A' < 0 ? x - 'A' + 26 : x - 'A') % 26 + 'A'
#define CEIL(x) ((x - (int)x) ? (int)(x + 1) : (int)(x))
#define ISALPHA(x) ((x > 0x40 && x < 0x5B) || (x > 0x60 && x < 0x7B))
#define MAXFILESIZE (1<<16)
#define UPPER(x) ((x > 0x60 && x < 0x7B) ? (x - 0x20) : x)

typedef unsigned int uint;

///////////////////////////////////////////////////////////////////////////////
///                                  ENUMS                                  ///
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///                                 STRUCTS                                 ///
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///                                FUNCTIONS                                ///
///////////////////////////////////////////////////////////////////////////////

/**
 * Decrypts a string message given an integer key.alignas
 * 
 * @param key Integer key used to decrypt the message.
 * @param msg String message to decrypt.
 * 
 * @returns Decrypted message.
 */
char * decrypt(int key, char* msg);

/**
 * Determines the message from either input path or direct CLI.
 * 
 * @param path_or_msg Either raw message string or path to text file.
 * 
 * @returns String message.
 */
char * determine_msg(char* path_or_msg);

/**
 * Encrypts a string message given an integer key.
 * 
 * @param key Integer key used to encrypt the message.
 * @param msg String message to encrypt.
 * 
 * @returns Encrypted message.
 */
char * encrypt(int key, char* msg);

/**
 * Parses arguments for main and modifies them in the static global context.
 * 
 * @param argc Integer count of CLI args.
 * @param argv Pointer to strings of CLI.
 * 
 * @throw EXIT_FAILURE Usage error if wrong # of args.
 */
void parse_args(int argc, char** argv);

/**
 * Applies a Caeserian shift to the provided message. Message must be upper
 *      alpha. This is an inplace operation.
 * @param msg String message to shift.
 * @param shift integer to shift by.
 */
void shift_caeserian(char* msg, int shift);

///////////////////////////////////////////////////////////////////////////////
///                                   MAIN                                  ///
///////////////////////////////////////////////////////////////////////////////
