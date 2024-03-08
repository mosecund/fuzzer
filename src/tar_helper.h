//
// Created by Ismael Secundar on 25/02/2024.
// Taken from : https://www.gnu.org/software/tar/manual/html_node/Standard.html

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


#ifndef FUZZER__TAR_HELPER_H_
#define FUZZER__TAR_HELPER_H_

/**
 * @brief Tar header
 *
 */

/* tar Header Block, from POSIX 1003.1-1990.  */

/* POSIX header.  */

typedef struct tar_header
{                              /* byte offset */
  char name[100];               /*   0 */
  char mode[8];                 /* 100 */
  char uid[8];                  /* 108 */
  char gid[8];                  /* 116 */
  char size[12];                /* 124 */
  char mtime[12];               /* 136 */
  char chksum[8];               /* 148 */
  char typeflag;                /* 156 */
  char linkname[100];           /* 157 */
  char magic[6];                /* 257 */
  char version[3];              /* 263 */
  char uname[32];               /* 265 */
  char gname[32];               /* 297 */
  char devmajor[8];             /* 329 */
  char devminor[8];             /* 337 */
  char prefix[155];             /* 345 */
  char padding[12];             /* 500 */
} tar_h;

#define TMAGIC   "ustar"        /* ustar and a null */
#define TMAGLEN  6
#define TVERSION "00"           /* 00 and no null */
#define TVERSLEN 2

/* Values used in typeflag field.  */
#define REGTYPE  '0'            /* regular file */
#define AREGTYPE '\0'           /* regular file */
#define LNKTYPE  '1'            /* link */
#define SYMTYPE  '2'            /* reserved */
#define CHRTYPE  '3'            /* character special */
#define BLKTYPE  '4'            /* block special */
#define DIRTYPE  '5'            /* directory */
#define FIFOTYPE '6'            /* FIFO special */
#define CONTTYPE '7'            /* reserved */

#define XHDTYPE  'x'            /* Extended header referring to the
                                   next file in the archive */
#define XGLTYPE  'g'            /* Global extended header */

/* Bits used in the mode field, values in octal.  */
#define TSUID    04000          /* set UID on execution */
#define TSGID    02000          /* set GID on execution */
#define TSVTX    01000          /* reserved */
/* file permissions */
#define TUREAD   00400          /* read by owner */
#define TUWRITE  00200          /* write by owner */
#define TUEXEC   00100          /* execute/search by owner */
#define TGREAD   00040          /* read by group */
#define TGWRITE  00020          /* write by group */
#define TGEXEC   00010          /* execute/search by group */
#define TOREAD   00004          /* read by other */
#define TOWRITE  00002          /* write by other */
#define TOEXEC   00001          /* execute/search by other */

/*
 * /TODO MODIFY following
 * */
#define ALLPERM  "0007777"


#define BASIC_UID "0000000"
#define BASIC_GID "0000000"

#define END_FILE_BLOCK_SIZE 512
#define NUMBER_END_BLOCKS 2
#define NUMBER_END_BYTES END_FILE_BLOCK_SIZE * NUMBER_END_BLOCKS

#define ARCHIVE_NAME "archive.tar"

unsigned int calculate_checksum(tar_h* input);
void define_content_size(tar_h* header, int content_size);
void initialize_header(tar_h* header);
void make_tar_empty(tar_h* header, int checksum);
void make_tar(tar_h* header, char* content, size_t content_size, char* end_bytes_buffer, size_t end_size, int checksum);
void update_header_field(char* header_field_to_change, char* new_header_value, size_t size);

#endif //FUZZER__TAR_HELPER_H_