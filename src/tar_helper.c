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

  // Define the archive file name, could also be passed as a parameter or defined globally
  const char* filename = "archive.tar";

  // Recompute checksum if required
  if (checksum == 1) {
    calculate_checksum(header);
  }

  // Open the file for writing in binary mode to avoid any newline translation issues
  fp = fopen(filename, "wb");
  if (fp == NULL) {
    perror("Failed to open file for writing");
    return;
  }

  // Write the header to the file
  fwrite(header, sizeof(tar_h), 1, fp);

  // If there is content and its size is greater than 0, write it to the file
  if (content != NULL && content_size > 0) {
    fwrite(content, content_size, 1, fp);
  }

  // If end bytes are provided and their size is greater than 0, write them to the file
  if (end_bytes_buffer != NULL && end_size > 0) {
    fwrite(end_bytes_buffer, end_size, 1, fp);
  }

  // Close the file
  fclose(fp);
}

void update_header_field(char* header_field_to_change, char* new_header_value, size_t size) {
  strncpy(header_field_to_change, new_header_value, size);
}

void initialize_header(tar_h* header) {
  // Clear the header structure
  memset(header, 0, sizeof(tar_h));

  // Generate a random file name
  int fileNumber = rand() % 100; // Random number for file name
  snprintf(header->name, sizeof(header->name), "%d.txt", fileNumber);

  // Set permissions
  snprintf(header->mode, sizeof(header->mode), "%07o", ALLPERM); // Convert permissions to octal
  snprintf(header->uid, sizeof(header->uid), "%07o", BASIC_UID); // Convert UID to octal
  snprintf(header->gid, sizeof(header->gid), "%07o", BASIC_GID); // Convert GID to octal

  // Initialize other header fields
  define_content_size(header, 0); // For an empty file
  snprintf(header->mtime, sizeof(header->mtime), "%011o", (unsigned long) time(NULL)); // Modification time in octal
  header->typeflag = REGTYPE; // Regular file type

  // Magic and version for UStar format
  strncpy(header->magic, TMAGIC, sizeof(header->magic));
  strncpy(header->version, TVERSION, sizeof(header->version));

  // Owner and group name
  strncpy(header->uname, "root", sizeof(header->uname));
  strncpy(header->gname, "root", sizeof(header->gname));

  // Major and minor device numbers (for device files, but required fields)
  snprintf(header->devmajor, sizeof(header->devmajor), "%07o", 0);
  snprintf(header->devminor, sizeof(header->devminor), "%07o", 0);

  // Calculate and set the checksum for the header
  calculate_checksum(header);
}

void define_content_size(tar_h* header, int size) {
  snprintf(header->size, sizeof(header->size), "%011o", size);
}