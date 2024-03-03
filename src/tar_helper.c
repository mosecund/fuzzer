//
// Created by Ismael Secundar on 25/02/2024.
//
#include "tar_helper.h"

/**
 * taken from help.c
 * Computes the checksum for a tar header and encode it on the header
 * @param entry: The tar header
 * @return the value of the checksum
 */
unsigned int calculate_checksum(tar_h* input){
  // use spaces for the checksum bytes while calculating the checksum
  memset(input->chksum, ' ', 8);

  // sum of entire metadata
  unsigned int check = 0;
  unsigned char* raw = (unsigned char*) input;
  for(int i = 0; i < 512; i++){
    check += raw[i];
  }

  snprintf(input->chksum, sizeof(input->chksum), "%06o0", check);

  input->chksum[6] = '\0';
  input->chksum[7] = ' ';
  return check;
}

void make_tar(tar_h* header,
                char* content,
                size_t content_size,
                char* end_bytes_buffer,
                size_t end_size,
                int checksum){

  FILE *fp;


  // Recompute checksum in certain case
  if (checksum == 1) {
    calculate_checksum(header);
  }

  fp = fopen(ARCHIVE_NAME, "wb");
  // Write header into file
  fwrite(header, sizeof(tar_h), 1, fp);

  // Write content into file
  fwrite(content, content_size, 1, fp);

  // Write the end bytes in the file.
  fwrite(end_bytes_buffer, end_size, 1, fp);

  // Close file
  fclose(fp);
}
void make_tar_empty(tar_h* header, int checksum){

  char end_bytes[NUMBER_END_BYTES];
  memset(end_bytes, 0, NUMBER_END_BYTES);

  // Create basic tar with perfect number of bytes at the end
  make_tar(header, "", 0, end_bytes, NUMBER_END_BYTES, checksum);
}

void update_header_field(char* header_field_to_change, char* new_header_value, size_t size) {
  strncpy(header_field_to_change, new_header_value, size);
}

void initialize_header(tar_h* header) {

  // Reset data of header
  memset(header, 0, sizeof(tar_h));

  // Random name for file
  char random_name[100];
  snprintf(random_name, 7, "%d.txt", (rand() % 100));

  sprintf(header->name, random_name);
  sprintf(header->mode, "0007777");
  sprintf(header->uid, "0000000");
  sprintf(header->gid, "0000000");
  define_content_size(header, 0);
  sprintf(header->mtime, "14220157140");
  header->typeflag = REGTYPE;
  header->linkname[0] = 0;
  sprintf(header->magic, TMAGIC);
  sprintf(header->version, TVERSION);

  sprintf(header->uname, "root");
  sprintf(header->gname, "root");
  sprintf(header->devmajor, "0000000");
  sprintf(header->devminor, "0000000");

  calculate_checksum(header);

}

void define_content_size(tar_h* header, int size) {
  snprintf(header->size, 12, "%011o", size);
}