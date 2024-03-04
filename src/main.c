#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "fuzz.h"
#include "tar_helper.h"

int main(int argc, char const *argv[])
{
  if (argc != 2) {
    printf("Please kindly insert the path to the tar extractor_apple");
    return -1;
  }

  // Initialize random generator
  srand(time(NULL));

  char* extractor = argv[1];

  fuzz(extractor);

  return 0;
}
