#ifndef BEANSCRIPT_UTILITY_H
#define BEANSCRIPT_UTILITY_H

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#include "src/main.h"

int str_array_find(const char** array, int size, const char* target);
int str_remove_trailing_delimiters(char *S, const char *delimiters);
int str_remove_leading_ignored_chars(char *S, const char *delimiters);

int int_array_get_or_default(int* int_array, int int_array_len, int idx, int default_value);
int get_min_int_3(int a, int b, int c);

time_t get_current_time();

#endif //BEANSCRIPT_UTILITY_H