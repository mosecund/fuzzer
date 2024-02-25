//
// Created by Ismael Secundar on 25/02/2024.
//

#include "fuzz.h"
#include "tar_helper.h"

/** BONUS (for fun, no additional points) without modifying this code,
 * compile it and use the executable to restart our computer.
 */

/**
 * Launches another axecutable given as argument,
 * parses its output and check whether or not it matches "*** The program has crashed ***".
 * @param the path to the executable
 * @return -1 if the executable cannot be launched,
 *          0 if it is launched but does not print "*** The program has crashed ***",
 *          1 if it is launched and prints "*** The program has crashed ***".
 *
 * BONUS (for fun, no additional marks) without modifying this code,
 * compile it and use the executable to restart our computer.
 */
int main(int argc, char* argv[])
{
  if (argc < 2)
    return -1;
  int rv = 0;
  char cmd[51];
  strncpy(cmd, argv[1], 25);
  cmd[26] = '\0';
  strncat(cmd, " archive.tar", 25);
  char buf[33];
  FILE *fp;

  if ((fp = popen(cmd, "r")) == NULL) {
    printf("Error opening pipe!\n");
    return -1;
  }

  if(fgets(buf, 33, fp) == NULL) {
    printf("No output\n");
    goto finally;
  }
  if(strncmp(buf, "*** The program has crashed ***\n", 33)) {
    printf("Not the crash message\n");
    goto finally;
  } else {
    printf("Crash message\n");
    rv = 1;
    goto finally;
  }
  finally:
  if(pclose(fp) == -1) {
    printf("Command not found\n");
    rv = -1;
  }
  return rv;
}
