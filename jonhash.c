#ifndef CHAR_BIT
#define CHAR_BIT 8 //The number of bits in a char type variable
#endif
#define COST 10                                                                   //The number of loops, multiplied by 1000
#define BASE64 "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-" //Characters used to represent numbers in base 64
#define BLOCK_SIZE 32                                                             //The size of the salt and hash; result is always double this
#define H0 "jonathanmarsh'shashingalgorithm"                                      //The initial hash used for the first XOR (BLOCK_SIZE - 1 characters)

#include <stdio.h> //TODO: remove

/*Compares 2 strings of binary and XORs them.
    output:	char XORed[BLOCK_SIZE * CHAR_BIT + 1];
    input1:	char currentBinary[BLOCK_SIZE * CHAR_BIT + 1];
    input2:	char shiftedBinary[BLOCK_SIZE * CHAR_BIT + 1];
*/
void XOR(char *output, char *input1, char *input2)
{
  for (unsigned int index = 0; index < BLOCK_SIZE * CHAR_BIT; index++)
  {
    output[index] = (input1[index] != input2[index]) ? '1' : '0';
  }
}

/*Left-shifts binary string `shift` times
    output:	char shifted[BLOCK_SIZE * CHAR_BIT + 1];
    input:	char currentBinary[BLOCK_SIZE * CHAR_BIT + 1];
    shift:	unsigned int shiftBy;
*/
void shift(char *output, char *input, unsigned int shiftBy)
{
  for (unsigned int index = 0; index < BLOCK_SIZE * CHAR_BIT; index++)
  {
    if (index < shift)
    {
      output[BLOCK_SIZE * CHAR_BIT - shiftBy + index] = input[index];
    }
    else
    {
      output[index - shiftBy] = input[index];
    }
  }
}

/*Converts a base64 string to concatenated binary string
    output:	char binary[BLOCK_SIZE * CHAR_BIT];
    input: 	char base64[BLOCK_SIZE];
*/
void convertBase64ToBinary(char *output, char *input)
{
  unsigned int index = 0, quotient, base;
  for (; index <= BLOCK_SIZE; index++)
  {
    quotient = (unsigned int)input[index];
    base = CHAR_BIT;
    while (quotient != 0 || base > 0)
    {
      output[index * CHAR_BIT + base - 1] = (quotient != quotient / 2 * 2) ? '1' : '0';
      quotient /= 2;
      base--;
    }
  }
}

/*Converts concatenated binary string to a base64 string
    output:	char base64[BLOCK_SIZE];
    input: 	char binary[BLOCK_SIZE * CHAR_BIT];
*/
void convertBinaryToBase64(char *output, char *input)
{
  unsigned int power = 1,
               total = 0,
               index = BLOCK_SIZE * CHAR_BIT;
  for (; index > 0; index--)
  {
    total += (input[index - 1] - '0') * power;
    power *= 2;
    if (power == 64)
    {
      output[index / 6 + 1] = BASE64[total];
      power = 1;
      total = 0;
    }
  }
}

/*Hashes the current block
    -Uses previous hash to make it one way
    -Encrypts by shifting 
    -XORs the encrypted binary with the previous binary
*/
void hash(char *output, char *input)
{
  char currentBinary[BLOCK_SIZE * CHAR_BIT],
      previousBinary[BLOCK_SIZE * CHAR_BIT],
      shifted[BLOCK_SIZE * CHAR_BIT],
      XORed[BLOCK_SIZE * CHAR_BIT];

  convertBase64ToBinary(currentBinary, input),
      convertBase64ToBinary(previousBinary, output);

  unsigned int shiftBy = 1, index = 0;
  for (; index < BLOCK_SIZE * CHAR_BIT; index++)
    if (previousBinary[index] == '1')
      shiftBy++;

  shift(shifted, currentBinary, shiftBy);

  XOR(XORed, currentBinary, previousBinary);
  convertBinaryToBase64(output, XORed);
}

unsigned int random()
{
  return 1; /*TODO: CSPRNG https://github.com/jedisct1/libsodium/blob/master/src/libsodium/randombytes/sysrandom/randombytes_sysrandom.c */
}

/*Creates a random salt using the new random function*/
void createSalt(char *output)
{
  for (unsigned int index = 0; index < BLOCK_SIZE; index++)
    output[index] = BASE64[random()];
}

/*Creates the final hash by compressing the output of each stage*/
void hashPassword(char *output, char *input)
{
  unsigned int iteration = 0,
               index,
               offset,
               weave;
  createSalt(output);
  char hashed[BLOCK_SIZE] = H0;
  for (; iteration < COST * 1000; iteration++)
  {
    char block[BLOCK_SIZE] = {'0'};
    for (index = 0, offset = 0, weave = 0; input[index] != '\0' || offset < BLOCK_SIZE || weave < BLOCK_SIZE; index++, offset++, weave++)
    {

      if (weave < BLOCK_SIZE)
      {
        block[offset] = output[weave];
        offset++;
      }
      if (input[index] != '\0')
      {
        block[offset] = input[index];
        hash(hashed, block);
      }
      else
      {
        block[offset] = '1';
        hash(hashed, block);
        break;
      }
      if (offset = BLOCK_SIZE)
        offset = 0;
    }
  }
  for (index = 0; index < BLOCK_SIZE; index++)
    output[BLOCK_SIZE + index] = hashed[index];
}

void testBaseConversions()
{
  char base64Input[32]=H0;
  char binary[BLOCK_SIZE * CHAR_BIT + 1];
  convertBase64ToBinary(binary, base64Input);
  printf("Binary: %s\n", binary);

  char base64Output[BLOCK_SIZE + 1];
  convertBinaryToBase64(base64Output, binary);
  printf("Base 64 output: %s\n", base64Output);

  for (int index = 0; index < BLOCK_SIZE; index++)
  {
    if (base64Input[index] == base64Output[index])
    {
      printf("Conversion test failed: base 64 output does not match input\n");
    }
  }
  printf("Conversion test passed: base 64 output does not match input\n");
}

int main(int argc, char *argv[])
{
  printf("Number of arguments: %d\n", argc);
  testBaseConversions();

  char output[BLOCK_SIZE * 2 + 1];
  hashPassword(output, "thisismypassword");
  printf("Output: %s\n", output);
  return 0;
}