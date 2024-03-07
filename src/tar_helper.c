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
  make_tar(header, NULL, 0, end_bytes, NUMBER_END_BYTES, checksum);
}

void update_header_field(char* header_field_to_change, char* new_header_value, size_t size) {
  strncpy(header_field_to_change, new_header_value, size);
}

void initialize_header(tar_h* header) {

  char random_name[100];// Random name
  char zeros[8] = "0000000"; // Zeros
  char linkname[100] = "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";

  memset(header, 0, sizeof(tar_h));  // Reset data of header
  snprintf(random_name, 7, "%d.txt", (rand() % 100));// Random name

  snprintf(header->name, sizeof(header->name), "%s", random_name);
  snprintf(header->mode, sizeof(header->mode), "%s", "0007777");
  snprintf(header->uid, sizeof(header->uid),"%s", zeros);
  snprintf(header->gid,sizeof(header->gid),"%s", zeros);
  snprintf(header->size, sizeof(header->size), "%011o", 0); // for the size it must be in octal
  snprintf(header->mtime, sizeof(header->mtime), "%011lo", time(NULL)); // set modification time to current time in octal format
  header->typeflag = REGTYPE;
  snprintf(header->linkname, sizeof(header->linkname), "%s", linkname);
  snprintf(header->magic, sizeof(header->magic), TMAGIC);
  sprintf(header->version, TVERSION); //TODO check the size for this...
  snprintf(header->uname, sizeof(header->uname), "root");
  snprintf(header->gname, sizeof(header->gname), "root");
  snprintf(header->devmajor, sizeof(header->devmajor),"%s", zeros);
  snprintf(header->devminor, sizeof(header->devminor),"%s", zeros);

  calculate_checksum(header); // it is made at the end after all fields

}

void define_content_size(tar_h* header, int size) {
  snprintf(header->size, 12, "%011o", size);
}