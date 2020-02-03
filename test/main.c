#include "jonhash.h"
#include "test.h"


#include <stdio.h> //TODO: remove

int main(int argc, char *argv[])
{
  printf("Number of arguments: %d\n", argc);
  for (unsigned int index = 0; index < argc; index++)
  {
    printf(" -Argument %02d: %s\n", argc, argv[index]);
  }
  printf("\n");

  int testResult = testHash();
  if (testResult != 0)
  {
    return testResult;
  }
 
  char output[BLOCK_SIZE * 2 + 1];
  hashPassword(output, "thisismypassword");
  printf("Output: %s\n", output);
  return 0;
}