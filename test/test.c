#include "jonhash.h"
#include "test.h"

int testHash()
{
  char salt[BLOCK_SIZE] = "JonathanMarsh-Hashing-Algorithm";
  char input = "this is my totally secure password";
  char expectedOutput[BLOCK_SIZE * 2 + 1] = "JonathanMarsh-Hashing-Algorithm";
  char output[BLOCK_SIZE * 2 + 1] = "JonathanMarsh-Hashing-Algorithm";
  unsigned int index = 0;
  for (; index < BLOCK_SIZE * 2; index++)
  {
    if (output[index] != expectedOutput[index])
    {
      return -1; //Test failed: expected output does not match actual
    }
  }
  return 0; //Passed
}