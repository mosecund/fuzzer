//
// Created by Ismael Secundar on 25/02/2024.
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <wchar.h>

#include "tar_helper.h"

#ifndef FUZZER__FUZZ_H_
#define FUZZER__FUZZ_H_
int test_extraction(char* extractor_path) ;
void test_ending_bytes(char* extractor_path);
void test_size(char* extractor_path);
void test_typeflag(char* extractor_path);
void fuzz(char* path_of_extractor);

#endif //FUZZER__FUZZ_H_
