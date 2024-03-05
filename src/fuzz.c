//
// Created by Ismael Secundar on 25/02/2024.
//
#include "fuzz.h"
#include "tar_helper.h"
/*
 * Initialization of variables that will be used after
 * */
/***
 *
char name[100];
char mode[8];      OK
char uid[8];    OK
char gid[8];    OK
char size[12];      OK
char mtime[12];
char chksum[8];
char typeflag;  OK
char linkname[100];
char magic[6]; OK
char version[3]; OK
char uname[32];
char gname[32];
char devmajor[8];
char devminor[8];
char prefix[155];
char padding[12];
 */
static tar_h header; // header of the tar file to be created

static int ctr_test = 0;
static int ctr_success = 0;

static wchar_t NOT_ASCII_CHARS[] = {L'⚽', L'⚾', L'⌕', L'∩'};

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

static int modes_size = 8;

const char* extractorPath = "./extractor_apple"; // Path to your extractor_apple executable
const char* archivePath = "archive.tar"; // Path to the tar file to test


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
int test_extraction(char* extractor_path) {

  // Increment number of test done
  ctr_test++;

  int rv = 0;
  char cmd[84];
  strncpy(cmd, extractor_path, 58);
  cmd[59] = '\0';
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
    // Rename file on success
    char success_name[100];
    snprintf(success_name, 100, "success_archive%d.tar", ctr_success);
    rename(ARCHIVE_NAME, success_name);
    // Success counter increased by 1
    ctr_success++;
    goto finally;
  }
  finally:
  if(pclose(fp) == -1) {
    printf("Command not found\n");
    rv = -1;
  }
  return rv;
}

/**
 * Test the ending bytes of the tar file
 * @param path_of_extractor
 */
void test_ending_bytes(char *path_of_extractor) {
  printf("\nFUZZING ON END BYTES\n");

  // Define lengths to test
  int end_lengths[] = {0, 1, NUMBER_END_BYTES / 2 , NUMBER_END_BYTES - 1, NUMBER_END_BYTES, NUMBER_END_BYTES + 1, NUMBER_END_BYTES * 2};
  int number_lengths = 7;

  // Define longest buffer of 0 possible
  char end_bytes[NUMBER_END_BYTES * 2];
  memset(end_bytes, 0, NUMBER_END_BYTES * 2);

  // Test with a file with content = "Hello World!"
  char content[] = "Hello World!";
  size_t content_size = strlen(content);

  for (int i = 0; i < number_lengths; i++)
  {
    // Reset header
    initialize_header(&header);

    // Without file content ////////
    make_tar(&header, "", 0, end_bytes, end_lengths[i], 1);
    test_extraction(path_of_extractor);


    // With file content //////////
    // Define size of content
    define_content_size(&header, content_size);
    make_tar(&header, content, content_size, end_bytes, end_lengths[i], 1);
    test_extraction(path_of_extractor);
  }
}

void basic_test(char* extractor_path, char* field_name, int size, int checksum) {
  //TODO Change the name of the function and the order
  int size_not_octal = size - 1;
  // Reset header
  initialize_header(&header);

  // Empty field
  update_header_field(field_name, "", size);
  make_tar_empty(&header, checksum);
  test_extraction(extractor_path);

  // Reset header
  initialize_header(&header);
//
  // Not Octal
  memset(field_name, '9', size_not_octal);
  field_name[size_not_octal] = 0;
  make_tar_empty(&header, checksum);
  test_extraction(extractor_path);

  // Reset header
  initialize_header(&header);

  // Not correctly terminated
  memset(field_name, '1', size);
  make_tar_empty(&header, checksum);
  test_extraction(extractor_path);

  // Reset header
  initialize_header(&header);

  // Terminate in the middle
  memset(field_name, 0, size);
  memset(field_name, '1', size / 2 );
  make_tar_empty(&header, checksum);
  test_extraction(extractor_path);

  // Reset header
  initialize_header(&header);

  // Playing with 0's
  memset(field_name, 0, size);
  make_tar_empty(&header, checksum);
  test_extraction(extractor_path);

  // Reset header
  initialize_header(&header);

  memset(field_name, '0', size - 1);
  field_name[size - 1] = 0;
  make_tar_empty(&header, checksum);
  test_extraction(extractor_path);

  // Reset header
  initialize_header(&header);

  memset(field_name, 0, size - 1);
  field_name[size - 1] = '0';
  make_tar_empty(&header, checksum);
  test_extraction(extractor_path);

  // Reset header
  initialize_header(&header);

  memset(field_name, 0, size);
  memset(field_name, '0', size / 2);
  make_tar_empty(&header, checksum);
  test_extraction(extractor_path);

  // Reset header
  initialize_header(&header);

  // Not numeric
  update_header_field(field_name, "test", size);
  make_tar_empty(&header, checksum);
  test_extraction(extractor_path);

  // Reset header
  initialize_header(&header);

//   Non ASCII - Test with 3 different variations
  update_header_field(field_name, &NOT_ASCII_CHARS[0], size);
  make_tar_empty(&header, checksum);
  test_extraction(extractor_path);

  // Reset header
  initialize_header(&header);

  update_header_field(field_name, &NOT_ASCII_CHARS[2],size );
  make_tar_empty(&header, checksum);
  test_extraction(extractor_path);

  // Reset header
  initialize_header(&header);

  update_header_field(field_name, &NOT_ASCII_CHARS[3],size );
  make_tar_empty(&header, checksum);
  test_extraction(extractor_path);

}
void test_mode(char* extractor_path) {

  printf("\nFUZZING ON MODE FIELD\n");
  basic_test(extractor_path, header.mode, 8, 1);

  char test_mode[8];

  // We can test every possible mode in the range of known modes
  //TODO make a seperate method for the iteration

  for (int i = 0; i < 12; i++)
  {
    initialize_header(&header);
    snprintf(test_mode, 8, "%07o", MODES[i]);
    update_header_field(header.mode, test_mode, 8);
    make_tar_empty(&header, 1);
    test_extraction(extractor_path);
  }
}

void test_size(char* extractor_path) {

  printf("\nFUZZING ON SIZE FIELD\n");

  basic_test(extractor_path, header.size, 12, 1);


  // Test with a file with content = "Hello World!"
  char content[] = "Testing size field";
  size_t content_size = strlen(content);

  // Test with different sizes similar to the endbytes sizes
  int content_sizes[] = {-1, 0, 1, 18, 20, 100, NUMBER_END_BYTES};

  // Reset header
  initialize_header(&header);

  // END BYTES
  char end_bytes[NUMBER_END_BYTES];
  memset(end_bytes, 0, NUMBER_END_BYTES);


  // Loop on all header content size
  //TODO make a seperate method for the iteration

  for (int i = 0; i < 7; i++)
  {
    define_content_size(&header, content_sizes[i]);
    make_tar(&header, content, content_size, end_bytes, NUMBER_END_BYTES, 1);
    test_extraction(extractor_path);
  }
}
void test_typeflag(char* extractor_path) {

  printf("\nFUZZING ON FLAGTYPE FIELD\n");

  initialize_header(&header);
  // Test with different sizes

  // Test for non-ASCII characters
  //TODO make a seperate method for the iteration

  for(int i = 0; i < 4; i++)
  {
    header.typeflag = NOT_ASCII_CHARS[i];
    make_tar_empty(&header, 1);
    test_extraction(extractor_path);
  }
  //TODO make a seperate method for the iteration

  for (int i = 0; i < 256; i++)
  {
    char char_typeflag = (char) i;
    header.typeflag = char_typeflag;
    make_tar_empty(&header, 1);
    test_extraction(extractor_path);
  }
}
void test_version(char *path_of_extractor){
    printf("\nFUZZING ON VERSION FIELD\n");
    basic_test(path_of_extractor, header.version, 2, 1);
    char version []= TVERSION;
  initialize_header(&header);
// iterate over all possible values

//TODO make a seperate method for the iteration
    for (int i = 0; i < 64; i++)
    {
      version[0] = i / 8 + '0';
      version[1] =  i % 8 + '0';

      update_header_field(header.version, version, 2);
      make_tar_empty(&header, 1);
      test_extraction(path_of_extractor);
    }


}
void test_magic(char* extractor_path) {

  printf("\nFUZZING ON MAGIC FIELD\n");
  basic_test(extractor_path, header.magic, 6, 1);
}
void test_uid(char* extractor_path) {

  printf("\nFUZZING ON UID FIELD\n");
  basic_test(extractor_path, header.uid, 8, 1);
}
void test_gid(char* extractor_path) {

  printf("\nFUZZING ON GID FIELD\n");
  basic_test(extractor_path, header.gid, 8, 1);
}
void clean() {
  system("rm -f *.txt");
  system("rm -f " ARCHIVE_NAME);
}
void fuzz(char *path_of_extractor){

    // Test 1: Create an empty tar file
//    printf("Test %d: Create an empty tar file\n", ctr_test++);
//  initialize_header(&header);
//  make_tar_empty(&header, 1);
//    test_extraction(path_of_extractor);
  test_mode(path_of_extractor);
  test_ending_bytes(path_of_extractor);
  test_size(path_of_extractor);
  test_typeflag(path_of_extractor);
  test_version(path_of_extractor);
  test_magic(path_of_extractor);
  test_uid(path_of_extractor);
    test_gid(path_of_extractor);
  printf("Number of tests completed: %d\n", ctr_test);
  printf("Number of crashes found: %d\n", ctr_success);
//  system("ls -all *.txt");
  printf("-------------------------- cleaning --------------\n");
    clean();

}
