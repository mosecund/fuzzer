//
// Created by Ismael Secundar on 25/02/2024.
//
#include "fuzz.h"
#include "tar_helper.h"
/*
 * Initialization of variables that will be used after
 * */

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

const char* extractorPath = "./extractor"; // Path to your extractor executable
const char* archivePath = "test_archive.tar"; // Path to the tar file to test


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

  int size_of_buffer = 51; // Size of the buffer to store the command to be executed
  int size_of_output = 33; // Size of the buffer to store the output of the executable
  ctr_test++; // Increase the number of test
  int return_value = 0; // value to return
  char cmd[size_of_buffer]; // Create a buffer to store the command to be executed
  strncpy(cmd, extractor_path, 25); // Copy the path of the extractor to the command buffer
  cmd[26] = '\0'; // End the path of the extractor with the \0 character

  // Copy the path of the archive to the command buffer
  strncat(cmd, " archive.tar", 25);

  // Create a buffer to store output of executable
  char output_buffer[size_of_output];

  // Declare a file pointer
  FILE *fp;

  // Make sure it was possible to open the pipe on the side.
  // Pipe = executable
  if ((fp = popen(cmd, "r")) == NULL) {
    printf("Error opening pipe!\n");
    return -1;
  }

  // Read the 32 first bytes of the fp and store them in buf.
  // If there is nothing to read => No Output
  if(fgets(output_buffer, 33, fp) == NULL) {
    printf("No output\n");
    goto finally;
  }
  // If the output corresponds to expected crash message
  // STRNCMP returns 0 if both strings match. Therefore, it will only go through
  // the else when the strings match.
  if(strncmp(output_buffer, "*** The program has crashed ***\n", 33)) {
    printf("Not the crash message\n");
    goto finally;
  } else {
    printf("Crash message\n");
    // Set return value to 1
    return_value = 1;

    // Rename file on success
    char success_name[100];
    snprintf(success_name, 100, "success_archive%d.tar", ctr_success);
    rename(ARCHIVE_NAME, success_name);
    // Success counter increased by 1
    ctr_success++;

    goto finally;
  }
  // Try to close the executable. If a problem occurs, return -1
  finally:
  if(pclose(fp) == -1) {
    printf("Command not found\n");
    return_value = -1;
  }
  return return_value;
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

  // Non ASCII - Test with 3 different variations
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
void iterate_over_all_modes(){
  char modes[8];
  size_t size_modes = 8;
  for (int i = 0; i < 12; i++)
  {
    initialize_header(&header);
    snprintf(modes, 8, "%07o", MODES[i]);//TODO voir ce que signifie le 07o
    update_header_field(header.mode, modes,size_modes);
    make_tar_empty(&header, 1);
    test_extraction(extractorPath);
  }
}

void test_mode(char* extractor_path) {
  printf("\nFUZZING ON MODE\n");

  // Test with different sizes
  basic_test(extractor_path, header.mode, 8, 1);
  iterate_over_all_modes();

}
void fuzz(char *path_of_extractor){

    // Test 1: Create an empty tar file
//    printf("Test %d: Create an empty tar file\n", ctr_test++);
//  initialize_header(&header);
//  make_tar_empty(&header, 1);
//    test_extraction(path_of_extractor);
//      test_ending_bytes(path_of_extractor);
      test_mode(path_of_extractor);



}
