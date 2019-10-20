#include "../src/jonhash.h"
#include "./test.h"

#include <stdio.h>

int main(int argc, char *argv[])
{
  printf("Number of arguments: %d\n", argc);
  for (unsigned int index = 0; index < argc; index++)
  {
    printf(" - Argument %d: %s\n", index, argv[index]);
  }
  printf("\n");

  int testResult = test();
  if (testResult != 0)
  {
    return testResult;
  }
 
  char output[BLOCK_SIZE * 2 + 1];
  hash(output, "thisismypassword");
  printf("Output: %s\n", output);
  return 0;
}
