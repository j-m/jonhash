#include "jonhash.h"

unsigned short lfsr = 0xACE1u;
unsigned bit;

unsigned random()
{
  bit  = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ) & 1;
  return lfsr =  (lfsr >> 1) | (bit << 15);
}
// https://stackoverflow.com/a/7603688/4907552

void createSalt(char *output)
{
  unsigned int index = 0;
  for (; index < BLOCK_SIZE; index++)
  {
    output[index] = BASE64[random() % 64];
  }
}

void shift(char *shiftedString, char *input, int numberOfBitsToShiftBy)
{
  unsigned int numberOfCharactersToShiftBy = numberOfBitsToShiftBy / CHAR_BIT;
  numberOfBitsToShiftBy = numberOfBitsToShiftBy - numberOfCharactersToShiftBy * CHAR_BIT;
  unsigned int index = 0;
  for (; index < BLOCK_SIZE; index++)
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

int bitCount(unsigned int u)
{
  unsigned int uCount;
  uCount = u - ((u >> 1) & 033333333333) - ((u >> 2) & 011111111111);
  return ((uCount + (uCount >> 3)) & 030707070707) % 63;
}
// https://stackoverflow.com/a/8871435/4907552

int countOnesInBinaryOfCharArray(char *input)
{
  unsigned int total = 0;
  unsigned int index = 0;
  for (; input[index] != '\0'; index++)
  {
    total += bitCount(input[index]);
  }
  return total;
}

void XOR(char *output, char *input1, char *input2)
{
  unsigned int index;
  for (index = 0; index < BLOCK_SIZE; index++)
  {
    output[index] = input1[index] ^ input2[index];
  }
}

void hashBlock(char *output, char *input)
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

void hashWithSalt(char *output, char *input, char *salt)
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
        hashBlock(hashed, block);
      }
      else
      {
        block[offset] = '1';
        hashBlock(hashed, block);
        break;
      }
      if (offset == BLOCK_SIZE)
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

void hash(char *output, char *input)
{
  char salt[BLOCK_SIZE];
  createSalt(salt);
  hashWithSalt(output, input, salt);
}
