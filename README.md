# Shape Cipher
The cipher is constructed to be **simple** to understand and compute by hand while still obfuscating the message.
In this way it can be easily verified while also serving as a practice exercise for coding.

The cipher consists of two steps:
    1. [Caesarian Shift](https://en.wikipedia.org/wiki/Caesar_cipher)
    2. [Columnar Transposition](https://en.wikipedia.org/wiki/Transposition_cipher)

EXAMPLE:

    INPUT : msg -> hellofromafar    key -> 4
    1.      lippsjvsqejev
    2.      lsqv    ije   pvj   pse
    OUTPUT : lsqvijepvjpse

## Notes
1. The C implementation given aligns the message to be evenly divisable by the key, simplifying the math for decoding. This changes the encoded message however.
