//
// Created by Ismael Secundar on 25/02/2024.
//
#include "fuzz.h"
#include "tar_helper.h"
/*
 * Initialization of variables that will be used after
 * */
static tar_h header;

static int ctr_test = 0;
static int ctr_success = 0;

static char NON_ASCII_CHARACTERS[] = {'💻', '©', '漢', 'ö'}; // taken from https://wpengine.com/builders/quick-tips-wpgraphql-non-ascii-queries/

static int MODES[] = {TSUID,
                      TSGID,
                      TSVTX,
                      TUREAD,
                      TUWRITE,
                      TUEXEC,
                      TGREAD,
                      TGWRITE,
                      TGEXEC,
                      TOREAD,
                      TOWRITE,
                      TOEXEC
                      };



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
int extraction_test(char *path_of_extractor)
{

  int return_value = 0;

  char cmd[51];

  strncpy(cmd, path_of_extractor, 25);
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
    return_value = 1;

    char filename_sucess[100]; // getting the name of the file that succeeded
    snprintf(filename_sucess, 100, "archived_successfull%d.tar", ctr_success);
    ctr_success++;
    rename("archive.tar", filename_sucess);

    goto finally;
  }
  finally:
  if(pclose(fp) == -1) {
    printf("Command not found\n");
    return_value = -1;
  }
  return return_value;
}

