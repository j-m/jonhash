#include "jonhash.h"

void shift(char *shiftedString, char *input, int numberOfBitsToShiftBy)
{
  unsigned int numberOfCharactersToShiftBy = numberOfBitsToShiftBy / CHAR_BIT;
  numberOfBitsToShiftBy = numberOfBitsToShiftBy - numberOfCharactersToShiftBy * CHAR_BIT;
  unsigned int index = 0;
  for (; input[index] != '\0'; index++)
  {
    char cn = index + numberOfCharactersToShiftBy;
    if (cn > BLOCK_SIZE)
    {
      cn -= BLOCK_SIZE;
    }
    shiftedString[index] = (char)(input[cn] << numberOfBitsToShiftBy);
    if (input[cn + 1] == '\0')
    {
      shiftedString[index] += (input[0] >> (CHAR_BIT - numberOfBitsToShiftBy));
    }
    else
    {
      shiftedString[index] += (input[cn + 1] >> (CHAR_BIT - numberOfBitsToShiftBy));
    }
  }
}

int countOnesInBinaryOfCharArray()
{
  return 1 % BLOCK_SIZE; //TODO: Count the number of 1s in the binary representation of the char array, modulused by BLOCK_SIZE
}

void hash(char *output, char *input)
{
  char shifted[BLOCK_SIZE];
  unsigned int shiftBy = countOnesInBinaryOfCharArray(input);
  if (shiftBy > 0)
  {
    shift(shifted, input, shiftBy);
  }
  char XORed[BLOCK_SIZE];
  XOR(XORed, input, output);
}

unsigned int random()
{
  return 1; /*TODO: Return 0 - 63 with a CSPRNG https://github.com/jedisct1/libsodium/blob/master/src/libsodium/randombytes/sysrandom/randombytes_sysrandom.c */
}

void createSalt(char *output)
{
  unsigned int index = 0;
  for (; index < BLOCK_SIZE; index++)
  {
    output[index] = BASE64[random()];
  }
}

void hashPassword(char *output, char *input)
{
  unsigned int iteration = 0, index = 0, offset = 0,  weave = 0;
  char hashed[BLOCK_SIZE] = H0;
  for (; iteration < COST * 1000; iteration++)
  {
    char block[BLOCK_SIZE];
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
      {
        offset = 0;
      }
    }
  }
  for (index = 0; index < BLOCK_SIZE; index++)
  {
    output[BLOCK_SIZE + index] = hashed[index];
  }
}

void hashPassword(char *hash, char *password)
{
  createSalt(output);
  hashPassword(hash, password, salt);
}