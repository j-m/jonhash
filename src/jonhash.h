#ifndef JONHASH 
#define JONHASH

#ifndef CHAR_BIT
#define CHAR_BIT 8                                                                //The number of bits in a char type variable
#endif
#define COST 10                                                                   //The number of loops, multiplied by 1000
#define BASE64 "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-" //Characters used to represent numbers in base 64
#define BLOCK_SIZE 32                                                             //The size of the salt and hash; result is always double this
#define H0 "JonathanMarsh-Hashing-Algorithm"                                      //The initial hash used for the first XOR (BLOCK_SIZE - 1 of BASE64 characters)

void createSalt(char *output);
void hash(char *output, char *input);
void hashWithSalt(char *output, char *input, char *salt);

#endif
