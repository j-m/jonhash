#include "../src/jonhash.h"
#include "./test.h"

#include <stdio.h>

int test_salt() {
  char saltOutput1[BLOCK_SIZE] = {0};
  char saltOutput2[BLOCK_SIZE] = {0};
  createSalt(saltOutput1);
  createSalt(saltOutput2);
  unsigned int index = 0;
  for (; index < BLOCK_SIZE * 2; index++)
  {
    if (saltOutput1[index] != saltOutput2[index])
    {
      return 0;
    }
  }
  return 1;
}

int test_hash() {
  char salt[BLOCK_SIZE] = "JonathanMarsh-Hashing-Algorithm";
  char* input = "this is my totally secure password";
  char expectedOutput[BLOCK_SIZE * 2 + 1] = "JonathanMarsh-Hashing-Algorithm";
  char output[BLOCK_SIZE * 2 + 1];
  hashWithSalt(output, input, salt);
  unsigned int index = 0;
  
  for (; index < BLOCK_SIZE * 2; index++)
  {
    if (output[index] != expectedOutput[index])
    {
      return 1;
    }
  }
  return 0;
}

int test()
{
  int saltresult = test_salt();
  int hashresult = test_hash();
  return saltresult | hashresult;
}
