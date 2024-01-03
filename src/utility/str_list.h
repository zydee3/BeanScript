#ifndef BEANSCRIPT_STR_LIST_H
#define BEANSCRIPT_STR_LIST_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>

#include "src/main.h"

// Forward Declaration
typedef struct StrListStruct StrList;

// Constructor and Destructor
StrList*    str_list_new(int resize_value, bool is_using_shared_memory);
StrList*    str_list_copy(StrList* ref_list);
void        str_list_delete(StrList** ptr_list);

// Accessor Functions
int         str_list_get_size(StrList* list);
char*       str_list_get_str(StrList* list, int index);
int         str_list_get_index(StrList* list, const char* str);
char*       str_list_concatenate(StrList* list, const char* split_str);

// Mutator Functions
void        str_list_insert_str(StrList* list, char* str);
void        str_list_clear(StrList* list);

// Utility Functions
void        str_list_print(StrList* list, bool should_format);

#endif //BEANSCRIPT_STR_LIST_H